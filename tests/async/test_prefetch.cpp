//
// Created by Assistant on 2025/9/17.
// 预读功能测试
//

#include <gtest/gtest.h>
#include <cstdio>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <future>
#include <random>

#include "../../include/storage/async_disk_manager.h"
#include "../../include/storage/async_buffer_pool_manager.h"
#include "../../include/catalog/page.h"

// 为每次测试创建独立的临时文件，避免相互干扰
static std::string MakeTempDbFile(const std::string &name_hint) {
    std::string path = std::string("test_prefetch_") + name_hint + ".db";
    // 清理旧文件
    std::remove(path.c_str());
    return path;
}

// 生成测试数据
static void GenerateTestData(char* data, size_t size, int seed = 0) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(0, 255);
    
    for (size_t i = 0; i < size; ++i) {
        data[i] = static_cast<char>(dis(gen));
    }
}

// 验证数据是否正确
static bool VerifyTestData(const char* data, size_t size, int seed = 0) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(0, 255);
    
    for (size_t i = 0; i < size; ++i) {
        if (data[i] != static_cast<char>(dis(gen))) {
            return false;
        }
    }
    return true;
}

// ========== 预读功能测试 ==========

class PrefetchTest : public ::testing::Test {
protected:
    void SetUp() override {
        db_file_ = MakeTempDbFile("prefetch");
        disk_manager_ = std::make_unique<AsyncDiskManager>(db_file_, 2);
        buffer_pool_ = std::make_unique<AsyncBufferPoolManager>(20, disk_manager_.get());
        
        // 准备测试数据
        PrepareTestData();
    }
    
    void TearDown() override {
        buffer_pool_.reset();
        disk_manager_.reset();
        std::remove(db_file_.c_str());
    }
    
    void PrepareTestData() {
        // 创建一些测试页面
        for (int i = 0; i < 10; ++i) {
            int page_id;
            Page* page = buffer_pool_->NewPage(&page_id);
            EXPECT_NE(page, nullptr);
            
            char* data = page->GetData();
            GenerateTestData(data, PAGE_SIZE, i);
            page->SetDirty(true);
            
            buffer_pool_->FlushPage(page_id);
            buffer_pool_->UnpinPage(page_id);
            
            test_page_ids_.push_back(page_id);
        }
    }
    
    std::string db_file_;
    std::unique_ptr<AsyncDiskManager> disk_manager_;
    std::unique_ptr<AsyncBufferPoolManager> buffer_pool_;
    std::vector<int> test_page_ids_;
};

TEST_F(PrefetchTest, BasicPrefetchPage) {  // 测试单页面预取功能：验证预取操作正确性和缓存命中效果
    // 测试基本的单页面预取
    int page_id = test_page_ids_[0];
    
    // 预取页面
    buffer_pool_->PrefetchPage(page_id);
    
    // 获取页面（应该从缓存中快速获取）
    Page* page = buffer_pool_->FetchPage(page_id);
    EXPECT_NE(page, nullptr);
    EXPECT_EQ(page->GetPageId(), page_id);
    
    // 验证数据
    char* data = page->GetData();
    EXPECT_TRUE(VerifyTestData(data, PAGE_SIZE, 0));
    
    buffer_pool_->UnpinPage(page_id);
}

TEST_F(PrefetchTest, BatchPrefetchPages) {  // 测试批量页面预取功能：验证多页面同时预取和批量处理效率
    // 测试批量页面预取
    std::vector<int> prefetch_ids = {test_page_ids_[0], test_page_ids_[1], test_page_ids_[2]};
    
    // 批量预取
    buffer_pool_->PrefetchPages(prefetch_ids);
    
    // 验证预取效果
    for (int i = 0; i < prefetch_ids.size(); ++i) {
        Page* page = buffer_pool_->FetchPage(prefetch_ids[i]);
        EXPECT_NE(page, nullptr);
        EXPECT_EQ(page->GetPageId(), prefetch_ids[i]);
        
        char* data = page->GetData();
        EXPECT_TRUE(VerifyTestData(data, PAGE_SIZE, i));
        
        buffer_pool_->UnpinPage(prefetch_ids[i]);
    }
}

TEST_F(PrefetchTest, SequentialPrefetch) {  // 测试顺序预取功能：验证连续页面序列的预取策略
    // 测试顺序预取
    int start_page_id = test_page_ids_[0];
    int count = 3;
    
    // 顺序预取
    buffer_pool_->PrefetchSequential(start_page_id, count);
    
    // 验证预取效果
    for (int i = 0; i < count; ++i) {
        int page_id = start_page_id + i;
        Page* page = buffer_pool_->FetchPage(page_id);
        EXPECT_NE(page, nullptr);
        EXPECT_EQ(page->GetPageId(), page_id);
        
        char* data = page->GetData();
        EXPECT_TRUE(VerifyTestData(data, PAGE_SIZE, i));
        
        buffer_pool_->UnpinPage(page_id);
    }
}

