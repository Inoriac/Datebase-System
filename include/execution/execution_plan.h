// 定义算子类和执行计划树
#ifndef EXECUTION_PLAN_H
#define EXECUTION_PLAN_H

#include "../ast.h"
#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <unordered_map>

// 前向声明
class TableManager;
struct TableInfo;

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
    DELETE_OP,
    JOIN_OP,
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
    // 兼容两种构造函数
    CreateTableOperator(const std::string &table_name, const std::vector<std::pair<std::string, std::string>> &columns, TableManager* table_manager = nullptr)
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
    // 兼容三种构造函数
    // 1. 新版本：使用Tuple和column_names
    InsertOperator(const std::string &table_name, const std::vector<std::variant<int, std::string, double, bool>>& values, std::vector<std::string> column_names)
        : table_name(table_name), column_names(std::move(column_names)), table_manager_(nullptr)
    {
        type = INSERT_OP;
        // 直接复制values，类型已经匹配
        this->values = values;
    }
    
    // 1.1. 新版本：使用Tuple、column_names和TableManager
    InsertOperator(const std::string &table_name, const std::vector<std::variant<int, std::string, double, bool>>& values, std::vector<std::string> column_names, TableManager* table_manager)
        : table_name(table_name), column_names(std::move(column_names)), table_manager_(table_manager)
    {
        type = INSERT_OP;
        // 直接复制values，类型已经匹配
        this->values = values;
    }
    
    // 2. 旧版本：使用TableManager
    InsertOperator(const std::string &table_name,
                   const std::vector<std::variant<int, std::string, double, bool>> &values,
                   TableManager* table_manager = nullptr)
        : table_name(table_name), values(values), table_manager_(table_manager)
    {
        type = INSERT_OP;
    }
    
    // 3. 旧版本：使用columns和values
    InsertOperator(const std::string &table_name,
                   const std::vector<std::string>& columns,
                   const std::vector<std::variant<int, std::string>> &values)
        : table_name(table_name), columns(columns), table_manager_(nullptr)
    {
        type = INSERT_OP;
        // 转换values类型，从旧版本variant到新版本variant
        for (const auto& val : values) {
            if (std::holds_alternative<int>(val)) {
                this->values.push_back(std::get<int>(val));
            } else if (std::holds_alternative<std::string>(val)) {
                this->values.push_back(std::get<std::string>(val));
            }
        }
    }
    
    std::unique_ptr<Tuple> next() override;
    std::string table_name;
    std::vector<std::string> columns; // 可选列列表
    std::vector<std::string> column_names; // 新版本列名
    std::vector<std::variant<int, std::string, double, bool>> values;
    TableManager* table_manager_;
};


// 更新（Update）算子
class UpdateOperator : public Operator {
public:
    // 兼容两种构造函数
    // 1. 新版本：包含table_name和TableManager
    UpdateOperator(std::unique_ptr<Operator>&& child_op, const std::string& table_name, const std::vector<std::pair<std::string, ASTNode*>>& updates, TableManager* table_manager = nullptr)
     : table_name(table_name), updates(updates), table_manager_(table_manager) {
        type = UPDATE_OP;
        child = std::move(child_op);
    }
    
    // 2. 旧版本：不包含table_name
    UpdateOperator(std::unique_ptr<Operator>&& child_op, const std::vector<std::pair<std::string, ASTNode*>>& updates)
     : updates(updates) {
        type = UPDATE_OP;
        child = std::move(child_op);
    }
    
    std::unique_ptr<Tuple> next() override;
    std::string table_name; // 新版本需要表名
    std::vector<std::pair<std::string, ASTNode*>> updates;
    TableManager* table_manager_; // TableManager指针
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
    // 兼容两种构造函数
    SeqScanOperator(const std::string &table_name, TableManager* table_manager);
    SeqScanOperator(const std::string &table_name);
    
    std::unique_ptr<Tuple> next() override;
    std::string table_name;
    size_t current_row_index = 0; // 表数据迭代索引
    std::vector<Tuple> all_records_; // 缓存所有记录

private:
    TableManager* table_manager_;
};

// 过滤（Filter）算子
class FilterOperator : public Operator
{
public:
    // 兼容三种构造函数
    // 1. 新版本：使用tables映射
    FilterOperator(std::unique_ptr<Operator> &&child_op, ASTNode *condition, const std::unordered_map<std::string, const TableInfo*>& tables)
        : condition(condition), tables(tables), table_manager_(nullptr)
    {
        type = FILTER_OP;
        child = std::move(child_op);
        current_row_index = 0;
    }
    
    // 2. 旧版本：使用TableManager
    FilterOperator(std::unique_ptr<Operator> &&child_op, ASTNode *condition, TableManager* table_manager = nullptr)
        : condition(condition), table_manager_(table_manager)
    {
        type = FILTER_OP;
        child = std::move(child_op);
        current_row_index = 0;
    }
    
    // 3. 旧版本：不使用TableManager
    FilterOperator(std::unique_ptr<Operator> &&child_op, ASTNode *condition)
        : condition(condition), table_manager_(nullptr)
    {
        type = FILTER_OP;
        child = std::move(child_op);
        current_row_index = 0;
    }
    
    std::unique_ptr<Tuple> next() override;
    ASTNode *condition;

private:
    bool evaluateCondition(const Tuple& tuple, ASTNode* condition);
    std::unordered_map<std::string, const TableInfo*> tables; // 新版本表信息映射
    TableManager* table_manager_; // 旧版本TableManager
    std::vector<Tuple> all_records_;
    size_t current_row_index;
};

// 投影（Project）算子
class ProjectOperator : public Operator
{
public:
    // 兼容两种构造函数
    // 1. 新版本：包含table_name和tables映射
    ProjectOperator(std::unique_ptr<Operator> &&child_op, const std::string &table_name, const std::vector<std::string> &columns, const std::unordered_map<std::string, const TableInfo*>& tables)
        : table_name(table_name), columns(columns), tables(tables)
    {
        type = PROJECT_OP;
        child = std::move(child_op);
    }
    
    // 2. 旧版本：只包含columns
    ProjectOperator(std::unique_ptr<Operator> &&child_op, const std::vector<std::string> &columns)
        : columns(columns)
    {
        type = PROJECT_OP;
        child = std::move(child_op);
    }
    
    std::unique_ptr<Tuple> next() override;
    std::string table_name; // 新版本需要表名
    std::vector<std::string> columns;
    std::unordered_map<std::string, const TableInfo*> tables; // 新版本表信息映射

private:
};

#endif