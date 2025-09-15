//
// 各组件专用日志器实现
//

#include "../../include/log/component_loggers.h"

namespace DatabaseSystem {
namespace Log {

// BPlusTreeLogger实现
BPlusTreeLogger::BPlusTreeLogger() : Logger("BPlusTree", "logs/bplus_tree.log") {
    SetFileOutput(true);
}

void BPlusTreeLogger::LogInsert(const std::string& key, int record_id) {
    Debug("Insert: key={}, record_id={}", key, record_id);
}

void BPlusTreeLogger::LogRemove(const std::string& key) {
    Debug("Remove: key={}", key);
}

void BPlusTreeLogger::LogFind(const std::string& key, bool found, int record_id) {
    if (found) {
        Debug("Find: key={}, found=true, record_id={}", key, record_id);
    } else {
        Debug("Find: key={}, found=false", key);
    }
}

void BPlusTreeLogger::LogSplit(const std::string& node_type, int old_keys, int left_keys, int right_keys, const std::string& separator_key) {
    Info("Split: {} node split, old_keys={}, left_keys={}, right_keys={}, separator_key={}", 
         node_type, old_keys, left_keys, right_keys, separator_key);
}

void BPlusTreeLogger::LogNodeOperation(const std::string& operation, int page_id, int key_count) {
    Debug("Node operation: {} on page_id={}, key_count={}", operation, page_id, key_count);
}

void BPlusTreeLogger::LogRangeQuery(const std::string& start_key, const std::string& end_key, int result_count) {
    Info("Range query: start_key={}, end_key={}, result_count={}", start_key, end_key, result_count);
}

void BPlusTreeLogger::LogTreeStructure(int height, int root_page_id) {
    Info("Tree structure: height={}, root_page_id={}", height, root_page_id);
}

// StorageLogger实现
StorageLogger::StorageLogger() : Logger("Storage", "logs/storage.log") {
    SetFileOutput(true);
}

void StorageLogger::LogPageOperation(const std::string& operation, int page_id, bool success) {
    if (success) {
        Debug("Page operation: {} page_id={}, success=true", operation, page_id);
    } else {
        Error("Page operation: {} page_id={}, success=false", operation, page_id);
    }
}

void StorageLogger::LogBufferOperation(const std::string& operation, int page_id, bool hit) {
    Debug("Buffer operation: {} page_id={}, hit={}", operation, page_id, hit ? "true" : "false");
}

void StorageLogger::LogDiskOperation(const std::string& operation, const std::string& filename, bool success) {
    if (success) {
        Debug("Disk operation: {} filename={}, success=true", operation, filename);
    } else {
        Error("Disk operation: {} filename={}, success=false", operation, filename);
    }
}

void StorageLogger::LogAllocation(int page_id, bool success) {
    if (success) {
        Debug("Page allocation: page_id={}, success=true", page_id);
    } else {
        Error("Page allocation: page_id={}, success=false", page_id);
    }
}

void StorageLogger::LogDeallocation(int page_id) {
    Debug("Page deallocation: page_id={}", page_id);
}

// ExecutionLogger实现
ExecutionLogger::ExecutionLogger() : Logger("Execution", "logs/execution.log") {
    SetFileOutput(true);
}

void ExecutionLogger::LogQueryPlan(const std::string& query, const std::string& plan) {
    Info("Query plan: query={}, plan={}", query, plan);
}

void ExecutionLogger::LogOperatorExecution(const std::string& operator_name, int rows_processed) {
    Debug("Operator execution: {} processed {} rows", operator_name, rows_processed);
}

void ExecutionLogger::LogQueryResult(int rows_returned, double execution_time) {
    Info("Query result: {} rows returned in {} ms", rows_returned, execution_time);
}

void ExecutionLogger::LogError(const std::string& error_message) {
    Error("Execution error: {}", error_message);
}

// SQLLogger实现
SQLLogger::SQLLogger() : Logger("SQL", "logs/sql.log") {
    SetFileOutput(true);
}

void SQLLogger::LogParseStart(const std::string& sql) {
    Debug("Parse start: {}", sql);
}

void SQLLogger::LogParseSuccess(const std::string& sql) {
    Info("Parse success: {}", sql);
}

void SQLLogger::LogParseError(const std::string& sql, const std::string& error) {
    Error("Parse error: sql={}, error={}", sql, error);
}

void SQLLogger::LogSemanticAnalysis(const std::string& analysis_result) {
    Debug("Semantic analysis: {}", analysis_result);
}

void SQLLogger::LogOptimization(const std::string& optimization) {
    Info("Query optimization: {}", optimization);
}

// TableLogger实现
TableLogger::TableLogger() : Logger("Table", "logs/table.log") {
    SetFileOutput(true);
}

void TableLogger::LogTableCreation(const std::string& table_name, bool success) {
    if (success) {
        Info("Table creation: {} created successfully", table_name);
    } else {
        Error("Table creation: {} creation failed", table_name);
    }
}

void TableLogger::LogTableDrop(const std::string& table_name, bool success) {
    if (success) {
        Info("Table drop: {} dropped successfully", table_name);
    } else {
        Error("Table drop: {} drop failed", table_name);
    }
}

void TableLogger::LogSchemaChange(const std::string& table_name, const std::string& change) {
    Info("Schema change: {} - {}", table_name, change);
}

void TableLogger::LogIndexCreation(const std::string& table_name, const std::string& column_name, bool success) {
    if (success) {
        Info("Index creation: {}.{} index created successfully", table_name, column_name);
    } else {
        Error("Index creation: {}.{} index creation failed", table_name, column_name);
    }
}

} // namespace Log
} // namespace DatabaseSystem
