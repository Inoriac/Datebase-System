#include "executor.h"
#include "symbol_table.h" // 需要访问 catalog
#include "log/log_config.h"
#include <iostream>
#include <stdexcept>

// 实现每个算子的 next() 方法

// CreateTableOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> CreateTableOperator::next() {
    // 实际的建表逻辑
    TableInfo new_table;
    new_table.name = table_name;
    for (const auto& col : columns) {
        new_table.columns[col.first] = {col.first, col.second};
        new_table.column_order.push_back(col.first);
    }
    catalog.addTable(new_table);
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    logger->Info("Table '{}' created successfully", table_name);
    return nullptr; // 没有数据行返回
}

// InsertOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> InsertOperator::next() {
    // 实际的插入逻辑
    if (!catalog.tableExists(table_name)) {
        throw std::runtime_error("Table '" + table_name + "' does not exist.");
    }
    // TODO: 这里需要一个地方来存储实际的数据，例如一个简单的内存表
    // catalog.insertData(table_name, values);
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    logger->Info("Values inserted into table '{}' successfully", table_name);
    return nullptr;
}

// SeqScanOperator (迭代模型)
std::unique_ptr<Tuple> SeqScanOperator::next() {
    // 这是一个简化版本，假设我们有实际数据
    // TODO: 你需要一个实际的数据存储来代替这个模拟
    
    // 模拟数据
    static const std::vector<Tuple> users_data = {
        {101, std::string("Alice")},
        {102, std::string("Bob")},
        {103, std::string("Charlie")}
    };
    
    // 如果没有更多数据，返回空指针
    if (current_row_index >= users_data.size()) {
        return nullptr;
    }
    
    // 返回当前行数据的副本，并移动到下一行
    auto result_tuple = std::make_unique<Tuple>(users_data[current_row_index]);
    current_row_index++;
    return result_tuple;
}

// FilterOperator (迭代模型)
std::unique_ptr<Tuple> FilterOperator::next() {
    // 从子算子获取数据，直到找到匹配的
    std::unique_ptr<Tuple> current_tuple = nullptr;
    do {
        current_tuple = child->next();
        if (current_tuple == nullptr) {
            return nullptr; // 没有更多数据了
        }
        // TODO: 这里需要实现 WHERE 子句的求值逻辑
        // 简化版本：假设条件 id > 102
        if (std::get<int>((*current_tuple)[0]) > 102) {
            return current_tuple;
        }
    } while (current_tuple != nullptr);
    
    return nullptr;
}

// ProjectOperator (迭代模型)
std::unique_ptr<Tuple> ProjectOperator::next() {
    // 从子算子获取一行完整数据
    std::unique_ptr<Tuple> input_tuple = child->next();
    if (input_tuple == nullptr) {
        return nullptr; // 没有更多数据了
    }
    
    // 根据指定的列（this->columns）创建新的元组
    auto result_tuple = std::make_unique<Tuple>();
    // 这是一个简化版本，假设我们总是投影 id 和 name
    result_tuple->push_back((*input_tuple)[0]); // id
    result_tuple->push_back((*input_tuple)[1]); // name
    
    return result_tuple;
}

// Executor 类实现
std::vector<Tuple> Executor::execute() {
    std::vector<Tuple> results;
    if (!plan_root_) {
        return results;
    }
    
    // 如果是 CREATE TABLE 或 INSERT，直接调用 next() 一次即可
    if (plan_root_->type == CREATE_TABLE_OP || plan_root_->type == INSERT_OP) {
        plan_root_->next();
        return results;
    }
    
    // 如果是 SELECT 或 DELETE，循环调用 next() 直到没有更多数据
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
ProjectOperator::ProjectOperator(std::unique_ptr<Operator>&& child, const std::vector<std::string>& columns)
    : columns(columns) { // `columns` 是一个复制，所以 const 引用没问题
    this->child = std::move(child);
    type = PROJECT_OP;
}