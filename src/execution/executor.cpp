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
std::unique_ptr<Tuple> CreateTableOperator::next()
{
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
    return nullptr; // 没有数据行返回
}

// InsertOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> InsertOperator::next() {
    if (!catalog.tableExists(table_name)) {
        throw std::runtime_error("Table '" + table_name + "' does not exist.");
    }

    const TableInfo& table_info = catalog.getTable(table_name);
    Tuple final_tuple;

    if (column_names.empty()) {
        // 没有显式列名，按默认顺序插入
        final_tuple = values;
    } else {
        // 有显式列名，需要根据表定义重新排序值
        final_tuple.resize(table_info.column_order.size());
        std::unordered_map<std::string, LiteralValue> value_map;
        for (size_t i = 0; i < column_names.size(); ++i) {
            value_map[column_names[i]] = values[i];
        }

        for (size_t i = 0; i < table_info.column_order.size(); ++i) {
            const std::string& col_name = table_info.column_order[i];
            final_tuple[i] = value_map.at(col_name);
        }
    }

    in_memory_data[table_name].push_back(final_tuple);
    // ... (日志和返回代码)
    return nullptr;
}

// SeqScanOperator (迭代模型)
std::unique_ptr<Tuple> SeqScanOperator::next()
{
    if (current_row_index >= in_memory_data[table_name].size())
    {
        return nullptr;
    }
    // 返回当前行数据的副本，并移动到下一行
    auto result_tuple = std::make_unique<Tuple>(in_memory_data[table_name][current_row_index]);
    current_row_index++;
    return result_tuple;
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
        if (evaluateExpression(condition, *current_tuple, tables))
        {
            return current_tuple;
        }
    } while (current_tuple != nullptr);

    return nullptr;
}

// ProjectOperator (迭代模型)
std::unique_ptr<Tuple> ProjectOperator::next()
{
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

std::unique_ptr<Tuple> UpdateOperator::next()
{
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
                    stored_tuple[update_indices[i]] = update_values[i];
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
SeqScanOperator::SeqScanOperator(const std::string &table_name)
    : table_name(table_name)
{
    type = SEQ_SCAN_OP;
    current_row_index = 0; // 初始化
}
ProjectOperator::ProjectOperator(std::unique_ptr<Operator> &&child, const std::string &table_name, const std::vector<std::string> &columns, const std::unordered_map<std::string, const TableInfo *> &tables)
    : table_name(table_name), columns(columns), tables(tables)
{
    this->child = std::move(child);
    type = PROJECT_OP;
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

InsertOperator::InsertOperator(const std::string& table_name, Tuple values, std::vector<std::string> column_names)
    : table_name(table_name), values(std::move(values)), column_names(std::move(column_names)) {
    type = INSERT_OP;
}