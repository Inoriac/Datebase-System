cmake_minimum_required(VERSION 4.0)
project(Datebase_System)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(include)

# ====================
# storage 模块库
# ====================
add_library(storage
        src/storage/async_disk_manager.cpp
        src/storage/async_buffer_pool_manager.cpp
        # 保留原文件作为备份，但不再编译
        # src/storage/disk_manager.cpp
        # src/storage/buffer_pool_manager.cpp
)

target_include_directories(storage PUBLIC include)

# ====================
# log 模块库
# ====================
add_library(log
        src/log/logger.cpp
        src/log/component_loggers.cpp
        src/log/log_config.cpp
)

target_include_directories(log PUBLIC include)

# ====================
# catalog 模块库
# ====================
add_library(catalog
        src/catalog/record_serializer.cpp
        src/catalog/table_manager.cpp
        src/catalog/table_schema_manager.cpp
        src/catalog/symbol_table.cpp
        src/bplus/bplus_tree.cpp
)

target_include_directories(catalog PUBLIC include)
target_link_libraries(catalog PRIVATE log)


# ====================
# execution 模块库
# ====================
add_library(execution
        src/execution/plan_generator.cpp
        src/execution/executor.cpp
        src/utils/utils.cpp
)

target_include_directories(execution PUBLIC include)
target_link_libraries(execution PRIVATE log catalog)

# ====================
# 主程序
# ====================
add_executable(Database_System
        src/main.cpp
        src/sql/sql_parser.tab.cpp
        src/sql/sql_lexer.yy.cpp
)
target_link_libraries(Database_System PRIVATE storage catalog log execution)
target_include_directories(Database_System PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/sql)

# ====================
# SQL 可执行程序（运行 src/sql/main.cpp）
# ====================
add_executable(sql
        src/sql/main.cpp
        src/sql/sql_semantic_analyzer.cpp
        src/sql/sql_parser.tab.cpp
        src/sql/sql_lexer.yy.cpp
)

target_include_directories(sql PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/sql)
target_link_libraries(sql PRIVATE execution catalog log)

# 运行时资源：将 test.sql 复制到可执行文件目录
add_custom_command(TARGET sql POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sql/test.sql
        $<TARGET_FILE_DIR:sql>/test.sql)

# ====================
# GoogleTest
# ====================
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)  # 避免安装
FetchContent_MakeAvailable(googletest)

enable_testing()

# ====================
# 测试程序
# ====================
# 1. 定义测试程序
add_executable(test_db
        tests/test_storage.cpp
        tests/test_table_schema_manager.cpp
        tests/test_table_manager_projection.cpp
        tests/test_primary_key_constraints.cpp
        tests/test_simple_init.cpp
        tests/test_query_performance.cpp
        tests/bplus/test_bplus_tree_merge.cpp
        tests/bplus/test_bplus_tree_split.cpp
        tests/bplus/test_bplus_tree_split_detailed.cpp
)

# 2. 链接库和头文件
target_link_libraries(test_db PRIVATE storage catalog log gtest gtest_main)
target_include_directories(test_db PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/_deps/googletest-src/googletest/include)

# 3. 启用 gtest 自动发现测试
include(GoogleTest)
gtest_discover_tests(test_db)

# ====================
# 异步IO示例程序
# ====================
add_executable(test_async_integration
        tests/test_async_integration.cpp
)
target_link_libraries(test_async_integration PRIVATE storage catalog log)
