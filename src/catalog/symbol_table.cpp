#include "symbol_table.h"

// 全局 Catalog 对象的定义
Catalog catalog;

// Catalog 类成员函数的实现
void Catalog::addTable(const TableInfo& table) {
    tables[table.name] = table;
}

bool Catalog::tableExists(const std::string& name) const {
    return tables.count(name) > 0;
}

const TableInfo& Catalog::getTable(const std::string& name) const {
    if (!tableExists(name)) {
        throw std::runtime_error("Table '" + name + "' does not exist.");
    }
    return tables.at(name);
}