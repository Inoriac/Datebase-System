cmake_minimum_required(VERSION 3.16)  # 建议不要用 4.0，CLion 默认最高到 3.x
project(Database_System)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ====================
# 头文件路径
# ====================
include_directories(include)

# ====================
# Flex/Bison
# ====================
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(MyParser src/sql/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp)
FLEX_TARGET(MyScanner src/sql/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cpp)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

# ====================
# storage 模块库
# ====================
add_library(storage
        src/storage/disk_manager.cpp
        src/storage/buffer_pool_manager.cpp
)

target_include_directories(storage PUBLIC include)

# ====================
# catalog 模块库
# ====================
add_library(catalog
        src/catalog/record_serializer.cpp
        src/catalog/table_manager.cpp
        src/catalog/table_schema_manager.cpp
)

target_include_directories(catalog PUBLIC include)
target_link_libraries(catalog PUBLIC storage)

# ====================
# execution 模块库
# ====================
add_library(execution
        src/execution/plan_generator.cpp
        src/execution/executor.cpp
)
target_include_directories(execution PUBLIC include)
target_link_libraries(execution PUBLIC catalog)

# ====================
# 主程序
# ====================
add_executable(Database_System
        src/main.cpp
        src/sql/sql_semantic_analyzer.cpp
        ${BISON_MyParser_OUTPUTS}
        ${FLEX_MyScanner_OUTPUTS}
)

target_include_directories(Database_System PRIVATE include)
target_link_libraries(Database_System PRIVATE execution catalog storage)

# ====================
# SQL 前端测试程序
# ====================
add_executable(sql_test
        src/sql/main.cpp
        src/sql/sql_semantic_analyzer.cpp
        ${BISON_MyParser_OUTPUTS}
        ${FLEX_MyScanner_OUTPUTS}
)
target_include_directories(sql_test PRIVATE include)
target_link_libraries(sql_test PRIVATE execution catalog storage)

# ====================
# GoogleTest
# ====================
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# ====================
# 测试程序
# ====================
add_executable(test_db
        tests/test_storage.cpp
        tests/test_table_schema_manager.cpp
        tests/test_table_manager_projection.cpp
)

target_link_libraries(test_db PRIVATE storage catalog gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(test_db)
