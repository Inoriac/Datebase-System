#include "executor.h"
#include "symbol_table.h" // 需要访问 catalog
#include "log/log_config.h"
#include "utils.h" // 用于求值
#include <iostream>
#include <stdexcept>

// 全局的内存数据存储
std::unordered_map<std::string, std::vector<Tuple>> in_memory_data;

// 实现每个算子的 next() 方法

// CreateTableOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> CreateTableOperator::next() {
    TableInfo new_table;
    new_table.name = table_name;
    for (const auto& col : columns) {
        new_table.columns[col.first] = {col.first, col.second};
        new_table.column_order.push_back(col.first);
    }
    catalog.addTable(new_table);
    // 为新表创建内存数据存储
    in_memory_data[table_name] = {};
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    logger->Info("Table '{}' created successfully", table_name);
    return nullptr; // 没有数据行返回
}

// InsertOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> InsertOperator::next() {
    if (!catalog.tableExists(table_name)) {
        throw std::runtime_error("Table '" + table_name + "' does not exist.");
    }
    // 将值插入到对应的内存表中
    in_memory_data[table_name].push_back(values);
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    logger->Info("Values inserted into table '{}' successfully", table_name);
    return nullptr;
}

// SeqScanOperator (迭代模型)
std::unique_ptr<Tuple> SeqScanOperator::next() {
    if (current_row_index >= in_memory_data[table_name].size()) {
        return nullptr;
    }
    // 返回当前行数据的副本，并移动到下一行
    auto result_tuple = std::make_unique<Tuple>(in_memory_data[table_name][current_row_index]);
    current_row_index++;
    return result_tuple;
}

// FilterOperator (迭代模型)
std::unique_ptr<Tuple> FilterOperator::next() {
    std::unique_ptr<Tuple> current_tuple = nullptr;
    do {
        current_tuple = child->next();
        if (current_tuple == nullptr) {
            return nullptr; // 没有更多数据了
        }
        // 使用通用的求值函数来评估 WHERE 条件
        if (evaluateExpression(condition_node, *current_tuple, tables)) {
            return current_tuple;
        }
    } while (current_tuple != nullptr);
    
    return nullptr;
}

// ProjectOperator (迭代模型)
std::unique_ptr<Tuple> ProjectOperator::next() {
    std::unique_ptr<Tuple> input_tuple = child->next();
    if (input_tuple == nullptr) {
        return nullptr; // 没有更多数据了
    }
    
    const TableInfo& table_info = *tables.at(table_name);
    auto result_tuple = std::make_unique<Tuple>();
    
    for (const auto& col_name : columns) {
        // 查找列在原始元组中的索引
        int index = -1;
        for (size_t i = 0; i < table_info.column_order.size(); ++i) {
            if (table_info.column_order[i] == col_name) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            result_tuple->push_back((*input_tuple)[index]);
        }
    }
    
    return result_tuple;
}

// UpdateOperator (非迭代模型)
std::unique_ptr<Tuple> UpdateOperator::next() {
    // 获取需要更新的元组
    std::vector<Tuple> tuples_to_update;
    std::unique_ptr<Tuple> current_tuple;
    while ((current_tuple = child->next()) != nullptr) {
        tuples_to_update.push_back(*current_tuple);
    }
    
    // 更新内存中的数据
    std::vector<Tuple>& table_data = in_memory_data[table_name];
    
    for (const auto& tuple_to_update : tuples_to_update) {
        for (auto& stored_tuple : table_data) {
            // 找到需要更新的元组（这里简化为通过主键id查找）
            if (std::get<int>(stored_tuple[0]) == std::get<int>(tuple_to_update[0])) {
                // 更新元组中的值
                for (const auto& update_pair : updates) {
                    const std::string& col_name = update_pair.first;
                    ASTNode* value_node = update_pair.second;
                    
                    const TableInfo& table_info = catalog.getTable(table_name);
                    for (size_t i = 0; i < table_info.column_order.size(); ++i) {
                        if (table_info.column_order[i] == col_name) {
                            stored_tuple[i] = evaluateLiteral(value_node);
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    logger->Info("{} rows updated in table '{}'", tuples_to_update.size(), table_name);
    return nullptr;
}

// Executor 类实现
std::vector<Tuple> Executor::execute() {
    std::vector<Tuple> results;
    if (!plan_root_) {
        return results;
    }
    
    // 对于 CREATE TABLE, INSERT, UPDATE，直接调用 next() 一次即可
    if (plan_root_->type == CREATE_TABLE_OP || plan_root_->type == INSERT_OP || plan_root_->type == UPDATE_OP) {
        plan_root_->next();
        return results;
    }
    
    // 对于 SELECT, DELETE，循环调用 next() 直到没有更多数据
    while (true) {
        std::unique_ptr<Tuple> tuple = plan_root_->next();
        if (tuple == nullptr) {
            break;
        }
        results.push_back(*tuple);
    }
    
    return results;
}

// 构造函数
SeqScanOperator::SeqScanOperator(const std::string& table_name)
    : table_name(table_name) {
    type = SEQ_SCAN_OP;
    current_row_index = 0; // 初始化
}
ProjectOperator::ProjectOperator(std::unique_ptr<Operator>&& child, const std::string& table_name, const std::vector<std::string>& columns, const std::unordered_map<std::string, const TableInfo*>& tables)
    : table_name(table_name), columns(columns), tables(tables) {
    this->child = std::move(child);
    type = PROJECT_OP;
}
FilterOperator::FilterOperator(std::unique_ptr<Operator>&& child, ASTNode* condition_node, const std::unordered_map<std::string, const TableInfo*>& tables)
    : condition_node(condition_node), tables(tables) {
    this->child = std::move(child);
    type = FILTER_OP;
}
UpdateOperator::UpdateOperator(std::unique_ptr<Operator>&& child, const std::string& table_name, const std::vector<std::pair<std::string, ASTNode*>>& updates)
    : table_name(table_name), updates(updates) {
    this->child = std::move(child);
    type = UPDATE_OP;
}