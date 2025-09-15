#include "ast.h"
#include "symbol_table.h"
#include "semantic_error.h"
#include <iostream>
#include <unordered_set>
#include <string>
#include <variant>
#include <unordered_map>
#include "utils.h"

// 打印当前正在解析的语句的AST结构
void printCurrentStatementAST(ASTNode *statement_node)
{
    std::cout << "\n--- 正在解析的AST ---\n";
    printAST(statement_node, 0);
    std::cout << "---------------------\n\n";
}
bool typesMatch(const ASTNode *value_node, const std::string &column_type)
{
    if (value_node->type == INTEGER_LITERAL_NODE && (column_type == "INT" || column_type == "INTEGER"))
    {
        return true;
    }
    if (value_node->type == STRING_LITERAL_NODE && column_type == "STRING")
    {
        return true;
    }
    return false;
}

void check_column_exists(ASTNode *node, const std::unordered_map<std::string, const TableInfo *> &tables)
{
    if (node->type != IDENTIFIER_NODE)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Expected an IDENTIFIER_NODE.", node);
    }

    std::string col_name = std::get<std::string>(node->value);

    size_t dot_pos = col_name.find('.');
    std::string table_alias = "";
    std::string pure_col_name = col_name;

    if (dot_pos != std::string::npos)
    {
        table_alias = col_name.substr(0, dot_pos);
        pure_col_name = col_name.substr(dot_pos + 1);
    }

    bool column_found = false;
    for (const auto &pair : tables)
    {
        const std::string &current_table_name = pair.first;
        const TableInfo *table_info = pair.second;

        if (!table_alias.empty() && table_alias != current_table_name)
        {
            continue;
        }

        if (table_info->columns.count(pure_col_name) > 0)
        {
            column_found = true;
            break;
        }
    }

    if (!column_found)
    {
        throw SemanticError(SemanticError::COLUMN_NOT_FOUND, "Column '" + col_name + "' does not exist in the specified tables.", node);
    }
}
void check_where_clause(ASTNode *node, const std::unordered_map<std::string, const TableInfo *> &tables)
{
    if (!node)
    {
        return;
    }

    switch (node->type)
    {
    case BINARY_EXPR:
    case EQUAL_OPERATOR:
    case GREATER_THAN_OPERATOR:
    case LESS_THAN_OPERATOR:
    case GREATER_THAN_OR_EQUAL_OPERATOR:
    case LESS_THAN_OR_EQUAL_OPERATOR:
        if (node->children.size() != 2)
        {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed binary expression.", node);
        }
        check_where_clause(node->children[0], tables);
        check_where_clause(node->children[1], tables);
        break;

    case IDENTIFIER_NODE:
        check_column_exists(node, tables);
        break;

    case INTEGER_LITERAL_NODE:
    case STRING_LITERAL_NODE:
    case DATA_TYPE_NODE:
        break;

    default:
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid node type in WHERE clause.", node);
    }
}

void check_create_statement(ASTNode *statement_node)
{
    if (statement_node->children.size() < 2)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed CREATE TABLE statement AST. Not enough children nodes.", statement_node);
    }
    ASTNode *table_name_node = statement_node->children[0];
    std::string table_name = std::get<std::string>(table_name_node->value);

    if (catalog.tableExists(table_name))
    {
        throw SemanticError(SemanticError::DUPLICATE_TABLE, "Table '" + table_name + "' already exists.", table_name_node);
    }

    ASTNode *col_definitions_node = statement_node->children[1];
    TableInfo new_table;
    new_table.name = table_name;
    std::unordered_set<std::string> column_names_set;

    for (ASTNode *col_node : col_definitions_node->children)
    {
        if (col_node->type != IDENTIFIER_NODE || col_node->children.empty())
        {
            std::cout << "man.\n";
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid column definition. Expected an IDENTIFIER_NODE with a type child.", col_node);
            std::cout << "man.\n";
        }

        std::string col_name = std::get<std::string>(col_node->value);

        if (column_names_set.count(col_name) > 0)
        {
            throw SemanticError(SemanticError::DUPLICATE_COLUMN, "Duplicate column name '" + col_name + "' in CREATE TABLE statement.", col_node);
        }
        column_names_set.insert(col_name);

        ASTNode *col_type_node = col_node->children[0];

        if (col_type_node->type != DATA_TYPE_NODE)
        {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid node type in column definitions. Expected DATA_TYPE_NODE.", col_type_node);
        }

        std::string col_type = std::get<std::string>(col_type_node->value);

        new_table.columns[col_name] = {col_name, col_type};
        new_table.column_order.push_back(col_name);
    }
    catalog.addTable(new_table);
    std::cout << "Semantic check passed for CREATE TABLE '" << table_name << "'.\n";
}

