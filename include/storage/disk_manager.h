//
// Created by Huang_cj on 2025/9/8.
//

#pragma once
#include <fstream>
#include <set>
#include <string>
#include <vector>

class DiskManager {
public:
    explicit DiskManager(const std::string &db_file);
    ~DiskManager();

    void ReadPage(int page_id, char *page_data);
    void WritePage(int page_id, const char *page_data);

    int AllocatePage(); // 分配新页，返回 page_id
    void DeallocatePage(int page_id);   // 释放页

    int GetTotalPages() const { return num_pages_; }
    int GetFreePages() const { return static_cast<int>(free_list_.size()); }

    bool CompactDatabase(); // 压缩数据库文件，移除末尾的空闲页

private:
    std::fstream db_io_;    // 文件输入输出流
    std::string db_file_;
    int num_pages_; // 文件里已有的页数
    std::vector<int> free_list_;    // 空闲页列表
    int next_page_id_;      // 下个页ID

    void InitializePageIdManagement();

    void TruncateFile(int new_size);
};