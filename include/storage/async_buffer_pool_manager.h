//
// Created by Huang_cj on 2025/9/8.
// 异步缓冲池管理器 - 支持异步IO操作
//

#pragma once
#include "../catalog/page.h"
#include "storage/async_disk_manager.h"
#include <unordered_map>
#include <list>
#include <memory>
#include <future>
#include <mutex>
#include <vector>

// 异步缓冲池管理器 - 完全兼容BufferPoolManager接口
class AsyncBufferPoolManager {
public:
    AsyncBufferPoolManager(size_t pool_size, AsyncDiskManager *disk_manager);
    ~AsyncBufferPoolManager();
    
    // 同步接口（完全兼容原BufferPoolManager）
    Page* FetchPage(int page_id);
    bool UnpinPage(int page_id);
    bool FlushPage(int page_id);
    Page* NewPage(int *page_id);
    bool DeletePage(int page_id);
    int EvictPage();
    
    // 异步接口（额外功能）
    std::future<Page*> FetchPageAsync(int page_id);
    std::future<bool> UnpinPageAsync(int page_id);
    std::future<bool> FlushPageAsync(int page_id);
    std::future<Page*> NewPageAsync(int *page_id);
    std::future<bool> DeletePageAsync(int page_id);
    
    // 批量异步操作
    std::future<std::vector<Page*>> FetchPagesAsync(const std::vector<int>& page_ids);
    std::future<bool> UnpinPagesAsync(const std::vector<int>& page_ids);
    
    // 预取操作
    void PrefetchPage(int page_id);
    void PrefetchPages(const std::vector<int>& page_ids);
    
    // 智能预读功能
    void PrefetchSequential(int start_page_id, int count);
    void PrefetchRange(int start_page_id, int end_page_id);
    void SetPrefetchConfig(size_t max_prefetch_pages, size_t prefetch_threshold);
    
    // 预读统计
    struct PrefetchStats {
        size_t total_prefetch_requests = 0;
        size_t successful_prefetches = 0;
        size_t cache_hits_from_prefetch = 0;
        double prefetch_hit_rate = 0.0;
    };
    PrefetchStats GetPrefetchStats() const;
    
private:
    size_t pool_size_;
    AsyncDiskManager *disk_manager_;
    
    Page *pages_;
    std::unordered_map<int, Page*> page_table_;
    std::list<int> lru_list_;
    std::unordered_map<int, int> page_ref_count_;
    
    // 预读配置和统计
    size_t max_prefetch_pages_ = 10;  // 最大预读页面数
    size_t prefetch_threshold_ = 3;   // 预读阈值
    mutable PrefetchStats prefetch_stats_;
    mutable std::mutex prefetch_stats_mutex_;
    
    // 异步操作辅助
    Page* DoFetchPage(int page_id);
    bool DoUnpinPage(int page_id);
    bool DoFlushPage(int page_id);
    Page* DoNewPage(int *page_id);
    bool DoDeletePage(int page_id);
};
