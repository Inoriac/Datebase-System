#include "../../include/execution/executor.h"
#include "symbol_table.h" // 需要访问 catalog
#include "log/log_config.h"
#include "catalog/table_manager.h"
#include "catalog/table_schema_manager.h"
#include <iostream>
#include <stdexcept>

// 实现每个算子的 next() 方法

// CreateTableOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> CreateTableOperator::next() {
    // 使用实际的TableManager而不是模拟的catalog
    TableSchema schema(table_name);
    for (const auto& col : columns) {
        DataType dt = (col.second == "INT" || col.second == "INTEGER") ? DataType::Int : DataType::Varchar;
        int len = (dt == DataType::Varchar) ? 255 : 4;
        schema.AddColumn(Column(col.first, dt, len, true));
    }
    schema.primary_key_index_ = 0;
    
    // 调用实际的TableManager
    bool success = table_manager_->CreateTable(schema);
    if (!success) {
        throw std::runtime_error("Failed to create table: " + table_name);
    }
    
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    logger->Info("Table '{}' created successfully", table_name);
    return nullptr; // 没有数据行返回
}

// InsertOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> InsertOperator::next() {
    // 检查表是否存在
    if (!table_manager_->TableExists(table_name)) {
        throw std::runtime_error("Table '" + table_name + "' does not exist.");
    }
    
    // 构建Record
    Record rec;
    for (const auto& value : values) {
        rec.AddValue(value);
    }
    
    // 调用实际的TableManager
    bool success = table_manager_->InsertRecord(table_name, rec);
    if (!success) {
        throw std::runtime_error("Failed to insert into table: " + table_name);
    }
    
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    logger->Info("Values inserted into table '{}' successfully", table_name);
    return nullptr;
}

// SeqScanOperator (迭代模型)
std::unique_ptr<Tuple> SeqScanOperator::next() {
    // 使用实际的TableManager进行数据扫描
    if (current_row_index == 0) {
        // 首次调用，获取所有记录
        std::vector<Record> records = table_manager_->SelectRecords(table_name);
        all_records_.clear();
        
        // 将Record转换为Tuple
        for (const auto& record : records) {
            Tuple tuple;
            for (const auto& value : record.values_) {
                tuple.push_back(value);
            }
            all_records_.push_back(tuple);
        }
    }
    
    // 如果没有更多数据，返回空指针
    if (current_row_index >= all_records_.size()) {
        return nullptr;
    }
    
    // 返回当前行数据的副本，并移动到下一行
    auto result_tuple = std::make_unique<Tuple>(all_records_[current_row_index]);
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
SeqScanOperator::SeqScanOperator(const std::string& table_name, TableManager* table_manager)
    : table_name(table_name), table_manager_(table_manager) {
    type = SEQ_SCAN_OP;
    current_row_index = 0; // 初始化
}
ProjectOperator::ProjectOperator(std::unique_ptr<Operator>&& child, const std::vector<std::string>& columns)
    : columns(columns) { // `columns` 是一个复制，所以 const 引用没问题
    this->child = std::move(child);
    type = PROJECT_OP;
}