/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_CAO_DESKTOP_DATABASE_SYS_DATEBASE_SYSTEM_SRC_SQL_SQL_PARSER_TAB_HPP_INCLUDED
# define YY_YY_HOME_CAO_DESKTOP_DATABASE_SYS_DATEBASE_SYSTEM_SRC_SQL_SQL_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 22 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"

    #include "ast.h"

#line 53 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.hpp"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENTIFIER = 258,              /* IDENTIFIER  */
    STRING_CONST = 259,            /* STRING_CONST  */
    INTEGER_CONST = 260,           /* INTEGER_CONST  */
    K_CREATE = 261,                /* K_CREATE  */
    K_TABLE = 262,                 /* K_TABLE  */
    K_INSERT = 263,                /* K_INSERT  */
    K_INTO = 264,                  /* K_INTO  */
    K_VALUES = 265,                /* K_VALUES  */
    K_SELECT = 266,                /* K_SELECT  */
    K_FROM = 267,                  /* K_FROM  */
    K_WHERE = 268,                 /* K_WHERE  */
    K_DELETE = 269,                /* K_DELETE  */
    K_INT = 270,                   /* K_INT  */
    K_VARCHAR = 271,               /* K_VARCHAR  */
    OP_EQ = 272,                   /* OP_EQ  */
    OP_LT = 273,                   /* OP_LT  */
    OP_GT = 274,                   /* OP_GT  */
    OP_LTE = 275,                  /* OP_LTE  */
    OP_GTE = 276,                  /* OP_GTE  */
    OP_NEQ = 277                   /* OP_NEQ  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 32 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"

    int int_val;
    char* str_val;
    ASTNode* node; 

#line 98 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_HOME_CAO_DESKTOP_DATABASE_SYS_DATEBASE_SYSTEM_SRC_SQL_SQL_PARSER_TAB_HPP_INCLUDED  */
