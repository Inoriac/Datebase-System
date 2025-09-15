#include "utils.h"
#include <iostream>
#include <variant>

std::string nodeTypeToString(ASTNodeType type)
{
    switch (type)
    {
    case ROOT_NODE:
        return "ROOT_NODE";
    case CREATE_TABLE_STMT:
        return "CREATE_TABLE_STMT";
    case INSERT_STMT:
        return "INSERT_STMT";
    case SELECT_STMT:
        return "SELECT_STMT";
    case DELETE_STMT:
        return "DELETE_STMT";
    case EXPRESSION_NODE:
        return "EXPRESSION_NODE";
    case IDENTIFIER_NODE:
        return "IDENTIFIER_NODE";
    case DATA_TYPE_NODE:
        return "DATA_TYPE_NODE";
    case INTEGER_LITERAL_NODE:
        return "INTEGER_LITERAL_NODE";
    case STRING_LITERAL_NODE:
        return "STRING_LITERAL_NODE";
    case COLUMN_DEFINITIONS_LIST:
        return "COLUMN_DEFINITIONS_LIST";
    case COLUMN_LIST:
        return "COLUMN_LIST";
    case VALUES_LIST:
        return "VALUES_LIST";
    case SELECT_LIST:
        return "SELECT_LIST";
    case WHERE_CLAUSE:
        return "WHERE_CLAUSE";
    case FROM_CLAUSE:
        return "FROM_CLAUSE";
    case EQUAL_OPERATOR:
        return "EQUAL_OPERATOR";
    case GREATER_THAN_OPERATOR:
        return "GREATER_THAN_OPERATOR";
    case LESS_THAN_OPERATOR:
        return "LESS_THAN_OPERATOR";
    case GREATER_THAN_OR_EQUAL_OPERATOR:
        return "GREATER_THAN_OR_EQUAL_OPERATOR";
    case LESS_THAN_OR_EQUAL_OPERATOR:
        return "LESS_THAN_OR_EQUAL_OPERATOR";
    case BINARY_EXPR:
        return "BINARY_EXPR";
    case UPDATE_STMT:
        return "UPDATE_STMT";
    case SET_CLAUSE:
        return "SET_CLAUSE";
    case JOIN_CLAUSE:
        return "JOIN_CLAUSE";
    case ORDER_BY_CLAUSE:
        return "ORDER_BY_CLAUSE";
    case GROUP_BY_CLAUSE:
        return "GROUP_BY_CLAUSE";
    case ON_CONDITION:
        return "ON_CONDITION";
    default:
        return "UNKNOWN_NODE";
    }
}

void printAST(ASTNode *node, int depth)
{
    if (!node)
    {
        return;
    }

    // 打印当前节点的缩进
    for (int i = 0; i < depth; ++i)
    {
        std::cout << "  ";
    }

    // 打印节点类型和值
    std::cout << "- " << nodeTypeToString(node->type);
    if (std::holds_alternative<std::string>(node->value))
    {
        std::cout << ": " << std::get<std::string>(node->value);
    }
    else if (std::holds_alternative<int>(node->value))
    {
        std::cout << ": " << std::get<int>(node->value);
    }
    // 打印节点的位置
    std::cout << "  (Line: " << node->location.first_line
              << ", Column: " << node->location.first_column << ")" << std::endl;

    // std::cout << std::endl;

    // 递归打印所有子节点
    for (ASTNode *child : node->children)
    {
        printAST(child, depth + 1);
    }
}