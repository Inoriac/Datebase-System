%{
#include <string>

/* * 包含 Bison 生成的头文件。
 * 这个文件里包含了所有 Token (K_CREATE, IDENTIFIER等) 的定义，
 * 以及全局变量 yylval 的定义。
 */
#include "sql_parser.tab.hpp" 

// 覆盖默认的 yyerror 函数声明，因为我们已经在 .y 文件中定义了它
#define YY_DECL int yylex()
void yyerror(const char* s);
extern YYLTYPE yylloc; // Bison 提供的位置变量

// 定义计数器
static int column_count = 1;

/* * 定义一个宏，在每次匹配 Token 后自动更新位置信息。
 * Flex 会在执行每个规则的动作代码之前，先执行 YY_USER_ACTION。
 */
#define YY_USER_ACTION \
    yylloc.first_line = yylineno; \
    yylloc.first_column = column_count; \
    column_count += yyleng; \
    yylloc.last_line = yylineno; \
    yylloc.last_column = column_count - 1;

%}

%option noyywrap
%option yylineno

DIGIT    [0-9]
LETTER   [a-zA-Z]
ID_CHAR  [a-zA-Z0-9_]

%%

[ \t]+                  { /* 忽略空白 */ }
\n+                     { column_count = 1; /* 遇到换行，重置列号 */ }
--.*                    { column_count = 1; /* 注释到行尾，也重置 */ }

"SELECT"                { return K_SELECT; }
"FROM"                  { return K_FROM; }
"WHERE"                 { return K_WHERE; }
"CREATE"                { return K_CREATE; }
"TABLE"                 { return K_TABLE; }
"INSERT"                { return K_INSERT; }
"INTO"                  { return K_INTO; }
"VALUES"                { return K_VALUES; }
"DELETE"                { return K_DELETE; }
"INT"                   { return K_INT; }
"VARCHAR"               { return K_VARCHAR; }

">="                    { return OP_GTE; }
"<="                    { return OP_LTE; }
"!="                    { return OP_NEQ; }
">"                     { return OP_GT; }
"<"                     { return OP_LT; }
"="                     { return OP_EQ; }

{LETTER}({ID_CHAR})* {
    /* * 识别标识符：
    * 1. 将匹配到的字符串 yytext 复制一份 (因为 yytext 是内部缓冲区，很快会被覆盖)。
    * strdup 是完成这个任务的便捷函数。
    * 2. 将复制后的字符串指针存入 yylval.str_val。
    * 3. 返回 IDENTIFIER Token 类型。
    */
    yylval.str_val = strdup(yytext);
    return IDENTIFIER;
}


{DIGIT}+                {
    /* * 识别整型常量：
    * 1. 使用 atoi 将字符串转换为整数。
    * 2. 将整数值存入 yylval.int_val。
    * 3. 返回 INTEGER_CONST Token 类型。
    */
    yylval.int_val = atoi(yytext);
    return INTEGER_CONST;
}

'([^']|'')*'            {
    /* 识别字符串常量，处理方式与标识符类似 */
    yylval.str_val = strdup(yytext);
    return STRING_CONST;
}

[(),;]                  { 
    /* * 对于单字符的分隔符，Bison可以直接使用它们的ASCII值作为Token类型。
    * 所以我们直接返回这个字符
    */
    return yytext[0]; 
}
"*"                    {
    /* 对于*,直接返回* */
    return yytext[0];
}

.                       { 
    /* 遇到无法识别的字符 */
    std::string err_msg = "Unrecognized character: ";
    err_msg += yytext;
    yyerror(err_msg.c_str());
}

%%