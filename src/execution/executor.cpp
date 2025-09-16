#include "../../include/execution/executor.h"
#include "catalog/table_manager.h"
#include "catalog/table_schema_manager.h"
#include <iostream>
#include <stdexcept>

// 从 AST 生成条件字符串
static std::string build_condition_from_where(ASTNode* where) {
    if (!where || where->children.empty()) {
        return "";
    }
    
    // 如果传入的where节点本身就是expression节点（比较操作符）
    ASTNode* expr = where;
    if (where->type == WHERE_CLAUSE && !where->children.empty()) {
        expr = where->children[0];
    }
    
    if (!expr) {
        return "";
    }
    
    // 检查是否是支持的操作符类型
    bool is_supported_operator = (expr->type == BINARY_EXPR || 
                                 expr->type == EQUAL_OPERATOR ||
                                 expr->type == GREATER_THAN_OPERATOR ||
                                 expr->type == GREATER_THAN_OR_EQUAL_OPERATOR ||
                                 expr->type == LESS_THAN_OPERATOR ||
                                 expr->type == LESS_THAN_OR_EQUAL_OPERATOR);
    
    if (!is_supported_operator) {
        return "";
    }
    
    if (expr->children.size() < 2) {
        return "";
    }
    
    std::string lhs, op, rhs;
    
    // children[0]: IDENTIFIER_NODE (列名)
    if (expr->children[0]->type == IDENTIFIER_NODE) {
        if (std::holds_alternative<std::string>(expr->children[0]->value))
            lhs = std::get<std::string>(expr->children[0]->value);
    }
    
    // 操作符在expr节点本身的value中
    if (std::holds_alternative<std::string>(expr->value))
        op = std::get<std::string>(expr->value);
    
    // children[1]: INTEGER_LITERAL_NODE 或 STRING_LITERAL_NODE (值)
    if (expr->children[1]->type == INTEGER_LITERAL_NODE) {
        if (std::holds_alternative<int>(expr->children[1]->value))
            rhs = std::to_string(std::get<int>(expr->children[1]->value));
        else if (std::holds_alternative<std::string>(expr->children[1]->value))
            rhs = std::get<std::string>(expr->children[1]->value);
    } else if (expr->children[1]->type == STRING_LITERAL_NODE) {
        if (std::holds_alternative<std::string>(expr->children[1]->value)) {
            std::string s = std::get<std::string>(expr->children[1]->value);
            if (!s.empty() && (s.front() == '\'' || s.front() == '"') && s.back() == s.front()) {
                if (s.size() >= 2) s = s.substr(1, s.size()-2);
            }
            rhs = "'" + s + "'";
        }
    } else if (expr->children[1]->type == IDENTIFIER_NODE) {
        if (std::holds_alternative<std::string>(expr->children[1]->value))
            rhs = std::get<std::string>(expr->children[1]->value);
    }
    
    if (lhs.empty() || op.empty() || rhs.empty()) {
        return "";
    }
    
    return lhs + op + rhs;
}

// 实现每个算子的 next() 方法

// CreateTableOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> CreateTableOperator::next() {
    // 使用实际的TableManager而不是模拟的catalog
    TableSchema schema(table_name);
    for (const auto& col : columns) {
        DataType dt = (col.second == "INT" || col.second == "INTEGER") ? DataType::Int : DataType::Varchar;
        int len = (dt == DataType::Varchar) ? 255 : 4;
        schema.AddColumn(Column(col.first, dt, len, true));
    }
    schema.primary_key_index_ = 0;
    
    // 调用实际的TableManager
    bool success = table_manager_->CreateTable(schema);
    if (!success) {
        throw std::runtime_error("Failed to create table: " + table_name);
    }
    
    return nullptr; // 没有数据行返回
}

// InsertOperator (非迭代模型，直接执行)
std::unique_ptr<Tuple> InsertOperator::next() {
    // 检查表是否存在
    if (!table_manager_->TableExists(table_name)) {
        throw std::runtime_error("Table '" + table_name + "' does not exist.");
    }
    
    // 构建Record
    Record rec;
    for (const auto& value : values) {
        rec.AddValue(value);
    }
    
    // 调用实际的TableManager
    bool success = table_manager_->InsertRecord(table_name, rec);
    if (!success) {
        throw std::runtime_error("Failed to insert into table: " + table_name);
    }
    
    return nullptr;
}

