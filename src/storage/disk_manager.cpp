//
// Created by Huang_cj on 2025/9/8.
//

#include "storage/disk_manager.h"
#include "storage/page.h"
#include <iostream>
#include <algorithm>

DiskManager::DiskManager(const std::string &db_file) : db_file_(db_file), num_pages_(0) {
    db_io_.open(db_file_, std::ios::in | std::ios::out | std::ios::binary);
    if (!db_io_.is_open()) {
        // 如果文件不存在，就创建新文件
        db_io_.open(db_file_, std::ios::out | std::ios::binary);
        db_io_.close();
        db_io_.open(db_file_, std::ios::in | std::ios::out | std::ios::binary);
    }
    db_io_.seekg(0, std::ios::end); // 将读指针移动至文件末尾(参考点文件末尾，偏移量为0)
    num_pages_ = db_io_.tellg() / PAGE_SIZE;

    InitializePageIdManagement();
}

DiskManager::~DiskManager() {
    db_io_.close();
}

void DiskManager::InitializePageIdManagement() {
    if (num_pages_ == 0) {
        // 新数据库，从0开始分配页ID
        next_page_id_ = 0;
        free_list_.clear();
    } else {
        // 现有数据库，从当前页数开始分配
        next_page_id_ = num_pages_;
        // TODO: 可能需要修改，新增标记位
        // 简化：假设所有现有页都是有效的，不加载删除信息
        free_list_.clear();
    }
}

// TODO: 这里可能需要加入异步IO
void DiskManager::ReadPage(int page_id, char *page_data) {
    if (page_id >= num_pages_) {
        std::cout << "Invalid page read: " << page_id << std::endl;
        memset(page_data, 0, PAGE_SIZE);    // 初始化空页
        return;
    }
    
    // 检查页面是否已被删除
    if (IsPageFree(page_id)) {
        std::cout << "Attempted to read deleted page: " << page_id << std::endl;
        memset(page_data, 0, PAGE_SIZE);    // 初始化空页
        return;
    }
    
    // 读取 page_id 页的内容至 page_data 指向的内存
    db_io_.clear();
    db_io_.seekg(page_id * PAGE_SIZE, std::ios::beg);
    db_io_.read(page_data, PAGE_SIZE);  // 从起始位置开始，往后读 PAGE_SIZE 个字节的内容，放到 page_data 指向的内存中
}

// TODO: 这里可能需要加入异步IO
void DiskManager::WritePage(int page_id, const char *page_data) {
    if (page_id >= num_pages_) {
        num_pages_ = page_id + 1;
    }
    db_io_.clear();
    db_io_.seekp(page_id * PAGE_SIZE, std::ios::beg);   // 移动写指针
    db_io_.write(page_data, PAGE_SIZE);
    db_io_.flush();     // 将缓冲区的数据强制写入磁盘
}

int DiskManager::AllocatePage() {
    if (!free_list_.empty()) {
        int page_id = free_list_.back();
        free_list_.pop_back();
        return page_id;
    }
    return num_pages_++; // 没有空闲页就追加
}

// 将 page_id 对应的页放回到空闲列表
void DiskManager::DeallocatePage(int page_id) {
    if (page_id < num_pages_) {
        free_list_.push_back(page_id);
    } else {
        std::cerr << "Warning: trying to deallocate invalid page_id " << page_id << std:: endl;
    }
}

// 检查页面是否已被删除
bool DiskManager::IsPageFree(int page_id) const {
    if (page_id < 0 || page_id >= num_pages_) {
        return true; // 无效页ID视为已删除
    }
    return std::find(free_list_.begin(), free_list_.end(), page_id) != free_list_.end();
}

bool DiskManager::CompactDatabase() {
    // 找到最大的有效页ID
    int max_valid_page = -1;
    for (int i = 0; i < num_pages_; i++) {
        bool is_free = std::find(free_list_.begin(), free_list_.end(), i) != free_list_.end();
        if (!is_free) {
            max_valid_page = i;
        }
    }

    if (max_valid_page >= 0 && max_valid_page < num_pages_) {
        // 截断文件
        TruncateFile(max_valid_page + 1);
        num_pages_ = max_valid_page + 1;
        next_page_id_ = num_pages_;

        // 清理 free_list 中超出文件大小的页ID (通过 remove_if遍历list所有元素)
        free_list_.erase(
            std::remove_if(free_list_.begin(), free_list_.end(),
                [this](int page_id) { return page_id >= next_page_id_; }),
                free_list_.end()
        );

        return true;
    }
    return false;
}

void DiskManager::TruncateFile(int new_size) {
    // 关闭当前文件流
    db_io_.close();

    // 创建临时文件
    std::string temp_file = db_file_ + ".tmp";
    std::ofstream temp(temp_file, std::ios::binary);

    // 复制前 new_size 页到临时文件
    std::ifstream src(db_file_, std::ios::binary);
    for (int i = 0; i < new_size; ++i) {
        char page_data[PAGE_SIZE];
        src.read(page_data, PAGE_SIZE);
        temp.write(page_data, PAGE_SIZE);
    }
    src.close();
    temp.close();

    // 替换原文件
    std::remove(db_file_.c_str());
    std::rename(temp_file.c_str(), db_file_.c_str());

    // 重新打开文件流
    db_io_.open(db_file_, std::ios::in | std::ios::out | std::ios::binary);
}




