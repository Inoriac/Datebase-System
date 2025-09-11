#include "ast.h"
#include "symbol_table.h"
#include <iostream>
#include <stdexcept>
#include <unordered_set>


bool typesMatch(const ASTNode *value_node, const std::string &column_type)
{
    if (value_node->type == INTEGER_LITERAL_NODE && column_type == "INT")
    {
        return true;
    }
    if (value_node->type == STRING_LITERAL_NODE && column_type == "STRING")
    {
        return true;
    }
    // 更多类型（如 FLOAT, DATE）可以在这里添加
    return false;
}

void check_create_statement(ASTNode *statement_node) {
    if (statement_node->children.size() < 2) {
        throw std::runtime_error("Malformed CREATE TABLE statement AST. Not enough children nodes.");
    }
    ASTNode *table_name_node = statement_node->children[0];
    std::string table_name = std::get<std::string>(table_name_node->value);

    if (tableExists(table_name)) {
        throw std::runtime_error("Table '" + table_name + "' already exists.");
    }

    ASTNode *col_definitions_node = statement_node->children[1];
    TableInfo new_table;
    new_table.name = table_name;
    std::unordered_set<std::string> column_names_set;

    for (ASTNode *col_node : col_definitions_node->children) {
        if (col_node->type != IDENTIFIER_NODE || col_node->children.empty()) {
            throw std::runtime_error("Invalid column definition. Expected an IDENTIFIER_NODE with a type child.");
        }
        
        std::string col_name = std::get<std::string>(col_node->value);

        if (column_names_set.count(col_name) > 0) {
            throw std::runtime_error("Duplicate column name '" + col_name + "' in CREATE TABLE statement.");
        }
        column_names_set.insert(col_name);

        ASTNode *col_type_node = col_node->children[0];

        if (col_type_node->type != DATA_TYPE_NODE) {
            throw std::runtime_error("Invalid node type in column definitions. Expected DATA_TYPE_NODE.");
        }

        std::string col_type = std::get<std::string>(col_type_node->value);
        
        new_table.columns[col_name] = {col_name, col_type};
        new_table.column_order.push_back(col_name);
    }
    symbol_table[table_name] = new_table;
    std::cout << "Semantic check passed for CREATE TABLE '" << table_name << "'.\n";
}


void check_insert_statement(ASTNode *statement_node)
{
    if (statement_node->children.size() < 2)
    {
        throw std::runtime_error("Malformed INSERT statement AST.");
    }

    ASTNode *table_name_node = statement_node->children[0];
    ASTNode *values_list_node = statement_node->children[1];

    std::string table_name = std::get<std::string>(table_name_node->value);

    // 1. 表存在性检查
    // 这一步检查要插入的表是否存在于符号表中
    if (!tableExists(table_name))
    {
        throw std::runtime_error("Table '" + table_name + "' does not exist.");
    }

    const TableInfo &table_info = symbol_table.at(table_name);

    // 2. 列数/列序检查
    // 这一步检查 INSERT 的值数量是否与表中定义的列数匹配
    size_t value_count = values_list_node->children.size();
    size_t column_count = table_info.column_order.size();
    if (value_count != column_count)
    {
        throw std::runtime_error("Value count (" + std::to_string(value_count) +
                                 ") does not match column count (" + std::to_string(column_count) + ") in table '" + table_name + "'.");
    }

    // 3. 类型一致性检查
    // 这一步遍历每个值，检查其类型是否与对应列的类型匹配
    for (size_t i = 0; i < value_count; ++i)
    {
        const ASTNode *value_node = values_list_node->children[i];
        const std::string &col_name = table_info.column_order[i];

        // 确保列在符号表中存在（虽然前面的列数检查已经包含了这部分逻辑）
        if (table_info.columns.count(col_name) == 0)
        {
            throw std::runtime_error("Column '" + col_name + "' does not exist in symbol table.");
        }

        const ColumnInfo &col_info = table_info.columns.at(col_name);

        if (!typesMatch(value_node, col_info.type))
        {
            throw std::runtime_error("Type mismatch for column '" + col_name +
                                     "'. Expected " + col_info.type + ", but got an incompatible value.");
        }
    }

    std::cout << "Semantic check passed for INSERT INTO '" << table_name << "'.\n";
}

