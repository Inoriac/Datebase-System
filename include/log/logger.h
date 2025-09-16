//
// 数据库系统日志模块
// 提供分层的日志记录功能
//

#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <map>

namespace DatabaseSystem {
namespace Log {

// 日志级别枚举
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    FATAL = 4
};

// 日志级别名称
constexpr const char* LogLevelNames[] = {
    "DEBUG",
    "INFO", 
    "WARN",
    "ERROR",
    "FATAL"
};

// 基础日志类
class Logger {
public:
    Logger(const std::string& name, const std::string& log_file = "");
    virtual ~Logger() = default;

    // 设置日志级别
    void SetLogLevel(LogLevel level);
    
    // 设置是否输出到控制台
    void SetConsoleOutput(bool enable);
    
    // 设置是否输出到文件
    void SetFileOutput(bool enable);

    // 日志输出方法
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warn(const std::string& message);
    void Error(const std::string& message);
    void Fatal(const std::string& message);

    // 格式化日志输出
    template<typename... Args>
    void Debug(const std::string& format, Args&&... args) {
        Log(LogLevel::DEBUG, FormatString(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void Info(const std::string& format, Args&&... args) {
        Log(LogLevel::INFO, FormatString(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void Warn(const std::string& format, Args&&... args) {
        Log(LogLevel::WARN, FormatString(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void Error(const std::string& format, Args&&... args) {
        Log(LogLevel::ERROR, FormatString(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void Fatal(const std::string& format, Args&&... args) {
        Log(LogLevel::FATAL, FormatString(format, std::forward<Args>(args)...));
    }

protected:
    virtual void Log(LogLevel level, const std::string& message);
    std::string FormatString(const std::string& format);
    std::string GetCurrentTimeString();
    
    template<typename T, typename... Args>
    std::string FormatString(const std::string& format, T&& value, Args&&... args) {
        std::ostringstream oss;
        size_t pos = 0;
        size_t arg_index = 0;
        
        while (pos < format.length()) {
            size_t placeholder_pos = format.find("{}", pos);
            if (placeholder_pos == std::string::npos) {
                oss << format.substr(pos);
                break;
            }
            
            oss << format.substr(pos, placeholder_pos - pos);
            if (arg_index == 0) {
                oss << value;
                arg_index++;
                pos = placeholder_pos + 2;
                if constexpr (sizeof...(args) > 0) {
                    return FormatString(oss.str() + format.substr(pos), std::forward<Args>(args)...);
                }
            } else {
                oss << "{}";
                pos = placeholder_pos + 2;
            }
        }
        
        return oss.str();
    }

private:
    std::string name_;
    std::string log_file_;
    LogLevel log_level_;
    bool console_output_;
    bool file_output_;
    std::mutex log_mutex_;
    std::unique_ptr<std::ofstream> file_stream_;
};

// 全局日志管理器
class LogManager {
public:
    static LogManager& Instance();
    
    // 获取或创建组件日志器
    std::shared_ptr<Logger> GetLogger(const std::string& name);
    
    // 设置全局日志级别
    void SetGlobalLogLevel(LogLevel level);
    
    // 设置日志目录
    void SetLogDirectory(const std::string& directory);

private:
    LogManager() = default;
    std::mutex loggers_mutex_;
    std::map<std::string, std::shared_ptr<Logger>> loggers_;
    LogLevel global_log_level_;
    std::string log_directory_;
};

// 便捷宏定义
#define LOG_DEBUG(logger, ...) (logger)->Debug(__VA_ARGS__)
#define LOG_INFO(logger, ...) (logger)->Info(__VA_ARGS__)
#define LOG_WARN(logger, ...) (logger)->Warn(__VA_ARGS__)
#define LOG_ERROR(logger, ...) (logger)->Error(__VA_ARGS__)
#define LOG_FATAL(logger, ...) (logger)->Fatal(__VA_ARGS__)

} // namespace Log
} // namespace DatabaseSystem