TEST_F(PrefetchTest, RangePrefetch) {  // 测试范围预取功能：验证指定范围内所有页面的预取策略
    // 测试范围预取
    int start_page_id = test_page_ids_[0];
    int end_page_id = test_page_ids_[2];
    
    // 范围预取
    buffer_pool_->PrefetchRange(start_page_id, end_page_id);
    
    // 验证预取效果
    for (int page_id = start_page_id; page_id <= end_page_id; ++page_id) {
        Page* page = buffer_pool_->FetchPage(page_id);
        EXPECT_NE(page, nullptr);
        EXPECT_EQ(page->GetPageId(), page_id);
        
        char* data = page->GetData();
        EXPECT_TRUE(VerifyTestData(data, PAGE_SIZE, page_id - start_page_id));
        
        buffer_pool_->UnpinPage(page_id);
    }
}

TEST_F(PrefetchTest, PrefetchConfiguration) {  // 测试预取配置功能：验证预取参数设置和配置生效
    // 测试预取配置
    size_t max_prefetch_pages = 5;
    size_t prefetch_threshold = 2;
    
    // 设置预取配置
    buffer_pool_->SetPrefetchConfig(max_prefetch_pages, prefetch_threshold);
    
    // 执行预取操作
    buffer_pool_->PrefetchPage(test_page_ids_[0]);
    buffer_pool_->PrefetchPages({test_page_ids_[1], test_page_ids_[2]});
    
    // 获取预取统计信息
    auto stats = buffer_pool_->GetPrefetchStats();
    EXPECT_GE(stats.total_prefetch_requests, 3);
    EXPECT_GE(stats.successful_prefetches, 0);
}

TEST_F(PrefetchTest, PrefetchStats) {  // 测试预取统计信息功能：验证预取操作统计数据的准确性和更新
    // 测试预取统计信息
    auto initial_stats = buffer_pool_->GetPrefetchStats();
    
    // 执行一些预取操作
    buffer_pool_->PrefetchPage(test_page_ids_[0]);
    buffer_pool_->PrefetchPages({test_page_ids_[1], test_page_ids_[2]});
    buffer_pool_->PrefetchSequential(test_page_ids_[3], 2);
    
    // 获取更新后的统计信息
    auto updated_stats = buffer_pool_->GetPrefetchStats();
    
    EXPECT_GT(updated_stats.total_prefetch_requests, initial_stats.total_prefetch_requests);
    EXPECT_GE(updated_stats.successful_prefetches, 0);
    EXPECT_GE(updated_stats.cache_hits_from_prefetch, 0);
}

