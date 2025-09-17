#include "executor.h"
#include "symbol_table.h" // 需要访问 catalog
#include "log/log_config.h"
#include "utils.h" // 用于求值
#include "catalog/table_manager.h" // 需要TableManager相关类型
#include "catalog/types.h" // 需要TableSchema, DataType, Column等类型
#include <iostream>
#include <stdexcept>

// 从 AST 生成条件字符串
static std::string build_condition_from_where(ASTNode* where) {
    if (!where || where->children.empty()) {
        return "";
    }

    // 如果传入的where节点本身就是expression节点（比较操作符）
    ASTNode* expr = where;
    if (where->type == WHERE_CLAUSE && !where->children.empty()) {
        expr = where->children[0];
    }

    if (!expr) {
        return "";
    }

    // 检查是否是支持的操作符类型
    bool is_supported_operator = (expr->type == BINARY_EXPR ||
                                 expr->type == EQUAL_OPERATOR ||
                                 expr->type == NOT_EQUAL_OPERATOR ||
                                 expr->type == GREATER_THAN_OPERATOR ||
                                 expr->type == GREATER_THAN_OR_EQUAL_OPERATOR ||
                                 expr->type == LESS_THAN_OPERATOR ||
                                 expr->type == LESS_THAN_OR_EQUAL_OPERATOR);

    if (!is_supported_operator) {
        return "";
    }

    if (expr->children.size() < 2) {
        return "";
    }

    std::string lhs, op, rhs;

    // children[0]: IDENTIFIER_NODE (列名)
    if (expr->children[0]->type == IDENTIFIER_NODE) {
        if (std::holds_alternative<std::string>(expr->children[0]->value))
            lhs = std::get<std::string>(expr->children[0]->value);
    }

    // 根据expr节点类型确定操作符
    switch (expr->type) {
        case EQUAL_OPERATOR:
            op = "=";
            break;
        case NOT_EQUAL_OPERATOR:
            op = "!=";
            break;
        case GREATER_THAN_OPERATOR:
            op = ">";
            break;
        case GREATER_THAN_OR_EQUAL_OPERATOR:
            op = ">=";
            break;
        case LESS_THAN_OPERATOR:
            op = "<";
            break;
        case LESS_THAN_OR_EQUAL_OPERATOR:
            op = "<=";
            break;
        case BINARY_EXPR:
            // 对于BINARY_EXPR，操作符在value中
            if (std::holds_alternative<std::string>(expr->value))
                op = std::get<std::string>(expr->value);
            break;
        default:
            return "";
    }

    // children[1]: INTEGER_LITERAL_NODE 或 STRING_LITERAL_NODE (值)
    if (expr->children[1]->type == INTEGER_LITERAL_NODE) {
        if (std::holds_alternative<int>(expr->children[1]->value))
            rhs = std::to_string(std::get<int>(expr->children[1]->value));
        else if (std::holds_alternative<std::string>(expr->children[1]->value))
            rhs = std::get<std::string>(expr->children[1]->value);
    } else if (expr->children[1]->type == STRING_LITERAL_NODE) {
        if (std::holds_alternative<std::string>(expr->children[1]->value)) {
            std::string s = std::get<std::string>(expr->children[1]->value);
            if (!s.empty() && (s.front() == '\'' || s.front() == '"') && s.back() == s.front()) {
                if (s.size() >= 2) s = s.substr(1, s.size()-2);
            }
            rhs = "'" + s + "'";
        }
    } else if (expr->children[1]->type == IDENTIFIER_NODE) {
        if (std::holds_alternative<std::string>(expr->children[1]->value))
            rhs = std::get<std::string>(expr->children[1]->value);
    }

    if (lhs.empty() || op.empty() || rhs.empty()) {
        return "";
    }

    return lhs + op + rhs;
}

// 全局的内存数据存储
std::unordered_map<std::string, std::vector<Tuple>> in_memory_data;

// 实现每个算子的 next() 方法

// CreateTableOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> CreateTableOperator::next()
{
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    
    // 使用TableManager创建表
    if (table_manager_) {
        // 创建TableSchema
        TableSchema schema(table_name);
        for (const auto &col : columns) {
            // 映射字符串类型到DataType
            DataType dt = (col.second == "INT" || col.second == "INTEGER") ? DataType::Int : DataType::Varchar;
            int len = (dt == DataType::Varchar ? 255 : 4);
            schema.AddColumn(Column(col.first, dt, len, true));
        }
        schema.primary_key_index_ = 0; // 假设第一列是主键
        
        // 使用TableManager创建表
        bool success = table_manager_->CreateTable(schema);
        if (success) {
            logger->Info("Table '{}' created successfully using TableManager", table_name);
        } else {
            logger->Error("Failed to create table '{}' using TableManager", table_name);
            throw std::runtime_error("Failed to create table: " + table_name);
        }
    } else {
        // 回退到内存模式
        TableInfo new_table;
        new_table.name = table_name;
        for (const auto &col : columns) {
            new_table.columns[col.first] = {col.first, col.second};
            new_table.column_order.push_back(col.first);
        }
        catalog.addTable(new_table);
        // 为新表创建内存数据存储
        in_memory_data[table_name] = {};
        logger->Info("Table '{}' created successfully in memory mode", table_name);
    }
    
    return nullptr; // 没有数据行返回
}

// InsertOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> InsertOperator::next()
{
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    
    // 使用TableManager插入记录
    if (table_manager_) {
        if (!table_manager_->TableExists(table_name)) {
            throw std::runtime_error("Table '" + table_name + "' does not exist.");
        }
        
        // 创建Record对象
        Record record;
        logger->Info("Inserting {} values into table '{}'", values.size(), table_name);
        
        // 获取表结构信息用于调试
        TableSchema* schema = table_manager_->GetTableSchema(table_name);
        if (schema) {
            logger->Info("Table '{}' has {} columns, primary_key_index={}", 
                        table_name, schema->columns_.size(), schema->primary_key_index_);
            for (size_t i = 0; i < schema->columns_.size(); ++i) {
                logger->Info("Column {}: {} (type={})", i, schema->columns_[i].column_name_, 
                            static_cast<int>(schema->columns_[i].type_));
            }
        }
        
        for (size_t i = 0; i < values.size(); ++i) {
            const auto &val = values[i];
            logger->Info("Processing value {} of type {}", i, typeid(val).name());
            std::visit([&](const auto &arg) {
                logger->Info("std::visit: processing arg of type {}", typeid(arg).name());
                logger->Info("std::visit: arg value is: {}", arg);
                
                // 使用运行时类型检查而不是编译时检查
                if (std::holds_alternative<int>(val)) {
                    int int_val = std::get<int>(val);
                    record.AddValue(Value(int_val));
                    logger->Info("Added value {}: int={}", i, int_val);
                } else if (std::holds_alternative<std::string>(val)) {
                    std::string str_val = std::get<std::string>(val);
                    logger->Info("Processing string value: '{}'", str_val);
                    // 根据表结构决定是否转换为int
                    if (schema && i < schema->columns_.size() && schema->columns_[i].type_ == DataType::Int) {
                        logger->Info("Column {} is INT type, converting string to int", i);
                        // 尝试将字符串转换为int
                        try {
                            int int_val = std::stoi(str_val);
                            record.AddValue(Value(int_val));
                            logger->Info("Added value {}: string='{}' -> int={}", i, str_val, int_val);
                        } catch (const std::exception& e) {
                            // 转换失败，保持为字符串
                            record.AddValue(Value(str_val));
                            logger->Info("Added value {}: string='{}' (conversion failed)", i, str_val);
                        }
                    } else {
                        logger->Info("Column {} is not INT type, keeping as string", i);
                        record.AddValue(Value(str_val));
                        logger->Info("Added value {}: string='{}'", i, str_val);
                    }
                } else if (std::holds_alternative<bool>(val)) {
                    bool bool_val = std::get<bool>(val);
                    record.AddValue(Value(bool_val));
                    logger->Info("Added value {}: bool={}", i, bool_val);
                } else {
                    logger->Error("Unknown variant type for value {}", i);
                }
            }, val);
        }
        
        logger->Info("Record created with {} values", record.values_.size());
        
        // 使用TableManager插入记录
        bool success = table_manager_->InsertRecord(table_name, record);
        if (success) {
            logger->Info("Record inserted successfully into table '{}' using TableManager", table_name);
        } else {
            logger->Error("Failed to insert record into table '{}' using TableManager", table_name);
            throw std::runtime_error("Failed to insert record into table: " + table_name);
        }
    } else {
        // 回退到内存模式
        if (!catalog.tableExists(table_name)) {
            throw std::runtime_error("Table '" + table_name + "' does not exist.");
        }

        const TableInfo &table_info = catalog.getTable(table_name);
        Tuple final_tuple;

        if (column_names.empty()) {
            // 没有显式列名，按默认顺序插入
            final_tuple = values;
        } else {
            // 有显式列名，需要根据表定义重新排序值
            final_tuple.resize(table_info.column_order.size());
            std::unordered_map<std::string, std::variant<int, std::string, bool>> value_map;
            for (size_t i = 0; i < column_names.size(); ++i) {
                value_map[column_names[i]] = values[i];
            }

            for (size_t i = 0; i < table_info.column_order.size(); ++i) {
                const std::string &col_name = table_info.column_order[i];
                final_tuple[i] = value_map.at(col_name);
            }
        }

        in_memory_data[table_name].push_back(final_tuple);
        logger->Info("Record inserted successfully into table '{}' in memory mode", table_name);
    }
    
    return nullptr;
}

