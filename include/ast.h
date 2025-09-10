#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

// 定义你的节点类型
enum ASTNodeType {
    SQL_STATEMENTS_LIST,
    CREATE_TABLE_STMT,
    INSERT_STMT,
    SELECT_STMT,
    DELETE_STMT,              // 新增: DELETE语句节点
    EXPRESSION_NODE,            // 新增: 通用表达式节点 (用于WHERE)
    IDENTIFIER_NODE,
    INTEGER_LITERAL_NODE,
    STRING_LITERAL_NODE,
    COLUMN_DEFINITIONS_LIST,
    COLUMN_LIST,                // 新增: 独立的列名列表节点
    VALUES_LIST
};

// 定义类并内联实现方法
class ASTNode {
public:
    ASTNodeType type;
    std::string value;
    std::vector<ASTNode*> children;

    ASTNode(ASTNodeType node_type, const std::string& node_value = "")
        : type(node_type), value(node_value) {}

    ~ASTNode() {
        for (ASTNode* child : children) {
            delete child;
        }
    }

    void addChild(ASTNode* child_node) {
        if (child_node) {
            children.push_back(child_node);
        }
    }
};

#endif