#!/bin/bash
# 该脚本供解析器开发者使用，用于从 .l 和 .y 文件重新生成 C++ 代码。
# 依赖: flex, bison

set -e
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

SRC_DIR="$(pwd)"

echo -e "${YELLOW}--- 1. 调用 Bison 生成解析器代码... ---${NC}"
bison -d -o "$SRC_DIR/sql_parser.tab.cpp" "$SRC_DIR/sql_parser.y"
echo "成功生成: sql_parser.tab.cpp 和 sql_parser.tab.hpp"

echo -e "${YELLOW}--- 2. 调用 Flex 生成词法分析器代码... ---${NC}"
flex -o "$SRC_DIR/sql_lexer.yy.cpp" "$SRC_DIR/sql_lexer.l"
echo "成功生成: sql_lexer.yy.cpp"

echo -e "${GREEN}代码生成完毕！请记得将以下新生成的文件提交到 Git:${NC}"
echo "  - src/sql/sql_parser.tab.cpp"
echo "  - src/sql/sql_parser.tab.hpp"
echo "  - src/sql/sql_lexer.yy.cpp"