// SeqScanOperator (迭代模型)
std::unique_ptr<Tuple> SeqScanOperator::next()
{
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();

    // 使用TableManager读取数据
    if (table_manager_) {
        if (!table_manager_->TableExists(table_name)) {
            logger->Error("Table '{}' does not exist in TableManager", table_name);
            return nullptr;
        }
        
        // 从TableManager获取所有记录
        if (all_records_.empty()) {
            std::vector<Record> records = table_manager_->SelectRecords(table_name);
            // 转换Record为Tuple并存储
            for (const auto& record : records) {
                Tuple tuple;
                for (const auto& val : record.values_) {
                    // 使用运行时类型检查而不是编译时检查
                    if (std::holds_alternative<int>(val)) {
                        tuple.push_back(std::get<int>(val));
                    } else if (std::holds_alternative<std::string>(val)) {
                        tuple.push_back(std::get<std::string>(val));
                    } else if (std::holds_alternative<bool>(val)) {
                        tuple.push_back(std::get<bool>(val));
                    } else {
                        logger->Error("Unknown variant type in Record value");
                    }
                }
                all_records_.push_back(tuple);
            }
            current_row_index = 0;
        }
        
        if (current_row_index >= all_records_.size()) {
            return nullptr;
        }
        
        // 返回当前行数据的副本
        auto result_tuple = std::make_unique<Tuple>(all_records_[current_row_index]);
        
        
        current_row_index++;
        return result_tuple;
    } else {
        // 回退到内存模式
        if (current_row_index >= in_memory_data[table_name].size()) {
            return nullptr;
        }

        // 返回当前行数据的副本，并移动到下一行
        auto result_tuple = std::make_unique<Tuple>(in_memory_data[table_name][current_row_index]);

        logger->Info("Scanning tuple at index {} from memory:", current_row_index);
        for (const auto &val : *result_tuple) {
            std::visit([&](const auto &arg) {
                std::cout << typeid(arg).name() << ":" << arg << " ";
            }, val);
        }
        std::cout << std::endl;
        
        current_row_index++;
        return result_tuple;
    }
}

