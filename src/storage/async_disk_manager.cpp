//
// Created by Huang_cj on 2025/9/14.
// 异步磁盘管理器实现
//

#include "storage/async_disk_manager.h"
#include "../../include/catalog/page.h"
#include <iostream>
#include <algorithm>
#include <cstring>

AsyncDiskManager::AsyncDiskManager(const std::string &db_file, size_t worker_threads) 
    : db_file_(db_file), num_pages_(0), next_page_id_(0), should_stop_(false) {
    
    // 打开数据库文件
    db_io_.open(db_file_, std::ios::in | std::ios::out | std::ios::binary);
    if (!db_io_.is_open()) {
        // 如果文件不存在，就创建新文件
        db_io_.open(db_file_, std::ios::out | std::ios::binary);
        db_io_.close();
        db_io_.open(db_file_, std::ios::in | std::ios::out | std::ios::binary);
    }
    
    db_io_.seekg(0, std::ios::end);
    num_pages_ = db_io_.tellg() / PAGE_SIZE;
    
    InitializePageIdManagement();
    
    // 启动工作线程
    Start();
}

AsyncDiskManager::~AsyncDiskManager() {
    Stop();
    db_io_.close();
}

void AsyncDiskManager::InitializePageIdManagement() {
    if (num_pages_ == 0) {
        next_page_id_ = 0;
        free_list_.clear();
    } else {
        next_page_id_ = num_pages_;
        free_list_.clear();
    }
}

void AsyncDiskManager::Start() {
    should_stop_ = false;
    // 启动工作线程（默认2个）
    for (size_t i = 0; i < 2; ++i) {
        worker_threads_.emplace_back(&AsyncDiskManager::WorkerThread, this);
    }
}

void AsyncDiskManager::Stop() {
    should_stop_ = true;
    queue_cv_.notify_all();
    
    for (auto& thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    worker_threads_.clear();
}

void AsyncDiskManager::WorkerThread() {
    while (!should_stop_) {
        std::unique_ptr<IOTask> task;
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_cv_.wait(lock, [this] { return !task_queue_.empty() || should_stop_; });
            
            if (should_stop_ && task_queue_.empty()) {
                break;
            }
            
            if (!task_queue_.empty()) {
                task = std::move(task_queue_.front());
                task_queue_.pop();
            }
        }
        
        if (task) {
            ProcessTask(std::move(task));
        }
    }
}

void AsyncDiskManager::ProcessTask(std::unique_ptr<IOTask> task) {
    bool success = false;
    
    try {
        if (task->operation == IOOperation::READ) {
            if (task->page_id >= num_pages_) {
                std::cout << "Invalid page read: " << task->page_id << std::endl;
                memset(task->page_data, 0, PAGE_SIZE);
                success = true;
            } else if (IsPageFree(task->page_id)) {
                std::cout << "Attempted to read deleted page: " << task->page_id << std::endl;
                memset(task->page_data, 0, PAGE_SIZE);
                success = true;
            } else {
                db_io_.clear();
                db_io_.seekg(task->page_id * PAGE_SIZE, std::ios::beg);
                db_io_.read(task->page_data, PAGE_SIZE);
                success = db_io_.good();
            }
        } else if (task->operation == IOOperation::WRITE) {
            if (task->page_id >= num_pages_) {
                num_pages_ = task->page_id + 1;
            }
            db_io_.clear();
            db_io_.seekp(task->page_id * PAGE_SIZE, std::ios::beg);
            db_io_.write(task->page_data, PAGE_SIZE);
            db_io_.flush();
            success = db_io_.good();
        }
    } catch (const std::exception& e) {
        std::cerr << "IO Error: " << e.what() << std::endl;
        success = false;
    }
    
    task->promise.set_value(success);
}

std::future<bool> AsyncDiskManager::ReadPageAsync(int page_id, char *page_data) {
    auto task = std::make_unique<IOTask>(IOOperation::READ, page_id, page_data);
    auto future = task->promise.get_future();
    
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        task_queue_.push(std::move(task));
    }
    queue_cv_.notify_one();
    
    return future;
}