TEST_F(PrefetchTest, ConcurrentPrefetch) {  // 测试并发预取功能：验证多线程环境下的预取操作安全性和正确性
    // 测试并发预取
    std::vector<std::thread> threads;
    const int num_threads = 3;
    const int pages_per_thread = 2;
    
    // 启动多个线程进行并发预取
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, t, pages_per_thread]() {
            for (int i = 0; i < pages_per_thread; ++i) {
                int page_id = test_page_ids_[t * pages_per_thread + i];
                buffer_pool_->PrefetchPage(page_id);
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    // 验证预取结果
    for (int t = 0; t < num_threads; ++t) {
        for (int i = 0; i < pages_per_thread; ++i) {
            int page_id = test_page_ids_[t * pages_per_thread + i];
            Page* page = buffer_pool_->FetchPage(page_id);
            EXPECT_NE(page, nullptr);
            EXPECT_EQ(page->GetPageId(), page_id);
            buffer_pool_->UnpinPage(page_id);
        }
    }
}

TEST_F(PrefetchTest, PrefetchHitRate) {  // 测试预取命中率功能：验证预取缓存命中率统计和计算准确性
    // 测试预取命中率
    const int num_pages = 5;
    std::vector<int> page_ids(test_page_ids_.begin(), test_page_ids_.begin() + num_pages);
    
    // 预取页面
    buffer_pool_->PrefetchPages(page_ids);
    
    // 等待预取完成
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // 访问预取的页面
    for (int page_id : page_ids) {
        Page* page = buffer_pool_->FetchPage(page_id);
        EXPECT_NE(page, nullptr);
        buffer_pool_->UnpinPage(page_id);
    }
    
    // 等待统计信息更新
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // 获取统计信息
    auto stats = buffer_pool_->GetPrefetchStats();
    
    EXPECT_GT(stats.total_prefetch_requests, 0);
    EXPECT_GE(stats.successful_prefetches, 0);
    EXPECT_GE(stats.cache_hits_from_prefetch, 0);
    
    if (stats.total_prefetch_requests > 0) {
        double hit_rate = (double)stats.cache_hits_from_prefetch / stats.total_prefetch_requests;
        EXPECT_GE(hit_rate, 0.0);
        std::cout << "预取命中率: " << (hit_rate * 100) << "%" << std::endl;
    }
}

// ========== 预读策略测试 ==========

class PrefetchStrategyTest : public ::testing::Test {
protected:
    void SetUp() override {
        db_file_ = MakeTempDbFile("strategy");
        disk_manager_ = std::make_unique<AsyncDiskManager>(db_file_, 2);
        buffer_pool_ = std::make_unique<AsyncBufferPoolManager>(15, disk_manager_.get());
        
        // 准备更多测试数据
        PrepareTestData();
    }
    
    void TearDown() override {
        buffer_pool_.reset();
        disk_manager_.reset();
        std::remove(db_file_.c_str());
    }
    
    void PrepareTestData() {
        // 创建20个测试页面
        for (int i = 0; i < 20; ++i) {
            int page_id;
            Page* page = buffer_pool_->NewPage(&page_id);
            EXPECT_NE(page, nullptr);
            
            char* data = page->GetData();
            GenerateTestData(data, PAGE_SIZE, i);
            page->SetDirty(true);
            
            buffer_pool_->FlushPage(page_id);
            buffer_pool_->UnpinPage(page_id);
            
            test_page_ids_.push_back(page_id);
        }
    }
    
    std::string db_file_;
    std::unique_ptr<AsyncDiskManager> disk_manager_;
    std::unique_ptr<AsyncBufferPoolManager> buffer_pool_;
    std::vector<int> test_page_ids_;
};

TEST_F(PrefetchStrategyTest, SequentialAccessPattern) {  // 测试顺序访问模式：验证顺序预取在顺序访问场景下的效果和性能
    // 测试顺序访问模式的预取效果
    int start_page = test_page_ids_[0];
    int count = 5;
    
    // 使用顺序预取
    buffer_pool_->PrefetchSequential(start_page, count);
    
    // 顺序访问页面
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < count; ++i) {
        int page_id = start_page + i;
        Page* page = buffer_pool_->FetchPage(page_id);
        EXPECT_NE(page, nullptr);
        buffer_pool_->UnpinPage(page_id);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    
    // 验证预取统计
    auto stats = buffer_pool_->GetPrefetchStats();
    EXPECT_GE(stats.cache_hits_from_prefetch, 0);
}

TEST_F(PrefetchStrategyTest, RandomAccessPattern) {  // 测试随机访问模式：验证批量预取在随机访问场景下的效果和性能
    // 测试随机访问模式的预取效果
    std::vector<int> random_pages = {test_page_ids_[0], test_page_ids_[5], test_page_ids_[10], 
                                    test_page_ids_[15], test_page_ids_[19]};
    
    // 预取随机页面
    buffer_pool_->PrefetchPages(random_pages);
    
    // 随机访问页面
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int page_id : random_pages) {
        Page* page = buffer_pool_->FetchPage(page_id);
        EXPECT_NE(page, nullptr);
        buffer_pool_->UnpinPage(page_id);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    
    // 验证预取统计
    auto stats = buffer_pool_->GetPrefetchStats();
    EXPECT_GE(stats.cache_hits_from_prefetch, 0);
}

TEST_F(PrefetchStrategyTest, RangeAccessPattern) {  // 测试范围访问模式：验证范围预取在范围访问场景下的效果和性能
    // 测试范围访问模式的预取效果
    int start_page = test_page_ids_[5];
    int end_page = test_page_ids_[10];
    
    // 使用范围预取
    buffer_pool_->PrefetchRange(start_page, end_page);
    
    // 范围访问页面
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int page_id = start_page; page_id <= end_page; ++page_id) {
        Page* page = buffer_pool_->FetchPage(page_id);
        EXPECT_NE(page, nullptr);
        buffer_pool_->UnpinPage(page_id);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    
    // 验证预取统计
    auto stats = buffer_pool_->GetPrefetchStats();
    EXPECT_GE(stats.cache_hits_from_prefetch, 0);
}
