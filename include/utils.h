#ifndef UTILS_H
#define UTILS_H
#include "ast.h"

std::string nodeTypeToString(ASTNodeType type);
void printAST(ASTNode* node, int depth = 0);

#endif