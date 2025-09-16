# Database System

一个用C++实现的轻量级关系型数据库系统，支持完整的SQL操作、数据持久化、索引管理和异步I/O。系统采用现代数据库架构设计，具有良好的可扩展性和性能。

## 🚀 核心特性

- **完整SQL支持**: CREATE TABLE, INSERT, SELECT, UPDATE, DELETE
- **数据持久化**: 基于文件的表数据存储和恢复
- **索引系统**: B+树索引，支持主键和唯一索引
- **异步I/O**: 异步磁盘管理和缓冲池管理
- **执行引擎**: 基于算子的查询执行引擎
- **日志系统**: 分层日志记录，支持多组件独立日志
- **类型系统**: 支持INT、VARCHAR、BOOLEAN数据类型
- **线程安全**: 支持多线程并发访问

## 📁 项目结构

```
Datebase-System/
├── include/                    # 头文件目录
│   ├── ast.h                  # 抽象语法树定义
│   ├── execution_plan.h       # 执行计划算子定义
│   ├── executor.h             # 执行器接口
│   ├── plan_generator.h       # 执行计划生成器
│   ├── catalog/               # 目录管理
│   │   ├── table_manager.h    # 表管理器
│   │   ├── table_schema_manager.h  # 表模式管理器
│   │   ├── record_serializer.h     # 记录序列化器
│   │   ├── page.h             # 页面管理
│   │   └── types.h            # 数据类型定义
│   ├── storage/               # 存储层
│   │   ├── async_disk_manager.h      # 异步磁盘管理器
│   │   ├── async_buffer_pool_manager.h  # 异步缓冲池管理器
│   │   ├── buffer_pool_manager.h     # 缓冲池管理器
│   │   ├── disk_manager.h     # 磁盘管理器
│   │   └── prefetch_config.h  # 预读配置
│   ├── log/                   # 日志系统
│   │   ├── logger.h           # 日志器基类
│   │   ├── log_config.h       # 日志配置
│   │   └── component_loggers.h # 组件日志器
│   ├── bplus/                 # B+树索引
│   │   └── bplus_tree.h       # B+树实现
│   └── utils.h                # 工具函数
├── src/                       # 源代码目录
│   ├── main.cpp               # 主程序入口
│   ├── execution/             # 执行引擎
│   │   ├── executor.cpp       # 执行器实现
│   │   └── plan_generator.cpp # 执行计划生成器
│   ├── catalog/               # 目录管理实现
│   │   ├── table_manager.cpp  # 表管理器实现
│   │   ├── table_schema_manager.cpp # 表模式管理器实现
│   │   ├── record_serializer.cpp    # 记录序列化器实现
│   │   └── symbol_table.cpp   # 符号表实现
│   ├── storage/               # 存储层实现
│   │   ├── async_disk_manager.cpp   # 异步磁盘管理器实现
│   │   ├── async_buffer_pool_manager.cpp # 异步缓冲池管理器实现
│   │   ├── buffer_pool_manager.cpp  # 缓冲池管理器实现
│   │   ├── disk_manager.cpp   # 磁盘管理器实现
│   │   └── prefetch_config.cpp # 预读配置实现
│   ├── log/                   # 日志系统实现
│   │   ├── logger.cpp         # 日志器实现
│   │   ├── log_config.cpp     # 日志配置实现
│   │   └── component_loggers.cpp # 组件日志器实现
│   ├── bplus/                 # B+树实现
│   │   └── bplus_tree.cpp     # B+树实现
│   ├── sql/                   # SQL解析器
│   │   ├── sql_parser.y       # Bison语法文件
│   │   ├── sql_lexer.l        # Flex词法文件
│   │   ├── sql_lexer.yy.cpp   # 生成的词法分析器代码
│   │   ├── sql_parser.tab.cpp # 生成的语法分析器代码
│   │   ├── sql_parser.tab.hpp # 生成的语法分析器头文件
│   │   └── sql_semantic_analyzer.cpp  # 语义分析器
│   └── utils/                 # 工具函数实现
│       └── utils.cpp          # 工具函数实现
├── tests/                     # 测试文件
│   ├── bplus/                 # B+树测试
│   │   ├── test_bplus_tree_split.cpp
│   │   ├── test_bplus_tree_merge.cpp
│   │   └── test_bplus_tree_split_detailed.cpp
│   ├── test_storage.cpp       # 存储层测试
│   ├── test_table_manager_projection.cpp # 表管理测试
│   ├── test_primary_key_constraints.cpp  # 主键约束测试
│   ├── test_query_performance.cpp        # 查询性能测试
│   └── test_async_integration.cpp        # 异步集成测试
├── examples/                  # 示例代码
│   ├── access_history_example.cpp # 访问历史示例
│   └── prefetch_example.cpp   # 预读功能示例
├── docs/                      # 设计文档
│   ├── 01_系统架构总览.md
│   ├── 02_SQL解析器设计.md
│   ├── 03_执行引擎设计.md
│   ├── 04_存储引擎设计.md
│   ├── 05_日志系统设计.md
│   ├── 06_索引系统设计.md
│   ├── 07_目录管理设计.md
│   └── 08_预读功能设计.md
├── logs/                      # 日志文件目录
│   ├── bplus_tree.log         # B+树操作日志
│   ├── sql.log               # SQL执行日志
│   └── table.log             # 表操作日志
└── CMakeLists.txt             # CMake构建配置
```

