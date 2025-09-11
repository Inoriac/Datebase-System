/* Bison 定义部分 */
%{
#include <iostream>
#include <string>
#include "ast.h" // 包含 AST 节点定义

// 声明由 Flex 生成的词法分析函数
extern int yylex(); 

// 声明错误报告函数
void yyerror(const char *s);

// 存储最终AST的根节点
ASTNode* ast_root = nullptr;

%}

%code requires {
    #include "ast.h"
}

/* * 定义 yylval 的联合体类型。
 * 语法分析器通过这个联合体从词法分析器接收不同类型的值。
 */
%union {
    int int_val;
    char* str_val;
    ASTNode* node; 
}

/* * 定义 Token。
 * <str_val> 表示这个Token关联的值是联合体中的 str_val 成员 (char*)。
 * <int_val> 表示关联的值是 int_val 成员 (int)。
 */
%token <str_val> IDENTIFIER 
%token <str_val> STRING_CONST
%token <int_val> INTEGER_CONST

// 定义没有关联值的关键字Token
%token K_CREATE K_TABLE 
%token K_INSERT K_INTO K_VALUES 
%token K_SELECT K_FROM K_WHERE
%token K_DELETE
%token K_INT K_VARCHAR

/* * 非终结符 (Rules) 
 * 几乎所有的非终结符都会返回一个 ASTNode* 指针
 */
%type <node> statements statement create_statement insert_statement select_statement
%type <node> column_definitions column_definition column_list column_name value_list value
%type <node> optional_column_list delete_statement
%type <node> select_list optional_where_clause where_clause expression
%type <node> comparison_operator literal
%type <node> data_type optional_varchar_length

// 定义操作符 Token
%token OP_EQ OP_LT OP_GT OP_LTE OP_GTE OP_NEQ
//     =     <     >     <=     >=     !=

/* 定义语法规则的起始点 */
%start program

%%
/* Bison 语法规则部分 */

// 程序的顶层结构：一个或多个 SQL 语句
program: 
    statements {
        // 当整个程序解析完成，$1是sql_statements返回的根节点
        // 将它赋值给全局变量 ast_root
        ast_root = $1;
        std::cout << "[Parser] SQL parsing complete. AST root is set." << std::endl;
    }
    ;

statements:
    statement {
        // 只有一个语句，创建一个列表节点，并把该语句作为子节点
        $$ = new ASTNode(ROOT_NODE, "");
        $$->addChild($1);
    }
    | statements statement {
        // 已经有一个列表了($1)，把新的语句($2)加进去
        $1->addChild($2);
        $$ = $1; // 将更新后的列表向上传递
    }
    ;

// 单条 SQL 语句，必须以分号结尾
statement:
    create_statement ';' { $$ = $1; }
    | insert_statement ';' { $$ = $1; }
    | select_statement ';' { $$ = $1; }
    | delete_statement ';' { $$ = $1; }
    | error ';' { yyerrok; } // 错误恢复：如果一条语句语法错误，跳过它直到分号
    ;

/* --- CREATE TABLE 语句的文法 ---
 * 示例: CREATE TABLE users (id INT, name VARCHAR);
 */
create_statement:
    K_CREATE K_TABLE IDENTIFIER '(' column_definitions ')' {
        // 创建一个 CREATE_TABLE_STMT 节点
        $$ = new ASTNode(CREATE_TABLE_STMT, "");
        // 子节点1: 表名 (IDENTIFIER)
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $3));
        // 子节点2: 列定义列表
        $$->addChild($5);

        free($3); // 释放由词法分析器中 strdup 分配的内存
    }
    ;

column_definitions:
    column_definition {
        $$ = new ASTNode(COLUMN_DEFINITIONS_LIST, "");
        $$->addChild($1);
    }
    | column_definitions ',' column_definition {
        $1->addChild($3);
        $$ = $1;
    }
    ;

column_definition:
    IDENTIFIER data_type {
        $$ = new ASTNode(IDENTIFIER_NODE, $1);
        $$->addChild($2);
        free($1);
    }
    ;

