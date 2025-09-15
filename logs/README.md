# 数据库系统日志模块

## 概述

本日志系统为数据库系统提供了分层的日志记录功能，支持不同组件的独立日志记录和统一的日志管理。

## 日志结构

### 日志级别
- **DEBUG**: 调试信息，用于开发调试
- **INFO**: 一般信息，记录重要操作
- **WARN**: 警告信息，记录潜在问题
- **ERROR**: 错误信息，记录操作失败
- **FATAL**: 致命错误，记录严重问题

### 组件日志器
- **BPlusTreeLogger**: B+树操作日志
- **StorageLogger**: 存储层操作日志
- **ExecutionLogger**: 执行层操作日志
- **SQLLogger**: SQL解析和执行日志
- **TableLogger**: 表管理操作日志

## 日志文件

日志文件按组件分别存储：
- `logs/bplus_tree.log` - B+树操作日志
- `logs/sql.log` - SQL相关日志
- `logs/table.log` - 表管理日志

**注意**：存储层和执行层日志仅输出到控制台，不生成文件。

## 使用方法

### 基本使用

```cpp
#include "log/log_config.h"

// 初始化日志系统
DatabaseSystem::Log::LogConfig::Initialize();

// 获取组件日志器
auto logger = DatabaseSystem::Log::LogConfig::GetBPlusTreeLogger();

// 记录日志
logger->Info("操作完成");
logger->Error("操作失败: {}", error_message);
```

### 配置日志

```cpp
// 设置日志级别
DatabaseSystem::Log::LogConfig::SetLogLevel(LogLevel::INFO);

// 设置日志目录
DatabaseSystem::Log::LogConfig::SetLogDirectory("custom_logs");

// 控制输出
DatabaseSystem::Log::LogConfig::SetConsoleOutput(true);  // 控制台输出
DatabaseSystem::Log::LogConfig::SetFileOutput(true);     // 文件输出
```

### 格式化日志

```cpp
// 支持格式化字符串
logger->Info("插入键值: key={}, record_id={}", key, record_id);
logger->Error("操作失败: 错误代码={}, 错误信息={}", error_code, error_msg);
```

## 日志格式

日志格式：`[时间戳] [级别] [组件名] 消息内容`

示例：
```
[2024-01-15 14:30:25.123] [INFO] [BPlusTree] 插入键值: key=123, record_id=456
[2024-01-15 14:30:25.124] [DEBUG] [BPlusTree] 叶子节点分裂: 原键数=50, 左键数=25, 右键数=25
[2024-01-15 14:30:25.125] [ERROR] [Storage] 页面分配失败: page_id=100
```

## 性能考虑

- 日志记录是线程安全的
- 支持异步日志记录（可扩展）
- 日志级别过滤减少不必要的I/O操作
- 文件输出支持缓冲，提高性能

## 扩展

要添加新的组件日志器：

1. 在 `component_loggers.h` 中定义新的日志器类
2. 在 `component_loggers.cpp` 中实现日志器
3. 在 `log_config.h` 和 `log_config.cpp` 中添加获取方法
4. 在组件中使用新的日志器

## 注意事项

- 日志文件会自动创建，确保有足够的磁盘空间
- 生产环境建议设置合适的日志级别，避免过多的DEBUG日志
- 定期清理日志文件，避免占用过多磁盘空间
