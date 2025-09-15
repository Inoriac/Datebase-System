#!/bin/bash
# 该脚本专门用于测试词法分析器 (Flex) 和语法分析器 (Bison) 的集成。
# 它会编译一个不包含语义分析等其他业务逻辑的最小化可执行程序。

set -e # 任何命令失败则立即退出
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

PROJECT_ROOT=$(pwd)
PROJECT_ROOT=${PROJECT_ROOT%/src/sql}
SRC_DIR="$PROJECT_ROOT/src/sql"
INCLUDE_DIR="$PROJECT_ROOT/include"
BUILD_DIR="$PROJECT_ROOT/build"
EXECUTABLE_NAME="parser_unit_test" # 为这个测试程序专门命名
TEST_SQL_FILE="$SRC_DIR/test.sql"

# --- 清理函数 ---
cleanup() {
    echo -e "${YELLOW}--- 清理临时文件... ---${NC}"
    rm -f "$SRC_DIR/sql_lexer.yy.cpp" "$SRC_DIR/sql_parser.tab.cpp" "$SRC_DIR/sql_parser.tab.hpp"
}
# 设置陷阱，在脚本退出或出错时自动调用 cleanup
# trap 'cleanup' EXIT

# --- 主流程 ---

echo -e "${YELLOW}--- 1. 准备构建环境... ---${NC}"
mkdir -p "$BUILD_DIR"

echo -e "${YELLOW}--- 2. 调用 Bison 生成解析器代码... ---${NC}"
bison -d -o "$SRC_DIR/sql_parser.tab.cpp" "$SRC_DIR/sql_parser.y"

echo -e "${YELLOW}--- 3. 调用 Flex 生成词法分析器代码... ---${NC}"
flex -o "$SRC_DIR/sql_lexer.yy.cpp" "$SRC_DIR/sql_lexer.l"

echo -e "${YELLOW}--- 4. 编译并链接一个最小化的测试程序... ---${NC}"
# *** 核心修改: 移除了 "$SRC_DIR/main.cpp" ***
g++ -std=c++17 -g \
    -I"$INCLUDE_DIR" \
    "$SRC_DIR/sql_parser.tab.cpp" \
    "$SRC_DIR/sql_lexer.yy.cpp" \
    -o "$BUILD_DIR/$EXECUTABLE_NAME"

echo -e "${GREEN}最小化编译成功！可执行文件位于: $BUILD_DIR/$EXECUTABLE_NAME${NC}"

# --- 自动运行测试 ---
echo -e "${YELLOW}--- 5. 自动运行测试... ---${NC}"
if [ ! -f "$TEST_SQL_FILE" ]; then
    echo -e "${RED}测试文件 '$TEST_SQL_FILE' 未找到！${NC}"
else
    echo "--- 解析器输出: ---"
    "$BUILD_DIR/$EXECUTABLE_NAME" "$TEST_SQL_FILE"
    echo "-------------------"
fi

echo -e "${GREEN}解析器单元测试执行完毕。${NC}"