void check_insert_statement(ASTNode *statement_node)
{
    // 检查AST结构，至少需要表名和VALUES子句
    if (statement_node->children.size() < 2)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed INSERT statement AST.", statement_node);
    }

    ASTNode *table_name_node = statement_node->children[0];
    std::string table_name = std::get<std::string>(table_name_node->value);

    // 检查表是否存在
    if (!catalog.tableExists(table_name))
    {
        throw SemanticError(SemanticError::TABLE_NOT_FOUND, "Table '" + table_name + "' does not exist.", table_name_node);
    }
    const TableInfo &table_info = catalog.getTable(table_name);
    
    // 提取值列表节点
    ASTNode *values_list_node = nullptr;

    // 检查是否存在显式列名列表
    if (statement_node->children.size() == 3)
    {
        // 存在显式列名列表的情况：
        // AST结构: [TABLE_NAME_NODE, COLUMN_LIST_NODE, VALUES_LIST_NODE]
        ASTNode *column_list_node = statement_node->children[1];
        values_list_node = statement_node->children[2];

        // 检查值的数量是否与显式列名数量匹配
        if (values_list_node->children.size() != column_list_node->children.size())
        {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Number of values does not match number of columns specified.", values_list_node);
        }

        // 遍历显式列名和值，进行语义检查
        for (size_t i = 0; i < column_list_node->children.size(); ++i)
        {
            ASTNode *col_name_node = column_list_node->children[i];
            const ASTNode *value_node = values_list_node->children[i];

            if (col_name_node->type != IDENTIFIER_NODE)
            {
                throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid column name in column list.", col_name_node);
            }
            std::string col_name = std::get<std::string>(col_name_node->value);

            // 检查列名是否存在于表中
            if (table_info.columns.find(col_name) == table_info.columns.end())
            {
                throw SemanticError(SemanticError::COLUMN_NOT_FOUND, "Column '" + col_name + "' does not exist in table '" + table_name + "'.", col_name_node);
            }

            // 检查值的类型是否与列的类型匹配
            const ColumnInfo &col_info = table_info.columns.at(col_name);
            if (!typesMatch(value_node, col_info.type))
            {
                throw SemanticError(SemanticError::TYPE_MISMATCH, "Type mismatch for column '" + col_name + "'. Expected " + col_info.type + ", but got an incompatible value.", value_node);
            }
        }
    }
    else if (statement_node->children.size() == 2)
    {
        // 隐式列名列表的情况：
        // AST结构: [TABLE_NAME_NODE, VALUES_LIST_NODE]
        values_list_node = statement_node->children[1];

        // 检查值的数量是否与表的所有列数匹配
        size_t value_count = values_list_node->children.size();
        size_t column_count = table_info.column_order.size();
        if (value_count != column_count)
        {
            throw SemanticError(SemanticError::TYPE_MISMATCH, "Value count (" + std::to_string(value_count) + ") does not match column count (" + std::to_string(column_count) + ") in table '" + table_name + "'.", values_list_node);
        }

        // 按顺序遍历值，并进行类型检查
        for (size_t i = 0; i < value_count; ++i)
        {
            const ASTNode *value_node = values_list_node->children[i];
            const std::string &col_name = table_info.column_order[i];

            if (table_info.columns.count(col_name) == 0)
            {
                throw SemanticError(SemanticError::COLUMN_NOT_FOUND, "Column '" + col_name + "' does not exist in symbol table.", value_node);
            }
            const ColumnInfo &col_info = table_info.columns.at(col_name);
            if (!typesMatch(value_node, col_info.type))
            {
                throw SemanticError(SemanticError::TYPE_MISMATCH, "Type mismatch for column '" + col_name + "'. Expected " + col_info.type + ", but got an incompatible value.", value_node);
            }
        }
    }
    else
    {
        // 异常的AST结构
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed INSERT statement AST structure.", statement_node);
    }
    
    std::cout << "Semantic check passed for INSERT INTO '" + table_name + "'.\n";
}

