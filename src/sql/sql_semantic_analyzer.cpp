#include "ast.h"
#include "symbol_table.h"
#include "semantic_error.h" // 包含自定义异常类
#include <iostream>
#include <unordered_set>
#include <string>
#include <variant>

// 辅助函数：检查AST节点值类型和表列类型是否匹配
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
// 辅助函数
void check_where_clause(ASTNode *node, const TableInfo &table_info)
{
    if (!node)
        return;

    // 如果是比较操作符节点（>，<，=等）
    if (node->type == GREATER_THAN_OPERATOR || node->type == LESS_THAN_OPERATOR ||
        node->type == EQUAL_OPERATOR || node->type == GREATER_THAN_OR_EQUAL_OPERATOR ||
        node->type == LESS_THAN_OR_EQUAL_OPERATOR)
    {

        // 检查左侧操作数（通常是列名）
        ASTNode *left_operand = node->children[0];
        ASTNode *right_operand = node->children[0];
        // 左侧操作数为列名 看他类型是否为标志赴
        if (left_operand->type == IDENTIFIER_NODE)
        {
            std::string col_name = std::get<std::string>(left_operand->value);
            // 看这个列是否存在
            if (table_info.columns.count(col_name) == 0)
            {
                throw SemanticError(SemanticError::COLUMN_NOT_FOUND, "Column '" + col_name + "' does not exist in the table.");
            }
            std::string col_type = table_info.columns.at(col_name).type;

            // 2. 检查右侧操作数（值）
            std::string value_type;
            if (right_operand->type == INTEGER_LITERAL_NODE)
            {
                value_type = "INT";
            }
            else if (right_operand->type == STRING_LITERAL_NODE)
            {
                value_type = "STRING";
            }
            else if (right_operand->type == IDENTIFIER_NODE)
            {
                // 如果右侧也是一个列名，需要检查它是否存在并获取类型
                // WHERE salary > bonus 比较两列的值 ()
                // todo 功能
                std::string right_col_name = std::get<std::string>(right_operand->value);
                if (table_info.columns.count(right_col_name) == 0)
                {
                    throw SemanticError(SemanticError::COLUMN_NOT_FOUND, "Column '" + right_col_name + "' does not exist in the table.");
                }
                value_type = table_info.columns.at(right_col_name).type;
            }
            else
            {
                throw SemanticError(SemanticError::SYNTAX_ERROR, "Unsupported operand type in WHERE clause.");
            }

            // 3. 比较类型是否兼容
            if (col_type != value_type)
            {
                throw SemanticError(SemanticError::TYPE_MISMATCH, "Cannot compare column '" + col_name + "' (" + col_type + ") with value of type " + value_type + ".");
            }
        }

        // 递归检查右侧操作数（可能是另一个列名或常量）
        check_where_clause(node->children[1], table_info);
    }
}

// 检查 CREATE TABLE 语句
void check_create_statement(ASTNode *statement_node)
{
    if (statement_node->children.size() < 2)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed CREATE TABLE statement AST. Not enough children nodes.");
    }
    ASTNode *table_name_node = statement_node->children[0];
    std::string table_name = std::get<std::string>(table_name_node->value);

    if (catalog.tableExists(table_name))
    {
        throw SemanticError(SemanticError::DUPLICATE_TABLE, "Table '" + table_name + "' already exists.");
    }

    ASTNode *col_definitions_node = statement_node->children[1];
    TableInfo new_table;
    new_table.name = table_name;
    std::unordered_set<std::string> column_names_set;

    for (ASTNode *col_node : col_definitions_node->children)
    {
        if (col_node->type != IDENTIFIER_NODE || col_node->children.empty())
        {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid column definition. Expected an IDENTIFIER_NODE with a type child.");
        }

        std::string col_name = std::get<std::string>(col_node->value);

        if (column_names_set.count(col_name) > 0)
        {
            throw SemanticError(SemanticError::DUPLICATE_COLUMN, "Duplicate column name '" + col_name + "' in CREATE TABLE statement.");
        }
        column_names_set.insert(col_name);

        ASTNode *col_type_node = col_node->children[0];

        if (col_type_node->type != DATA_TYPE_NODE)
        {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid node type in column definitions. Expected DATA_TYPE_NODE.");
        }

        std::string col_type = std::get<std::string>(col_type_node->value);

        new_table.columns[col_name] = {col_name, col_type};
        new_table.column_order.push_back(col_name);
    }
    catalog.addTable(new_table);
    std::cout << "Semantic check passed for CREATE TABLE '" << table_name << "'.\n";
}

