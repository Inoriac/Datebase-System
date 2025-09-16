// 定义算子类和执行计划树
#ifndef EXECUTION_PLAN_H
#define EXECUTION_PLAN_H

#include "../ast.h"
#include <vector>
#include <string>
#include <variant>
#include <memory>

// 前向声明
class TableManager;

// 代表一行数据，可以是任意类型
using Tuple = std::vector<std::variant<int, std::string, bool>>;

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
    CreateTableOperator(const std::string &table_name, const std::vector<std::pair<std::string, std::string>> &columns, TableManager* table_manager)
        : table_name(table_name), columns(columns), table_manager_(table_manager)
    {
        type = CREATE_TABLE_OP;
    }
    std::unique_ptr<Tuple> next() override;
    std::string table_name;
    std::vector<std::pair<std::string, std::string>> columns;
    TableManager* table_manager_;
};

// INSERT 算子
class InsertOperator : public Operator
{
public:
    InsertOperator(const std::string &table_name, const std::vector<std::variant<int, std::string, bool>> &values, TableManager* table_manager)
        : table_name(table_name), values(values), table_manager_(table_manager)
    {
        type = INSERT_OP;
    }
    std::unique_ptr<Tuple> next() override;
    std::string table_name;
    std::vector<std::variant<int, std::string, bool>> values;
    TableManager* table_manager_;
};

// 全表扫描（Sequential Scan）算子
class SeqScanOperator : public Operator
{
public:
    SeqScanOperator(const std::string &table_name, TableManager* table_manager);
    std::unique_ptr<Tuple> next() override;
    std::string table_name;
    size_t current_row_index = 0; // 表数据迭代索引
    std::vector<std::vector<std::variant<int, std::string, bool>>> all_records_; // 缓存所有记录

private:
    TableManager* table_manager_;
};

// 过滤（Filter）算子
class FilterOperator : public Operator
{
public:
    FilterOperator(std::unique_ptr<Operator> &&child_op, ASTNode *condition, TableManager* table_manager)
        : condition(condition), table_manager_(table_manager)
    {
        type = FILTER_OP;
        child = std::move(child_op);
        current_row_index = 0;
    }
    std::unique_ptr<Tuple> next() override;
    ASTNode *condition;
    
private:
    bool evaluateCondition(const Tuple& tuple, ASTNode* condition);
    TableManager* table_manager_;
    std::vector<Tuple> all_records_;
    size_t current_row_index;
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