void check_select_statement(ASTNode *statement_node)
{
    if (statement_node->children.size() < 2)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed SELECT statement AST.", statement_node);
    }
    ASTNode *select_list_node = statement_node->children[0];
    ASTNode *from_node = statement_node->children[1];
    if (from_node->type != FROM_CLAUSE)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid FROM clause. Expected a table name.", from_node);
    }

    std::unordered_map<std::string, const TableInfo *> available_tables;
    std::string table_name = std::get<std::string>(from_node->value);

    if (!catalog.tableExists(table_name))
    {
        throw SemanticError(SemanticError::TABLE_NOT_FOUND, "Table '" + table_name + "' does not exist.", from_node);
    }
    available_tables[table_name] = &catalog.getTable(table_name);

    if (!from_node->children.empty())
    {
        for (ASTNode *join_node : from_node->children)
        {
            if (join_node->type != JOIN_CLAUSE)
                continue;

            if (join_node->children.empty())
            {
                throw SemanticError(SemanticError::SYNTAX_ERROR, "JOIN子句结构不完整。", join_node);
            }

            std::string joined_table_name = std::get<std::string>(join_node->children[0]->value);
            if (!catalog.tableExists(joined_table_name))
            {
                throw SemanticError(SemanticError::TABLE_NOT_FOUND, "连接的表 '" + joined_table_name + "' 不存在。", join_node->children[0]);
            }
            available_tables[joined_table_name] = &catalog.getTable(joined_table_name);

            if (join_node->children.size() > 1)
            {
                ASTNode *on_condition_node = join_node->children[1];
                if (on_condition_node->type != ON_CONDITION || on_condition_node->children.empty())
                {
                    throw SemanticError(SemanticError::SYNTAX_ERROR, "JOIN条件'ON'无效。", on_condition_node);
                }
                check_where_clause(on_condition_node->children[0], available_tables);
            }
            else
            {
                throw SemanticError(SemanticError::SYNTAX_ERROR, "JOIN子句缺少ON条件。", join_node);
            }
        }
    }

    for (size_t i = 2; i < statement_node->children.size(); ++i)
    {
        ASTNode *clause_node = statement_node->children[i];

        switch (clause_node->type)
        {
        case WHERE_CLAUSE:
            if (!clause_node->children.empty())
            {
                check_where_clause(clause_node->children[0], available_tables);
            }
            break;
        case GROUP_BY_CLAUSE:
            for (ASTNode *col_node : clause_node->children)
            {
                check_column_exists(col_node, available_tables);
            }
            break;
        case ORDER_BY_CLAUSE:
            for (ASTNode *col_node : clause_node->children)
            {
                check_column_exists(col_node, available_tables);
            }
            break;
        default:
            throw SemanticError(SemanticError::SYNTAX_ERROR, "SELECT语句中存在无法识别的子句。", clause_node);
        }
    }

    for (ASTNode *col_node : select_list_node->children)
    {
        check_column_exists(col_node, available_tables);
    }

    std::cout << "Semantic check passed for SELECT statement.\n";
}

void check_delete_statement(ASTNode *statement_node)
{
    std::unordered_map<std::string, const TableInfo *> available_tables;

    if (statement_node->children.size() < 1)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed DELETE statement AST.", statement_node);
    }

    ASTNode *table_name_node = statement_node->children[0];
    if (table_name_node->type != FROM_CLAUSE)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid Table clause. Expected a table name.", table_name_node);
    }
    std::string table_name = std::get<std::string>(table_name_node->value);

    if (!catalog.tableExists(table_name))
    {
        throw SemanticError(SemanticError::TABLE_NOT_FOUND, "Table '" + table_name + "' does not exist.", table_name_node);
    }

    available_tables[table_name] = &catalog.getTable(table_name);

    if (statement_node->children.size() > 1)
    {
        ASTNode *where_clause_node = statement_node->children[1];
        if (where_clause_node->type != WHERE_CLAUSE)
        {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid WHERE clause.", where_clause_node);
        }
        check_where_clause(where_clause_node->children[0], available_tables);
    }
    std::cout << "Semantic check passed for DELETE TABLE '" << table_name << "'.\n";
}