data_type:
    K_INT {
        $$ = new ASTNode(DATA_TYPE_NODE, "INT");
    }
    | K_VARCHAR optional_varchar_length {
        // 将长度信息存储在 data_type 节点的 value 中
        $$ = new ASTNode(DATA_TYPE_NODE, "VARCHAR");
        // 如果有长度定义 ($2 不为 nullptr)，可以将其作为子节点
        $$->addChild($2);
    }
    ;

optional_varchar_length:
    /* empty */ { $$ = nullptr; }
    | '(' INTEGER_CONST ')' {
        $$ = new ASTNode(INTEGER_LITERAL_NODE, std::to_string($2));
    }
    ;

/* --- SELECT 语句的文法 ---
 * 示例: SELECT id, name FROM users WHERE id > 10;
 * SELECT * FROM users;
 */
select_statement:
    K_SELECT select_list K_FROM IDENTIFIER optional_where_clause {
        $$ = new ASTNode(SELECT_STMT, "");
        $$->addChild($2);
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $4));
        $$->addChild($5);
        free($4);
    }
    ;

select_list:
    '*' { 
        $$ = new ASTNode(SELECT_LIST, "");
        $$->addChild(new ASTNode(IDENTIFIER_NODE, "*"));
    }
    | column_list { 
        // 将 column_list 包装在 SELECT_LIST 节点中
        $$ = new ASTNode(SELECT_LIST, "");
        // column_list 的子节点是 IDENTIFIER_NODE, 把它们移过来
        for(auto child : $1->children) {
            $$->addChild(child);
        }
        $1->children.clear(); // 防止双重释放
        delete $1;
    }
    ;

column_list:
    column_name {
        $$ = new ASTNode(COLUMN_LIST, ""); 
        $$->addChild($1);
    }
    | column_list ',' column_name {
        $1->addChild($3);
        $$ = $1;
    }
    ;

column_name:
    IDENTIFIER {
        $$ = new ASTNode(IDENTIFIER_NODE, $1);
        free($1);
    }
    ;

optional_where_clause:
    /* 空规则，表示 WHERE 子句是可选的 */ { $$ = nullptr; }
    | where_clause { $$ = $1; }
    ;

where_clause:
    K_WHERE expression {
        $$ = new ASTNode(WHERE_CLAUSE, "");
        $$->addChild($2); // 将表达式作为 WHERE_CLAUSE 的子节点
    }
;

/* --- INSERT 语句的文法 ---
 * 示例: INSERT INTO users VALUES (1, 'Alice');
 */
insert_statement:
    K_INSERT K_INTO IDENTIFIER optional_column_list K_VALUES '(' value_list ')' {
        $$ = new ASTNode(INSERT_STMT, "");
        // 子节点1: 表名
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $3));
        // 子节点2: 可选的列列表 (可能为nullptr)
        $$->addChild($4);
        // 子节点3: 值列表
        $$->addChild($7);   
        free($3);
    }
    ;

/*
 * 可选的列列表。
 * 1. 它可以是空的 (对应 INSERT INTO table VALUES ...)。
 * 2. 或者它可以在括号中包含一个 `column_list`
 */
optional_column_list:
    /* empty */ { $$ = nullptr; }
    | '(' column_list ')' { $$ = $2; }
    ;

value_list:
    value {
        $$ = new ASTNode(VALUES_LIST, "");
        $$->addChild($1);
    }
    | value_list ',' value {
        $1->addChild($3);
        $$ = $1;
    }
    ;

value:
    literal { $$ = $1; }

/* --- DELETE 语句的文法 ---
 * 示例: DELETE FROM users WHERE id = 5;
 */
delete_statement:
    K_DELETE K_FROM IDENTIFIER optional_where_clause {
        $$ = new ASTNode(DELETE_STMT, "");
        // 子节点1: 表名
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $3));
        // 子节点2: 可选的 WHERE 子句 (可能为nullptr)
        $$->addChild($4);
        free($3);
    }
    ;


/* --- WHERE 条件子句的简单文法 ---
 * 注意: 为保持简单，这里只支持 "标识符 操作符 字面量" 形式
 */
