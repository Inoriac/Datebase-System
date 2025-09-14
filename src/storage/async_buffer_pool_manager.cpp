//
// Created by Huang_cj on 2025/9/14.
// 异步缓冲池管理器实现
//

#include "storage/async_buffer_pool_manager.h"
#include <iostream>
#include <cstring>

AsyncBufferPoolManager::AsyncBufferPoolManager(size_t pool_size, AsyncDiskManager *disk_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager) {
    pages_ = new Page[pool_size_];
    // 初始化所有页面
    for (size_t i = 0; i < pool_size_; ++i) {
        pages_[i].SetPageId(-1);
        pages_[i].SetDirty(false);
    }
}

AsyncBufferPoolManager::~AsyncBufferPoolManager() {
    // 析构前刷盘所有脏页
    for (auto &kv : page_table_) {
        FlushPage(kv.first);
    }
    delete[] pages_;
}

// 异步接口实现
std::future<Page*> AsyncBufferPoolManager::FetchPageAsync(int page_id) {
    return std::async(std::launch::async, [this, page_id]() {
        return DoFetchPage(page_id);
    });
}

std::future<bool> AsyncBufferPoolManager::UnpinPageAsync(int page_id) {
    return std::async(std::launch::async, [this, page_id]() {
        return DoUnpinPage(page_id);
    });
}

std::future<bool> AsyncBufferPoolManager::FlushPageAsync(int page_id) {
    return std::async(std::launch::async, [this, page_id]() {
        return DoFlushPage(page_id);
    });
}

std::future<Page*> AsyncBufferPoolManager::NewPageAsync(int *page_id) {
    return std::async(std::launch::async, [this, page_id]() {
        return DoNewPage(page_id);
    });
}

std::future<bool> AsyncBufferPoolManager::DeletePageAsync(int page_id) {
    return std::async(std::launch::async, [this, page_id]() {
        return DoDeletePage(page_id);
    });
}

std::future<std::vector<Page*>> AsyncBufferPoolManager::FetchPagesAsync(const std::vector<int>& page_ids) {
    return std::async(std::launch::async, [this, page_ids]() {
        std::vector<Page*> result;
        result.reserve(page_ids.size());
        
        for (int page_id : page_ids) {
            Page* page = DoFetchPage(page_id);
            result.push_back(page);
        }
        
        return result;
    });
}

std::future<bool> AsyncBufferPoolManager::UnpinPagesAsync(const std::vector<int>& page_ids) {
    return std::async(std::launch::async, [this, page_ids]() {
        bool all_success = true;
        for (int page_id : page_ids) {
            if (!DoUnpinPage(page_id)) {
                all_success = false;
            }
        }
        return all_success;
    });
}

// 预取操作
void AsyncBufferPoolManager::PrefetchPage(int page_id) {
    // 异步预取，不等待结果
    FetchPageAsync(page_id);
}

void AsyncBufferPoolManager::PrefetchPages(const std::vector<int>& page_ids) {
    // 异步预取多个页面
    FetchPagesAsync(page_ids);
}

// 同步接口实现（保持兼容性）
Page* AsyncBufferPoolManager::FetchPage(int page_id) {
    return DoFetchPage(page_id);
}

bool AsyncBufferPoolManager::UnpinPage(int page_id) {
    return DoUnpinPage(page_id);
}

bool AsyncBufferPoolManager::FlushPage(int page_id) {
    return DoFlushPage(page_id);
}

Page* AsyncBufferPoolManager::NewPage(int *page_id) {
    return DoNewPage(page_id);
}

bool AsyncBufferPoolManager::DeletePage(int page_id) {
    return DoDeletePage(page_id);
}

int AsyncBufferPoolManager::EvictPage() {
    if (lru_list_.empty()) return -1;
    
    int victim_page_id = lru_list_.back();
    lru_list_.pop_back();
    return victim_page_id;
}

