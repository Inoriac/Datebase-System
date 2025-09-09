/* 1. Bison 定义部分 */
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

/* * 定义 Token。Bison 会为它们分配整数值。
 * <str_val> 表示这个Token关联的值是联合体中的 str_val 成员 (char*)。
 * <int_val> 表示关联的值是 int_val 成员 (int)。
 */
%token <str_val> IDENTIFIER STRING_CONST
%token <int_val> INTEGER_CONST

// 定义没有关联值的关键字Token
%token K_CREATE K_TABLE K_INSERT K_INTO K_VALUES K_SELECT K_FROM K_WHERE

/* 定义语法规则的起始点 */
%start sql_statements

%%
/* 2. Bison 语法规则部分 */

sql_statements:
    sql_statement
    | sql_statements sql_statement
    ;

sql_statement:
    create_statement ';'
    | insert_statement ';'
    | select_statement ';'
    ;

create_statement:
    K_CREATE K_TABLE IDENTIFIER '(' column_definitions ')' {
        std::cout << "[Parser] Successfully parsed a CREATE TABLE statement." << std::endl;
        std::cout << "         Table Name: '" << $3 << "'" << std::endl;
        free($3); // 释放由 strdup 分配的内存
    }
    ;

column_definitions:
    /* 简单起见，这里我们不进一步解析列的定义，只匹配标识符 */
    IDENTIFIER 
    | column_definitions ',' IDENTIFIER
    ;

insert_statement:
    K_INSERT K_INTO IDENTIFIER K_VALUES '(' value_list ')' {
        std::cout << "[Parser] Successfully parsed an INSERT statement." << std::endl;
        std::cout << "         Table Name: '" << $3 << "'" << std::endl;
        free($3);
    }
    ;
    
value_list:
    literal
    | value_list ',' literal
    ;

literal:
    STRING_CONST { free($1); }
    | INTEGER_CONST
    ;

select_statement:
    K_SELECT IDENTIFIER K_FROM IDENTIFIER {
         std::cout << "[Parser] Successfully parsed a SELECT statement." << std::endl;
         std::cout << "         Column: '" << $2 << "', Table: '" << $4 << "'" << std::endl;
         free($2);
         free($4);
    }
    ;


%%
/* 3. C++ 用户代码部分 */

// 当语法分析器遇到语法错误时，会调用此函数
void yyerror(const char *s) {
    std::cerr << "[Parser] Syntax Error: " << s << std::endl;
}

// 主函数
int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror(argv[1]);
            return 1;
        }
    }
    
    // 调用 Bison 生成的解析函数
    // yyparse() 会在内部循环调用 yylex()
    yyparse();

    return 0;
}
