#include "plan_generator.h"
#include "symbol_table.h"
#include "log/log_config.h"
#include <iostream>

// 主方法：调用递归访问函数
std::unique_ptr<Operator> PlanGenerator::generatePlan(ASTNode *root)
{
    if (!root)
    {
        return nullptr;
    }
    if (root->type != ROOT_NODE || root->children.empty())
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid AST root node.", root); // 暂无位置信息
    }
    ASTNode *statement_node = root->children[0];
    return visit(statement_node);
}

// 递归访问函数
std::unique_ptr<Operator> PlanGenerator::visit(ASTNode *node)
{
    if (!node)
    {
        return nullptr;
    }

    switch (node->type)
    {
    case CREATE_TABLE_STMT:
    {
        std::string table_name = std::get<std::string>(node->children[0]->value);
        std::vector<std::pair<std::string, std::string>> columns;
        ASTNode *col_list = node->children[1];
        for (ASTNode *col_node : col_list->children)
        {
            std::string col_name = std::get<std::string>(col_node->value);
            std::string col_type = std::get<std::string>(col_node->children[0]->value);
            columns.push_back({col_name, col_type});
        }
        return std::make_unique<CreateTableOperator>(table_name, columns);
    }
    case INSERT_STMT:
    {
        std::string table_name = std::get<std::string>(node->children[0]->value);
        std::vector<std::variant<int, std::string>> values;
        // 查找 VALUES_LIST 节点，它可能在第二个或第三个位置
        ASTNode* val_list = nullptr;
        if (node->children.size() > 1) {
            val_list = node->children.back();
        }
        if (!val_list) {
            throw SemanticError(SemanticError::SYNTAX_ERROR, "VALUES clause not found in INSERT statement.", node);
        }

        for (ASTNode *val_node : val_list->children)
        {
            if (val_node->type == INTEGER_LITERAL_NODE)
            {
                values.push_back(std::get<int>(val_node->value));
            }
            else if (val_node->type == STRING_LITERAL_NODE)
            {
                values.push_back(std::get<std::string>(val_node->value));
            }
        }
        return std::make_unique<InsertOperator>(table_name, values);
    }
    case SELECT_STMT:
    {
        ASTNode *select_list_node = node->children[0];
        ASTNode *from_node = node->children[1];

        std::unique_ptr<Operator> plan_root = nullptr;
        std::string table_name = std::get<std::string>(from_node->value);

        // 如果有JOIN子句
        if (!from_node->children.empty()) {
            // TODO: 这里需要实现 JoinOperator，暂时只处理简单的SELECT
        }

        // 1. 创建 SeqScanOperator
        plan_root = std::make_unique<SeqScanOperator>(table_name);

        // 2. 如果有 WHERE 子句，创建 FilterOperator
        ASTNode* where_clause = nullptr;
        for (size_t i = 2; i < node->children.size(); ++i) {
            if (node->children[i]->type == WHERE_CLAUSE) {
                where_clause = node->children[i];
                break;
            }
        }
        if (where_clause && !where_clause->children.empty()) {
            std::unordered_map<std::string, const TableInfo*> tables;
            tables[table_name] = &catalog.getTable(table_name);
            plan_root = std::make_unique<FilterOperator>(std::move(plan_root), where_clause->children[0], tables);
        }

        // 3. 创建 ProjectOperator
        std::vector<std::string> columns;
        for (ASTNode *col_node : select_list_node->children)
        {
            columns.push_back(std::get<std::string>(col_node->value));
        }
        std::unordered_map<std::string, const TableInfo*> tables;
        tables[table_name] = &catalog.getTable(table_name);
        plan_root = std::make_unique<ProjectOperator>(std::move(plan_root), table_name, columns, tables);

        return plan_root;
    }
    case DELETE_STMT:
    {
        ASTNode *from_node = node->children[0];
        ASTNode *where_node = node->children[1];

        std::string table_name = std::get<std::string>(from_node->value);
        std::unique_ptr<Operator> plan = std::make_unique<SeqScanOperator>(table_name);

        // 创建 FilterOperator
        if (!where_node->children.empty()) {
            std::unordered_map<std::string, const TableInfo*> tables;
            tables[table_name] = &catalog.getTable(table_name);
            plan = std::make_unique<FilterOperator>(std::move(plan), where_node->children[0], tables);
        }

        // TODO: 这里需要一个专门的DeleteOperator来执行删除操作
        // 目前，我们只生成一个过滤计划，由执行器来处理
        return plan;
    }
    case UPDATE_STMT:
    {
        ASTNode *table_name_node = node->children[0];
        ASTNode *set_clause_node = node->children[1];

        std::string table_name = std::get<std::string>(table_name_node->value);
        std::unique_ptr<Operator> plan = std::make_unique<SeqScanOperator>(table_name);

        // 如果有 WHERE 子句，创建 FilterOperator
        if (node->children.size() > 2)
        {
            ASTNode *where_node = node->children[2];
            if (!where_node->children.empty())
            {
                std::unordered_map<std::string, const TableInfo*> tables;
                tables[table_name] = &catalog.getTable(table_name);
                plan = std::make_unique<FilterOperator>(std::move(plan), where_node->children[0], tables);
            }
        }

        // 解析 SET 子句
        std::vector<std::pair<std::string, ASTNode *>> updates;
        for (ASTNode *assignment_node : set_clause_node->children)
        {
            // 对于赋值操作 '=', 其子节点分别为列名和值
            std::string col_name = std::get<std::string>(assignment_node->children[0]->value);
            ASTNode *value_node = assignment_node->children[1];
            updates.emplace_back(col_name, value_node);
        }

        return std::make_unique<UpdateOperator>(std::move(plan), table_name, updates);
    }
    default:
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Unsupported statement type.", node); // 暂无位置信息
    }
    return nullptr;
}