std::future<bool> AsyncDiskManager::WritePageAsync(int page_id, const char *page_data) {
    // 需要复制数据，因为原始数据可能在任务执行前被修改
    char* data_copy = new char[PAGE_SIZE];
    memcpy(data_copy, page_data, PAGE_SIZE);
    
    auto task = std::make_unique<IOTask>(IOOperation::WRITE, page_id, data_copy);
    auto future = task->promise.get_future();
    
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        task_queue_.push(std::move(task));
    }
    queue_cv_.notify_one();
    
    return future;
}

std::future<bool> AsyncDiskManager::ReadPagesAsync(const std::vector<int>& page_ids, 
                                                  std::vector<std::unique_ptr<char[]>>& page_data) {
    return std::async(std::launch::async, [this, &page_ids, &page_data]() {
        bool all_success = true;
        for (size_t i = 0; i < page_ids.size(); ++i) {
            ReadPage(page_ids[i], page_data[i].get());
            // 这里可以添加错误检查
        }
        return all_success;
    });
}

std::future<bool> AsyncDiskManager::WritePagesAsync(const std::vector<int>& page_ids, 
                                                   const std::vector<const char*>& page_data) {
    return std::async(std::launch::async, [this, &page_ids, &page_data]() {
        bool all_success = true;
        for (size_t i = 0; i < page_ids.size(); ++i) {
            WritePage(page_ids[i], page_data[i]);
            // 这里可以添加错误检查
        }
        return all_success;
    });
}

// 同步接口（保持兼容性）
void AsyncDiskManager::ReadPage(int page_id, char *page_data) {
    if (page_id >= num_pages_) {
        std::cout << "Invalid page read: " << page_id << std::endl;
        memset(page_data, 0, PAGE_SIZE);
        return;
    }
    
    if (IsPageFree(page_id)) {
        std::cout << "Attempted to read deleted page: " << page_id << std::endl;
        memset(page_data, 0, PAGE_SIZE);
        return;
    }
    
    db_io_.clear();
    db_io_.seekg(page_id * PAGE_SIZE, std::ios::beg);
    db_io_.read(page_data, PAGE_SIZE);
}

void AsyncDiskManager::WritePage(int page_id, const char *page_data) {
    if (page_id >= num_pages_) {
        num_pages_ = page_id + 1;
    }
    db_io_.clear();
    db_io_.seekp(page_id * PAGE_SIZE, std::ios::beg);
    db_io_.write(page_data, PAGE_SIZE);
    db_io_.flush();
}

int AsyncDiskManager::AllocatePage() {
    if (!free_list_.empty()) {
        int page_id = free_list_.back();
        free_list_.pop_back();
        return page_id;
    }
    int page_id = num_pages_++;
    // 确保新分配的页面在文件中存在
    if (page_id > 0) {
        // 扩展文件到新页面大小
        db_io_.seekp(page_id * PAGE_SIZE - 1, std::ios::beg);
        db_io_.write("\0", 1);
        db_io_.flush();
    }
    return page_id;
}

void AsyncDiskManager::DeallocatePage(int page_id) {
    if (page_id < num_pages_) {
        free_list_.push_back(page_id);
    } else {
        std::cerr << "Warning: trying to deallocate invalid page_id " << page_id << std::endl;
    }
}

bool AsyncDiskManager::IsPageFree(int page_id) const {
    return std::find(free_list_.begin(), free_list_.end(), page_id) != free_list_.end();
}

void AsyncDiskManager::FlushAll() {
    // 等待所有任务完成
    std::unique_lock<std::mutex> lock(queue_mutex_);
    queue_cv_.wait(lock, [this] { return task_queue_.empty(); });
    
    // 强制刷新文件
    db_io_.flush();
}

bool AsyncDiskManager::CompactDatabase() {
    // 等待所有任务完成
    FlushAll();
    
    // 简单的压缩实现：移除末尾的空闲页
    int new_size = 0;
    for (int i = 0; i < num_pages_; ++i) {
        if (!IsPageFree(i)) {
            new_size = i + 1;
        }
    }
    
    if (new_size < num_pages_) {
        TruncateFile(new_size * PAGE_SIZE);
        num_pages_ = new_size;
        return true;
    }
    return false;
}

void AsyncDiskManager::TruncateFile(int new_size) {
    // 简单的文件截断实现
    db_io_.close();
    std::ofstream truncate_file(db_file_, std::ios::binary | std::ios::trunc);
    truncate_file.close();
    
    // 重新打开文件
    db_io_.open(db_file_, std::ios::in | std::ios::out | std::ios::binary);
}
