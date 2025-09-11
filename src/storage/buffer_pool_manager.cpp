//
// Created by Huang_cj on 2025/9/8.
//

#include "storage/buffer_pool_manager.h"
#include <iostream>
#include <cstring>

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager){
    pages_ = new Page[pool_size_];  // 分配缓冲池
}

BufferPoolManager::~BufferPoolManager() {
    // 析构前刷盘所有脏页
    for (auto &kv : page_table_) {
        FlushPage(kv.first);
    }
    delete[] pages_;
}

// 从缓存池中取一个页，如果不在内存中则从磁盘加载
Page *BufferPoolManager::FetchPage(int page_id) {
    // 首先检查页ID是否有效
    if (page_id >= disk_manager_->GetTotalPages()) {
        return nullptr; // 无效页ID，直接返回nullptr
    }

    // 检查页面是否已被删除
    if (disk_manager_->IsPageFree(page_id)) {
        return nullptr; // 页面已被删除，返回nullptr
    }

    // 缓存命中
    if (page_table_.count(page_id)) {
        Page *page = page_table_[page_id];
        lru_list_.remove(page_id);
        lru_list_.push_front(page_id);

        page_ref_count_[page_id]++; // 引用计数

        return page;
    }

    // 不在缓存中，尝试找到空闲帧；若无则淘汰一页并复用其帧
    Page *new_page = nullptr;
    for (size_t i = 0; i < pool_size_; i++) {
        if (pages_[i].GetPageId() == -1) {
            new_page = &pages_[i];
            break;
        }
    }
    if (new_page == nullptr) {
        // 没有空闲帧，需要淘汰
        int victim_page_id = EvictPage();
        if (victim_page_id == -1) {
            std::cerr << "BufferPoolManager: no victim page to evict!" << std::endl;
            return nullptr;
        }
        Page *victim_page = page_table_[victim_page_id];
        // 脏页写回
        if (victim_page->IsDirty()) {
            disk_manager_->WritePage(victim_page_id, victim_page->GetData());
        }
        // 从页表移除并复用该帧
        page_table_.erase(victim_page_id);
        victim_page->SetPageId(-1);
        new_page = victim_page;
    }


    // 从磁盘中加载
    new_page->SetPageId(page_id);
    new_page->SetDirty(false);
    // 读取至 new_page 中
    disk_manager_->ReadPage(page_id, new_page->GetData());

    page_table_[page_id] = new_page;
    lru_list_.push_front(page_id);
    return new_page;
}

// 释放一个页的缓冲占用，如果脏了就写回磁盘
bool BufferPoolManager::UnpinPage(int page_id) {
    // 找不到该页，page_id 有误
    if (!page_table_.count(page_id)) return false;
    Page *page = page_table_[page_id];
    if (page->IsDirty()) {
        disk_manager_->WritePage(page_id, page->GetData());
        page->SetDirty(false);
    }

    // 减少引用计数
    page_ref_count_[page_id]--;
    if (page_ref_count_[page_id] <= 0) {
        // 页继续留在缓冲池，在LRU算法中会对其进行淘汰
        page_ref_count_.erase(page_id);
    }

    return true;
}

// 新建页(同时给到page_id)
Page *BufferPoolManager::NewPage(int *page_id) {
    *page_id = disk_manager_->AllocatePage();
    Page *page = FetchPage(*page_id);
    if (page != nullptr) {
        memset(page->GetData(), 0, PAGE_SIZE);
        page->SetDirty(true);
    }
    return page;
}

// 删除页(逻辑与内存上的释放)
bool BufferPoolManager::DeletePage(int page_id) {
    // 检查引用数
    if (page_ref_count_.count(page_id) && page_ref_count_[page_id] > 0) {
        std::cerr << "Warning: trying to delete page with active references" << std::endl;
        return false;
    }

    bool page_existed = false;
    if (page_table_.count(page_id)) {
        Page *page = page_table_[page_id];
        if (page->IsDirty()) {
            disk_manager_->WritePage(page_id, page->GetData());
        }
        page_table_.erase(page_id);
        lru_list_.remove(page_id);
        page->SetPageId(-1);
        page_existed = true;
    }
    
    // 从磁盘释放页（如果页ID有效）
    if (page_id < disk_manager_->GetTotalPages()) {
        disk_manager_->DeallocatePage(page_id);
        return true;
    }
    
    // 只有当页在内存中存在时才返回true
    return page_existed;
}

// 简单 LRU：淘汰链表尾的页
int BufferPoolManager::EvictPage() {
    if (lru_list_.empty()) return -1;

    for (auto it = lru_list_.rbegin(); it != lru_list_.rend(); it++) {
        int page_id = *it;

        // 检查引用数
        if (!page_ref_count_.count(page_id) || page_ref_count_[page_id] <= 0) {
            // 找到可以淘汰的页
            lru_list_.remove(page_id);
            return page_id;
        }
    }
    // 所有页活跃
    return -1;
}

bool BufferPoolManager::FlushPage(int page_id) {
    if (!page_table_.count(page_id)) {
        return false;
    }
    Page *page = page_table_[page_id];
    disk_manager_->WritePage(page_id, page->GetData());
    page->SetDirty(false);
    return true;
}