// SeqScanOperator (迭代模型)
std::unique_ptr<Tuple> SeqScanOperator::next() {
    // 使用实际的TableManager进行数据扫描
    if (current_row_index == 0) {
        // 首次调用，获取所有记录
        std::vector<Record> records = table_manager_->SelectRecords(table_name);
        all_records_.clear();
        
        // 将Record转换为Tuple
        for (const auto& record : records) {
            Tuple tuple;
            for (const auto& value : record.values_) {
                tuple.push_back(value);
            }
            all_records_.push_back(tuple);
        }
    }
    
    // 如果没有更多数据，返回空指针
    if (current_row_index >= all_records_.size()) {
        return nullptr;
    }
    
    // 返回当前行数据的副本，并移动到下一行
    auto result_tuple = std::make_unique<Tuple>(all_records_[current_row_index]);
    current_row_index++;
    return result_tuple;
}

// FilterOperator (迭代模型) - 直接使用TableManager的SelectColumnsWithCondition
std::unique_ptr<Tuple> FilterOperator::next() {
    // 首次调用时，直接使用TableManager的SelectColumnsWithCondition方法
    if (current_row_index == 0) {
        // 获取表名（从SeqScanOperator中获取）
        std::string table_name = "";
        if (child && child->type == SEQ_SCAN_OP) {
            SeqScanOperator* seq_scan = static_cast<SeqScanOperator*>(child.get());
            table_name = seq_scan->table_name;
        }
        
        if (table_name.empty()) {
            return nullptr;
        }
        
        // 构建条件字符串
        std::string condition_str = build_condition_from_where(condition);
        
        // 获取要选择的列名
        std::vector<std::string> select_columns;
        
        // 检查是否有ProjectOperator指定了列
        if (child && child->type == PROJECT_OP) {
            ProjectOperator* proj_op = static_cast<ProjectOperator*>(child.get());
            select_columns = proj_op->columns;
        }
        
        // 如果没有指定列，或者指定了"*"，则选择所有列
        if (select_columns.empty() || (select_columns.size() == 1 && select_columns[0] == "*")) {
            TableSchema* schema = table_manager_->GetTableSchema(table_name);
            if (schema) {
                for (const auto& col : schema->columns_) {
                    select_columns.push_back(col.column_name_);
                }
            }
        }
        
        // 使用TableManager的SelectColumnsWithCondition方法
        std::vector<Record> records;
        if (condition_str.empty()) {
            records = table_manager_->SelectColumns(table_name, select_columns);
        } else {
            records = table_manager_->SelectColumnsWithCondition(table_name, select_columns, condition_str);
        }
        
        // 将Record转换为Tuple
        all_records_.clear();
        for (const auto& record : records) {
            Tuple tuple;
            for (const auto& value : record.values_) {
                tuple.push_back(value);
            }
            all_records_.push_back(tuple);
        }
    }
    
    // 如果没有更多数据，返回空指针
    if (current_row_index >= all_records_.size()) {
        return nullptr;
    }
    
    // 返回当前行数据的副本，并移动到下一行
    auto result_tuple = std::make_unique<Tuple>(all_records_[current_row_index]);
    current_row_index++;
    return result_tuple;
}


// ProjectOperator (迭代模型) - 简化实现，直接返回子算子的结果
std::unique_ptr<Tuple> ProjectOperator::next() {
    // 直接返回子算子的结果，因为FilterOperator已经处理了列选择和条件过滤
    return child->next();
}

// Executor 类实现
std::vector<Tuple> Executor::execute() {
    std::vector<Tuple> results;
    if (!plan_root_) {
        return results;
    }
    
    // 如果是 CREATE TABLE 或 INSERT，直接调用 next() 一次即可
    if (plan_root_->type == CREATE_TABLE_OP || plan_root_->type == INSERT_OP) {
        plan_root_->next();
        return results;
    }
    
    // 如果是 SELECT 或 DELETE，循环调用 next() 直到没有更多数据
    while (true) {
        std::unique_ptr<Tuple> tuple = plan_root_->next();
        if (tuple == nullptr) {
            break;
        }
        results.push_back(*tuple);
    }
    
    return results;
}



// 构造函数
SeqScanOperator::SeqScanOperator(const std::string& table_name, TableManager* table_manager)
    : table_name(table_name), table_manager_(table_manager) {
    type = SEQ_SCAN_OP;
    current_row_index = 0; // 初始化
}
ProjectOperator::ProjectOperator(std::unique_ptr<Operator>&& child, const std::vector<std::string>& columns)
    : columns(columns) { // `columns` 是一个复制，所以 const 引用没问题
    this->child = std::move(child);
    type = PROJECT_OP;
}