/* Bison 定义部分 */
%{
#include <iostream>
#include <string>
#include "ast.h" // 包含 AST 节点定义
#include "suggestion.h" // 包含智能纠错建议函数

// 声明由 Flex 生成的词法分析函数
extern int yylex(); 
extern FILE* yyin;

// 声明从 Flex 共享的全局变量
extern char* last_identifier_text;

void yyerror(const char* s); 
void yyerror(Location* locp, const char* s);

extern int yylineno;

// 存储最终AST的根节点
ASTNode* ast_root = nullptr;

// 存储当前解析状态
ParserState current_parser_state = STATE_EXPECTING_COMMAND;

#define YYLTYPE Location

%}

%code requires {
    #include "ast.h"
    #include "suggestion.h"
}

// 启用位置跟踪
%locations
// 启用详细错误信息
%define parse.error verbose

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
        $$ = new ASTNode(ROOT_NODE, "", @$);
        if ($1) $$->addChild($1);   // 在根节点下添加该语句
    }
    | statements statement {
        if ($2) $1->addChild($2);   // 已经有一个列表了($1)，把新的语句($2)加进去
        $$ = $1; // 将更新后的列表向上传递
    }
    ;

// 单条 SQL 语句，必须以分号结尾
statement:
    create_statement ';' { current_parser_state = STATE_EXPECTING_COMMAND; $$ = $1; }
    | insert_statement ';' { current_parser_state = STATE_EXPECTING_COMMAND; $$ = $1; }
    | select_statement ';' { current_parser_state = STATE_EXPECTING_COMMAND; $$ = $1; }
    | delete_statement ';' { current_parser_state = STATE_EXPECTING_COMMAND; $$ = $1; }
    | error ';'            { current_parser_state = STATE_EXPECTING_COMMAND; $$ = nullptr; yyerrok; }
    ;

/* --- CREATE TABLE 语句的文法 ---
 * 示例: CREATE TABLE users (id INT, name VARCHAR);
 */
create_statement:
    K_CREATE { current_parser_state = STATE_EXPECTING_TABLE_AFTER_CREATE; }
    K_TABLE IDENTIFIER '(' column_definitions ')' {
        // 创建一个 CREATE_TABLE_STMT 节点
        $$ = new ASTNode(CREATE_TABLE_STMT, "", @$);
        // 子节点1: 表名 (IDENTIFIER)
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $4, @4));
        // 子节点2: 列定义列表(column_definitions)
        $$->addChild($6);

        free($4); // 释放由词法分析器中 strdup 分配的内存
    }
    ;

column_definitions:
    column_definition {
        $$ = new ASTNode(COLUMN_DEFINITIONS_LIST, "", @$);
        // 只有一个列定义(column_definition)，作为子节点添加
        $$->addChild($1);
    }
    | column_definitions ',' column_definition {
        // 已经有一个列定义列表，把新的列定义加进去
        $1->addChild($3);
        $$ = $1;
    }
    ;

column_definition:
    IDENTIFIER { current_parser_state = STATE_EXPECTING_DATA_TYPE; }
    data_type {
        $$ = new ASTNode(IDENTIFIER_NODE, $1, @1);
        // 将数据类型作为子节点添加
        $$->addChild($3);
        free($1);
    }
    ;

data_type:
    K_INT {
        $$ = new ASTNode(DATA_TYPE_NODE, "INT", @$);
    }
    | K_VARCHAR optional_varchar_length {
        // 将长度信息存储在 data_type 节点的 value 中
        $$ = new ASTNode(DATA_TYPE_NODE, "VARCHAR", @$);
        // 如果有长度定义 ($2 不为 nullptr)，可以将其作为子节点
        $$->addChild($2);
    }
    ;

optional_varchar_length:
    /* empty */ { $$ = nullptr; }
    | '(' INTEGER_CONST ')' {
        $$ = new ASTNode(INTEGER_LITERAL_NODE, std::to_string($2), @$);
    }
    ;

