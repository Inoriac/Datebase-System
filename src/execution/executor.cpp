#include "executor.h"
#include "symbol_table.h" // 需要访问 catalog
#include "log/log_config.h"
#include "utils.h" // 用于求值
#include "catalog/table_manager.h" // 旧版本TableManager
#include "catalog/table_schema_manager.h"
#include <iostream>
#include <stdexcept>

// 全局的内存数据存储
std::unordered_map<std::string, std::vector<Tuple>> in_memory_data;

// 从 AST 生成条件字符串（旧版本需要）
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
    
    // 操作符在expr节点本身的value中
    if (std::holds_alternative<std::string>(expr->value))
        op = std::get<std::string>(expr->value);
    
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

// 实现每个算子的 next() 方法

// CreateTableOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> CreateTableOperator::next()
{
    if (table_manager_) {
        // 旧版本：使用TableManager
        TableSchema schema(table_name);
        for (const auto& col : columns) {
            DataType dt = (col.second == "INT" || col.second == "INTEGER") ? DataType::Int : DataType::Varchar;
            int len = (dt == DataType::Varchar) ? 255 : 4;
            schema.AddColumn(Column(col.first, dt, len, true));
        }
        schema.primary_key_index_ = 0;
        
        bool success = table_manager_->CreateTable(schema);
        if (!success) {
            throw std::runtime_error("Failed to create table: " + table_name);
        }
        
        auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
        logger->Info("Table '{}' created successfully", table_name);
    } else {
        // 新版本：使用内存存储
        TableInfo new_table;
        new_table.name = table_name;
        for (const auto &col : columns)
        {
            new_table.columns[col.first] = {col.first, col.second};
            new_table.column_order.push_back(col.first);
        }
        catalog.addTable(new_table);
        // 为新表创建内存数据存储
        in_memory_data[table_name] = {};
        auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
        logger->Info("Table '{}' created successfully", table_name);
    }
    return nullptr; // 没有数据行返回
}

// InsertOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> InsertOperator::next() {
    if (table_manager_) {
        // 旧版本：使用TableManager
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
    } else {
        // 新版本：使用内存存储
        if (!catalog.tableExists(table_name)) {
            throw std::runtime_error("Table '" + table_name + "' does not exist.");
        }

        const TableInfo& table_info = catalog.getTable(table_name);
        Tuple final_tuple;

        if (column_names.empty()) {
            // 没有显式列名，按默认顺序插入
            final_tuple.clear();
            for (const auto& val : values) {
                if (std::holds_alternative<int>(val)) {
                    final_tuple.push_back(std::get<int>(val));
                } else if (std::holds_alternative<std::string>(val)) {
                    final_tuple.push_back(std::get<std::string>(val));
                } else if (std::holds_alternative<bool>(val)) {
                    final_tuple.push_back(std::get<bool>(val));
                }
            }
        } else {
            // 有显式列名，需要根据表定义重新排序值
            final_tuple.resize(table_info.column_order.size());
            std::unordered_map<std::string, std::variant<int, std::string, bool>> value_map;
            for (size_t i = 0; i < column_names.size(); ++i) {
                // 转换类型
                if (std::holds_alternative<int>(values[i])) {
                    value_map[column_names[i]] = std::get<int>(values[i]);
                } else if (std::holds_alternative<std::string>(values[i])) {
                    value_map[column_names[i]] = std::get<std::string>(values[i]);
                } else if (std::holds_alternative<bool>(values[i])) {
                    value_map[column_names[i]] = std::get<bool>(values[i]);
                }
            }

            for (size_t i = 0; i < table_info.column_order.size(); ++i) {
                const std::string& col_name = table_info.column_order[i];
                if (value_map.find(col_name) != value_map.end()) {
                    const auto& val = value_map.at(col_name);
                    if (std::holds_alternative<int>(val)) {
                        final_tuple[i] = std::get<int>(val);
                    } else if (std::holds_alternative<std::string>(val)) {
                        final_tuple[i] = std::get<std::string>(val);
                    } else if (std::holds_alternative<bool>(val)) {
                        final_tuple[i] = std::get<bool>(val);
                    }
                }
            }
        }

        in_memory_data[table_name].push_back(final_tuple);
        auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
        logger->Info("Values inserted into table '{}' successfully", table_name);
    }
    return nullptr;
}

