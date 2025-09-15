//
// 数据库系统日志模块实现
//

#include "../../include/log/logger.h"
#include <filesystem>
#include <algorithm>

namespace DatabaseSystem {
namespace Log {

// Logger实现
Logger::Logger(const std::string& name, const std::string& log_file)
    : name_(name)
    , log_file_(log_file)
    , log_level_(LogLevel::DEBUG)
    , console_output_(true)
    , file_output_(false) {
    
    if (!log_file_.empty()) {
        // 确保目录存在
        std::filesystem::path file_path(log_file_);
        std::filesystem::create_directories(file_path.parent_path());
        
        file_output_ = true;
        file_stream_ = std::make_unique<std::ofstream>(log_file_, std::ios::app);
        
        // 检查文件是否成功打开
        if (!file_stream_->is_open()) {
            std::cerr << "Warning: Failed to open log file: " << log_file_ << std::endl;
            file_output_ = false;
        } else {
            std::cerr << "Successfully opened log file: " << log_file_ << std::endl;
        }
    }
}

void Logger::SetLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    log_level_ = level;
}

void Logger::SetConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    console_output_ = enable;
}

void Logger::SetFileOutput(bool enable) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    file_output_ = enable;
}

void Logger::Debug(const std::string& message) {
    Log(LogLevel::DEBUG, message);
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::INFO, message);
}

void Logger::Warn(const std::string& message) {
    Log(LogLevel::WARN, message);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::ERROR, message);
}

void Logger::Fatal(const std::string& message) {
    Log(LogLevel::FATAL, message);
}

void Logger::Log(LogLevel level, const std::string& message) {
    if (level < log_level_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(log_mutex_);
    
    std::string timestamp = GetCurrentTimeString();
    std::string level_name = LogLevelNames[static_cast<int>(level)];
    std::string log_message = "[" + timestamp + "] [" + level_name + "] [" + name_ + "] " + message;
    
    // 输出到控制台
    if (console_output_) {
        std::cout << log_message << std::endl;
    }
    
    // 输出到文件
    if (file_output_ && file_stream_ && file_stream_->is_open()) {
        *file_stream_ << log_message << std::endl;
        file_stream_->flush();
    }
}

std::string Logger::FormatString(const std::string& format) {
    return format;
}

std::string Logger::GetCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

// LogManager实现
LogManager& LogManager::Instance() {
    static LogManager instance;
    return instance;
}

std::shared_ptr<Logger> LogManager::GetLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(loggers_mutex_);
    
    auto it = loggers_.find(name);
    if (it != loggers_.end()) {
        return it->second;
    }
    
    // 创建新的日志器
    std::string log_file;
    if (!log_directory_.empty()) {
        std::filesystem::create_directories(log_directory_);
        log_file = log_directory_ + "/" + name + ".log";
    }
    
    auto logger = std::make_shared<Logger>(name, log_file);
    logger->SetLogLevel(global_log_level_);
    
    loggers_[name] = logger;
    return logger;
}

void LogManager::SetGlobalLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(loggers_mutex_);
    global_log_level_ = level;
    
    // 更新所有现有日志器的级别
    for (auto& pair : loggers_) {
        pair.second->SetLogLevel(level);
    }
}

void LogManager::SetLogDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(loggers_mutex_);
    log_directory_ = directory;
}

} // namespace Log
} // namespace DatabaseSystem
