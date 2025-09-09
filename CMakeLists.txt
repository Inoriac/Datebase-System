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
        src/storage/disk_manager.cpp
        src/storage/buffer_pool_manager.cpp
        src/storage/storage_engine.cpp
        src/storage/record_serializer.cpp
        src/storage/table_manager.cpp
)

target_include_directories(storage PUBLIC include)

# ====================
# 主程序
# ====================
add_executable(Database_System
        src/main.cpp
)
target_link_libraries(Database_System PRIVATE storage)

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
add_executable(test_storage
        tests/test_storage.cpp
)

# 2. 链接库
target_link_libraries(test_storage PRIVATE storage gtest gtest_main)

# 3. 启用 gtest 自动发现测试
include(GoogleTest)
gtest_discover_tests(test_storage)