/* --- SELECT 语句的文法 ---
 * 示例: SELECT id, name FROM users WHERE id > 10;
 * SELECT * FROM users;
 */
select_statement:
    K_SELECT select_list  { current_parser_state = STATE_EXPECTING_FROM_AFTER_COLUMNS; }
    K_FROM IDENTIFIER optional_where_clause {
        $$ = new ASTNode(SELECT_STMT, "", @$);
        // 子节点1: 列表 (select_list)
        $$->addChild($2);
        // 子节点2: 表名 (IDENTIFIER)
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $5, @5));
        // 子节点3: 可选的 WHERE 子句 (可能为nullptr)
        $$->addChild($6);
        free($5);
    }
    ;

select_list:
    '*' { 
        $$ = new ASTNode(SELECT_LIST, "", @$);
        // 使用一个特殊的 IDENTIFIER_NODE 来表示 '*'
        $$->addChild(new ASTNode(IDENTIFIER_NODE, "*", @1));
    }
    | column_list { 
        // 将 column_list 包装在 SELECT_LIST 节点中
        $$ = new ASTNode(SELECT_LIST, "", @$);
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
        $$ = new ASTNode(COLUMN_LIST, "", @$); 
        $$->addChild($1);
    }
    | column_list ',' column_name {
        $1->addChild($3);
        $$ = $1;
    }
    ;

column_name:
    IDENTIFIER {
        $$ = new ASTNode(IDENTIFIER_NODE, $1, @1);
        free($1);
    }
    ;

optional_where_clause:
    /* 空规则，表示 WHERE 子句是可选的 */ { $$ = nullptr; }
    | where_clause { $$ = $1; }
    ;

where_clause:
    K_WHERE expression {
        $$ = new ASTNode(WHERE_CLAUSE, "", @$);
        $$->addChild($2); // 将表达式作为 WHERE_CLAUSE 的子节点
    }
;

/* --- INSERT 语句的文法 ---
 * 示例: INSERT INTO users VALUES (1, 'Alice');
 */
insert_statement:
    K_INSERT { current_parser_state = STATE_EXPECTING_INTO_AFTER_INSERT; }
    K_INTO IDENTIFIER optional_column_list { current_parser_state = STATE_EXPECTING_VALUES_AFTER_TABLE; }
    K_VALUES '(' value_list ')' {
        $$ = new ASTNode(INSERT_STMT, "", @$);
        // 子节点1: 表名
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $4, @4));
        // 子节点2: 可选的列列表 (可能为nullptr)
        $$->addChild($5);
        // 子节点3: 值列表
        $$->addChild($9);   
        free($4);
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
        $$ = new ASTNode(VALUES_LIST, "", @$);
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
        $$ = new ASTNode(DELETE_STMT, "", @$);
        // 子节点1: 表名
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $3, @3));
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
        $$->addChild(new ASTNode(IDENTIFIER_NODE, $1, @1));
        $$->addChild($3);
        free($1);
    }
    ;

comparison_operator:
    OP_EQ  { $$ = new ASTNode(BINARY_EXPR, "=", @1);  }
    | OP_NEQ { $$ = new ASTNode(BINARY_EXPR, "!=", @1); }
    | OP_GT  { $$ = new ASTNode(BINARY_EXPR, ">", @1);  }
    | OP_GTE  { $$ = new ASTNode(BINARY_EXPR, ">=", @1); }
    | OP_LT  { $$ = new ASTNode(BINARY_EXPR, "<", @1);  }
    | OP_LTE  { $$ = new ASTNode(BINARY_EXPR, "<=", @1); }
    ;

literal:
    INTEGER_CONST {
        $$ = new ASTNode(INTEGER_LITERAL_NODE, std::to_string($1), @1);
    }
    | STRING_CONST {
        $$ = new ASTNode(STRING_LITERAL_NODE, $1, @1);
        free($1);
    }
    ;

