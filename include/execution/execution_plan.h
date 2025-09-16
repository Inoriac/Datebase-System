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
    UPDATE_OP, // <-- 新增
    DELETE_OP, // <-- 新增
    JOIN_OP,   // <-- 新增
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

class InsertOperator : public Operator
{
public:
    // 增加一个列列表参数，可以为空
    InsertOperator(const std::string &table_name,
                   const std::vector<std::string>& columns,
                   const std::vector<std::variant<int, std::string>> &values)
        : table_name(table_name), columns(columns), values(values)
    {
        type = INSERT_OP;
    }
    std::unique_ptr<Tuple> next() override;
    std::string table_name;
    std::vector<std::string> columns; // <-- 新增
    std::vector<std::variant<int, std::string>> values;
    std::vector<std::variant<int, std::string, bool>> values;
    TableManager* table_manager_;
};


// 更新（Update）算子
class UpdateOperator : public Operator {
public:
    // child_op 是 filtered rows, updates 是 SET 子句中的赋值表达式
    UpdateOperator(std::unique_ptr<Operator>&& child_op, const std::vector<std::pair<std::string, ASTNode*>>& updates)
     : updates(updates) {
        type = UPDATE_OP;
        child = std::move(child_op);
    }
    std::unique_ptr<Tuple> next() override;
    std::vector<std::pair<std::string, ASTNode*>> updates;
};

// 删除（Delete）算子
class DeleteOperator : public Operator {
public:
    // child_op 提供了需要被删除的行
    DeleteOperator(std::unique_ptr<Operator>&& child_op) {
        type = DELETE_OP;
        child = std::move(child_op);
    }
    std::unique_ptr<Tuple> next() override;
};

// 连接（Join）算子
class JoinOperator : public Operator {
public:
    std::unique_ptr<Operator> left;  // 左子节点
    std::unique_ptr<Operator> right; // 右子节点
    ASTNode* condition;              // 连接条件

    // Join 算子没有单一下级 child，而是 left 和 right
    JoinOperator(std::unique_ptr<Operator>&& left, std::unique_ptr<Operator>&& right, ASTNode* condition)
        : left(std::move(left)), right(std::move(right)), condition(condition) {
        type = JOIN_OP;
    }
    std::unique_ptr<Tuple> next() override;
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