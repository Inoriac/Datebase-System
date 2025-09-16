//
// 日志系统配置实现
//

#include "../../include/log/log_config.h"

namespace DatabaseSystem {
namespace Log {

// 静态成员定义
bool LogConfig::initialized_ = false;
std::shared_ptr<BPlusTreeLogger> LogConfig::bplus_tree_logger_ = nullptr;
std::shared_ptr<StorageLogger> LogConfig::storage_logger_ = nullptr;
std::shared_ptr<ExecutionLogger> LogConfig::execution_logger_ = nullptr;
std::shared_ptr<SQLLogger> LogConfig::sql_logger_ = nullptr;
std::shared_ptr<TableLogger> LogConfig::table_logger_ = nullptr;

void LogConfig::Initialize() {
    if (initialized_) {
        return;
    }
    
    // 设置默认日志目录
    SetLogDirectory("logs");
    
    // 设置默认日志级别
    SetLogLevel(LogLevel::INFO);
    
    // 创建各组件日志器
    bplus_tree_logger_ = std::make_shared<BPlusTreeLogger>();
    storage_logger_ = std::make_shared<StorageLogger>();
    execution_logger_ = std::make_shared<ExecutionLogger>();
    sql_logger_ = std::make_shared<SQLLogger>();
    table_logger_ = std::make_shared<TableLogger>();
    
    initialized_ = true;
}

void LogConfig::SetLogLevel(LogLevel level) {
    LogManager::Instance().SetGlobalLogLevel(level);
}

void LogConfig::SetLogDirectory(const std::string& directory) {
    LogManager::Instance().SetLogDirectory(directory);
}

void LogConfig::SetConsoleOutput(bool enable) {
    if (bplus_tree_logger_) bplus_tree_logger_->SetConsoleOutput(enable);
    if (storage_logger_) storage_logger_->SetConsoleOutput(enable);
    if (execution_logger_) execution_logger_->SetConsoleOutput(enable);
    if (sql_logger_) sql_logger_->SetConsoleOutput(enable);
    if (table_logger_) table_logger_->SetConsoleOutput(enable);
}

void LogConfig::SetFileOutput(bool enable) {
    if (bplus_tree_logger_) bplus_tree_logger_->SetFileOutput(enable);
    if (storage_logger_) storage_logger_->SetFileOutput(enable);
    if (execution_logger_) execution_logger_->SetFileOutput(enable);
    if (sql_logger_) sql_logger_->SetFileOutput(enable);
    if (table_logger_) table_logger_->SetFileOutput(enable);
}

std::shared_ptr<BPlusTreeLogger> LogConfig::GetBPlusTreeLogger() {
    if (!initialized_) {
        Initialize();
    }
    return bplus_tree_logger_;
}

std::shared_ptr<StorageLogger> LogConfig::GetStorageLogger() {
    if (!initialized_) {
        Initialize();
    }
    return storage_logger_;
}

std::shared_ptr<ExecutionLogger> LogConfig::GetExecutionLogger() {
    if (!initialized_) {
        Initialize();
    }
    return execution_logger_;
}

std::shared_ptr<SQLLogger> LogConfig::GetSQLLogger() {
    if (!initialized_) {
        Initialize();
    }
    return sql_logger_;
}

std::shared_ptr<TableLogger> LogConfig::GetTableLogger() {
    if (!initialized_) {
        Initialize();
    }
    return table_logger_;
}

} // namespace Log
} // namespace DatabaseSystem
