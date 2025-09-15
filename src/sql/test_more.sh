#!/bin/bash
# 该脚本用于编译和运行 sql 目录下的解析器代码。
# 请在 sql 目录内执行此脚本: ./parser.sh

# --- 配置 ---
set -e # 任何命令失败则立即退出
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 定义相对于当前目录 (sql/) 的路径
# ../../ 表示回到项目根目录
PROJECT_ROOT=$(pwd)/../..
SRC_ROOT="$PROJECT_ROOT/src"
INCLUDE_DIR="$PROJECT_ROOT/include"
BUILD_DIR="$PROJECT_ROOT/build"
TEST_SQL_FILE="$PROJECT_ROOT/src/sql/test.sql"
EXECUTABLE_NAME="sql_parser_app"

# --- 清理函数 ---
# 无论脚本成功或失败，都会在最后执行
cleanup() {
    echo -e "${YELLOW}--- 正在清理临时文件... ---${NC}"
    # 删除当前目录 (sql/) 下由 flex/bison 生成的文件
    rm -f sql_lexer.yy.cpp sql_parser.tab.cpp sql_parser.tab.hpp
    echo -e "${GREEN}清理完成。${NC}"
}

# --- 错误处理 ---
handle_error() {
    echo -e "${RED}错误发生在第 $1 行: 命令 '$2' 执行失败。${NC}"
    # 即使出错也尝试清理
    cleanup
    exit 1
}

# 设置陷阱 (trap)，在脚本退出、中断或出错时自动调用
# trap 'cleanup' EXIT
trap 'handle_error $LINENO "$BASH_COMMAND"' ERR

# --- 脚本主流程 ---

# 1. 创建 build 目录
echo -e "${YELLOW}--- 1. 准备构建环境... ---${NC}"
mkdir -p "$BUILD_DIR"

# 3. 自动查找所有 .cpp 源文件
echo -e "${YELLOW}--- 3. 自动查找所有源文件... ---${NC}"
# 使用 find 命令查找 src/ 目录下所有的 .cpp 文件
# -not -path '*/sql/main.cpp' 排除我们自己的 main.cpp，因为它会被单独列出
# 注意：这个命令会查找包括 catalog, storage 等目录下的所有 .cpp 文件
CPP_FILES=$(find "$SRC_ROOT" -name "*.cpp" -not -path "$SRC_ROOT/main.cpp")
echo "找到以下源文件:"
echo "$CPP_FILES" | sed 's/^/  /' # 打印找到的文件列表

# 4. 编译所有相关的 C++ 源文件并链接
echo -e "${YELLOW}--- 4. 使用 g++ 编译并链接所有源文件... ---${NC}"
g++ -std=c++17 -g \
    -I"$INCLUDE_DIR" \
    $CPP_FILES \
    -o "$BUILD_DIR/$EXECUTABLE_NAME"
    
echo -e "${GREEN}编译成功！可执行文件位于: $BUILD_DIR/$EXECUTABLE_NAME${NC}"

# 5. 运行可执行文件
echo -e "${YELLOW}--- 5. 运行程序，测试文件 ($TEST_SQL_FILE)... ---${NC}"
if [ ! -f "$TEST_SQL_FILE" ]; then
    echo -e "${RED}测试文件 '$TEST_SQL_FILE' 未找到！${NC}"
    exit 1
fi

echo "--- 程序输出如下: ---"
"$BUILD_DIR/$EXECUTABLE_NAME" "$TEST_SQL_FILE"
echo "--------------------------"

echo -e "${GREEN}--- 脚本执行完毕。 ---${NC}"

# 脚本正常结束时，陷阱会自动调用 cleanup 函数
