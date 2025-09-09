//
// Created by Huang_cj on 2025/9/8.
//

#pragma once
#include "storage/page.h"
#include "storage/disk_manager.h"
#include <unordered_map>
#include <list>

class BufferPoolManager {
public:
    BufferPoolManager(size_t pool_size, DiskManager *disk_manager);
    ~BufferPoolManager();

    Page* FetchPage(int page_id);   // 获取页
    bool UnpinPage(int page_id);    // 释放页
    bool FlushPage(int page_id);    // 写回页
    Page* NewPage(int *page_id);    // 分配新页
    bool DeletePage(int page_id);   // 删除页
    int EvictPage();                // 简单LRU

private:
    size_t pool_size_;
    DiskManager *disk_manager_;

    Page *pages_;   // 缓冲池数组
    std::unordered_map<int, Page*> page_table_; // 缓冲池的逻辑映射

    std::list<int> lru_list_;   // LRU 队列，存放page_id;
};