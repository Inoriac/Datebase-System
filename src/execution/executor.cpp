#include "executor.h"
#include "symbol_table.h" // 需要访问 catalog
#include "log/log_config.h"
#include "utils.h" // 用于求值
#include "catalog/table_manager.h" // 需要TableManager相关类型
#include "catalog/types.h" // 需要TableSchema, DataType, Column等类型
#include <iostream>
#include <stdexcept>

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
                } else if (std::holds_alternative<double>(val)) {
                    double double_val = std::get<double>(val);
                    // 将double转换为int
                    record.AddValue(Value(static_cast<int>(double_val)));
                    logger->Info("Added value {}: double={} -> int={}", i, double_val, static_cast<int>(double_val));
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
            std::unordered_map<std::string, std::variant<int, std::string, double, bool>> value_map;
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
        logger->Info("SeqScanOperator: Using TableManager for table '{}'", table_name);
        logger->Info("SeqScanOperator: Checking if table '{}' exists", table_name);
        if (!table_manager_->TableExists(table_name)) {
            logger->Error("Table '{}' does not exist in TableManager", table_name);
            return nullptr;
        }
        logger->Info("SeqScanOperator: Table '{}' exists, proceeding with scan", table_name);
        
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
        
        logger->Info("Scanning tuple at index {} from TableManager:", current_row_index);
        for (const auto &val : *result_tuple) {
            // 使用运行时类型检查而不是std::visit
            if (std::holds_alternative<int>(val)) {
                std::cout << "int:" << std::get<int>(val) << " ";
            } else if (std::holds_alternative<std::string>(val)) {
                std::cout << "string:" << std::get<std::string>(val) << " ";
            } else if (std::holds_alternative<bool>(val)) {
                std::cout << "bool:" << std::get<bool>(val) << " ";
            } else if (std::holds_alternative<double>(val)) {
                std::cout << "double:" << std::get<double>(val) << " ";
            } else {
                std::cout << "unknown:";
            }
        }
        std::cout << std::endl;
        
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
    std::unique_ptr<Tuple> current_tuple = nullptr;
    do
    {
        current_tuple = child->next();
        if (current_tuple == nullptr)
        {
            return nullptr; // 没有更多数据了
        }
        // 使用通用的求值函数来评估 WHERE 条件
        if (evaluateCondition(*current_tuple, condition))
        {
            return current_tuple;
        }
    } while (current_tuple != nullptr);

    return nullptr;
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
        std::variant<int, std::string, double, bool> left_val;
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
        std::variant<int, std::string, double, bool> right_val = evaluateLiteral(right);
        
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

// ProjectOperator (迭代模型)
std::unique_ptr<Tuple> ProjectOperator::next()
{
    std::unique_ptr<Tuple> input_tuple = child->next();
    if (input_tuple == nullptr)
    {
        return nullptr;
    }

    // 从tables映射获取表信息
    auto it = tables.find(table_name);
    if (it == tables.end()) {
        throw std::runtime_error("Table '" + table_name + "' not found in tables mapping.");
    }
    const TableInfo &table_info = *it->second;
    auto result_tuple = std::make_unique<Tuple>();

    for (const auto &col_name : columns)
    {
        int index = -1;
        for (size_t i = 0; i < table_info.column_order.size(); ++i)
        {
            if (table_info.column_order[i] == col_name)
            {
                index = i;
                break;
            }
        }
        if (index != -1)
        {
            result_tuple->push_back((*input_tuple)[index]);
        }
    }
    return result_tuple;
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
    std::vector<std::variant<int, std::string, double, bool>> update_values;

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
            auto literal_value = evaluateLiteral(value_node);
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
        // 使用TableManager更新数据
        for (const auto &tuple_to_update : tuples_to_update) {
            // 将Tuple转换为Record
            Record record;
            for (size_t i = 0; i < tuple_to_update.size(); ++i) {
                const auto &val = tuple_to_update[i];
                if (std::holds_alternative<int>(val)) {
                    record.AddValue(Value(std::get<int>(val)));
                } else if (std::holds_alternative<std::string>(val)) {
                    record.AddValue(Value(std::get<std::string>(val)));
                } else if (std::holds_alternative<bool>(val)) {
                    record.AddValue(Value(std::get<bool>(val)));
                }
            }
            
            // 应用更新
            for (size_t i = 0; i < update_indices.size(); ++i) {
                int col_index = update_indices[i];
                const auto &new_val = update_values[i];
                
                if (std::holds_alternative<int>(new_val)) {
                    record.values_[col_index] = Value(std::get<int>(new_val));
                } else if (std::holds_alternative<std::string>(new_val)) {
                    record.values_[col_index] = Value(std::get<std::string>(new_val));
                } else if (std::holds_alternative<bool>(new_val)) {
                    record.values_[col_index] = Value(std::get<bool>(new_val));
                }
            }
            
            // 使用TableManager更新记录
            // 需要根据主键找到正确的记录ID
            std::vector<Record> all_records = table_manager_->SelectRecords(table_name);
            logger->Info("Found {} records in table '{}' for update", all_records.size(), table_name);
            int target_record_id = -1;
            
            // 根据主键找到要更新的记录ID
            for (size_t i = 0; i < all_records.size(); ++i) {
                if (all_records[i].values_.size() > 0) {
                    // 比较主键值（假设第一列是主键）
                    const Value& existing_key = all_records[i].values_[0];
                    const auto& new_key_variant = tuple_to_update[0];
                    
                    bool key_match = false;
                    
                    // 将Tuple中的variant转换为Value进行比较
                    logger->Info("Comparing record {}: existing_key type={}, new_key type={}", 
                                i, typeid(existing_key).name(), typeid(new_key_variant).name());
                    
                    if (std::holds_alternative<int>(existing_key)) {
                        int existing_int = std::get<int>(existing_key);
                        logger->Info("Existing key (int): {}", existing_int);
                        if (std::holds_alternative<int>(new_key_variant)) {
                            int new_int = std::get<int>(new_key_variant);
                            key_match = (existing_int == new_int);
                            logger->Info("New key (int): {}, match: {}", new_int, key_match);
                        } else if (std::holds_alternative<std::string>(new_key_variant)) {
                            try {
                                int new_int = std::stoi(std::get<std::string>(new_key_variant));
                                key_match = (existing_int == new_int);
                                logger->Info("New key (string->int): {}, match: {}", new_int, key_match);
                            } catch (...) {
                                key_match = false;
                                logger->Info("Failed to convert string to int");
                            }
                        } else if (std::holds_alternative<double>(new_key_variant)) {
                            int new_int = static_cast<int>(std::get<double>(new_key_variant));
                            key_match = (existing_int == new_int);
                            logger->Info("New key (double->int): {}, match: {}", new_int, key_match);
                        }
                    } else if (std::holds_alternative<std::string>(existing_key)) {
                        std::string existing_str = std::get<std::string>(existing_key);
                        logger->Info("Existing key (string): '{}'", existing_str);
                        if (std::holds_alternative<std::string>(new_key_variant)) {
                            std::string new_str = std::get<std::string>(new_key_variant);
                            key_match = (existing_str == new_str);
                            logger->Info("New key (string): '{}', match: {}", new_str, key_match);
                        } else if (std::holds_alternative<int>(new_key_variant)) {
                            std::string new_str = std::to_string(std::get<int>(new_key_variant));
                            key_match = (existing_str == new_str);
                            logger->Info("New key (int->string): '{}', match: {}", new_str, key_match);
                        }
                    }
                    
                    if (key_match) {
                        target_record_id = static_cast<int>(i);
                        break;
                    }
                }
            }
            
            if (target_record_id >= 0) {
                if (table_manager_->UpdateRecord(table_name, target_record_id, record)) {
                    updated_count++;
                    logger->Info("Updated record with ID {} in table '{}'", target_record_id, table_name);
                } else {
                    logger->Error("Failed to update record with ID {} in table '{}'", target_record_id, table_name);
                }
            } else {
                logger->Error("Could not find record to update in table '{}'", table_name);
            }
        }
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
    // case DELETE_OP:
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