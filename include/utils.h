#ifndef UTILS_H
#define UTILS_H
#include "ast.h"
#include <variant>
#include <iostream>

#include <string>
#include <vector>

#include <algorithm>
#include <numeric>
std::string nodeTypeToString(ASTNodeType type);
void printAST(ASTNode* node, int depth = 0);
int get_edit_distance(const std::string& s1, const std::string& s2);
#endif