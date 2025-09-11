#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <variant>

// 定义你的节点类型
enum ASTNodeType
{
    ROOT_NODE,

    // 语句类型
    CREATE_TABLE_STMT,
    INSERT_STMT,
    SELECT_STMT,
    DELETE_STMT,              // 新增: DELETE语句节点
    
    // 叶子节点类型
    IDENTIFIER_NODE,
    DATA_TYPE_NODE,
    INTEGER_LITERAL_NODE,
    STRING_LITERAL_NODE,
    
    // 复合节点类型
    COLUMN_DEFINITIONS_LIST,
    COLUMN_LIST,                // 新增: 独立的列名列表节点
    VALUES_LIST,
    SELECT_LIST,  // 新增
    
    // 逻辑表达式节点类型
    WHERE_CLAUSE, // 新增
    EQUAL_OPERATOR,
    BINARY_EXPR // 二元表达式,value 为操作符类型，如 '=',左右子节点分别为操作数
};

// 定义类并内联实现方法
class ASTNode
{
public:
    ASTNodeType type;
    // 限定名
    std::variant<std::string, int, bool> value;
    std::vector<ASTNode *> children;

    ASTNode(ASTNodeType node_type, const std::string &node_value)
        : type(node_type), value(node_value) {}

    ASTNode(ASTNodeType node_type, const int node_value)
        : type(node_type), value(node_value) {}

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