## 🛠️ 构建和运行

### 环境要求

- **C++20** 编译器 (GCC 13.1.0+ 或 MSVC 2022+)
- **CMake 4.0+**
- **Flex 2.6+** (用于SQL词法分析)
- **Bison 3.0+** (用于SQL语法分析)

### 构建步骤

1. **克隆项目**
```bash
git clone <repository-url>
cd Datebase-System
```

2. **生成构建文件**
```bash
mkdir build
cd build
cmake ..
```

3. **编译项目**
```bash
cmake --build . --config Release
```

4. **运行主程序**
```bash
./Database_System
```

### 运行测试

```bash
# 运行所有测试
ctest

# 运行特定测试
./test_db

# 运行B+树测试
./test_bplus_tree_split
./test_bplus_tree_merge
```

## 💻 使用示例

### 启动数据库

```bash
./Database_System
```

### SQL操作示例

```sql
-- 创建表
CREATE TABLE users (id INT, name VARCHAR(50), age INT, email VARCHAR(100));

-- 插入数据
INSERT INTO users VALUES (1, 'Alice', 25, 'alice@example.com');
INSERT INTO users VALUES (2, 'Bob', 30, 'bob@example.com');
INSERT INTO users VALUES (3, 'Charlie', 28, 'charlie@example.com');

-- 查询数据
SELECT * FROM users;
SELECT name, age FROM users WHERE age > 25;

-- 更新数据
UPDATE users SET age = 26 WHERE id = 1;

-- 删除数据
DELETE FROM users WHERE id = 3;

-- 查看表信息
SHOW TABLES;
```

## 🏗️ 系统架构

### 核心组件

1. **SQL解析层**
   - **Flex词法分析器**: 将SQL文本转换为token流
   - **Bison语法分析器**: 将token流解析为抽象语法树(AST)
   - **语义分析器**: 验证SQL语句的语义正确性

2. **执行引擎**
   - **执行计划生成器**: 将AST转换为算子执行计划
   - **执行器**: 按计划执行数据库操作
   - **算子系统**: CreateTableOperator, InsertOperator, SeqScanOperator, FilterOperator, ProjectOperator, UpdateOperator, DeleteOperator

3. **存储引擎**
   - **异步磁盘管理器**: 管理磁盘I/O操作
   - **缓冲池管理器**: 管理内存中的页面缓存
   - **页面管理器**: 管理数据页面的分配和回收
   - **记录序列化器**: 处理数据的序列化和反序列化

4. **索引系统**
   - **B+树索引**: 提供高效的数据检索
   - **索引管理器**: 管理表的索引创建和维护
   - **主键索引**: 自动为主键列创建索引

5. **目录管理**
   - **表管理器**: 管理表的创建、删除和操作
   - **模式管理器**: 管理表结构定义
   - **元数据管理**: 管理表的元数据信息

6. **日志系统**
   - **分层日志**: 不同组件独立的日志记录
   - **日志配置**: 可配置的日志级别和输出
   - **组件日志器**: 专门的日志器实例

### 数据流

```
SQL语句 → 词法分析 → 语法分析 → AST → 执行计划 → 算子执行 → 存储操作 → 结果返回
```

### 存储架构

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   执行引擎      │    │   缓冲池管理    │    │   异步磁盘管理  │
│                │    │                │    │                │
│ - 算子执行      │───▶│ - 页面缓存      │───▶│ - 异步I/O      │
│ - 结果处理      │    │ - LRU替换      │    │ - 批量操作      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   索引系统      │    │   页面管理      │    │   文件系统      │
│                │    │                │    │                │
│ - B+树索引      │    │ - 页面分配      │    │ - 数据文件      │
│ - 键值管理      │    │ - 页面回收      │    │ - 日志文件      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