// SeqScanOperator (迭代模型) - 支持智能预读
std::unique_ptr<Tuple> SeqScanOperator::next()
{
    if (table_manager_) {
        // 旧版本：使用TableManager
        if (current_row_index == 0) {
            // 首次调用，获取所有记录
            std::vector<Record> records = table_manager_->SelectRecords(table_name);
            all_records_.clear();
            
            // 将Record转换为Tuple
            for (const auto& record : records) {
                Tuple tuple;
                for (const auto& value : record.values_) {
                    // 转换variant类型
                    if (std::holds_alternative<int>(value)) {
                        tuple.push_back(std::get<int>(value));
                    } else if (std::holds_alternative<std::string>(value)) {
                        tuple.push_back(std::get<std::string>(value));
                    } else if (std::holds_alternative<bool>(value)) {
                        tuple.push_back(std::get<bool>(value));
                    }
                }
                all_records_.push_back(tuple);
            }
            
            // 智能预读：如果记录数量较大，启动预读
            if (records.size() > 100) {
                // 这里可以集成AsyncBufferPoolManager进行页面预读
                // 例如：async_bpm_->PrefetchSequential(start_page_id, prefetch_count);
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
    } else {
        // 新版本：使用内存存储
        if (current_row_index >= in_memory_data[table_name].size())
        {
            return nullptr;
        }
        // 返回当前行数据的副本，并移动到下一行
        auto result_tuple = std::make_unique<Tuple>(in_memory_data[table_name][current_row_index]);
        current_row_index++;
        return result_tuple;
    }
}

// FilterOperator (迭代模型)
std::unique_ptr<Tuple> FilterOperator::next()
{
    if (table_manager_) {
        // 旧版本：使用TableManager
        // 首次调用时，直接使用TableManager的SelectColumnsWithCondition方法
        if (current_row_index == 0) {
            // 获取表名（从SeqScanOperator中获取）
            std::string table_name = "";
            if (child && child->type == SEQ_SCAN_OP) {
                SeqScanOperator* seq_scan = static_cast<SeqScanOperator*>(child.get());
                table_name = seq_scan->table_name;
            }
            
            if (table_name.empty()) {
                return nullptr;
            }
            
            // 构建条件字符串
            std::string condition_str = build_condition_from_where(condition);
            
            // 获取要选择的列名
            std::vector<std::string> select_columns;
            
            // 检查是否有ProjectOperator指定了列
            if (child && child->type == PROJECT_OP) {
                ProjectOperator* proj_op = static_cast<ProjectOperator*>(child.get());
                select_columns = proj_op->columns;
            }
            
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
                    // 转换variant类型
                    if (std::holds_alternative<int>(value)) {
                        tuple.push_back(std::get<int>(value));
                    } else if (std::holds_alternative<std::string>(value)) {
                        tuple.push_back(std::get<std::string>(value));
                    } else if (std::holds_alternative<bool>(value)) {
                        tuple.push_back(std::get<bool>(value));
                    }
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
    } else {
        // 新版本：使用内存存储和tables映射
        std::unique_ptr<Tuple> current_tuple = nullptr;
        do
        {
            current_tuple = child->next();
            if (current_tuple == nullptr)
            {
                return nullptr; // 没有更多数据了
            }
            // 使用通用的求值函数来评估 WHERE 条件
            if (evaluateExpression(condition, *current_tuple, tables))
            {
                return current_tuple;
            }
        } while (current_tuple != nullptr);

        return nullptr;
    }
}

// ProjectOperator (迭代模型)
std::unique_ptr<Tuple> ProjectOperator::next()
{
    if (table_name.empty() || tables.empty()) {
        // 旧版本：直接返回子算子的结果，因为FilterOperator已经处理了列选择和条件过滤
        return child->next();
    } else {
        // 新版本：使用tables映射进行列投影
        std::unique_ptr<Tuple> input_tuple = child->next();
        if (input_tuple == nullptr)
        {
            return nullptr; // 没有更多数据了
        }

        const TableInfo &table_info = *tables.at(table_name);
        auto result_tuple = std::make_unique<Tuple>();

        for (const auto &col_name : columns)
        {
            // 查找列在原始元组中的索引
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
}

std::unique_ptr<Tuple> UpdateOperator::next()
{
    if (table_name.empty()) {
        // 旧版本：UpdateOperator未实现
        auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
        logger->Warn("UpdateOperator not implemented in old version");
        return nullptr;
    } else {
        // 新版本：使用内存存储
        // 获取需要更新的元组
        std::vector<Tuple> tuples_to_update;
        std::unique_ptr<Tuple> current_tuple;
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
        const TableInfo &table_info = catalog.getTable(table_name);

        // 找出所有更新操作的目标列的索引和对应的值
        std::vector<int> update_indices;
        std::vector<std::variant<int, std::string, double, bool>> update_values;

        for (const auto &update_pair : updates)
        {
            const std::string &col_name = update_pair.first;
            ASTNode *value_node = update_pair.second;

            // 查找列的索引
            int col_index = -1;
            for (size_t i = 0; i < table_info.column_order.size(); ++i)
            {
                if (table_info.column_order[i] == col_name)
                {
                    col_index = i;
                    break;
                }
            }
            if (col_index != -1)
            {
                update_indices.push_back(col_index);
                // 确保 evaluateLiteral 返回的 variant 类型与 update_values 的类型兼容
                // 需要在 utils.h 中统一 variant 类型
                update_values.push_back(evaluateLiteral(value_node));
            }
            else
            {
                auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
                logger->Warn("Column '{}' not found in table '{}' for update.", col_name, table_name);
            }
        }

        // 更新内存中的数据
        std::vector<Tuple> &table_data = in_memory_data[table_name];
        size_t updated_count = 0;

        // 假设 id 是主键
        int id_col_index = -1;
        for (size_t i = 0; i < table_info.column_order.size(); ++i)
        {
            if (table_info.column_order[i] == "id")
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
                // 通过 'id' 找到需要更新的元组
                if (std::get<int>(stored_tuple[id_col_index]) == std::get<int>(tuple_to_update[id_col_index]))
                {
                    // 更新元组中的值
                    for (size_t i = 0; i < update_indices.size(); ++i)
                    {
                        // 转换variant类型
                        const auto& val = update_values[i];
                        if (std::holds_alternative<int>(val)) {
                            stored_tuple[update_indices[i]] = std::get<int>(val);
                        } else if (std::holds_alternative<std::string>(val)) {
                            stored_tuple[update_indices[i]] = std::get<std::string>(val);
                        } else if (std::holds_alternative<bool>(val)) {
                            stored_tuple[update_indices[i]] = std::get<bool>(val);
                        } else if (std::holds_alternative<double>(val)) {
                            stored_tuple[update_indices[i]] = static_cast<int>(std::get<double>(val));
                        }
                    }
                    updated_count++;
                    break;
                }
            }
        }

        auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
        logger->Info("{} rows updated in table '{}'", updated_count, table_name);
        return nullptr;
    }
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
    switch (plan_root_->type) {
        case CREATE_TABLE_OP:
        case INSERT_OP:
        // case DELETE_OP:
        case UPDATE_OP:
            // 非迭代型操作，只需调用一次 next()
            logger->Info("你好1");
            plan_root_->next();
            break;
        case PROJECT_OP:
        case FILTER_OP:
        case SEQ_SCAN_OP:
        // 在这里添加其他迭代型操作，例如 JOIN_OP
            // 迭代型操作，循环调用 next() 直到没有更多数据
            while (true)
            {
                logger->Info("你好1");
                std::unique_ptr<Tuple> tuple = plan_root_->next();
                if (tuple == nullptr)
                {
                    break;
                }
                logger->Info("你好2");
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
SeqScanOperator::SeqScanOperator(const std::string &table_name, TableManager* table_manager)
    : table_name(table_name), table_manager_(table_manager)
{
    type = SEQ_SCAN_OP;
    current_row_index = 0; // 初始化
}

SeqScanOperator::SeqScanOperator(const std::string &table_name)
    : table_name(table_name), table_manager_(nullptr)
{
    type = SEQ_SCAN_OP;
    current_row_index = 0; // 初始化
}
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
