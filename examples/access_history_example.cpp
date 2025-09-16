//
// 访问历史记录功能演示
// 展示recent_accesses的更新机制和使用方法
//

#include "../include/storage/prefetch_config.h"
#include <iostream>
#include <vector>

void demonstrateAccessHistory() {
    std::cout << "=== 访问历史记录功能演示 ===" << std::endl;
    
    auto& config_manager = PrefetchConfigManager::GetInstance();
    
    // 配置自适应预读策略
    PrefetchConfig config;
    config.strategy = PrefetchStrategy::ADAPTIVE;
    config.max_prefetch_pages = 10;
    config.prefetch_threshold = 5;  // 需要5次访问才触发预读
    config.prefetch_aggressiveness = 0.6;
    config_manager.SetGlobalConfig(config);
    
    std::cout << "配置完成：自适应预读，阈值=5，激进程度=0.6" << std::endl;
    
    // 模拟页面访问序列
    std::cout << "\n=== 模拟顺序访问模式 ===" << std::endl;
    
    // 顺序访问页面1-8
    for (int i = 1; i <= 8; ++i) {
        config_manager.RecordPageAccess("test_table", i);
        std::cout << "访问页面 " << i;
        
        // 检查是否应该预读
        if (i >= config.prefetch_threshold) {
            bool should_prefetch = config_manager.ShouldPrefetch("test_table", i + 1);
            std::cout << " -> 预读决策: " << (should_prefetch ? "是" : "否");
            
            if (should_prefetch) {
                size_t prefetch_count = config_manager.GetOptimalPrefetchCount("test_table");
                std::cout << " (预读数量: " << prefetch_count << ")";
            }
        }
        std::cout << std::endl;
    }
    
    // 显示访问历史
    auto recent_accesses = config_manager.GetRecentAccesses("test_table", 10);
    std::cout << "\n当前访问历史: ";
    for (size_t i = 0; i < recent_accesses.size(); ++i) {
        std::cout << recent_accesses[i];
        if (i < recent_accesses.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    // 清空历史，模拟随机访问
    std::cout << "\n=== 模拟随机访问模式 ===" << std::endl;
    config_manager.ClearAccessHistory("test_table");
    
    std::vector<int> random_accesses = {1, 5, 3, 9, 2, 7, 4, 6, 8, 10};
    for (int page_id : random_accesses) {
        config_manager.RecordPageAccess("test_table", page_id);
        std::cout << "访问页面 " << page_id;
        
        // 检查预读决策
        bool should_prefetch = config_manager.ShouldPrefetch("test_table", page_id + 1);
        std::cout << " -> 预读决策: " << (should_prefetch ? "是" : "否");
        std::cout << std::endl;
    }
    
    // 显示随机访问历史
    recent_accesses = config_manager.GetRecentAccesses("test_table", 10);
    std::cout << "\n随机访问历史: ";
    for (size_t i = 0; i < recent_accesses.size(); ++i) {
        std::cout << recent_accesses[i];
        if (i < recent_accesses.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    // 演示混合访问模式
    std::cout << "\n=== 模拟混合访问模式 ===" << std::endl;
    config_manager.ClearAccessHistory("test_table");
    
    std::vector<int> mixed_accesses = {1, 2, 5, 6, 7, 3, 4, 8, 9, 10};
    for (int page_id : mixed_accesses) {
        config_manager.RecordPageAccess("test_table", page_id);
        std::cout << "访问页面 " << page_id;
        
        // 检查预读决策
        bool should_prefetch = config_manager.ShouldPrefetch("test_table", page_id + 1);
        std::cout << " -> 预读决策: " << (should_prefetch ? "是" : "否");
        std::cout << std::endl;
    }
    
    // 显示混合访问历史
    recent_accesses = config_manager.GetRecentAccesses("test_table", 10);
    std::cout << "\n混合访问历史: ";
    for (size_t i = 0; i < recent_accesses.size(); ++i) {
        std::cout << recent_accesses[i];
        if (i < recent_accesses.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
}

void demonstrateAccessHistoryOverflow() {
    std::cout << "\n=== 访问历史溢出测试 ===" << std::endl;
    
    auto& config_manager = PrefetchConfigManager::GetInstance();
    
    // 清空历史
    config_manager.ClearAccessHistory("overflow_test");
    
    // 访问超过20个页面（历史记录最大长度）
    std::cout << "访问25个页面，测试历史记录溢出..." << std::endl;
    for (int i = 1; i <= 25; ++i) {
        config_manager.RecordPageAccess("overflow_test", i);
    }
    
    // 获取最近访问记录
    auto recent_accesses = config_manager.GetRecentAccesses("overflow_test", 10);
    std::cout << "最近10次访问: ";
    for (size_t i = 0; i < recent_accesses.size(); ++i) {
        std::cout << recent_accesses[i];
        if (i < recent_accesses.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    // 验证只保留了最近的20次访问
    auto all_accesses = config_manager.GetRecentAccesses("overflow_test", 25);
    std::cout << "总访问记录数: " << all_accesses.size() << " (应该最多20个)" << std::endl;
    
    if (!all_accesses.empty()) {
        std::cout << "最早的访问: " << all_accesses[0] << std::endl;
        std::cout << "最新的访问: " << all_accesses.back() << std::endl;
    }
}

void demonstrateMultipleTables() {
    std::cout << "\n=== 多表访问历史测试 ===" << std::endl;
    
    auto& config_manager = PrefetchConfigManager::GetInstance();
    
    // 为不同表记录访问历史
    std::cout << "为表A记录顺序访问..." << std::endl;
    for (int i = 1; i <= 5; ++i) {
        config_manager.RecordPageAccess("table_A", i);
    }
    
    std::cout << "为表B记录随机访问..." << std::endl;
    std::vector<int> random_pages = {10, 15, 12, 18, 11};
    for (int page_id : random_pages) {
        config_manager.RecordPageAccess("table_B", page_id);
    }
    
    // 获取不同表的访问历史
    auto history_A = config_manager.GetRecentAccesses("table_A", 10);
    auto history_B = config_manager.GetRecentAccesses("table_B", 10);
    
    std::cout << "表A的访问历史: ";
    for (size_t i = 0; i < history_A.size(); ++i) {
        std::cout << history_A[i];
        if (i < history_A.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    std::cout << "表B的访问历史: ";
    for (size_t i = 0; i < history_B.size(); ++i) {
        std::cout << history_B[i];
        if (i < history_B.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    // 测试预读决策
    bool should_prefetch_A = config_manager.ShouldPrefetch("table_A", 6);
    bool should_prefetch_B = config_manager.ShouldPrefetch("table_B", 20);
    
    std::cout << "表A预读决策: " << (should_prefetch_A ? "是" : "否") << std::endl;
    std::cout << "表B预读决策: " << (should_prefetch_B ? "是" : "否") << std::endl;
}

int main() {
    std::cout << "访问历史记录功能演示" << std::endl;
    std::cout << "====================" << std::endl;
    
    try {
        demonstrateAccessHistory();
        demonstrateAccessHistoryOverflow();
        demonstrateMultipleTables();
        
        std::cout << "\n演示完成！" << std::endl;
        std::cout << "\n访问历史记录功能总结:" << std::endl;
        std::cout << "1. 自动记录页面访问历史" << std::endl;
        std::cout << "2. 支持表级别的历史记录" << std::endl;
        std::cout << "3. 自动维护最近20次访问" << std::endl;
        std::cout << "4. 基于历史记录进行预读决策" << std::endl;
        std::cout << "5. 支持顺序、随机、混合访问模式识别" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "演示过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
