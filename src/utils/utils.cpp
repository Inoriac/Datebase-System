#include "utils.h"
#include <stdexcept>
#include <sstream>

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


// 辅助函数：将字符串按分隔符分割
std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str); // 修复1：现在 std::istringstream 完整
    while (std::getline(tokenStream, token, delimiter)) { // 修复1：现在 std::getline 可以正常工作
        tokens.push_back(token);
    }
    return tokens;
}

LiteralValue evaluateLiteral(ASTNode* node) {
    if (!node) {
        throw std::runtime_error("Attempted to evaluate a null AST node.");
    }
    // 使用 std::visit 安全地访问 variant 的值
    return std::visit([](const auto& val) -> LiteralValue {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val;
        }
        throw std::runtime_error("Unsupported variant type in evaluateLiteral.");
    }, node->value);
}

// 评估 AST 节点的值（可以是字面量或列名）
LiteralValue evaluateValue(ASTNode* node, const Tuple& tuple, const std::unordered_map<std::string, const TableInfo*>& tables) {
    if (!node) {
        throw std::runtime_error("Attempted to evaluate a null AST node.");
    }

    switch (node->type) {
        case INTEGER_LITERAL_NODE:
        case STRING_LITERAL_NODE:
            return evaluateLiteral(node);
        case IDENTIFIER_NODE: {
            // 这里我们假设 IDENTIFIER_NODE 存储的是 "table.column" 或 "column"
            std::string full_col_name = std::get<std::string>(node->value);
            std::string table_name, col_name;
            size_t dot_pos = full_col_name.find('.');
            if (dot_pos != std::string::npos) {
                table_name = full_col_name.substr(0, dot_pos);
                col_name = full_col_name.substr(dot_pos + 1);
            } else {
                // 如果没有表名，从提供的表中查找
                if (tables.size() == 1) {
                    table_name = tables.begin()->first;
                    col_name = full_col_name;
                } else {
                    throw std::runtime_error("Ambiguous column name '" + full_col_name + "'.");
                }
            }
            
            const TableInfo& table_info = *tables.at(table_name);
            int col_index = -1;
            for (size_t i = 0; i < table_info.column_order.size(); ++i) {
                if (table_info.column_order[i] == col_name) {
                    col_index = i;
                    break;
                }
            }
            if (col_index != -1) {
                return tuple[col_index];
            }
            throw std::runtime_error("Column '" + full_col_name + "' not found.");
        }
        default:
            throw std::runtime_error("Unsupported node type for value evaluation.");
    }
}
bool evaluateExpression(ASTNode* node, const Tuple& tuple, const std::unordered_map<std::string, const TableInfo*>& tables) {
    if (node->children.size() != 2) {
        throw std::runtime_error("Malformed binary expression: requires two children.");
    }
    
    LiteralValue left_val = evaluateValue(node->children[0], tuple, tables);
    LiteralValue right_val = evaluateValue(node->children[1], tuple, tables);

    // 修复方案：在 std::visit 内部处理所有类型组合
    return std::visit([node](const auto& lhs, const auto& rhs) -> bool {
        // 使用 if constexpr 来在编译时进行类型检查
        if constexpr (std::is_same_v<decltype(lhs), decltype(rhs)>) {
            // 如果两个操作数类型相同，则可以安全地进行比较
            switch (node->type) {
                case EQUAL_OPERATOR:
                    return lhs == rhs;
                case NOT_EQUAL_OPERATOR:
                    return lhs != rhs;
                case GREATER_THAN_OPERATOR:
                    return lhs > rhs;
                case LESS_THAN_OPERATOR:
                    return lhs < rhs;
                case GREATER_THAN_OR_EQUAL_OPERATOR:
                    return lhs >= rhs;
                case LESS_THAN_OR_EQUAL_OPERATOR:
                    return lhs <= rhs;
                default:
                    throw std::runtime_error("Unsupported operator in WHERE clause.");
            }
        } else {
            // 如果两个操作数类型不同，根据操作符返回 false
            // 例如 '1' == 100 应该为 false
            // 除非是 '!='，应该为 true
            switch (node->type) {
                case NOT_EQUAL_OPERATOR:
                    return true;
                default:
                    return false;
            }
        }
    }, left_val, right_val);
}







