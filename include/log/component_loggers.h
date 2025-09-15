//
// 各组件专用日志器
// 为不同模块提供专门的日志记录功能
//

#pragma once

#include "logger.h"

namespace DatabaseSystem {
namespace Log {

// B+树日志器
class BPlusTreeLogger : public Logger {
public:
    BPlusTreeLogger();
    
    // B+树专用日志方法
    void LogInsert(const std::string& key, int record_id);
    void LogRemove(const std::string& key);
    void LogFind(const std::string& key, bool found, int record_id = -1);
    void LogSplit(const std::string& node_type, int old_keys, int left_keys, int right_keys, const std::string& separator_key = "");
    void LogNodeOperation(const std::string& operation, int page_id, int key_count);
    void LogRangeQuery(const std::string& start_key, const std::string& end_key, int result_count);
    void LogTreeStructure(int height, int root_page_id);
};

// 存储层日志器
class StorageLogger : public Logger {
public:
    StorageLogger();
    
    // 存储层专用日志方法
    void LogPageOperation(const std::string& operation, int page_id, bool success);
    void LogBufferOperation(const std::string& operation, int page_id, bool hit);
    void LogDiskOperation(const std::string& operation, const std::string& filename, bool success);
    void LogAllocation(int page_id, bool success);
    void LogDeallocation(int page_id);
};

// 执行层日志器
class ExecutionLogger : public Logger {
public:
    ExecutionLogger();
    
    // 执行层专用日志方法
    void LogQueryPlan(const std::string& query, const std::string& plan);
    void LogOperatorExecution(const std::string& operator_name, int rows_processed);
    void LogQueryResult(int rows_returned, double execution_time);
    void LogError(const std::string& error_message);
};

// SQL解析日志器
class SQLLogger : public Logger {
public:
    SQLLogger();
    
    // SQL专用日志方法
    void LogParseStart(const std::string& sql);
    void LogParseSuccess(const std::string& sql);
    void LogParseError(const std::string& sql, const std::string& error);
    void LogSemanticAnalysis(const std::string& analysis_result);
    void LogOptimization(const std::string& optimization);
};

// 表管理日志器
class TableLogger : public Logger {
public:
    TableLogger();
    
    // 表管理专用日志方法
    void LogTableCreation(const std::string& table_name, bool success);
    void LogTableDrop(const std::string& table_name, bool success);
    void LogSchemaChange(const std::string& table_name, const std::string& change);
    void LogIndexCreation(const std::string& table_name, const std::string& column_name, bool success);
};

} // namespace Log
} // namespace DatabaseSystem
