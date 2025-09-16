//
// 预读配置管理
// 提供统一的预读策略配置和性能监控
//

#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>

// 预读策略类型
enum class PrefetchStrategy {
    NONE,           // 不预读
    SEQUENTIAL,     // 顺序预读
    RANDOM,         // 随机预读
    ADAPTIVE        // 自适应预读
};

// 预读配置
struct PrefetchConfig {
    PrefetchStrategy strategy = PrefetchStrategy::SEQUENTIAL;
    size_t max_prefetch_pages = 10;
    size_t prefetch_threshold = 3;
    bool enable_async_prefetch = true;
    double prefetch_aggressiveness = 0.5;  // 0.0-1.0，预读激进程度
};

// 预读性能统计
struct PrefetchPerformanceStats {
    size_t total_prefetch_requests = 0;
    size_t successful_prefetches = 0;
    size_t cache_hits_from_prefetch = 0;
    size_t wasted_prefetches = 0;  // 预读但未使用的页面
    double prefetch_hit_rate = 0.0;
    double prefetch_efficiency = 0.0;  // 预读效率
    std::chrono::milliseconds total_prefetch_time{0};
    std::chrono::milliseconds avg_prefetch_time{0};
};

// 预读配置管理器
class PrefetchConfigManager {
public:
    static PrefetchConfigManager& GetInstance();
    
    // 配置管理
    void SetGlobalConfig(const PrefetchConfig& config);
    void SetTableConfig(const std::string& table_name, const PrefetchConfig& config);
    PrefetchConfig GetConfig(const std::string& table_name = "") const;
    
    // 性能监控
    void RecordPrefetchRequest(const std::string& table_name, size_t page_count);
    void RecordPrefetchSuccess(const std::string& table_name, size_t page_count, 
                              std::chrono::milliseconds duration);
    void RecordPrefetchHit(const std::string& table_name, size_t hit_count);
    void RecordWastedPrefetch(const std::string& table_name, size_t wasted_count);
    
    PrefetchPerformanceStats GetStats(const std::string& table_name = "") const;
    void ResetStats(const std::string& table_name = "");
    
    // 自适应预读决策
    bool ShouldPrefetch(const std::string& table_name, int page_id, 
                       const std::vector<int>& recent_accesses) const;
    size_t GetOptimalPrefetchCount(const std::string& table_name, 
                                  const std::vector<int>& recent_accesses) const;
    
private:
    PrefetchConfigManager() = default;
    
    mutable std::mutex config_mutex_;
    mutable std::mutex stats_mutex_;
    
    PrefetchConfig global_config_;
    std::unordered_map<std::string, PrefetchConfig> table_configs_;
    std::unordered_map<std::string, PrefetchPerformanceStats> table_stats_;
    
    // 自适应算法辅助
    double CalculateAccessPatternScore(const std::vector<int>& recent_accesses) const;
    bool IsSequentialAccess(const std::vector<int>& recent_accesses) const;
};