// 内部实现方法
Page* AsyncBufferPoolManager::DoFetchPage(int page_id) {
    if (page_id >= disk_manager_->GetTotalPages()) {
        return nullptr;
    }

    // 缓存命中
    if (page_table_.count(page_id)) {
        Page *page = page_table_[page_id];
        lru_list_.remove(page_id);
        lru_list_.push_front(page_id);
        page_ref_count_[page_id]++;
        return page;
    }

    // 不在缓存中，尝试找到空闲帧
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
            std::cerr << "AsyncBufferPoolManager: no victim page to evict!" << std::endl;
            return nullptr;
        }
        Page *victim_page = page_table_[victim_page_id];
        if (victim_page->IsDirty()) {
            disk_manager_->WritePage(victim_page_id, victim_page->GetData());
        }
        page_table_.erase(victim_page_id);
        victim_page->SetPageId(-1);
        new_page = victim_page;
    }

    if (page_id >= disk_manager_->GetTotalPages()) {
        new_page->SetPageId(-1);
        return nullptr;
    }

    // 从磁盘中加载
    new_page->SetPageId(page_id);
    new_page->SetDirty(false);
    disk_manager_->ReadPage(page_id, new_page->GetData());

    page_table_[page_id] = new_page;
    lru_list_.push_front(page_id);
    return new_page;
}

bool AsyncBufferPoolManager::DoUnpinPage(int page_id) {
    if (!page_table_.count(page_id)) return false;
    
    Page *page = page_table_[page_id];
    if (page->IsDirty()) {
        disk_manager_->WritePage(page_id, page->GetData());
        page->SetDirty(false);
    }

    page_ref_count_[page_id]--;
    if (page_ref_count_[page_id] <= 0) {
        page_ref_count_.erase(page_id);
    }

    return true;
}

bool AsyncBufferPoolManager::DoFlushPage(int page_id) {
    if (!page_table_.count(page_id)) return false;
    
    Page *page = page_table_[page_id];
    if (page->IsDirty()) {
        disk_manager_->WritePage(page_id, page->GetData());
        page->SetDirty(false);
    }
    return true;
}

Page* AsyncBufferPoolManager::DoNewPage(int *page_id) {
    *page_id = disk_manager_->AllocatePage();
    if (*page_id == -1) return nullptr;

    Page *new_page = nullptr;
    for (size_t i = 0; i < pool_size_; i++) {
        if (pages_[i].GetPageId() == -1) {
            new_page = &pages_[i];
            break;
        }
    }
    
    if (new_page == nullptr) {
        int victim_page_id = EvictPage();
        if (victim_page_id == -1) {
            disk_manager_->DeallocatePage(*page_id);
            return nullptr;
        }
        Page *victim_page = page_table_[victim_page_id];
        if (victim_page->IsDirty()) {
            disk_manager_->WritePage(victim_page_id, victim_page->GetData());
        }
        page_table_.erase(victim_page_id);
        victim_page->SetPageId(-1);
        new_page = victim_page;
    }

    new_page->SetPageId(*page_id);
    new_page->SetDirty(false);
    std::memset(new_page->GetData(), 0, PAGE_SIZE);

    page_table_[*page_id] = new_page;
    lru_list_.push_front(*page_id);
    page_ref_count_[*page_id] = 1;

    return new_page;
}

bool AsyncBufferPoolManager::DoDeletePage(int page_id) {
    if (!page_table_.count(page_id)) {
        disk_manager_->DeallocatePage(page_id);
        return true;
    }

    Page *page = page_table_[page_id];
    if (page->IsDirty()) {
        disk_manager_->WritePage(page_id, page->GetData());
    }

    page_table_.erase(page_id);
    lru_list_.remove(page_id);
    page_ref_count_.erase(page_id);
    page->SetPageId(-1);

    disk_manager_->DeallocatePage(page_id);
    return true;
}