%%
/* C++ 代码部分 */
/* 错误处理函数现在可以打印精确的位置 */
void yyerror(Location* locp, const char* s) {
    std::string error_message(s);

    // 检查是否是“意外的标识符”错误
    if (error_message.find("unexpected IDENTIFIER") != std::string::npos && last_identifier_text != nullptr) {
        
        std::string suggestion = findClosestKeyword(last_identifier_text, current_parser_state);
        if (!suggestion.empty()) {
            // 如果找到了建议，就打印自定义的错误信息
            std::cerr << "[Parser] Error at line " << locp->first_line 
                      << ", column " << locp->first_column 
                      << ": syntax error, unexpected keyword '" << last_identifier_text << "'." << std::endl;
            std::cerr << "         Did you mean '" << suggestion << "'?" << std::endl;
            return; // 打印完自定义信息后直接返回，不再打印 Bison 的默认信息
        }
    }
    
    // 对于所有其他类型的错误，打印 Bison 提供的详细信息
    std::cerr << "[Parser] Error at line " << locp->first_line 
              << ", column " << locp->first_column 
              << ": " << s << std::endl;
}

// 需要一个不带位置的 yyerror 版本以兼容 Flex
void yyerror(const char* s) {
    yyerror(&yylloc, s);
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

std::string nodeTypeToString(ASTNodeType type)
{
    switch (type)
    {
    case ROOT_NODE:
        return "ROOT_NODE";
    case CREATE_TABLE_STMT:
        return "CREATE_TABLE_STMT";
    case INSERT_STMT:
        return "INSERT_STMT";
    case SELECT_STMT:
        return "SELECT_STMT";
    case DELETE_STMT:
        return "DELETE_STMT";
    case IDENTIFIER_NODE:
        return "IDENTIFIER_NODE";
    case DATA_TYPE_NODE:
        return "DATA_TYPE_NODE";
    case INTEGER_LITERAL_NODE:
        return "INTEGER_LITERAL_NODE";
    case STRING_LITERAL_NODE:
        return "STRING_LITERAL_NODE";
    case COLUMN_DEFINITIONS_LIST:
        return "COLUMN_DEFINITIONS_LIST";
    case VALUES_LIST:
        return "VALUES_LIST";
    case SELECT_LIST:
        return "SELECT_LIST";
    case WHERE_CLAUSE:
        return "WHERE_CLAUSE";
    case EQUAL_OPERATOR:
        return "EQUAL_OPERATOR";
    default:
        return "UNKNOWN_NODE";
    }
}
// 简单的 AST 打印函数，用于演示
void print_AST(ASTNode* node, int depth = 0) {
    if (!node)
    {
        return;
    }

    // 打印当前节点的缩进
    for (int i = 0; i < depth; ++i)
    {
        std::cout << "  ";
    }

    // 打印节点类型和值
    std::cout << "- " << nodeTypeToString(node->type);
    if (std::holds_alternative<std::string>(node->value))
    {
        std::cout << ": " << std::get<std::string>(node->value);
    }
    else if (std::holds_alternative<int>(node->value))
    {
        std::cout << ": " << std::get<int>(node->value);
    }
    /* std::cout << std::endl; */

    // 打印节点的位置
    std::cout << "  (Line: " << node->location.first_line 
              << ", Column: " << node->location.first_column << ")" << std::endl;

    // 递归打印所有子节点
    for (ASTNode *child : node->children)
    {
        print_AST(child, depth + 1);
    }
}
// 主函数 (用于独立测试)
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_sql_file>" << std::endl;
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cerr << "Cannot open file: " << argv[1] << std::endl;
        return 1;
    }

    int result = yyparse();
    fclose(yyin);

    if (result == 0 && ast_root != nullptr) {
        std::cout << "------------------------------------------" << std::endl;
        std::cout << "Parsing successful! AST Structure:" << std::endl;
        print_AST(ast_root);
        std::cout << "------------------------------------------" << std::endl;
        delete ast_root;
        ast_root = nullptr;
    } else {
        std::cerr << "------------------------------------------" << std::endl;
        std::cerr << "Parsing failed." << std::endl;
        std::cerr << "------------------------------------------" << std::endl;
    }

    return result;
}
