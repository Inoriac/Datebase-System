#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "execution_plan.h"
#include <memory>

class Executor {
public:
    // 接受执行计划的根算子
    Executor(std::unique_ptr<Operator> plan_root) : plan_root_(std::move(plan_root)) {}
    
    // 执行计划并返回所有结果
    std::vector<Tuple> execute();

private:
    std::unique_ptr<Operator> plan_root_;
};

#endif