void check_update_statement(ASTNode *statement_node)
{
    std::unordered_map<std::string, const TableInfo *> available_tables;

    if (statement_node->children.size() < 2)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed UPDATE statement AST.", statement_node);
    }

    ASTNode *table_name_node = statement_node->children[0];
    if (table_name_node->type != IDENTIFIER_NODE)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid Table clause. Expected a table name.", table_name_node);
    }
    std::string table_name = std::get<std::string>(table_name_node->value);

    if (!catalog.tableExists(table_name))
    {
        throw SemanticError(SemanticError::TABLE_NOT_FOUND, "Table '" + table_name + "' does not exist.", table_name_node);
    }
    available_tables[table_name] = &catalog.getTable(table_name);

    ASTNode *set_node = statement_node->children[1];
    if (set_node->type != SET_CLAUSE)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed UPDATE statement. Expected SET clause.", set_node);
    }

    for (ASTNode *assignment_node : set_node->children)
    {
        if (assignment_node->children.size() != 2 || assignment_node->type != EQUAL_OPERATOR)
        {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed SET clause. Expected assignments.", assignment_node);
        }
        ASTNode *col_set_node = assignment_node->children[0];
        ASTNode *value_node = assignment_node->children[1];

        if (col_set_node->type != IDENTIFIER_NODE)
        {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid column in SET clause.", col_set_node);
        }

        std::string col_name = std::get<std::string>(col_set_node->value);
        check_column_exists(col_set_node, available_tables);

        const TableInfo &table_info = *available_tables.at(table_name);
        if (table_info.columns.count(col_name) == 0)
        {
            throw SemanticError(SemanticError::COLUMN_NOT_FOUND, "Column '" + col_name + "' does not exist in table '" + table_name + "'.", col_set_node);
        }

        const ColumnInfo &col_info = table_info.columns.at(col_name);
        if (!typesMatch(value_node, col_info.type))
        {
            throw SemanticError(SemanticError::TYPE_MISMATCH, "Type mismatch in SET clause for column '" + col_name + "'.", value_node);
        }
    }

    if (statement_node->children.size() > 2)
    {
        ASTNode *where_clause_node = statement_node->children[2];
        if (where_clause_node->type != WHERE_CLAUSE)
        {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid WHERE clause.", where_clause_node);
        }
        check_where_clause(where_clause_node->children[0], available_tables);
    }
    std::cout << "Semantic check passed for UPDATE statement.\n";
}

void semantic_analysis(ASTNode *root_node)
{
    if (!root_node)
    {
        return;
    }
    std::cout << "--- 正在语义分析 ---\n";
    for (ASTNode *statement_node : root_node->children)
    {
        printCurrentStatementAST(statement_node);
        try
        {
            switch (statement_node->type)
            {
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
            case UPDATE_STMT:
                check_update_statement(statement_node);
                std::cout << "--- check_update_statement语义分析通过 ---\n";
                break;
            }
        }
        catch (const SemanticError &e)
        {
            std::pair<int, int> location = e.getLocation();
            std::cerr << "语义分析失败: " << e.what() << " (错误类型: " << e.getType() << ":" << e.getErrorDescription() << ")";
            std::cerr << "  (行: " << location.first << ", 列: " << location.second << ")" << std::endl;

            // if (location.first > 0)
            // {
            //     std::cerr << "  (行: " << location.first << ", 列: " << location.second << ")" << std::endl;
            // }
            return;
        }
        catch (const std::exception &e)
        {
            std::cerr << "发生意外错误: " << e.what() << "\n";
            return;
        }
    }
}