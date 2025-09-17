//
// Created by Huang_cj on 2025/9/8.
//

#pragma once
#include <cstring>
#include <cstdint>

constexpr int PAGE_SIZE = 4096;

class Page {
public:
    Page() : page_id_(-1), is_dirty_(false) {
        // 不在构造函数中初始化data_，减少内存分配开销
        // memset(data_ , 0, PAGE_SIZE);
    }

    inline char* GetData() {
        return data_;
    }

    inline int GetPageId() const {
        return page_id_;
    }

    inline void SetPageId(int page_id) {
        page_id_ = page_id;
    }

    inline bool IsDirty() const {
        return is_dirty_;
    }

    inline void SetDirty(bool dirty) {
        is_dirty_ = dirty;
    }

private:
    int page_id_;       // 页号
    bool is_dirty_;     // 是否被修改
    char data_[PAGE_SIZE]; // 页数据
};