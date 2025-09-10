#include "ast.h"
#include "symbol_table.h"
#include <iostream>
#include <stdexcept>

void check_create_statement(ASTNode* statement_node) {
    // 1. 获取表名节点
    if (statement_node->children.size() < 2) {
        throw std::runtime_error("Malformed CREATE TABLE statement AST.");
    }
    ASTNode* table_name_node = statement_node->children[0];
    std::string table_name = table_name_node->value;

    // 2. 检查表是否已存在
    if (tableExists(table_name)) {
        throw std::runtime_error("Table '" + table_name + "' already exists.");
    }

    // 3. 将新表信息添加到符号表
    ASTNode* col_definitions_node = statement_node->children[1];
    TableInfo new_table;
    new_table.name = table_name;

    for (ASTNode* col_node : col_definitions_node->children) {
        // 假设每个列节点是一个 IDENTIFIER_NODE，其值就是列名
        if (col_node->type != IDENTIFIER_NODE) {
            throw std::runtime_error("Invalid node type in column definitions.");
        }
        std::string col_name = col_node->value;
        // 注意: 简化处理，这里假设所有列都是STRING类型
        new_table.columns[col_name] = {col_name, "STRING"};
        new_table.column_order.push_back(col_name);
    }
    symbol_table[table_name] = new_table;
    std::cout << "Semantic check passed for CREATE TABLE '" << table_name << "'.\n";
}

void semantic_analysis(ASTNode* root_node) {
    if (!root_node) {
        return; // 空 AST
    }

    // 遍历 AST
    for (ASTNode* statement_node : root_node->children) {
        switch (statement_node->type) {
            case CREATE_TABLE_STMT:
                check_create_statement(statement_node);
                break;
            // case INSERT_STMT:
            //     check_insert_statement(statement_node);
            //     break;
            // case SELECT_STMT:
            //     check_select_statement(statement_node);
            //     break;
            // ...
        }
    }
}

