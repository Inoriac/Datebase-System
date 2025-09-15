#include "utils.h"
#include "log/log_config.h"
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

    auto logger = DatabaseSystem::Log::LogConfig::GetSQLLogger();

    // 打印当前节点的缩进
    std::string indent(depth * 2, ' ');

    // 打印节点类型和值
    std::string value_str;
    if (std::holds_alternative<std::string>(node->value))
    {
        value_str = ": " + std::get<std::string>(node->value);
    }
    else if (std::holds_alternative<int>(node->value))
    {
        value_str = ": " + std::to_string(std::get<int>(node->value));
    }

    // 打印节点的位置
    std::string location_str = "  (Line: " + std::to_string(node->location.first_line) +
                              ", Column: " + std::to_string(node->location.first_column) + ")";

    logger->Debug("{}- {}{}{}", indent, nodeTypeToString(node->type), value_str, location_str);

    // 递归打印所有子节点
    for (ASTNode *child : node->children)
    {
        printAST(child, depth + 1);
    }
}


// 简单的 Levenshtein 距离算法实现
int get_edit_distance(const std::string& s1, const std::string& s2) {
    // ... (这里可以插入一个 Levenshtein 距离的实现，网上有很多示例) ...
    // ... (例如，使用一个动态规划的算法) ...
    const size_t m(s1.size()), n(s2.size());
    std::vector<int> costs(n + 1);

    std::iota(costs.begin(), costs.end(), 0);

    for (size_t i = 1; i <= m; ++i) {
        costs[0] = i;
        int last_diag = i - 1;
        for (size_t j = 1; j <= n; ++j) {
            int old_diag = costs[j];
            costs[j] = std::min({
                costs[j] + 1,
                costs[j-1] + 1,
                last_diag + (s1[i - 1] == s2[j - 1] ? 0 : 1)
            });
            last_diag = old_diag;
        }
    }
    return costs[n];
}
