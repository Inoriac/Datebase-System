//
// 预读配置管理器实现
//

#include "storage/prefetch_config.h"
#include <algorithm>
#include <numeric>

PrefetchConfigManager& PrefetchConfigManager::GetInstance() {
    static PrefetchConfigManager instance;
    return instance;
}

void PrefetchConfigManager::SetGlobalConfig(const PrefetchConfig& config) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    global_config_ = config;
}

void PrefetchConfigManager::SetTableConfig(const std::string& table_name, const PrefetchConfig& config) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    table_configs_[table_name] = config;
}

PrefetchConfig PrefetchConfigManager::GetConfig(const std::string& table_name) const {
    std::lock_guard<std::mutex> lock(config_mutex_);
    
    if (table_name.empty()) {
        return global_config_;
    }
    
    auto it = table_configs_.find(table_name);
    if (it != table_configs_.end()) {
        return it->second;
    }
    
    return global_config_;
}

void PrefetchConfigManager::RecordPrefetchRequest(const std::string& table_name, size_t page_count) {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    table_stats_[table_name].total_prefetch_requests += page_count;
}

void PrefetchConfigManager::RecordPrefetchSuccess(const std::string& table_name, size_t page_count, 
                                                 std::chrono::milliseconds duration) {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    auto& stats = table_stats_[table_name];
    stats.successful_prefetches += page_count;
    stats.total_prefetch_time += duration;
    
    if (stats.successful_prefetches > 0) {
        stats.avg_prefetch_time = std::chrono::milliseconds(
            stats.total_prefetch_time.count() / stats.successful_prefetches);
    }
}

void PrefetchConfigManager::RecordPrefetchHit(const std::string& table_name, size_t hit_count) {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    auto& stats = table_stats_[table_name];
    stats.cache_hits_from_prefetch += hit_count;
    
    if (stats.total_prefetch_requests > 0) {
        stats.prefetch_hit_rate = static_cast<double>(stats.cache_hits_from_prefetch) / 
                                 stats.total_prefetch_requests;
    }
}

void PrefetchConfigManager::RecordWastedPrefetch(const std::string& table_name, size_t wasted_count) {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    auto& stats = table_stats_[table_name];
    stats.wasted_prefetches += wasted_count;
    
    if (stats.successful_prefetches > 0) {
        stats.prefetch_efficiency = static_cast<double>(stats.cache_hits_from_prefetch) / 
                                   (stats.cache_hits_from_prefetch + stats.wasted_prefetches);
    }
}

PrefetchPerformanceStats PrefetchConfigManager::GetStats(const std::string& table_name) const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    
    if (table_name.empty()) {
        // 返回全局统计
        PrefetchPerformanceStats global_stats;
        for (const auto& [name, stats] : table_stats_) {
            global_stats.total_prefetch_requests += stats.total_prefetch_requests;
            global_stats.successful_prefetches += stats.successful_prefetches;
            global_stats.cache_hits_from_prefetch += stats.cache_hits_from_prefetch;
            global_stats.wasted_prefetches += stats.wasted_prefetches;
            global_stats.total_prefetch_time += stats.total_prefetch_time;
        }
        
        if (global_stats.total_prefetch_requests > 0) {
            global_stats.prefetch_hit_rate = static_cast<double>(global_stats.cache_hits_from_prefetch) / 
                                           global_stats.total_prefetch_requests;
        }
        
        if (global_stats.successful_prefetches > 0) {
            global_stats.avg_prefetch_time = std::chrono::milliseconds(
                global_stats.total_prefetch_time.count() / global_stats.successful_prefetches);
        }
        
        if (global_stats.cache_hits_from_prefetch + global_stats.wasted_prefetches > 0) {
            global_stats.prefetch_efficiency = static_cast<double>(global_stats.cache_hits_from_prefetch) / 
                                             (global_stats.cache_hits_from_prefetch + global_stats.wasted_prefetches);
        }
        
        return global_stats;
    }
    
    auto it = table_stats_.find(table_name);
    if (it != table_stats_.end()) {
        return it->second;
    }
    
    return PrefetchPerformanceStats{};
}

void PrefetchConfigManager::ResetStats(const std::string& table_name) {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    
    if (table_name.empty()) {
        table_stats_.clear();
    } else {
        table_stats_.erase(table_name);
    }
}

