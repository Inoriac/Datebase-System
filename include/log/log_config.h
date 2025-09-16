//
// 日志系统配置
// 提供统一的日志配置和初始化
//

#pragma once

#include "logger.h"
#include "component_loggers.h"

namespace DatabaseSystem {
namespace Log {

// 日志配置类
class LogConfig {
public:
    // 初始化日志系统
    static void Initialize();
    
    // 设置日志级别
    static void SetLogLevel(LogLevel level);
    
    // 设置日志目录
    static void SetLogDirectory(const std::string& directory);
    
    // 设置控制台输出
    static void SetConsoleOutput(bool enable);
    
    // 设置文件输出
    static void SetFileOutput(bool enable);
    
    // 获取组件日志器
    static std::shared_ptr<BPlusTreeLogger> GetBPlusTreeLogger();
    static std::shared_ptr<StorageLogger> GetStorageLogger();
    static std::shared_ptr<ExecutionLogger> GetExecutionLogger();
    static std::shared_ptr<SQLLogger> GetSQLLogger();
    static std::shared_ptr<TableLogger> GetTableLogger();

private:
    static bool initialized_;
    static std::shared_ptr<BPlusTreeLogger> bplus_tree_logger_;
    static std::shared_ptr<StorageLogger> storage_logger_;
    static std::shared_ptr<ExecutionLogger> execution_logger_;
    static std::shared_ptr<SQLLogger> sql_logger_;
    static std::shared_ptr<TableLogger> table_logger_;
};

} // namespace Log
} // namespace DatabaseSystem
