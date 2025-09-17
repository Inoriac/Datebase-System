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
    // 我们假设 AST 的根节点是语句列表
    if (root->type != ROOT_NODE || root->children.empty())
    {
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Invalid AST root node.", root); // 暂无位置信息
    }
    // 假设只处理一个语句，返回第一个语句的执行计划
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
        // 获取表名
        std::string table_name = std::get<std::string>(node->children[0]->value);
        // 获取列定义
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

        // 检查是否有显式列名列表
        std::vector<std::string> column_names;
        ASTNode *values_list = nullptr;

        // 如果子节点数量超过 2，并且第二个子节点是 COLUMN_LIST
        if (node->children.size() > 2 && node->children[1]->type == COLUMN_LIST)
        {
            ASTNode *col_list = node->children[1];
            for (ASTNode *col_node : col_list->children)
            {
                column_names.push_back(std::get<std::string>(col_node->value));
            }
            values_list = node->children[2];
        }
        else
        {
            // 没有列名列表，直接处理值列表
            values_list = node->children[1];
        }

        // 获取值列表
        std::vector<LiteralValue> values;
        for (ASTNode *val_node : values_list->children)
        {
            values.push_back(evaluateLiteral(val_node)); // 使用辅助函数来简化
        }

        return std::make_unique<InsertOperator>(table_name, std::move(values), std::move(column_names));
    }
    case SELECT_STMT:
    {
        // 获取 FROM 子句的表名
        ASTNode *from_node = node->children[1];
        std::string table_name = std::get<std::string>(from_node->value);

        // 创建一个用于传递给算子的表信息映射
        std::unordered_map<std::string, const TableInfo *> tables;
        tables[table_name] = &catalog.getTable(table_name);

        // 1. 创建 SeqScanOperator
        std::unique_ptr<Operator> child_op = std::make_unique<SeqScanOperator>(table_name);

        // 2. 如果有 WHERE 子句，创建 FilterOperator
        if (node->children.size() > 2 && node->children[2]->type == WHERE_CLAUSE)
        {
            ASTNode *where_node = node->children[2];
            child_op = std::make_unique<FilterOperator>(std::move(child_op), where_node->children[0], tables);
        }

        // 3. 创建 ProjectOperator
        ASTNode *select_list_node = node->children[0];
        std::vector<std::string> columns;

        // 检查是否是 SELECT *
        if (select_list_node->children.size() == 1 && std::get<std::string>(select_list_node->children[0]->value) == "*")
        {
            // 从目录中获取所有列
            const TableInfo &table_info = catalog.getTable(table_name);
            columns = table_info.column_order;
        }
        else
        {
            // 否则，获取指定的列名
            for (ASTNode *col_node : select_list_node->children)
            {
                columns.push_back(std::get<std::string>(col_node->value));
            }
        }

        // 4. 创建最终的 ProjectOperator
        return std::make_unique<ProjectOperator>(std::move(child_op), table_name, columns, tables);
    }
    case DELETE_STMT:
    {
        // DELETE 语句可以看作是带有 WHERE 子句的特殊操作
        ASTNode *from_node = node->children[0];
        ASTNode *where_node = node->children[1];

        std::string table_name = std::get<std::string>(from_node->value);
        std::unordered_map<std::string, const TableInfo *> tables;
        tables[table_name] = &catalog.getTable(table_name);

        // 1. 创建 SeqScanOperator
        std::unique_ptr<Operator> scan_op = std::make_unique<SeqScanOperator>(table_name);

        // 2. 创建 FilterOperator
        std::unique_ptr<Operator> filter_op = std::make_unique<FilterOperator>(std::move(scan_op), where_node->children[0], tables);

        return filter_op; // DELETE 的最终算子也可以是 Filter，由后续执行器处理
    }
    case UPDATE_STMT:
    {
        // UPDATE 语句是一个 Scan -> Filter -> Update 的序列
        ASTNode *table_name_node = node->children[0];
        ASTNode *set_clause_node = node->children[1];

        // 1. 创建 SeqScanOperator
        std::string table_name = std::get<std::string>(table_name_node->value);
        std::unique_ptr<Operator> plan = std::make_unique<SeqScanOperator>(table_name);

        std::unordered_map<std::string, const TableInfo *> tables;
        tables[table_name] = &catalog.getTable(table_name);

        // 2. 如果有 WHERE 子句，创建 FilterOperator
        if (node->children.size() > 2)
        {
            ASTNode *where_node = node->children[2];
            if (!where_node->children.empty())
            {
                plan = std::make_unique<FilterOperator>(std::move(plan), where_node->children[0], tables);
            }
        }

        // 3. 解析 SET 子句
        std::vector<std::pair<std::string, ASTNode *>> updates;
        for (ASTNode *assignment_node : set_clause_node->children)
        {
            std::string col_name = std::get<std::string>(assignment_node->children[0]->value);
            ASTNode *value_node = assignment_node->children[1];
            updates.emplace_back(col_name, value_node);
        }

        // 4. 创建最终的 UpdateOperator
        return std::make_unique<UpdateOperator>(std::move(plan), table_name, updates);
    }
    default:
        throw SemanticError(SemanticError::SYNTAX_ERROR, "Unsupported statement type.", node); // 暂无位置信息
    }
    return nullptr;
}