expression:
    IDENTIFIER comparison_operator literal {
        $$ = $2;
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $1));
        $$->addChild($3);
        free($1);
    }
    ;

comparison_operator:
    OP_EQ  { $$ = new ASTNode(BINARY_EXPR, "=");  }
    | OP_NEQ { $$ = new ASTNode(BINARY_EXPR, "!="); }
    | OP_GT  { $$ = new ASTNode(BINARY_EXPR, ">");  }
    | OP_GTE  { $$ = new ASTNode(BINARY_EXPR, ">="); }
    | OP_LT  { $$ = new ASTNode(BINARY_EXPR, "<");  }
    | OP_LTE  { $$ = new ASTNode(BINARY_EXPR, "<="); }
    ;

literal:
    INTEGER_CONST {
        $$ = new ASTNode(INTEGER_LITERAL_NODE, std::to_string($1));
    }
    | STRING_CONST {
        $$ = new ASTNode(STRING_LITERAL_NODE, $1);
        free($1);
    }
    ;

%%
/* C++ 代码部分 */
// 当语法分析器遇到无法匹配的语法时，会调用此函数
void yyerror(const char *s) {
    // yylineno 是 Flex 提供的全局变量，用于追踪当前行号
    extern int yylineno;
    std::cerr << "[Parser] Syntax Error at line " << yylineno << ": " << s << std::endl;
}

// 定义缓冲区状态的类型
typedef struct yy_buffer_state* YY_BUFFER_STATE;
// 从一个空结尾的字符串创建扫描缓冲区
extern YY_BUFFER_STATE yy_scan_string(const char* str);
// 删除扫描缓冲区，释放内存
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

ASTNode* parse_sql_string(const std::string& sql) {
    // 1. 调用 Flex 的函数，为给定的字符串创建一个新的扫描缓冲区
    YY_BUFFER_STATE buffer = yy_scan_string(sql.c_str());

    // 2. 调用 Bison 的解析器。它会自动使用上面创建的缓冲区。
    int result = yyparse();
    
    // 3. 清理/删除 Flex 创建的缓冲区
    yy_delete_buffer(buffer);

    // 4. 如果解析成功，返回 AST 根节点，否则返回 nullptr
    if (result == 0) {
        return ast_root;
    }
    
    // 解析失败，ast_root 可能状态不确定，确保返回 nullptr
    // 并且如果 ast_root 已被部分创建，需要清理
    if (ast_root) {
        delete ast_root;
        ast_root = nullptr;
    }
    return nullptr;
}

// 简单的 AST 打印函数，用于演示
/* void print_ast(ASTNode* node, int indent = 0) {
    if (!node) return;
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "Type: " << node->type;
    if (!node->value.empty()) {
        std::cout << ", Value: '" << node->value << "'";
    }
    std::cout << std::endl;
    for (ASTNode* child : node->children) {
        print_ast(child, indent + 1);
    }
} */

// 主函数 (用于独立测试)
/* int main() {
    std::string sql_query;
    std::cout << "Enter SQL statements. Type 'exit' or 'quit' to leave." << std::endl;

    while (true) {
        std::cout << "SQL> ";
        std::getline(std::cin, sql_query);

        if (sql_query == "exit" || sql_query == "quit") {
            break;
        }

        if (sql_query.empty()) {
            continue;
        }

        // 调用我们封装的解析函数
        ASTNode* root = parse_sql_string(sql_query);

        if (root) {
            std::cout << "------------------------------------------" << std::endl;
            std::cout << "Parsing successful! AST Structure:" << std::endl;
            print_ast(root); // 打印 AST 树进行验证
            std::cout << "------------------------------------------" << std::endl;
            
            // 在这里，您可以将 root 交给语义分析器
            // semantic_analyzer.analyze(root);

            // 清理内存
            delete root;
            ast_root = nullptr; // 重置全局指针
        } else {
            std::cerr << "------------------------------------------" << std::endl;
            std::cerr << "Parsing failed." << std::endl;
            std::cerr << "------------------------------------------" << std::endl;
        }
    }

    std::cout << "Goodbye!" << std::endl;
    return 0;
} */