// 访问历史管理实现
void PrefetchConfigManager::RecordPageAccess(const std::string& table_name, int page_id) {
    std::lock_guard<std::mutex> lock(access_history_mutex_);
    
    auto& history = table_access_history_[table_name];
    
    // 添加新的访问记录
    history.push_back(page_id);
    
    // 限制历史记录长度，保持最近20次访问
    const size_t max_history_size = 20;
    if (history.size() > max_history_size) {
        history.pop_front();
    }
}

std::vector<int> PrefetchConfigManager::GetRecentAccesses(const std::string& table_name, size_t max_count) const {
    std::lock_guard<std::mutex> lock(access_history_mutex_);
    
    auto it = table_access_history_.find(table_name);
    if (it == table_access_history_.end()) {
        return {};
    }
    
    const auto& history = it->second;
    size_t count = std::min(max_count, history.size());
    
    std::vector<int> result;
    result.reserve(count);
    
    // 从最新的访问开始取
    auto start_it = history.end() - count;
    for (auto it = start_it; it != history.end(); ++it) {
        result.push_back(*it);
    }
    
    return result;
}

void PrefetchConfigManager::ClearAccessHistory(const std::string& table_name) {
    std::lock_guard<std::mutex> lock(access_history_mutex_);
    
    if (table_name.empty()) {
        table_access_history_.clear();
    } else {
        table_access_history_.erase(table_name);
    }
}

bool PrefetchConfigManager::ShouldPrefetch(const std::string& table_name, int page_id) const {
    auto config = GetConfig(table_name);
    
    if (config.strategy == PrefetchStrategy::NONE) {
        return false;
    }
    
    // 获取最近的访问历史
    auto recent_accesses = GetRecentAccesses(table_name, config.prefetch_threshold);
    
    if (config.strategy == PrefetchStrategy::SEQUENTIAL) {
        return IsSequentialAccess(recent_accesses);
    }
    
    if (config.strategy == PrefetchStrategy::ADAPTIVE) {
        double score = CalculateAccessPatternScore(recent_accesses);
        return score > config.prefetch_aggressiveness;
    }
    
    return true;  // RANDOM strategy
}

size_t PrefetchConfigManager::GetOptimalPrefetchCount(const std::string& table_name) const {
    auto config = GetConfig(table_name);
    auto stats = GetStats(table_name);
    
    // 获取最近的访问历史
    auto recent_accesses = GetRecentAccesses(table_name, config.prefetch_threshold);
    
    size_t base_count = config.max_prefetch_pages;
    
    if (config.strategy == PrefetchStrategy::ADAPTIVE) {
        // 根据预读效率调整预读数量
        if (stats.prefetch_efficiency > 0.8) {
            base_count = static_cast<size_t>(base_count * 1.2);  // 增加预读
        } else if (stats.prefetch_efficiency < 0.3) {
            base_count = static_cast<size_t>(base_count * 0.7);  // 减少预读
        }
    }
    
    if (IsSequentialAccess(recent_accesses)) {
        base_count = static_cast<size_t>(base_count * 1.5);  // 顺序访问增加预读
    }
    
    return std::min(base_count, config.max_prefetch_pages);
}

double PrefetchConfigManager::CalculateAccessPatternScore(const std::vector<int>& recent_accesses) const {
    if (recent_accesses.size() < 2) {
        return 0.0;
    }
    
    // 计算顺序性得分
    int sequential_count = 0;
    for (size_t i = 1; i < recent_accesses.size(); ++i) {
        if (recent_accesses[i] == recent_accesses[i-1] + 1) {
            sequential_count++;
        }
    }
    
    return static_cast<double>(sequential_count) / (recent_accesses.size() - 1);
}

bool PrefetchConfigManager::IsSequentialAccess(const std::vector<int>& recent_accesses) const {
    if (recent_accesses.size() < 3) {
        return false;
    }
    
    // 检查是否大部分访问都是顺序的
    int sequential_count = 0;
    for (size_t i = 1; i < recent_accesses.size(); ++i) {
        if (recent_accesses[i] == recent_accesses[i-1] + 1) {
            sequential_count++;
        }
    }
    
    double sequential_ratio = static_cast<double>(sequential_count) / (recent_accesses.size() - 1);
    return sequential_ratio > 0.7;  // 70%以上是顺序访问
}
