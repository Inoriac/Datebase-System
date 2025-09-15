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

// *** 全局变量存储最近的标识符 ***
char* last_identifier_text = nullptr;

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

%x COMMENT

DIGIT    [0-9]
LETTER   [a-zA-Z]
ID_CHAR  [a-zA-Z0-9_]

%%

<INITIAL>"/*"         { BEGIN(COMMENT); }
<COMMENT>"*/"         { BEGIN(INITIAL); }
<COMMENT>\n           { column_count = 1; /* 注释内的换行也需要重置列号 */ }
<COMMENT>.            { /* 忽略注释内的所有其他字符 */ }
<COMMENT><<EOF>>      { 
    yyerror("Unterminated block comment"); 
    BEGIN(INITIAL); // 遇到文件结尾，恢复状态
}

<INITIAL>[ \t]+                  { /* 忽略空白 */ }
<INITIAL>\n+                     { column_count = 1; /* 遇到换行，重置列号 */ }
<INITIAL>--.*                    { column_count = 1; /* 注释到行尾，也重置 */ }

<INITIAL>"SELECT"                { return K_SELECT; }
<INITIAL>"FROM"                  { return K_FROM; }
<INITIAL>"WHERE"                 { return K_WHERE; }
<INITIAL>"CREATE"                { return K_CREATE; }
<INITIAL>"TABLE"                 { return K_TABLE; }
<INITIAL>"INSERT"                { return K_INSERT; }
<INITIAL>"INTO"                  { return K_INTO; }
<INITIAL>"VALUES"                { return K_VALUES; }
<INITIAL>"DELETE"                { return K_DELETE; }
<INITIAL>"UPDATE"                { return K_UPDATE; }  
<INITIAL>"SET"                   { return K_SET; }    
<INITIAL>"INT"                   { return K_INT; }
<INITIAL>"VARCHAR"               { return K_VARCHAR; }
<INITIAL>"JOIN"                  { return K_JOIN; }   
<INITIAL>"ON"                    { return K_ON; }
<INITIAL>"GROUP"                 { return K_GROUP; }  
<INITIAL>"BY"                    { return K_BY; }     
<INITIAL>"ORDER"                 { return K_ORDER; }


<INITIAL>">="                    { return OP_GTE; }
<INITIAL>"<="                    { return OP_LTE; }
<INITIAL>"!="                    { return OP_NEQ; }
<INITIAL>">"                     { return OP_GT; }
<INITIAL>"<"                     { return OP_LT; }
<INITIAL>"="                     { return OP_EQ; }

<INITIAL>{LETTER}({ID_CHAR})* {
    /* * 识别标识符：
    * 1. 将匹配到的字符串 yytext 复制一份 (因为 yytext 是内部缓冲区，很快会被覆盖)。
    * strdup 是完成这个任务的便捷函数。
    * 2. 将复制后的字符串指针存入 yylval.str_val。
    * 3. 返回 IDENTIFIER Token 类型。
    */

    // *** 更新全局变量用以智能纠错 ***
    if (last_identifier_text) {
        free(last_identifier_text);
    }
    last_identifier_text = strdup(yytext);

    yylval.str_val = strdup(yytext);
    return IDENTIFIER;
}


<INITIAL>{DIGIT}+                {
    /* * 识别整型常量：
    * 1. 使用 atoi 将字符串转换为整数。
    * 2. 将整数值存入 yylval.int_val。
    * 3. 返回 INTEGER_CONST Token 类型。
    */
    yylval.int_val = atoi(yytext);
    return INTEGER_CONST;
}

<INITIAL>'([^']|'')*'            {
    /* 识别字符串常量，处理方式与标识符类似 */
    yylval.str_val = strdup(yytext);
    return STRING_CONST;
}

<INITIAL>[(),;]                  { 
    /* * 对于单字符的分隔符，Bison可以直接使用它们的ASCII值作为Token类型。
    * 所以我们直接返回这个字符
    */
    return yytext[0]; 
}
<INITIAL>"*"                    {
    /* 对于*,直接返回* */
    return yytext[0];
}

<INITIAL>"." { return yytext[0]; }

<INITIAL>.                       { 
    /* 遇到无法识别的字符 */
    std::string err_msg = "Unrecognized character: ";
    err_msg += yytext;
    yyerror(err_msg.c_str());
}

%%