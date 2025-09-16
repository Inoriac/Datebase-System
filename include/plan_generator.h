#ifndef PLAN_GENERATOR_H
#define PLAN_GENERATOR_H

#include "ast.h"
#include "execution_plan.h"
#include "semantic_error.h"
#include "utils.h"

#include <string>
#include <vector>
#include <memory>

class PlanGenerator {
public:
    // 将AST转换为执行计划
    std::unique_ptr<Operator> generatePlan(ASTNode* root);

    // 以树形结构输出执行计划
    static void printPlanTree(const Operator* op, int indent = 0);

    // 以JSON格式输出执行计划
    static std::string planToJSON(const Operator* op);

private:
    // 递归辅助函数
    std::unique_ptr<Operator> visit(ASTNode* node);
};

#endif