/* Bison 定义部分 */
%{
#include <iostream>
#include <string>
#include <ast.h>

// 声明由 Flex 生成的词法分析函数
extern int yylex(); 

// 声明错误报告函数
void yyerror(const char *s);

// 存储最终AST的根节点
ASTNode* ast_root = nullptr;

%}

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
        $$ = new ASTNode(SQL_STATEMENTS_LIST);
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
        $$ = new ASTNode(CREATE_TABLE_STMT);
        // 子节点1: 表名 (IDENTIFIER)
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $3));
        // 子节点2: 列定义列表
        $$->addChild($5);

        free($3); // 释放由词法分析器中 strdup 分配的内存
    }
    ;

column_definitions:
    column_definition {
        $$ = new ASTNode(COLUMN_DEFINITIONS_LIST);
        $$->addChild($1);
    }
    | column_definitions ',' column_definition {
        $1->addChild($3);
        $$ = $1;
    }
    ;

column_definition:
    IDENTIFIER K_INT {
        $$ = new ASTNode(IDENTIFIER_NODE, $1);
        free($1);
    }
    | IDENTIFIER K_VARCHAR '(' INTEGER_CONST ')' {
        $$ = new ASTNode(IDENTIFIER_NODE, $1);
        free($1);
    }
    ;

/* --- SELECT 语句的文法 ---
 * 示例: SELECT id, name FROM users WHERE id > 10;
 * SELECT * FROM users;
 */
select_statement:
    K_SELECT select_list K_FROM IDENTIFIER optional_where_clause {
        $$ = new ASTNode(SELECT_STMT);
        $$->addChild($2);
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $4));
        $$->addChild($5);
        free($4);
    }
    ;

select_list:
    '*' { 
        $$ = new ASTNode(IDENTIFIER_NODE, "*");
    }
    | column_list {  $$ = $1; }
    ;

column_list:
    column_name {
        $$ = new ASTNode(COLUMN_LIST);
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
        // 直接返回表达式子树
        $$ = $2;
    }
;

/* --- INSERT 语句的文法 ---
 * 示例: INSERT INTO users VALUES (1, 'Alice');
 */
insert_statement:
    K_INSERT K_INTO IDENTIFIER optional_column_list K_VALUES '(' value_list ')' {
        $$ = new ASTNode(INSERT_STMT);
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
        $$ = new ASTNode(VALUES_LIST);
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
        $$ = new ASTNode(DELETE_STMT);
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
        // $2 是 comparison_operator 返回的节点，其value已设为操作符
        $$ = $2; 
        // 将列名和字面量作为操作符节点的子节点
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $1));
        $$->addChild($3);
        free($1);
    }
    ;

comparison_operator:
    OP_EQ  { $$ = new ASTNode(EXPRESSION_NODE, "=");  }
    | OP_NEQ { $$ = new ASTNode(EXPRESSION_NODE, "!="); }
    | OP_GT  { $$ = new ASTNode(EXPRESSION_NODE, ">");  }
    | OP_GTE  { $$ = new ASTNode(EXPRESSION_NODE, ">="); }
    | OP_LT  { $$ = new ASTNode(EXPRESSION_NODE, "<");  }TT
    | OP_LTE  { $$ = new ASTNode(EXPRESSION_NODE, "<="); }
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

// 主函数 (用于独立测试)
/* int main(int argc, char **argv) {
    // 设置 Flex 从文件读取输入
    extern FILE *yyin;
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror(argv[1]);
            return 1;
        }
    }
    
    // 调用 Bison 生成的解析函数
    // yyparse() 会在内部循环调用 yylex() 直到文件末尾
    // 返回 0 表示成功，非 0 表示有语法错误
    if (yyparse() == 0) {
        std::cout << "Parsing completed successfully." << std::endl;
    } else {
        std::cout << "Parsing failed due to syntax errors." << std::endl;
    }

    return 0;
} */