// 打印函数保持不变
void PlanGenerator::printPlanTree(const Operator *op, int indent) {
    if (!op) return;

    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    std::string indent_str(indent * 2, ' ');
    logger->Debug("{} - ", indent_str);

    switch (op->type) {
    case CREATE_TABLE_OP: {
        const auto *create_op = static_cast<const CreateTableOperator *>(op);
        logger->Debug("CreateTable (name: {})", create_op->table_name);
        break;
    }
    case INSERT_OP: {
        const auto *insert_op = static_cast<const InsertOperator *>(op);
        logger->Debug("Insert (table: {})", insert_op->table_name);
        break;
    }
    case PROJECT_OP: {
        const auto *proj_op = static_cast<const ProjectOperator *>(op);
        std::string columns_str;
        for (const auto &col : proj_op->columns) {
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
        const auto *scan_op = static_cast<const SeqScanOperator *>(op);
        logger->Debug("SeqScan (table: {})", scan_op->table_name);
        break;
    }
    case UPDATE_OP: {
        const auto *update_op = static_cast<const UpdateOperator *>(op);
        logger->Debug("Update (table: {})", update_op->table_name);
        if (update_op->child) {
            printPlanTree(update_op->child.get(), indent + 1);
        }
        break;
    }
    default:
        logger->Debug("Unsupported Operator");
    }
}

std::string PlanGenerator::planToJSON(const Operator *op) {
    if (!op) return "null";

    std::string json = "{";
    switch (op->type) {
    case CREATE_TABLE_OP: {
        const auto *create_op = static_cast<const CreateTableOperator *>(op);
        json += "\"type\": \"CreateTable\", \"table\": \"" + create_op->table_name + "\"";
        break;
    }
    case INSERT_OP: {
        const auto *insert_op = static_cast<const InsertOperator *>(op);
        json += "\"type\": \"Insert\", \"table\": \"" + insert_op->table_name + "\"";
        break;
    }
    case PROJECT_OP: {
        const auto *proj_op = static_cast<const ProjectOperator *>(op);
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
        const auto *scan_op = static_cast<const SeqScanOperator *>(op);
        json += "\"type\": \"SeqScan\", \"table\": \"" + scan_op->table_name + "\"";
        break;
    }
    case UPDATE_OP: {
        const auto *update_op = static_cast<const UpdateOperator *>(op);
        json += "\"type\": \"Update\", \"table\": \"" + update_op->table_name + "\", \"child\": " + planToJSON(update_op->child.get());
        break;
    }
    default:
        json += "\"type\": \"Unsupported\"";
    }
    json += "}";
    return json;
}