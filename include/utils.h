#ifndef UTILS_H
#define UTILS_H
#include "ast.h"
#include "execution_plan.h"
#include "symbol_table.h"

#include <variant>
#include <iostream>

#include <string>
#include <vector>
#include <unordered_map>

#include <algorithm>
#include <numeric>


std::string nodeTypeToString(ASTNodeType type);
void printAST(ASTNode* node, int depth = 0);
int get_edit_distance(const std::string& s1, const std::string& s2);


// 定义一个别名，以便在多个地方使用相同的 variant 类型
using LiteralValue = std::variant<int, std::string, double, bool>;


// 辅助函数：将字符串按分隔符分割
std::vector<std::string> splitString(const std::string& str, char delimiter);

// 评估字面量节点的值
LiteralValue evaluateLiteral(ASTNode* node);

// 评估 AST 节点的值（可以是字面量或列名）
LiteralValue evaluateValue(ASTNode* node, const Tuple& tuple, const std::unordered_map<std::string, const TableInfo*>& tables);

// 评估表达式的布尔结果
bool evaluateExpression(ASTNode* node, const Tuple& tuple, const std::unordered_map<std::string, const TableInfo*>& tables);

#endif