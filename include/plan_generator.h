#ifndef PLAN_GENERATOR_H
#define PLAN_GENERATOR_H

#include "ast.h"
#include "execution/execution_plan.h"
#include "semantic_error.h"
#include <string>
#include <vector>
#include <memory>

// 前向声明
class TableManager;

class PlanGenerator {
public:
    // 构造函数，接受TableManager指针
    PlanGenerator(TableManager* table_manager) : table_manager_(table_manager) {}
    
    // 将AST转换为执行计划
    std::unique_ptr<Operator> generatePlan(ASTNode* root);
    
    // 访问单个AST节点并生成执行计划
    std::unique_ptr<Operator> visit(ASTNode* node);

    // 以树形结构输出执行计划
    static void printPlanTree(const Operator* op, int indent = 0);

    // 以JSON格式输出执行计划
    static std::string planToJSON(const Operator* op);

private:
    
    TableManager* table_manager_;
};

#endif