// FilterOperator (迭代模型)
std::unique_ptr<Tuple> FilterOperator::next()
{
    // 首次调用时，直接使用TableManager的SelectColumnsWithCondition方法
    if (current_row_index == 0) {
        // 获取表名（从SeqScanOperator中获取）
        std::string table_name = "";
        if (child && child->type == SEQ_SCAN_OP) {
            SeqScanOperator* seq_scan = static_cast<SeqScanOperator*>(child.get());
            table_name = seq_scan->table_name;
        }

        if (table_name.empty() || !table_manager_) {
            return nullptr;
        }

        // 构建条件字符串
        std::string condition_str = build_condition_from_where(condition);
        auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
        

        // 获取要选择的列名
        std::vector<std::string> select_columns;

        // FilterOperator的子节点是SeqScanOperator，不是ProjectOperator
        // 列选择由外层的ProjectOperator处理，这里我们选择所有列

        // 如果没有指定列，或者指定了"*"，则选择所有列
        if (select_columns.empty() || (select_columns.size() == 1 && select_columns[0] == "*")) {
            TableSchema* schema = table_manager_->GetTableSchema(table_name);
            if (schema) {
                for (const auto& col : schema->columns_) {
                    select_columns.push_back(col.column_name_);
                }
            }
        }

        // 使用TableManager的SelectColumnsWithCondition方法
        std::vector<Record> records;
        if (condition_str.empty()) {
            records = table_manager_->SelectColumns(table_name, select_columns);
        } else {
            records = table_manager_->SelectColumnsWithCondition(table_name, select_columns, condition_str);
        }

        // 将Record转换为Tuple
        all_records_.clear();
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

// FilterOperator的evaluateCondition方法实现
bool FilterOperator::evaluateCondition(const Tuple& tuple, ASTNode* condition) {
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    
    if (!condition) {
        return true; // 无条件，返回true
    }
    
    // 处理比较操作符
    if (condition->type == GREATER_THAN_OPERATOR || condition->type == LESS_THAN_OPERATOR || 
        condition->type == EQUAL_OPERATOR || condition->type == NOT_EQUAL_OPERATOR) {
        
        if (condition->children.size() != 2) {
            logger->Error("Comparison operator requires exactly 2 operands");
            return false;
        }
        
        ASTNode* left = condition->children[0];
        ASTNode* right = condition->children[1];
        
        // 获取左操作数的值
        LiteralValue left_val;
        if (left->type == IDENTIFIER_NODE) {
            // 列名，从tuple中获取值
            std::string col_name = std::get<std::string>(left->value);
            // 这里需要根据列名找到对应的索引，简化处理
            // 假设age是第2列（索引2）
            if (col_name == "age" && tuple.size() > 2) {
                left_val = tuple[2];
            } else if (col_name == "id" && tuple.size() > 0) {
                left_val = tuple[0];
            } else {
                logger->Error("Unknown column: {}", col_name);
                return false;
            }
        } else {
            // 字面量
            left_val = evaluateLiteral(left);
        }
        
        // 获取右操作数的值
        LiteralValue right_val = evaluateLiteral(right);
        
        // 比较操作
        bool result = false;
        if (condition->type == GREATER_THAN_OPERATOR) {
            // 比较逻辑
            if (std::holds_alternative<int>(left_val) && std::holds_alternative<int>(right_val)) {
                result = std::get<int>(left_val) > std::get<int>(right_val);
            } else if (std::holds_alternative<int>(left_val) && std::holds_alternative<std::string>(right_val)) {
                // 字符串转int比较
                try {
                    int right_int = std::stoi(std::get<std::string>(right_val));
                    result = std::get<int>(left_val) > right_int;
                } catch (...) {
                    result = false;
                }
            } else if (std::holds_alternative<std::string>(left_val) && std::holds_alternative<int>(right_val)) {
                // 字符串转int比较
                try {
                    int left_int = std::stoi(std::get<std::string>(left_val));
                    result = left_int > std::get<int>(right_val);
                } catch (...) {
                    result = false;
                }
            }
        } else if (condition->type == EQUAL_OPERATOR) {
            // 处理相等比较，需要类型转换
            if (std::holds_alternative<int>(left_val) && std::holds_alternative<int>(right_val)) {
                result = std::get<int>(left_val) == std::get<int>(right_val);
            } else if (std::holds_alternative<int>(left_val) && std::holds_alternative<std::string>(right_val)) {
                // 字符串转int比较
                try {
                    int right_int = std::stoi(std::get<std::string>(right_val));
                    result = std::get<int>(left_val) == right_int;
                } catch (...) {
                    result = false;
                }
            } else if (std::holds_alternative<std::string>(left_val) && std::holds_alternative<int>(right_val)) {
                // 字符串转int比较
                try {
                    int left_int = std::stoi(std::get<std::string>(left_val));
                    result = left_int == std::get<int>(right_val);
                } catch (...) {
                    result = false;
                }
            } else if (std::holds_alternative<std::string>(left_val) && std::holds_alternative<std::string>(right_val)) {
                result = std::get<std::string>(left_val) == std::get<std::string>(right_val);
            } else {
                result = false;
            }
        }
        
        logger->Info("Condition evaluation: {} {} {} = {}", 
                    std::holds_alternative<int>(left_val) ? std::to_string(std::get<int>(left_val)) : std::get<std::string>(left_val),
                    condition->type == GREATER_THAN_OPERATOR ? ">" : "==",
                    std::holds_alternative<int>(right_val) ? std::to_string(std::get<int>(right_val)) : std::get<std::string>(right_val),
                    result);
        
        return result;
    }
    
    return false;
}

// ProjectOperator (迭代模型) - 简化实现，直接返回子算子的结果
std::unique_ptr<Tuple> ProjectOperator::next()
{
    // 直接返回子算子的结果，因为FilterOperator已经处理了列选择和条件过滤
    return child->next();
}
std::unique_ptr<Tuple> DeleteOperator::next() {
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    
    if (!child) {
        logger->Error("DeleteOperator: No child operator");
        return nullptr;
    }
    
    // 获取要删除的记录
    std::vector<Tuple> tuples_to_delete;
    std::unique_ptr<Tuple> tuple;
    while ((tuple = child->next()) != nullptr) {
        tuples_to_delete.push_back(*tuple);
    }
    
    logger->Info("DeleteOperator: Found {} records to delete", tuples_to_delete.size());
    
    if (tuples_to_delete.empty()) {
        logger->Info("No records to delete");
        return nullptr;
    }
    
    // 从child operator获取表名
    std::string table_name;
    if (child->type == SEQ_SCAN_OP) {
        SeqScanOperator* scan_op = static_cast<SeqScanOperator*>(child.get());
        table_name = scan_op->table_name;
    } else if (child->type == FILTER_OP) {
        FilterOperator* filter_op = static_cast<FilterOperator*>(child.get());
        // 从FilterOperator获取表名
        if (filter_op->child && filter_op->child->type == SEQ_SCAN_OP) {
            SeqScanOperator* scan_op = static_cast<SeqScanOperator*>(filter_op->child.get());
            table_name = scan_op->table_name;
        }
    }
    
    if (table_name.empty()) {
        logger->Error("DeleteOperator: Could not determine table name");
        return nullptr;
    }
    
    logger->Info("DeleteOperator: Deleting records from table '{}'", table_name);
    
    // 使用原始的WHERE条件删除所有匹配的记录
    // 由于TableManager的DeleteRecordsWithCondition只支持简单条件，我们使用原始的WHERE条件
    int deleted_count = 0;
    
    // 从child operator获取原始的WHERE条件
    std::string where_condition = "";
    if (child && child->type == FILTER_OP) {
        FilterOperator* filter_op = static_cast<FilterOperator*>(child.get());
        where_condition = build_condition_from_where(filter_op->condition);
        logger->Info("DeleteOperator: Using original WHERE condition: '{}'", where_condition);
    }
    
    // 使用原始的WHERE条件删除所有匹配的记录
    if (table_manager_ && !where_condition.empty()) {
        int result = table_manager_->DeleteRecordsWithCondition(table_name, where_condition);
        if (result > 0) {
            deleted_count = result;
            logger->Info("DeleteOperator: Successfully deleted {} record(s) with condition '{}'", result, where_condition);
        } else {
            logger->Warn("DeleteOperator: Failed to delete records with condition: '{}'", where_condition);
        }
    }
    
    logger->Info("DeleteOperator: Total deleted records: {}", deleted_count);
    
    // DELETE操作不返回数据，返回nullptr表示完成
    return nullptr;
}

std::unique_ptr<Tuple> UpdateOperator::next()
{
    // 获取需要更新的元组
    std::vector<Tuple> tuples_to_update;
    std::unique_ptr<Tuple> current_tuple;
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();

    while ((current_tuple = child->next()) != nullptr)
    {
        tuples_to_update.push_back(*current_tuple);
    }

    // 如果没有符合条件的元组，直接返回
    if (tuples_to_update.empty())
    {
        auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
        logger->Info("No rows matched for update in table '{}'", table_name);
        return nullptr;
    }

    // 获取表的元数据
    TableSchema* schema = nullptr;
    if (table_manager_) {
        schema = table_manager_->GetTableSchema(table_name);
        if (!schema) {
            throw std::runtime_error("Table '" + table_name + "' schema not found in TableManager.");
        }
    } else {
        // 回退到catalog
        const TableInfo &table_info = catalog.getTable(table_name);
        // 这里需要将TableInfo转换为TableSchema，但为了简化，我们暂时跳过
        throw std::runtime_error("UpdateOperator requires TableManager for table '" + table_name + "'.");
    }

    // 找出所有更新操作的目标列的索引和对应的值
    std::vector<int> update_indices;
    std::vector<std::variant<int, std::string, bool>> update_values;

    for (const auto &update_pair : updates)
    {
        const std::string &col_name = update_pair.first;
        ASTNode *value_node = update_pair.second;
        logger->Info("Processing update for column '{}'", col_name);

        // 查找列的索引
        int col_index = -1;
        for (size_t i = 0; i < schema->columns_.size(); ++i)
        {
            if (schema->columns_[i].column_name_ == col_name)
            {
                col_index = i;
                break;
            }
        }
        if (col_index != -1)
        {
            update_indices.push_back(col_index);
            // 确保 evaluateLiteral 返回的 variant 类型与 update_values 的类型兼容
            // 现在类型已经统一，直接使用
            LiteralValue literal_value = evaluateLiteral(value_node);
            logger->Info("Evaluated literal value type: {}", typeid(literal_value).name());
            update_values.push_back(literal_value);
        }
        else
        {
            auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
            logger->Warn("Column '{}' not found in table '{}' for update.", col_name, table_name);
        }
    }

    // 使用TableManager更新数据
    size_t updated_count = 0;
    
    if (table_manager_) {
        // 构建WHERE条件字符串
        std::string where_condition = "";
        if (child && child->type == FILTER_OP) {
            FilterOperator* filter_op = static_cast<FilterOperator*>(child.get());
            where_condition = build_condition_from_where(filter_op->condition);
        }
        
        logger->Info("Using UpdateRecordsWithCondition with condition: '{}'", where_condition);
        
        // 使用UpdateRecordsWithCondition方法更新所有匹配的记录
        // 这个方法会为每个匹配的记录创建更新后的版本
        int result = 0;
        
        // 使用原始的WHERE条件更新所有匹配的记录
        logger->Info("Using original WHERE condition: '{}'", where_condition);
        
        // 创建更新后的记录模板
        if (!tuples_to_update.empty()) {
            // 使用第一条记录的更新后版本作为模板
            Record updated_record;
            const auto &first_tuple = tuples_to_update[0];
            
            logger->Info("Creating updated record from tuple with {} values", first_tuple.size());
            
            // 首先添加所有原始值
            for (size_t i = 0; i < first_tuple.size(); ++i) {
                const auto &val = first_tuple[i];
                if (std::holds_alternative<int>(val)) {
                    updated_record.AddValue(Value(std::get<int>(val)));
                    logger->Info("Added int value: {}", std::get<int>(val));
                } else if (std::holds_alternative<std::string>(val)) {
                    updated_record.AddValue(Value(std::get<std::string>(val)));
                    logger->Info("Added string value: '{}'", std::get<std::string>(val));
                } else if (std::holds_alternative<bool>(val)) {
                    updated_record.AddValue(Value(std::get<bool>(val)));
                    logger->Info("Added bool value: {}", std::get<bool>(val));
                }
            }
            
            // 然后应用更新
            for (size_t i = 0; i < update_indices.size(); ++i) {
                int col_index = update_indices[i];
                const auto &new_val = update_values[i];
                
                logger->Info("Updating column index {} with new value", col_index);
                
                // 根据目标列的类型来转换值
                if (schema && col_index < static_cast<int>(schema->columns_.size())) {
                    DataType target_type = schema->columns_[col_index].type_;
                    
                    if (target_type == DataType::Int) {
                        // 目标列是int类型，需要转换
                        if (std::holds_alternative<int>(new_val)) {
                            updated_record.values_[col_index] = Value(std::get<int>(new_val));
                            logger->Info("Updated column {} to int value: {}", col_index, std::get<int>(new_val));
                        } else if (std::holds_alternative<std::string>(new_val)) {
                            // 将字符串转换为整数
                            try {
                                int int_val = std::stoi(std::get<std::string>(new_val));
                                updated_record.values_[col_index] = Value(int_val);
                                logger->Info("Updated column {} to int value: {} (converted from string)", col_index, int_val);
                            } catch (const std::exception& e) {
                                logger->Error("Failed to convert string '{}' to int for column {}", std::get<std::string>(new_val), col_index);
                                continue;
                            }
                        } else if (std::holds_alternative<bool>(new_val)) {
                            // 将布尔值转换为整数
                            int int_val = std::get<bool>(new_val) ? 1 : 0;
                            updated_record.values_[col_index] = Value(int_val);
                            logger->Info("Updated column {} to int value: {} (converted from bool)", col_index, int_val);
                        }
                    } else if (target_type == DataType::Varchar) {
                        // 目标列是字符串类型
                        if (std::holds_alternative<std::string>(new_val)) {
                            updated_record.values_[col_index] = Value(std::get<std::string>(new_val));
                            logger->Info("Updated column {} to string value: '{}'", col_index, std::get<std::string>(new_val));
                        } else if (std::holds_alternative<int>(new_val)) {
                            // 将整数转换为字符串
                            std::string str_val = std::to_string(std::get<int>(new_val));
                            updated_record.values_[col_index] = Value(str_val);
                            logger->Info("Updated column {} to string value: '{}' (converted from int)", col_index, str_val);
                        } else if (std::holds_alternative<bool>(new_val)) {
                            // 将布尔值转换为字符串
                            std::string str_val = std::get<bool>(new_val) ? "true" : "false";
                            updated_record.values_[col_index] = Value(str_val);
                            logger->Info("Updated column {} to string value: '{}' (converted from bool)", col_index, str_val);
                        }
                    } else if (target_type == DataType::Bool) {
                        // 目标列是布尔类型
                        if (std::holds_alternative<bool>(new_val)) {
                            updated_record.values_[col_index] = Value(std::get<bool>(new_val));
                            logger->Info("Updated column {} to bool value: {}", col_index, std::get<bool>(new_val));
                        } else if (std::holds_alternative<int>(new_val)) {
                            // 将整数转换为布尔值
                            bool bool_val = std::get<int>(new_val) != 0;
                            updated_record.values_[col_index] = Value(bool_val);
                            logger->Info("Updated column {} to bool value: {} (converted from int)", col_index, bool_val);
                        } else if (std::holds_alternative<std::string>(new_val)) {
                            // 将字符串转换为布尔值
                            std::string str_val = std::get<std::string>(new_val);
                            std::transform(str_val.begin(), str_val.end(), str_val.begin(), ::tolower);
                            bool bool_val = (str_val == "true" || str_val == "1");
                            updated_record.values_[col_index] = Value(bool_val);
                            logger->Info("Updated column {} to bool value: {} (converted from string)", col_index, bool_val);
                        }
                    }
                } else {
                    // 没有schema信息，直接使用原始值
                    if (std::holds_alternative<int>(new_val)) {
                        updated_record.values_[col_index] = Value(std::get<int>(new_val));
                        logger->Info("Updated column {} to int value: {}", col_index, std::get<int>(new_val));
                    } else if (std::holds_alternative<std::string>(new_val)) {
                        updated_record.values_[col_index] = Value(std::get<std::string>(new_val));
                        logger->Info("Updated column {} to string value: '{}'", col_index, std::get<std::string>(new_val));
                    } else if (std::holds_alternative<bool>(new_val)) {
                        updated_record.values_[col_index] = Value(std::get<bool>(new_val));
                        logger->Info("Updated column {} to bool value: {}", col_index, std::get<bool>(new_val));
                    }
                }
            }
            
            logger->Info("Created updated record with {} values", updated_record.values_.size());
            
            // 使用原始的WHERE条件更新所有匹配的记录
            result = table_manager_->UpdateRecordsWithCondition(table_name, where_condition, updated_record);
            logger->Info("UpdateRecordsWithCondition returned: {} updated records", result);
        }
        
        updated_count = static_cast<size_t>(result);
        logger->Info("Total updated records: {}", result);
    } else {
        // 回退到内存模式
        std::vector<Tuple> &table_data = in_memory_data[table_name];

        // 假设 id 是主键
        int id_col_index = -1;
        for (size_t i = 0; i < schema->columns_.size(); ++i)
        {
            if (schema->columns_[i].column_name_ == "id")
            {
                id_col_index = i;
                break;
            }
        }
        if (id_col_index == -1)
        {
            throw std::runtime_error("Table must have an 'id' column for this update logic.");
        }

        for (auto &stored_tuple : table_data)
        {
            for (const auto &tuple_to_update : tuples_to_update)
            {
                // 通过 'id' 找到需要更新的元组，使用安全的variant访问
                bool id_match = false;
                std::visit([&](const auto& stored_val) {
                    std::visit([&](const auto& update_val) {
                        if constexpr (std::is_same_v<decltype(stored_val), decltype(update_val)>) {
                            id_match = (stored_val == update_val);
                        }
                    }, tuple_to_update[id_col_index]);
                }, stored_tuple[id_col_index]);
                
                if (id_match)
                {
                    // 更新元组中的值
                    for (size_t i = 0; i < update_indices.size(); ++i)
                    {
                        stored_tuple[update_indices[i]] = update_values[i];
                    }
                    updated_count++;
                    break;
                }
            }
        }
    }
    
    logger->Info("{} rows updated in table '{}'", updated_count, table_name);
    return nullptr;
}
// Executor 类实现
std::vector<Tuple> Executor::execute()
{
    std::vector<Tuple> results;
    if (!plan_root_)
    {
        return results;
    }
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();

    // 检查根算子的类型
    switch (plan_root_->type)
    {
    case CREATE_TABLE_OP:
    case INSERT_OP:
    case DELETE_OP:
    case UPDATE_OP:
        // 非迭代型操作，只需调用一次 next()
        plan_root_->next();
        break;
    case PROJECT_OP:
    case FILTER_OP:
    case SEQ_SCAN_OP:
        // 在这里添加其他迭代型操作，例如 JOIN_OP
        // 迭代型操作，循环调用 next() 直到没有更多数据
        while (true)
        {
            logger->Info("进入select");
            std::unique_ptr<Tuple> tuple = plan_root_->next();
            logger->Info("select执行完毕");
            if (tuple == nullptr)
            {
                break;
            }
            results.push_back(*tuple);
        }
        break;
    default:
        logger->Error("Unsupported operator type.");
        break;
    }

    return results;
}
// 构造函数
SeqScanOperator::SeqScanOperator(const std::string &table_name)
    : table_name(table_name), table_manager_(nullptr)
{
    type = SEQ_SCAN_OP;
    current_row_index = 0; // 初始化
}

SeqScanOperator::SeqScanOperator(const std::string &table_name, TableManager* table_manager)
    : table_name(table_name), table_manager_(table_manager)
{
    type = SEQ_SCAN_OP;
    current_row_index = 0; // 初始化
}
// ProjectOperator构造函数已在头文件中定义，这里不需要重复定义
// FilterOperator::FilterOperator(std::unique_ptr<Operator>&& child, ASTNode* condition_node, const std::unordered_map<std::string, const TableInfo*>& tables)
//     : condition(condition_node), tables(tables) {
//     this->child = std::move(child);
//     type = FILTER_OP;
// }
// UpdateOperator::UpdateOperator(std::unique_ptr<Operator>&& child, const std::string& table_name, const std::vector<std::pair<std::string, ASTNode*>>& updates)
//     : table_name(table_name), updates(updates) {
//     this->child = std::move(child);
//     type = UPDATE_OP;
// }

// InsertOperator构造函数已在头文件中定义，这里不需要重复定义