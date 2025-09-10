/* Bison 定义部分 */
%{
#include <iostream>
#include <string>

// 声明由 Flex 生成的词法分析函数
extern int yylex(); 

// 声明错误报告函数
void yyerror(const char *s);

%}

/* * 定义 yylval 的联合体类型。
 * 语法分析器通过这个联合体从词法分析器接收不同类型的值。
 */
%union {
    int int_val;
    char* str_val; 
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

// 定义操作符 Token
%token OP_EQ OP_LT OP_GT OP_LTE OP_GTE OP_NEQ
//     =     <     >     <=     >=     !=

/* 定义语法规则的起始点 */
%start program

%%
/* Bison 语法规则部分 */

// 程序的顶层结构：一个或多个 SQL 语句
program:
    statements
    ;

statements:
    statement
    | statements statement
    ;

// 单条 SQL 语句，必须以分号结尾
statement:
    create_statement ';'
    | insert_statement ';'
    | select_statement ';'
    | delete_statement ';'
    | error ';' { yyerrok; } // 错误恢复：如果一条语句语法错误，跳过它直到分号
    ;

/* --- CREATE TABLE 语句的文法 ---
 * 示例: CREATE TABLE users (id INT, name VARCHAR);
 */
create_statement:
    K_CREATE K_TABLE IDENTIFIER '(' column_definitions ')' {
        std::cout << "[Parser] Parsed a CREATE TABLE statement for table '" << $3 << "'." << std::endl;
        free($3); // 释放由词法分析器中 strdup 分配的内存
    }
    ;

column_definitions:
    column_definition
    | column_definitions ',' column_definition
    ;

column_definition:
    IDENTIFIER data_type {
        // 在这里可以构建列定义的AST节点
        free($1);
    }
    ;

data_type:
    K_INT
    | K_VARCHAR 
    ;

/* --- SELECT 语句的文法 ---
 * 示例: SELECT id, name FROM users WHERE id > 10;
 * SELECT * FROM users;
 */
select_statement:
    K_SELECT select_list K_FROM IDENTIFIER optional_where_clause {
        std::cout << "[Parser] Parsed a SELECT statement on table '" << $4 << "'." << std::endl;
        free($4);
    }
    ;

select_list:
    '*'
    | id_list 
    ;

id_list:
    IDENTIFIER { free($1); }
    | id_list ',' IDENTIFIER { free($3); }
    ;

optional_where_clause:
    /* 空规则，表示 WHERE 子句是可选的 */
    | K_WHERE condition
    ;

/* --- INSERT 语句的文法 ---
 * 示例: INSERT INTO users VALUES (1, 'Alice');
 */
insert_statement:
    K_INSERT K_INTO IDENTIFIER optional_column_list K_VALUES '(' value_list ')' {
        std::cout << "[Parser] Parsed an INSERT statement for table '" << $3 << "'." << std::endl;
        free($3);
    }
    ;

/*
 * 新增规则：可选的列列表。
 * 1. 它可以是空的 (对应 INSERT INTO table VALUES ...)。
 * 2. 或者它可以在括号中包含一个 `column_list` (我们重用了 SELECT 的规则！)。
 */
optional_column_list:
    /* empty */
    | '(' id_list ')'
    ;

value_list:
    literal
    | value_list ',' literal
    ;

literal:
    INTEGER_CONST
    | STRING_CONST { free($1); }
    ;


/* --- DELETE 语句的文法 ---
 * 示例: DELETE FROM users WHERE id = 5;
 */
delete_statement:
    K_DELETE K_FROM IDENTIFIER optional_where_clause {
        std::cout << "[Parser] Parsed a DELETE statement on table '" << $3 << "'." << std::endl;
        free($3);
    }
    ;


/* --- WHERE 条件子句的简单文法 ---
 * 注意: 为保持简单，这里只支持 "标识符 操作符 字面量" 形式
 */
condition:
    IDENTIFIER comparison_operator literal {
        free($1);
    }
    ;

comparison_operator:
    OP_EQ | OP_NEQ | OP_GT | OP_LT | OP_GTE | OP_LTE
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
// 在实际集成中，yyparse() 可能会被其他模块调用
int main(int argc, char **argv) {
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
}
