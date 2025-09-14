//
// Created by Huang_cj on 2025/9/8.
// 异步磁盘管理器 - 支持异步IO操作
//

#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>

// IO操作类型
enum class IOOperation {
    READ,
    WRITE
};

// IO任务结构
struct IOTask {
    IOOperation operation;
    int page_id;
    char* page_data;
    std::promise<bool> promise;
    
    IOTask(IOOperation op, int pid, char* data) 
        : operation(op), page_id(pid), page_data(data) {}
};

// 异步磁盘管理器 - 完全兼容DiskManager接口
class AsyncDiskManager {
public:
    explicit AsyncDiskManager(const std::string &db_file, size_t worker_threads = 2);
    ~AsyncDiskManager();
    
    // 同步接口（完全兼容原DiskManager）
    void ReadPage(int page_id, char *page_data);
    void WritePage(int page_id, const char *page_data);
    int AllocatePage();
    void DeallocatePage(int page_id);
    int GetTotalPages() const { return num_pages_; }
    int GetFreePages() const { return static_cast<int>(free_list_.size()); }
    bool IsPageFree(int page_id) const;
    bool CompactDatabase(); // 添加缺失的方法
    
    // 异步读写接口（额外功能）
    std::future<bool> ReadPageAsync(int page_id, char *page_data);
    std::future<bool> WritePageAsync(int page_id, const char *page_data);
    
    // 批量异步操作
    std::future<bool> ReadPagesAsync(const std::vector<int>& page_ids, 
                                   std::vector<std::unique_ptr<char[]>>& page_data);
    std::future<bool> WritePagesAsync(const std::vector<int>& page_ids, 
                                    const std::vector<const char*>& page_data);
    
    // 控制接口
    void Start();
    void Stop();
    void FlushAll(); // 强制刷新所有待写入数据
    
private:
    std::string db_file_;
    std::fstream db_io_;
    int num_pages_;
    std::vector<int> free_list_;
    int next_page_id_;
    
    // 异步IO相关
    std::vector<std::thread> worker_threads_;
    std::queue<std::unique_ptr<IOTask>> task_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::atomic<bool> should_stop_;
    
    // 批量操作优化
    std::mutex batch_mutex_;
    std::vector<std::unique_ptr<IOTask>> pending_tasks_;
    
    void InitializePageIdManagement();
    void WorkerThread();
    void ProcessTask(std::unique_ptr<IOTask> task);
    void TruncateFile(int new_size);
};
