#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>

// 列的元数据
struct ColumnInfo {
    std::string name;
    std::string type; // 例如 "INT", "STRING"
};

// 表的元数据
struct TableInfo {
    std::string name;
    std::unordered_map<std::string, ColumnInfo> columns; // 按名称快速查找列
    std::vector<std::string> column_order; // 记录列的原始顺序
};

// 全局符号表
extern std::unordered_map<std::string, TableInfo> symbol_table;

// 检查表是否存在的函数
bool tableExists(const std::string& table_name);

#endif