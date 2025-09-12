// 定义算子类和执行计划树
#ifndef EXECUTION_PLAN_H
#define EXECUTION_PLAN_H

#include "ast.h"
#include <vector>
#include <string>
#include <variant>
#include <memory>

// 代表一行数据，可以是任意类型
using Tuple = std::vector<std::variant<int, std::string>>;

// 算子类型
enum OperatorType
{
    CREATE_TABLE_OP,
    INSERT_OP,
    SEQ_SCAN_OP,
    FILTER_OP,
    PROJECT_OP,
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
class InsertOperator : public Operator
{
public:
    InsertOperator(const std::string &table_name, const std::vector<std::variant<int, std::string>> &values)
        : table_name(table_name), values(values)
    {
        type = INSERT_OP;
    }
    std::unique_ptr<Tuple> next() override;
    std::string table_name;
    std::vector<std::variant<int, std::string>> values;
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
    FilterOperator(std::unique_ptr<Operator> &&child_op, ASTNode *condition)
        : condition(condition)
    {
        type = FILTER_OP;
        child = std::move(child_op);
    }
    std::unique_ptr<Tuple> next() override;
    ASTNode *condition;
};

// 投影（Project）算子
class ProjectOperator : public Operator
{
public:
    ProjectOperator(std::unique_ptr<Operator> &&child_op, const std::vector<std::string> &columns);
    std::unique_ptr<Tuple> next() override;
    std::vector<std::string> columns;

private:
};

#endif