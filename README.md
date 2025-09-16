# Database System

一个用C++实现的轻量级关系型数据库系统，支持SQL语句解析、执行计划生成、数据存储和查询功能。

## 🚀 特性

- **SQL解析器**: 基于Flex/Bison的SQL词法和语法分析器
- **执行引擎**: 基于算子的查询执行引擎，支持CREATE、INSERT、SELECT、DELETE操作
- **存储引擎**: 异步I/O的磁盘管理和缓冲池管理
- **索引系统**: B+树索引支持，提供高效的数据检索
- **日志系统**: 分层的日志记录系统，支持多组件独立日志
- **线程安全**: 支持多线程并发访问
- **类型系统**: 支持INT、VARCHAR、BOOLEAN数据类型

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
│   │   └── types.h            # 数据类型定义
│   ├── storage/               # 存储层
│   │   ├── async_disk_manager.h      # 异步磁盘管理器
│   │   ├── async_buffer_pool_manager.h  # 异步缓冲池管理器
│   │   └── buffer_pool_manager.h     # 缓冲池管理器
│   ├── log/                   # 日志系统
│   │   ├── logger.h           # 日志器基类
│   │   ├── log_config.h       # 日志配置
│   │   └── component_loggers.h # 组件日志器
│   └── bplus/                 # B+树索引
│       └── bplus_tree.h       # B+树实现
├── src/                       # 源代码目录
│   ├── main.cpp               # 主程序入口
│   ├── execution/             # 执行引擎
│   │   ├── executor.cpp       # 执行器实现
│   │   └── plan_generator.cpp # 执行计划生成器
│   ├── catalog/               # 目录管理实现
│   ├── storage/               # 存储层实现
│   ├── log/                   # 日志系统实现
│   ├── bplus/                 # B+树实现
│   └── sql/                   # SQL解析器
│       ├── sql_parser.y       # Bison语法文件
│       ├── sql_lexer.l        # Flex词法文件
│       └── sql_semantic_analyzer.cpp  # 语义分析器
├── tests/                     # 测试文件
├── docs/                      # 文档
└── CMakeLists.txt             # CMake构建配置
```

## 🛠️ 构建和运行

### 环境要求

- C++20 编译器 (GCC 13.1.0+ 或 MSVC 2022+)
- CMake 4.0+
- Flex 2.6+
- Bison 3.0+

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
```

## 💻 使用示例

### 启动数据库

```bash
./Database_System
```

### SQL操作示例

```sql
-- 创建表
CREATE TABLE students (id INT, name VARCHAR(50), age INT);

-- 插入数据
INSERT INTO students VALUES (1, 'Alice', 20);
INSERT INTO students VALUES (2, 'Bob', 22);

-- 查询数据
SELECT * FROM students;
SELECT name FROM students WHERE age >= 21;

-- 删除数据
DELETE FROM students WHERE id = 1;
```

## 🏗️ 系统架构

### 核心组件

1. **SQL解析层**
   - Flex词法分析器：将SQL文本转换为token流
   - Bison语法分析器：将token流解析为抽象语法树(AST)
   - 语义分析器：验证SQL语句的语义正确性

2. **执行引擎**
   - 执行计划生成器：将AST转换为算子执行计划
   - 执行器：按计划执行数据库操作
   - 算子系统：CREATE、INSERT、SELECT、DELETE等算子

3. **存储引擎**
   - 异步磁盘管理器：管理磁盘I/O操作
   - 缓冲池管理器：管理内存中的页面缓存
   - 页面管理器：管理数据页面的分配和回收

4. **索引系统**
   - B+树索引：提供高效的数据检索
   - 索引管理器：管理表的索引创建和维护

5. **目录管理**
   - 表管理器：管理表的创建、删除和操作
   - 模式管理器：管理表结构定义
   - 记录序列化器：处理数据的序列化和反序列化

### 数据流

```
SQL语句 → 词法分析 → 语法分析 → AST → 执行计划 → 算子执行 → 存储操作 → 结果返回
```

## 🔧 配置和日志

### 日志系统

系统提供分层的日志记录功能：

- **日志级别**: DEBUG, INFO, WARN, ERROR, FATAL
- **组件日志**: B+树、存储、执行、SQL、表管理
- **日志文件**: `logs/bplus_tree.log`, `logs/sql.log`, `logs/table.log`

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
- **表管理测试**: 测试表的创建、数据操作
- **性能测试**: 测试查询性能
- **集成测试**: 测试异步I/O集成

运行测试：
```bash
./test_db
```

## 📊 性能特性

- **异步I/O**: 支持异步磁盘读写操作
- **缓冲池**: 智能的页面缓存管理
- **索引优化**: B+树索引提供O(log n)的查询性能
- **线程安全**: 支持多线程并发访问
- **内存管理**: 高效的内存分配和回收

## 🔒 线程安全

系统在关键组件中实现了线程安全：

- **日志系统**: 使用mutex保护日志输出
- **异步I/O**: 使用条件变量和原子操作
- **缓冲池**: 使用锁机制保护页面访问

## 🚧 开发状态

- ✅ SQL解析器 (CREATE, INSERT, SELECT, DELETE)
- ✅ 执行引擎 (算子系统)
- ✅ 存储引擎 (异步I/O, 缓冲池)
- ✅ 索引系统 (B+树)
- ✅ 日志系统 (分层日志)
- ✅ 测试套件
- 🔄 事务支持 (开发中)
- 🔄 查询优化器 (计划中)

## 🤝 贡献

欢迎提交Issue和Pull Request来改进这个项目。

## 📄 许可证

本项目采用MIT许可证。

## 📞 联系方式

如有问题或建议，请通过Issue联系。

---

**注意**: 这是一个教学和研究用的数据库系统实现，不建议在生产环境中使用。