void PlanGenerator::printPlanTree(const Operator *op, int indent)
{
    if (!op)
        return;

    auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();
    std::string indent_str(indent * 2, ' ');
    logger->Debug("{} - ", indent_str);

    switch (op->type)
    {
    case CREATE_TABLE_OP:
    {
        const auto *create_op = static_cast<const CreateTableOperator *>(op);
        logger->Debug("CreateTable (name: {})", create_op->table_name);
        break;
    }
    case INSERT_OP:
    {
        const auto *insert_op = static_cast<const InsertOperator *>(op);
        logger->Debug("Insert (table: {})", insert_op->table_name);
        break;
    }
    case PROJECT_OP:
    {
        const auto *proj_op = static_cast<const ProjectOperator *>(op);
        std::string columns_str;
        for (const auto &col : proj_op->columns)
        {
            columns_str += col + " ";
        }
        logger->Debug("Project (columns: {})", columns_str);
        if (proj_op->child)
        {
            printPlanTree(proj_op->child.get(), indent + 1);
        }
        break;
    }
    case FILTER_OP:
    {
        logger->Debug("Filter (condition: ...)");
        if (op->child)
        {
            printPlanTree(op->child.get(), indent + 1);
        }
        break;
    }
    case SEQ_SCAN_OP:
    {
        const auto *scan_op = static_cast<const SeqScanOperator *>(op);
        logger->Debug("SeqScan (table: {})", scan_op->table_name);
        break;
    }
    case UPDATE_OP:
    {
        const auto *update_op = static_cast<const UpdateOperator *>(op);
        logger->Debug("Update (table: {})", update_op->table_name);
        if (update_op->child)
        {
            printPlanTree(update_op->child.get(), indent + 1);
        }
        break;
    }
    default:
        logger->Debug("Unsupported Operator");
    }
}

std::string PlanGenerator::planToJSON(const Operator *op)
{
    if (!op)
        return "null";

    std::string json = "{";
    switch (op->type)
    {
    case CREATE_TABLE_OP:
    {
        const auto *create_op = static_cast<const CreateTableOperator *>(op);
        json += "\"type\": \"CreateTable\", \"table\": \"" + create_op->table_name + "\"";
        break;
    }
    case INSERT_OP:
    {
        const auto *insert_op = static_cast<const InsertOperator *>(op);
        json += "\"type\": \"Insert\", \"table\": \"" + insert_op->table_name + "\"";
        break;
    }
    case PROJECT_OP:
    {
        const auto *proj_op = static_cast<const ProjectOperator *>(op);
        json += "\"type\": \"Project\", \"columns\": [";
        for (size_t i = 0; i < proj_op->columns.size(); ++i)
        {
            json += "\"" + proj_op->columns[i] + "\"" + (i == proj_op->columns.size() - 1 ? "" : ",");
        }
        json += "], \"child\": " + planToJSON(proj_op->child.get());
        break;
    }
    case FILTER_OP:
    {
        json += "\"type\": \"Filter\", \"condition\": \"...\", \"child\": " + planToJSON(op->child.get());
        break;
    }
    case SEQ_SCAN_OP:
    {
        const auto *scan_op = static_cast<const SeqScanOperator *>(op);
        json += "\"type\": \"SeqScan\", \"table\": \"" + scan_op->table_name + "\"";
        break;
    }
    case UPDATE_OP:
    {
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


std::string PlanGenerator::planToSExpression(const Operator *op)
{
    if (!op)
    {
        return "()";
    }

    std::stringstream ss;
    ss << "(";

    switch (op->type)
    {
    case CREATE_TABLE_OP:
    {
        const auto *create_op = static_cast<const CreateTableOperator *>(op);
        ss << "CREATE_TABLE " << create_op->table_name;
        break;
    }
    case INSERT_OP:
    {
        const auto *insert_op = static_cast<const InsertOperator *>(op);
        ss << "INSERT " << insert_op->table_name;
        break;
    }
    case PROJECT_OP:
    {
        const auto *proj_op = static_cast<const ProjectOperator *>(op);
        ss << "PROJECT (";
        for (size_t i = 0; i < proj_op->columns.size(); ++i)
        {
            ss << proj_op->columns[i] << (i == proj_op->columns.size() - 1 ? "" : " ");
        }
        ss << ") " << planToSExpression(proj_op->child.get());
        break;
    }
    case FILTER_OP:
    {
        ss << "FILTER (...) " << planToSExpression(op->child.get());
        break;
    }
    case SEQ_SCAN_OP:
    {
        const auto *scan_op = static_cast<const SeqScanOperator *>(op);
        ss << "SEQ_SCAN " << scan_op->table_name;
        break;
    }
    case UPDATE_OP:
    {
        const auto *update_op = static_cast<const UpdateOperator *>(op);
        ss << "UPDATE " << update_op->table_name << " " << planToSExpression(update_op->child.get());
        break;
    }
    default:
        ss << "UNSUPPORTED_OP";
    }
    ss << ")";
    return ss.str();
}