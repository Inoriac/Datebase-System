#!/bin/bash

# --- 脚本设置 ---
# set -e: 当任何命令失败时立即退出脚本
# set -u: 尝试使用未定义的变量时报错
set -eu

# --- 文件名定义 ---
# 源文件
LEXER_FILE="sql_lexer.l"
PARSER_FILE="sql_parser.y"
TEST_SQL_FILE="test.sql"

# 获取脚本所在的目录，也就是项目根目录
PROJECT_ROOT=$(dirname "$(realpath "$0")")
INCLUDE_DIR="$PROJECT_ROOT/../../include"

# 由工具生成的文件
PARSER_CPP="sql_parser.tab.cpp"
PARSER_HPP="sql_parser.tab.hpp"
LEXER_CPP="sql_lexer.yy.cpp"
TARGET_EXEC="sql_parser"

# --- 清理函数 ---
# 这个函数负责删除所有在编译过程中产生的临时文件
cleanup() {
  echo "--- 正在清理生成的临时文件... ---"
  # rm -f: -f 表示强制删除，即使文件不存在也不报错
  rm -f "$PARSER_CPP" "$LEXER_CPP" "$TARGET_EXEC"
}

# --- 核心：设置陷阱 (trap) ---
# trap ... EXIT: 这条命令注册了一个“陷阱”。
# 无论脚本是正常结束，还是因为错误中断，或者被用户Ctrl+C终止，
# EXIT 信号都会被触发，从而执行 cleanup 函数。
# 这是确保目录干净的最可靠方法。
# trap cleanup EXIT

# --- 编译步骤 ---
echo "--- 1. 检查所需文件... ---"
# 检查源文件是否存在
if [ ! -f "$LEXER_FILE" ] || [ ! -f "$PARSER_FILE" ] || [ ! -f "$TEST_SQL_FILE" ]; then
    echo "错误：缺少必要的源文件!"
    echo "请确保 '$LEXER_FILE', '$PARSER_FILE', 和 '$TEST_SQL_FILE' 都在当前目录下。"
    exit 1
fi

echo "--- 2. 使用 Bison 编译语法文件 ($PARSER_FILE)... ---"
# -d: 生成头文件(.hpp), -o: 指定输出的.cpp文件名
bison -d -o "$PARSER_CPP" "$PARSER_FILE"

echo "--- 3. 使用 Flex 编译词法文件 ($LEXER_FILE)... ---"
# -o: 指定输出的.cpp文件名
flex -o "$LEXER_CPP" "$LEXER_FILE"

echo "--- 4. 使用 g++ 编译链接 C++ 代码... ---"
# -lfl: 链接 flex 库，这是运行 flex 生成的代码所必需的
g++ -std=c++17 -g \
    -I"$INCLUDE_DIR" \
    "sql_parser.tab.cpp" \
    "sql_lexer.yy.cpp" \
    -o "$TARGET_EXEC" -lfl

# --- 运行步骤 ---
echo "--- 5. 运行解析器，测试文件 ($TEST_SQL_FILE)... ---"
echo "--- 解析器输出如下: ---"
# 将测试SQL文件作为参数传递给生成的可执行程序
./"$TARGET_EXEC" "$TEST_SQL_FILE"
echo "--------------------------"


# --- 结束 ---
# 脚本将在这里正常退出，然后 trap 会自动调用 cleanup 函数
echo "--- 测试脚本执行完毕。 ---"
