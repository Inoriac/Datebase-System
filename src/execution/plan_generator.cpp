#include "plan_generator.h"
#include "symbol_table.h"
#include "log/log_config.h"
#include "catalog/table_manager.h"
#include <iostream>


// 主方法：调用递归访问函数
std::unique_ptr<Operator> PlanGenerator::generatePlan(ASTNode* root) {
    if (!root) {
        return nullptr;
    }
    // 我们假设 AST 的根节点是语句列表
    if (root->type != ROOT_NODE || root->children.empty()) {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid AST root node.", root); // 暂无位置信息

    }
    // 假设只处理一个语句，返回第一个语句的执行计划
    ASTNode* statement_node = root->children[0];
    return visit(statement_node);
}

// 递归访问函数
std::unique_ptr<Operator> PlanGenerator::visit(ASTNode* node) {
    if (!node) {
        return nullptr;
    }

    switch (node->type) {
        case CREATE_TABLE_STMT: {
            // 获取表名
            std::string table_name = std::get<std::string>(node->children[0]->value);
            // 获取列定义
            std::vector<std::pair<std::string, std::string>> columns;
            ASTNode* col_list = node->children[1];
            for (ASTNode* col_node : col_list->children) {
                std::string col_name = std::get<std::string>(col_node->value);
                std::string col_type = std::get<std::string>(col_node->children[0]->value);
                columns.push_back({col_name, col_type});
            }
            return std::make_unique<CreateTableOperator>(table_name, columns, table_manager_);
        }
        case INSERT_STMT: {
            // 检查children是否存在
            if (node->children.empty()) {
                throw std::runtime_error("INSERT statement has no children");
            }
            
            // 获取表名 - children[0] 是 IDENTIFIER_NODE 类型
            if (!node->children[0]) {
                throw std::runtime_error("Table name node is null");
            }
            std::string table_name = std::get<std::string>(node->children[0]->value);
            
            // 获取值列表 - 根据语法定义，值列表在最后一个子节点位置
            std::vector<std::variant<int, std::string, bool>> values;
            if (node->children.size() < 2) {
                throw std::runtime_error("INSERT statement missing value list");
            }
            
            // 值列表总是在最后一个子节点
            ASTNode* val_list = node->children.back();
            if (!val_list) {
                throw std::runtime_error("Value list node is null");
            }
            
            for (size_t i = 0; i < val_list->children.size(); ++i) {
                ASTNode* val_node = val_list->children[i];
                if (!val_node) {
                    continue;
                }
                
                if (val_node->type == INTEGER_LITERAL_NODE) {
                    // INTEGER_LITERAL_NODE 的值实际上是字符串，需要转换为整数
                    std::string str_value = std::get<std::string>(val_node->value);
                    int int_value = std::stoi(str_value);
                    values.push_back(int_value);
                } else if (val_node->type == STRING_LITERAL_NODE) {
                    values.push_back(std::get<std::string>(val_node->value));
                } else if (std::holds_alternative<bool>(val_node->value)) {
                    values.push_back(std::get<bool>(val_node->value));
                }
            }
            
            return std::make_unique<InsertOperator>(table_name, values, table_manager_);
        }
        case SELECT_STMT: {
            // SELECT 语句的执行计划是树形结构
            ASTNode* select_list_node = node->children[0];
            ASTNode* from_node = node->children[1];
            
            std::unique_ptr<Operator> plan_root = nullptr;
            
            // 1. 创建 SeqScanOperator
            // from_node 是 FROM_CLAUSE 类型，表名在其 value 中
            std::string table_name = std::get<std::string>(from_node->value);
            std::unique_ptr<Operator> scan_op = std::make_unique<SeqScanOperator>(table_name, table_manager_);
            plan_root = std::move(scan_op);
            
            // 2. 如果有 WHERE 子句，创建 FilterOperator
            if (node->children.size() > 2) {
                ASTNode* where_node = node->children[2];
                std::unique_ptr<Operator> filter_op = std::make_unique<FilterOperator>(std::move(plan_root), where_node->children[0], table_manager_);
                plan_root = std::move(filter_op);
            }
            
            // 3. 创建 ProjectOperator
            std::vector<std::string> columns;
            
            // 处理SELECT列表
            if (select_list_node->children.size() == 1 &&
                select_list_node->children[0]->type == IDENTIFIER_NODE &&
                std::holds_alternative<std::string>(select_list_node->children[0]->value) &&
                std::get<std::string>(select_list_node->children[0]->value) == "*") {
                // SELECT * - 获取所有列
                TableSchema* schema = table_manager_->GetTableSchema(table_name);
                if (schema) {
                    for (const auto& col : schema->columns_) {
                        columns.push_back(col.column_name_);
                    }
                }
            } else {
                // SELECT column1, column2, ... - 获取指定列
                for (ASTNode* col_node : select_list_node->children) {
                    if (col_node->type == IDENTIFIER_NODE &&
                        std::holds_alternative<std::string>(col_node->value)) {
                        columns.push_back(std::get<std::string>(col_node->value));
                    }
                }
            }
            
            std::unique_ptr<Operator> project_op = std::make_unique<ProjectOperator>(std::move(plan_root), columns);
            plan_root = std::move(project_op);

            return plan_root;
        }
        case DELETE_STMT: {
            // DELETE 语句可以看作是带有 WHERE 子句的特殊操作
            ASTNode* from_node = node->children[0];
            ASTNode* where_node = node->children[1];

            // 1. 创建 SeqScanOperator
            std::string table_name = std::get<std::string>(from_node->value);
            std::unique_ptr<Operator> scan_op = std::make_unique<SeqScanOperator>(table_name, table_manager_);
            
            // 2. 创建 FilterOperator
            std::unique_ptr<Operator> filter_op = std::make_unique<FilterOperator>(std::move(scan_op), where_node->children[0], table_manager_);
            
            return filter_op; // DELETE 的最终算子也可以是 Filter，由后续执行器处理
        }
        default:
            throw SemanticError(SemanticError::SYNTAX_ERROR, "Unsupported statement type.", node); // 暂无位置信息
    }
    return nullptr;
}


