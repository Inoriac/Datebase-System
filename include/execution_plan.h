#ifndef EXECUTION_PLAN_H
#define EXECUTION_PLAN_H

#include "ast.h"
#include "symbol_table.h" // 需要访问 symbol table
#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <unordered_map>

// 代表一行数据，可以是任意类型
using Tuple = std::vector<std::variant<int, std::string, double, bool>>;

// 算子类型
enum OperatorType
{
    CREATE_TABLE_OP,
    INSERT_OP,
    SEQ_SCAN_OP,
    FILTER_OP,
    PROJECT_OP,
    UPDATE_OP,
    JOIN_OP,
    ORDER_BY_OP,
    GROUP_BY_OP,
    INVALID_OP
};

// 所有算子的基类
class Operator
{
public:
    virtual ~Operator() = default;
    virtual std::unique_ptr<Tuple> next() = 0;

    OperatorType type = INVALID_OP;
    std::unique_ptr<Operator> child = nullptr;
    std::unique_ptr<Operator> right_child = nullptr; // 用于 JOIN 等二元算子
};

// CREATE TABLE 算子
class CreateTableOperator : public Operator
{
public:
    CreateTableOperator(const std::string &table_name, const std::vector<std::pair<std::string, std::string>> &columns)
        : table_name(table_name), columns(columns)
    {
        type = CREATE_TABLE_OP;
    }
    std::unique_ptr<Tuple> next() override;
    std::string table_name;
    std::vector<std::pair<std::string, std::string>> columns;
};

// INSERT 算子
class InsertOperator : public Operator {
public:
    // 新增构造函数参数
    std::string table_name;
    InsertOperator(const std::string& table_name, Tuple values, std::vector<std::string> column_names);
    std::unique_ptr<Tuple> next() override;

private:
    Tuple values;
    std::vector<std::string> column_names; // 存储列名
};

// 全表扫描（Sequential Scan）算子
class SeqScanOperator : public Operator
{
public:
    SeqScanOperator(const std::string &table_name);
    std::unique_ptr<Tuple> next() override;
    std::string table_name;
    size_t current_row_index = 0; // 模拟表数据迭代

private:
};

// 过滤（Filter）算子
class FilterOperator : public Operator
{
public:
    FilterOperator(std::unique_ptr<Operator> &&child_op, ASTNode *condition, const std::unordered_map<std::string, const TableInfo *> &tables)
        : condition(condition), tables(tables)
    {
        type = FILTER_OP;
        child = std::move(child_op);
    }
    std::unique_ptr<Tuple> next() override;
    ASTNode *condition;
    const std::unordered_map<std::string, const TableInfo *> &tables;
};

// 投影（Project）算子
class ProjectOperator : public Operator
{
public:
    ProjectOperator(std::unique_ptr<Operator> &&child_op, const std::string &table_name, const std::vector<std::string> &columns, const std::unordered_map<std::string, const TableInfo *> &tables);
    std::unique_ptr<Tuple> next() override;
    std::string table_name;
    std::vector<std::string> columns;
    const std::unordered_map<std::string, const TableInfo *> &tables;
};

// UPDATE 算子
class UpdateOperator : public Operator
{
public:
    UpdateOperator(std::unique_ptr<Operator> &&child_op, const std::string &table_name, const std::vector<std::pair<std::string, ASTNode *>> &updates)
        : table_name(table_name), updates(updates)
    {
        type = UPDATE_OP;
        child = std::move(child_op);
    }
    std::unique_ptr<Tuple> next() override;
    std::string table_name;
    std::vector<std::pair<std::string, ASTNode *>> updates;
};

// JOIN 算子
class JoinOperator : public Operator
{
public:
    JoinOperator(std::unique_ptr<Operator> &&left_child, std::unique_ptr<Operator> &&right_child, ASTNode *on_condition)
        : on_condition(on_condition)
    {
        type = JOIN_OP;
        child = std::move(left_child);
        this->right_child = std::move(right_child);
    }
    std::unique_ptr<Tuple> next() override;
    ASTNode *on_condition;
    // TODO: 实现更复杂的 Join 逻辑，例如哈希表和缓存
};

// ORDER BY 算子
class OrderByOperator : public Operator
{
public:
    OrderByOperator(std::unique_ptr<Operator> &&child_op, const std::vector<std::pair<std::string, bool>> &order_columns)
        : order_columns(order_columns)
    {
        type = ORDER_BY_OP;
        child = std::move(child_op);
    }
    std::unique_ptr<Tuple> next() override;

private:
    std::vector<std::pair<std::string, bool>> order_columns;
};

// GROUP BY 算子
class GroupByOperator : public Operator
{
public:
    GroupByOperator(std::unique_ptr<Operator> &&child_op, const std::vector<std::string> &group_columns)
        : group_columns(group_columns)
    {
        type = GROUP_BY_OP;
        child = std::move(child_op);
    }
    std::unique_ptr<Tuple> next() override;

private:
    std::vector<std::string> group_columns;
};

#endif