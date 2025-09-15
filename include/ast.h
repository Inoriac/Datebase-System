#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <variant>

struct Location
{
    int first_line = 0;
    int first_column = 0;
    int last_line = 0;
    int last_column = 0;
};

// 定义你的节点类型
enum ASTNodeType
{
    ROOT_NODE = 0,

    // 语句类型
    CREATE_TABLE_STMT = 1,
    INSERT_STMT = 2,
    SELECT_STMT = 3,
    DELETE_STMT = 4,

    EXPRESSION_NODE = 5,

    IDENTIFIER_NODE = 6,
    DATA_TYPE_NODE = 7,
    INTEGER_LITERAL_NODE = 8,
    STRING_LITERAL_NODE = 9,

    // 复合节点类型
    COLUMN_DEFINITIONS_LIST = 10,
    COLUMN_LIST = 11,
    VALUES_LIST = 12,
    SELECT_LIST = 13,

    WHERE_CLAUSE = 14, // where 14
    FROM_CLAUSE = 15,  // from 15

    EQUAL_OPERATOR = 16,                 //= 16
    GREATER_THAN_OPERATOR = 17,          //> 17
    LESS_THAN_OPERATOR = 18,             //< 18
    GREATER_THAN_OR_EQUAL_OPERATOR = 19, //>= 19
    LESS_THAN_OR_EQUAL_OPERATOR = 20,    //<= 20
    BINARY_EXPR = 21,                    // 二元表达式,value 为操作符类型，如 '=',左右子节点分别为操作数

    // 扩展
    UPDATE_STMT = 22,
    SET_CLAUSE = 23,
    JOIN_CLAUSE = 24,
    ORDER_BY_CLAUSE = 25,
    GROUP_BY_CLAUSE = 26,
    ON_CONDITION = 27,

};

// 定义类并内联实现方法
class ASTNode
{
public:
    ASTNodeType type;
    // 限定名
    std::variant<std::string, int, bool> value;
    std::vector<ASTNode *> children;

    Location location;

    ASTNode(ASTNodeType node_type, const std::string &node_value)
        : type(node_type), value(node_value) {}

    ASTNode(ASTNodeType node_type, const int node_value)
        : type(node_type), value(node_value) {}

    ASTNode(ASTNodeType node_type, const std::string node_value, const Location &loc)
        : type(node_type), value(node_value), location(loc) {}

    ~ASTNode()
    {
        for (ASTNode *child : children)
        {
            delete child;
        }
    }

    void addChild(ASTNode *child_node)
    {
        if (child_node)
        {
            children.push_back(child_node);
        }
    }
};

#endif