## 🔧 配置和日志

### 日志系统

系统提供分层的日志记录功能：

- **日志级别**: DEBUG, INFO, WARN, ERROR, FATAL
- **组件日志**: B+树、存储、执行、SQL、表管理
- **日志文件**: 
  - `logs/bplus_tree.log` - B+树操作日志
  - `logs/sql.log` - SQL执行日志
  - `logs/table.log` - 表操作日志

### 配置示例

```cpp
// 初始化日志系统
DatabaseSystem::Log::LogConfig::Initialize();

// 设置日志级别
DatabaseSystem::Log::LogConfig::SetLogLevel(LogLevel::INFO);

// 获取组件日志器
auto logger = DatabaseSystem::Log::LogConfig::GetSQLLogger();
logger->Info("SQL语句执行完成");
```

## 🧪 测试

项目包含完整的测试套件：

- **存储层测试**: 测试磁盘管理和缓冲池功能
- **B+树测试**: 测试索引的插入、删除、分裂、合并
- **表管理测试**: 测试表的创建、数据操作、投影查询
- **主键约束测试**: 测试主键唯一性约束
- **性能测试**: 测试查询性能
- **集成测试**: 测试异步I/O集成

运行测试：
```bash
# 运行所有测试
./test_db

# 运行特定测试
./test_bplus_tree_split
./test_table_manager_projection
./test_primary_key_constraints
```

## 📊 性能特性

- **异步I/O**: 支持异步磁盘读写操作，提高并发性能
- **缓冲池**: 智能的页面缓存管理，减少磁盘I/O
- **索引优化**: B+树索引提供O(log n)的查询性能
- **线程安全**: 支持多线程并发访问
- **内存管理**: 高效的内存分配和回收
- **预读机制**: 智能的数据预读，提高查询效率

## 🔒 线程安全

系统在关键组件中实现了线程安全：

- **日志系统**: 使用mutex保护日志输出
- **异步I/O**: 使用条件变量和原子操作
- **缓冲池**: 使用锁机制保护页面访问
- **索引系统**: 使用读写锁保护索引操作

## 🚧 开发状态

### ✅ 已完成功能

- **SQL解析器**: CREATE, INSERT, SELECT, UPDATE, DELETE
- **执行引擎**: 完整的算子系统
- **存储引擎**: 异步I/O, 缓冲池管理
- **索引系统**: B+树索引, 主键索引
- **日志系统**: 分层日志记录
- **数据持久化**: 表数据文件存储
- **测试套件**: 完整的单元测试和集成测试

### 🔄 开发中功能

- **事务支持**: ACID特性实现
- **查询优化器**: 执行计划优化
- **更多数据类型**: FLOAT, DATE, TIMESTAMP等
- **复合索引**: 多列索引支持

### 📋 计划功能

- **分布式支持**: 多节点集群
- **备份恢复**: 数据备份和恢复机制
- **监控系统**: 性能监控和诊断
- **Web接口**: RESTful API支持

## 📚 设计文档

详细的设计文档位于 `docs/` 目录：

- [系统架构总览](docs/01_系统架构总览.md)
- [SQL解析器设计](docs/02_SQL解析器设计.md)
- [执行引擎设计](docs/03_执行引擎设计.md)
- [存储引擎设计](docs/04_存储引擎设计.md)
- [日志系统设计](docs/05_日志系统设计.md)
- [索引系统设计](docs/06_索引系统设计.md)
- [目录管理设计](docs/07_目录管理设计.md)
- [预读功能设计](docs/08_预读功能设计.md)

## 🤝 贡献

欢迎提交Issue和Pull Request来改进这个项目。

### 贡献指南

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开 Pull Request

## 📄 许可证

本项目采用MIT许可证。详见 [LICENSE](LICENSE) 文件。

## 📞 联系方式

如有问题或建议，请通过以下方式联系：

- 提交 [Issue](https://github.com/your-repo/issues)
- 发送邮件至 [your-email@example.com](mailto:your-email@example.com)

## 🙏 致谢

感谢所有为这个项目做出贡献的开发者和开源社区。

---

**注意**: 这是一个教学和研究用的数据库系统实现，展示了现代数据库系统的核心概念和实现技术。虽然功能完整，但不建议在生产环境中使用。