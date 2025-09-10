#include "ast.h"
#include "symbol_table.h"
#include "sql_semantic_analyzer.h" // 假设这是你的语义分析函数声明文件
#include <iostream>

// 声明 mock 数据生成函数
ASTNode* createMockCreateTableAst();
ASTNode* createMockInsertAst();

int main() {
    try {
        
        std::cout << "--- 正在测试 CREATE TABLE 语句的语义分析 ---\n";
        ASTNode* createAst = createMockCreateTableAst();
        check_create_statement(createAst);
        std::cout << "CREATE TABLE 语义分析通过。\n\n";

        // 创建一个单独的 AST 来模拟 INSERT 语句
        // std::cout << "--- 正在测试 INSERT INTO 语句的语义分析 ---\n";
        // ASTNode* insertAst = createMockInsertAst();
        // check_insert_statement(insertAst);
        // std::cout << "INSERT INTO 语义分析通过。\n\n";
    } catch (const std::exception& e) {
        std::cerr << "语义分析失败: " << e.what() << std::endl;
    }

    // 在程序结束时清理内存
    // delete createAst;
    // delete insertAst;
    
    return 0;
}


ASTNode* createMockCreateTableAst() {
    // 根节点（CREATE_TABLE_STMT）
    ASTNode* create_stmt = new ASTNode(CREATE_TABLE_STMT);

    // 表名节点
    ASTNode* table_name = new ASTNode(IDENTIFIER_NODE, "users");
    create_stmt->addChild(table_name);

    // 列定义列表节点
    ASTNode* col_definitions = new ASTNode(COLUMN_DEFINITIONS_LIST);
    create_stmt->addChild(col_definitions);

    // id 列节点
    ASTNode* id_col = new ASTNode(IDENTIFIER_NODE, "id");
    col_definitions->addChild(id_col);

    // name 列节点
    ASTNode* name_col = new ASTNode(IDENTIFIER_NODE, "name");
    col_definitions->addChild(name_col);
    
    // 注意：这里没有为列指定类型，如果你想支持，需要调整 ASTNode 的设计
    
    return create_stmt;
}

#include "ast.h"
#include <iostream>

// ... 假设你的符号表和语义分析函数已声明

ASTNode* createMockInsertAst() {
    // 根节点（INSERT_STMT）
    ASTNode* insert_stmt = new ASTNode(INSERT_STMT);

    // 表名节点
    ASTNode* table_name = new ASTNode(IDENTIFIER_NODE, "users");
    insert_stmt->addChild(table_name);

    // 值列表节点
    ASTNode* values_list = new ASTNode(VALUES_LIST);
    insert_stmt->addChild(values_list);

    // 整型常量值节点
    ASTNode* int_val = new ASTNode(INTEGER_LITERAL_NODE, "101");
    values_list->addChild(int_val);
    
    // 字符串常量值节点
    ASTNode* string_val = new ASTNode(STRING_LITERAL_NODE, "Alice");
    values_list->addChild(string_val);
    
    return insert_stmt;
}