// 检查 INSERT 语句
void check_insert_statement(ASTNode *statement_node)
{
    if (statement_node->children.size() < 2)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed INSERT statement AST.");
    }
    ASTNode *table_name_node = statement_node->children[0];
    ASTNode *values_list_node = statement_node->children[1];
    std::string table_name = std::get<std::string>(table_name_node->value);

    if (!catalog.tableExists(table_name))
    {
        throw SemanticError(SemanticError::TABLE_NOT_FOUND, "Table '" + table_name + "' does not exist.");
    }
    const TableInfo &table_info = catalog.getTable(table_name);
    size_t value_count = values_list_node->children.size(); // 2
    size_t column_count = table_info.column_order.size();   // 3
    if (value_count != column_count)
    {
        throw SemanticError(SemanticError::TYPE_MISMATCH, "Value count (" + std::to_string(value_count) + ") does not match column count (" + std::to_string(column_count) + ") in table '" + table_name + "'.");
    }

    for (size_t i = 0; i < value_count; ++i)
    {
        const ASTNode *value_node = values_list_node->children[i];
        const std::string &col_name = table_info.column_order[i];

        if (table_info.columns.count(col_name) == 0)
        {
            throw SemanticError(SemanticError::COLUMN_NOT_FOUND, "Column '" + col_name + "' does not exist in symbol table.");
        }
        const ColumnInfo &col_info = table_info.columns.at(col_name);
        if (!typesMatch(value_node, col_info.type))
        {
            throw SemanticError(SemanticError::TYPE_MISMATCH, "Type mismatch for column '" + col_name + "'. Expected " + col_info.type + ", but got an incompatible value.");
        }
    }
    std::cout << "Semantic check passed for INSERT INTO '" + table_name + "'.\n";
}

// 检查 SELECT 语句
void check_select_statement(ASTNode *statement_node)
{
    if (statement_node->children.size() < 2)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed SELECT statement AST.");
    }
    ASTNode *select_list_node = statement_node->children[0];
    ASTNode *from_node = statement_node->children[1];
    if (from_node->type != FROM_CLAUSE)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid FROM clause. Expected a table name.");
    }
    std::string table_name = std::get<std::string>(from_node->value);

    if (!catalog.tableExists(table_name))
    {
        throw SemanticError(SemanticError::TABLE_NOT_FOUND, "Table '" + table_name + "' does not exist.");
    }
    const TableInfo &table_info = catalog.getTable(table_name);

    if (statement_node->children[2])
    {
        // 中间节点 遍历中间节点 一个节点三个属性 id(列名) >(操作符) 8(value)
        ASTNode *where_clause_node = statement_node->children[2];
        if (where_clause_node->type != WHERE_CLAUSE)
        {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid WHERE clause.");
        }
        // 递归检查 WHERE 子句中的所有列名
        check_where_clause(where_clause_node->children[0], table_info);
    }

    for (ASTNode *col_node : select_list_node->children)
    {
        if (col_node->type != IDENTIFIER_NODE)
        {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid column in SELECT list. Expected a column name.");
        }
        std::string col_name = std::get<std::string>(col_node->value);
        if (table_info.columns.count(col_name) == 0)
        {
            throw SemanticError(SemanticError::COLUMN_NOT_FOUND, "Column '" + col_name + "' does not exist in table '" + table_name + "'.");
        }
    }
    std::cout << "Semantic check passed for SELECT statement.\n";
}

// 检查 DELETE 语句
void check_delete_statement(ASTNode *statement_node)
{
    if (statement_node->children.size() < 2)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Malformed DELETE statement AST.");
    }
    ASTNode *from_node = statement_node->children[0];
    ASTNode *where_node = statement_node->children[1];
    if (from_node->type != FROM_CLAUSE)
    {
        std::cout << from_node->type << "\n";
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid FROM clause. Expected a table name.");
    }
    std::string table_name = std::get<std::string>(from_node->value);

    if (!catalog.tableExists(table_name))
    {
        throw SemanticError(SemanticError::TABLE_NOT_FOUND, "Table '" + table_name + "' does not exist.");
    }
    const TableInfo &table_info = catalog.getTable(table_name);

    ASTNode *col_node = where_node->children[0]->children[0];
    ASTNode *value_node = where_node->children[0]->children[1];
    if (col_node->type != IDENTIFIER_NODE)
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid column in WHERE clause.");
    }
    std::string col_name = std::get<std::string>(col_node->value);

    if (table_info.columns.count(col_name) == 0)
    {
        throw SemanticError(SemanticError::COLUMN_NOT_FOUND, "Column '" + col_name + "' does not exist in table '" + table_name + "'.");
    }
    const ColumnInfo &col_info = table_info.columns.at(col_name);

    if (!typesMatch(value_node, col_info.type))
    {
        throw SemanticError(SemanticError::TYPE_MISMATCH, "Type mismatch in WHERE clause for column '" + col_name + "'.");
    }
    std::cout << "Semantic check passed for DELETE statement.\n";
}

// 主语义分析入口
void semantic_analysis(ASTNode *root_node)
{
    if (!root_node)
    {
        return;
    }
    std::cout << "--- 正在语义分析 ---\n";
    for (ASTNode *statement_node : root_node->children)
    {
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
            }
        }
        catch (const SemanticError &e)
        {
            std::cerr << "语义分析失败: " << e.what() << " (错误类型: " << e.getType() << ":" << e.getErrorDescription() << ")\n";
            // 可以在这里添加位置信息，但需要AST节点包含行/列号
            // 例如：std::cerr << "位置: Line " << statement_node->line << "\n";
            // 停止对剩余语句的分析
            return;
        }
        catch (const std::exception &e)
        {
            std::cerr << "发生意外错误: " << e.what() << "\n";
            return;
        }
    }
}