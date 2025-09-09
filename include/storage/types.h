//
// Created by Huang_cj on 2025/9/9.
//

#pragma once
#include <string>
#include <vector>
#include <variant>

// 支持的数据类型
enum class DataType {
    Int,
    Varchar,
    Bool
};

// 列定义
struct Column {
    std::string column_name_;   // 列名
    DataType type_;      // 数据类型
    int length_;         // 最大长度
    bool nullable_;      // 是否允许null

    Column(const std::string& n, DataType t, int len = 0, bool null =true)
        : column_name_(n), type_(t), length_(len), nullable_(null) {}
};

// 表结构定义
struct TableSchema {
    std::string table_name_;     // 表名
    std::vector<Column> columns_;
    int primary_key_index_;      // 主键索引,-1表示无主键

    TableSchema(const std::string& name)
        : table_name_(name), primary_key_index_(-1) {}

    // 添加列
    void AddColumn(const Column& column) {
        columns_.push_back(column);
    }

    // 获取列索引
    int GetColumnIndex(const std::string& col_name) const {
        for (size_t i = 0; i< columns_.size(); ++i) {
            if (columns_[i].column_name_ == col_name) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    // 获取总列数
    size_t GetColumnCount() const {
        return columns_.size();
    }
};

// 记录值(使用 variant 存储不同类型的值)
using Value = std::variant<int, std::string, bool>;

// 记录定义
struct Record {
    std::vector<Value> values_;
    bool is_deleted_;

    Record() : is_deleted_(false) {}

    // 添加值
    void AddValue(const Value& value) {
        values_.push_back(value);
    }

    // 获取值
    Value GetValue(int index) const {
        if (index >= 0 && index < static_cast<int>(values_.size()) ) {
            return values_[index];
        }
        // 返回默认值
        return Value{};
    }

    // 设置值
    void SetValue(int index, const Value& value) {
        if (index >= 0 && index < static_cast<int>(values_.size()) ) {
            values_[index] = value;
        }
    }

    // 获取值数量
    size_t GetValueCount() const {
        return values_.size();
    }
};
