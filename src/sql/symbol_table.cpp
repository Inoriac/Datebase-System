#include "symbol_table.h"

std::unordered_map<std::string, TableInfo> symbol_table;

bool tableExists(const std::string& table_name) {
    return symbol_table.count(table_name) > 0;
}