// select
void check_select_statement(ASTNode *statement_node) {
    if (statement_node->children.size() < 2) {
        throw std::runtime_error("Malformed SELECT statement AST.");
    }

    ASTNode *select_list_node = statement_node->children[0];
    ASTNode *from_node = statement_node->children[1];

    if (from_node->type != IDENTIFIER_NODE) {
        throw std::runtime_error("Invalid FROM clause. Expected a table name.");
    }
    std::string table_name = std::get<std::string>(from_node->value);

    // 1. 表存在性检查
    if (!tableExists(table_name)) {
        throw std::runtime_error("Table '" + table_name + "' does not exist.");
    }
    const TableInfo &table_info = symbol_table.at(table_name);

    // 2. 列存在性检查
    for (ASTNode *col_node : select_list_node->children) {
        if (col_node->type != IDENTIFIER_NODE) {
            throw std::runtime_error("Invalid column in SELECT list. Expected a column name.");
        }
        std::string col_name = std::get<std::string>(col_node->value);

        if (table_info.columns.count(col_name) == 0) {
            throw std::runtime_error("Column '" + col_name + "' does not exist in table '" + table_name + "'.");
        }
    }
    std::cout << "Semantic check passed for SELECT statement.\n";
}

// delete
void check_delete_statement(ASTNode *statement_node) {
    if (statement_node->children.size() < 2) {
        throw std::runtime_error("Malformed DELETE statement AST.");
    }

    ASTNode *from_node = statement_node->children[0];
    ASTNode *where_node = statement_node->children[1];

    if (from_node->type != IDENTIFIER_NODE) {
        throw std::runtime_error("Invalid FROM clause. Expected a table name.");
    }
    std::string table_name = std::get<std::string>(from_node->value);

    // 1. 表存在性检查
    if (!tableExists(table_name)) {
        throw std::runtime_error("Table '" + table_name + "' does not exist.");
    }
    const TableInfo &table_info = symbol_table.at(table_name);

    // 2. 类型一致性检查 (WHERE 子句)
    // 假设 WHERE 子句的 AST 结构为:
    // WHERE_CLAUSE -> EQ_OPERATOR -> [IDENTIFIER_NODE (column), LITERAL_NODE (value)]
    ASTNode *col_node = where_node->children[0]->children[0];
    ASTNode *value_node = where_node->children[0]->children[1];

    if (col_node->type != IDENTIFIER_NODE) {
        throw std::runtime_error("Invalid column in WHERE clause.");
    }
    std::string col_name = std::get<std::string>(col_node->value);

    if (table_info.columns.count(col_name) == 0) {
        throw std::runtime_error("Column '" + col_name + "' does not exist in table '" + table_name + "'.");
    }

    const ColumnInfo &col_info = table_info.columns.at(col_name);
    
    // 检查列类型和值的类型是否匹配
    if (!typesMatch(value_node, col_info.type)) {
        throw std::runtime_error("Type mismatch in WHERE clause for column '" + col_name + "'.");
    }
    
    std::cout << "Semantic check passed for DELETE statement.\n";
}

void semantic_analysis(ASTNode *root_node) {
    if (!root_node) {
        return;
    }

    std::cout << "--- 正在语义分析 ---\n";
    for (ASTNode *statement_node : root_node->children) {
        switch (statement_node->type) {
            case CREATE_TABLE_STMT:
                check_create_statement(statement_node);
                std::cout << "--- check_create_statement语义分析通过 ---\n";
                break;
            case INSERT_STMT:
                check_insert_statement(statement_node);
                std::cout << "--- check_insert_statement语义分析通过 ---\n";
                break;
            case SELECT_STMT:
                check_select_statement(statement_node);
                std::cout << "--- check_select_statement语义分析通过 ---\n";
                break;
            case DELETE_STMT:
                check_delete_statement(statement_node);
                std::cout << "--- check_delete_statement语义分析通过 ---\n";
                break;
        }
    }
}