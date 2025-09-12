#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "table_info.h" // 包含 TableInfo 和 ColumnInfo 的定义
#include <stdexcept>

// 模式目录类
class Catalog {
public:
    void addTable(const TableInfo& table);
    bool tableExists(const std::string& name) const;
    const TableInfo& getTable(const std::string& name) const;
private:
    std::unordered_map<std::string, TableInfo> tables;
};

// 全局 Catalog 对象的声明
extern Catalog catalog;

#endif