void PlanGenerator::printPlanTree(const Operator* op, int indent) {
    if (!op) return;
    
    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    std::string indent_str(indent * 2, ' ');
    logger->Debug("{} - ", indent_str);
    
    switch (op->type) {
        case CREATE_TABLE_OP: {
            const auto* create_op = static_cast<const CreateTableOperator*>(op);
            logger->Debug("CreateTable (name: {})", create_op->table_name);
            break;
        }
        case INSERT_OP: {
            const auto* insert_op = static_cast<const InsertOperator*>(op);
            logger->Debug("Insert (table: {})", insert_op->table_name);
            break;
        }
        case PROJECT_OP: {
            const auto* proj_op = static_cast<const ProjectOperator*>(op);
            std::string columns_str;
            for (const auto& col : proj_op->columns) {
                columns_str += col + " ";
            }
            logger->Debug("Project (columns: {})", columns_str);
            if (proj_op->child) {
                printPlanTree(proj_op->child.get(), indent + 1);
            }
            break;
        }
        case FILTER_OP: {
            logger->Debug("Filter (condition: ...)");
            if (op->child) {
                printPlanTree(op->child.get(), indent + 1);
            }
            break;
        }
        case SEQ_SCAN_OP: {
            const auto* scan_op = static_cast<const SeqScanOperator*>(op);
            logger->Debug("SeqScan (table: {})", scan_op->table_name);
            break;
        }
        default:
            logger->Debug("Unsupported Operator");
    }
}

std::string PlanGenerator::planToJSON(const Operator* op) {
    if (!op) return "null";
    
    std::string json = "{";
    switch (op->type) {
        case CREATE_TABLE_OP: {
            const auto* create_op = static_cast<const CreateTableOperator*>(op);
            json += "\"type\": \"CreateTable\", \"table\": \"" + create_op->table_name + "\"";
            break;
        }
        case INSERT_OP: {
            const auto* insert_op = static_cast<const InsertOperator*>(op);
            json += "\"type\": \"Insert\", \"table\": \"" + insert_op->table_name + "\"";
            break;
        }
        case PROJECT_OP: {
            const auto* proj_op = static_cast<const ProjectOperator*>(op);
            json += "\"type\": \"Project\", \"columns\": [";
            for (size_t i = 0; i < proj_op->columns.size(); ++i) {
                json += "\"" + proj_op->columns[i] + "\"" + (i == proj_op->columns.size() - 1 ? "" : ",");
            }
            json += "], \"child\": " + planToJSON(proj_op->child.get());
            break;
        }
        case FILTER_OP: {
            json += "\"type\": \"Filter\", \"condition\": \"...\", \"child\": " + planToJSON(op->child.get());
            break;
        }
        case SEQ_SCAN_OP: {
            const auto* scan_op = static_cast<const SeqScanOperator*>(op);
            json += "\"type\": \"SeqScan\", \"table\": \"" + scan_op->table_name + "\"";
            break;
        }
        default:
            json += "\"type\": \"Unsupported\"";
    }
    json += "}";
    return json;
}