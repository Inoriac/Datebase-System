/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"

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


#line 99 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "sql_parser.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_STRING_CONST = 4,               /* STRING_CONST  */
  YYSYMBOL_INTEGER_CONST = 5,              /* INTEGER_CONST  */
  YYSYMBOL_K_CREATE = 6,                   /* K_CREATE  */
  YYSYMBOL_K_TABLE = 7,                    /* K_TABLE  */
  YYSYMBOL_K_INSERT = 8,                   /* K_INSERT  */
  YYSYMBOL_K_INTO = 9,                     /* K_INTO  */
  YYSYMBOL_K_VALUES = 10,                  /* K_VALUES  */
  YYSYMBOL_K_SELECT = 11,                  /* K_SELECT  */
  YYSYMBOL_K_FROM = 12,                    /* K_FROM  */
  YYSYMBOL_K_WHERE = 13,                   /* K_WHERE  */
  YYSYMBOL_K_DELETE = 14,                  /* K_DELETE  */
  YYSYMBOL_K_UPDATE = 15,                  /* K_UPDATE  */
  YYSYMBOL_K_SET = 16,                     /* K_SET  */
  YYSYMBOL_K_INT = 17,                     /* K_INT  */
  YYSYMBOL_K_VARCHAR = 18,                 /* K_VARCHAR  */
  YYSYMBOL_K_JOIN = 19,                    /* K_JOIN  */
  YYSYMBOL_K_ON = 20,                      /* K_ON  */
  YYSYMBOL_K_GROUP = 21,                   /* K_GROUP  */
  YYSYMBOL_K_BY = 22,                      /* K_BY  */
  YYSYMBOL_K_ORDER = 23,                   /* K_ORDER  */
  YYSYMBOL_OP_EQ = 24,                     /* OP_EQ  */
  YYSYMBOL_OP_LT = 25,                     /* OP_LT  */
  YYSYMBOL_OP_GT = 26,                     /* OP_GT  */
  YYSYMBOL_OP_LTE = 27,                    /* OP_LTE  */
  YYSYMBOL_OP_GTE = 28,                    /* OP_GTE  */
  YYSYMBOL_OP_NEQ = 29,                    /* OP_NEQ  */
  YYSYMBOL_30_ = 30,                       /* ';'  */
  YYSYMBOL_31_ = 31,                       /* ','  */
  YYSYMBOL_32_ = 32,                       /* '('  */
  YYSYMBOL_33_ = 33,                       /* ')'  */
  YYSYMBOL_34_ = 34,                       /* '*'  */
  YYSYMBOL_35_ = 35,                       /* '.'  */
  YYSYMBOL_YYACCEPT = 36,                  /* $accept  */
  YYSYMBOL_program = 37,                   /* program  */
  YYSYMBOL_statements = 38,                /* statements  */
  YYSYMBOL_statement = 39,                 /* statement  */
  YYSYMBOL_update_statement = 40,          /* update_statement  */
  YYSYMBOL_set_clause = 41,                /* set_clause  */
  YYSYMBOL_set_list = 42,                  /* set_list  */
  YYSYMBOL_assignment = 43,                /* assignment  */
  YYSYMBOL_create_statement = 44,          /* create_statement  */
  YYSYMBOL_45_1 = 45,                      /* $@1  */
  YYSYMBOL_column_definitions = 46,        /* column_definitions  */
  YYSYMBOL_column_definition = 47,         /* column_definition  */
  YYSYMBOL_48_2 = 48,                      /* $@2  */
  YYSYMBOL_data_type = 49,                 /* data_type  */
  YYSYMBOL_optional_varchar_length = 50,   /* optional_varchar_length  */
  YYSYMBOL_select_statement = 51,          /* select_statement  */
  YYSYMBOL_52_3 = 52,                      /* $@3  */
  YYSYMBOL_from_clause = 53,               /* from_clause  */
  YYSYMBOL_54_4 = 54,                      /* $@4  */
  YYSYMBOL_join_clause = 55,               /* join_clause  */
  YYSYMBOL_optional_group_by_clause = 56,  /* optional_group_by_clause  */
  YYSYMBOL_group_by_clause = 57,           /* group_by_clause  */
  YYSYMBOL_optional_order_by_clause = 58,  /* optional_order_by_clause  */
  YYSYMBOL_order_by_clause = 59,           /* order_by_clause  */
  YYSYMBOL_select_list = 60,               /* select_list  */
  YYSYMBOL_column_list = 61,               /* column_list  */
  YYSYMBOL_column_name = 62,               /* column_name  */
  YYSYMBOL_optional_where_clause = 63,     /* optional_where_clause  */
  YYSYMBOL_where_clause = 64,              /* where_clause  */
  YYSYMBOL_insert_statement = 65,          /* insert_statement  */
  YYSYMBOL_66_5 = 66,                      /* $@5  */
  YYSYMBOL_67_6 = 67,                      /* $@6  */
  YYSYMBOL_optional_column_list = 68,      /* optional_column_list  */
  YYSYMBOL_value_list = 69,                /* value_list  */
  YYSYMBOL_value = 70,                     /* value  */
  YYSYMBOL_delete_statement = 71,          /* delete_statement  */
  YYSYMBOL_expression = 72,                /* expression  */
  YYSYMBOL_column_ref = 73,                /* column_ref  */
  YYSYMBOL_comparison_operator = 74,       /* comparison_operator  */
  YYSYMBOL_literal = 75                    /* literal  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  25
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   100

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  36
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  67
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  119

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   284


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      32,    33,    34,     2,    31,     2,    35,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    30,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    95,    95,   104,   109,   117,   118,   119,   120,   121,
     122,   129,   140,   147,   152,   160,   174,   174,   188,   193,
     201,   201,   211,   214,   223,   224,   234,   234,   250,   250,
     263,   264,   279,   280,   284,   296,   297,   301,   313,   318,
     331,   335,   342,   346,   355,   356,   360,   370,   371,   370,
     390,   391,   395,   399,   406,   412,   426,   432,   441,   445,
     454,   455,   456,   457,   458,   459,   463,   466
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER",
  "STRING_CONST", "INTEGER_CONST", "K_CREATE", "K_TABLE", "K_INSERT",
  "K_INTO", "K_VALUES", "K_SELECT", "K_FROM", "K_WHERE", "K_DELETE",
  "K_UPDATE", "K_SET", "K_INT", "K_VARCHAR", "K_JOIN", "K_ON", "K_GROUP",
  "K_BY", "K_ORDER", "OP_EQ", "OP_LT", "OP_GT", "OP_LTE", "OP_GTE",
  "OP_NEQ", "';'", "','", "'('", "')'", "'*'", "'.'", "$accept", "program",
  "statements", "statement", "update_statement", "set_clause", "set_list",
  "assignment", "create_statement", "$@1", "column_definitions",
  "column_definition", "$@2", "data_type", "optional_varchar_length",
  "select_statement", "$@3", "from_clause", "$@4", "join_clause",
  "optional_group_by_clause", "group_by_clause",
  "optional_order_by_clause", "order_by_clause", "select_list",
  "column_list", "column_name", "optional_where_clause", "where_clause",
  "insert_statement", "$@5", "$@6", "optional_column_list", "value_list",
  "value", "delete_statement", "expression", "column_ref",
  "comparison_operator", "literal", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-60)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-3)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      15,   -23,   -60,   -60,    -2,     0,    16,    25,     3,   -60,
       1,     6,    17,    18,    19,   -60,    39,    41,    20,   -60,
     -60,    21,   -60,    50,    38,   -60,   -60,   -60,   -60,   -60,
     -60,   -60,    53,    54,    55,    47,    57,    48,    59,    48,
      31,    32,   -60,   -60,    48,   -60,    62,   -60,   -60,    42,
      36,   -60,   -60,    65,    57,   -60,    66,    49,    37,   -60,
      14,    23,    59,   -60,   -25,   -60,   -18,    61,    56,    51,
      58,   -60,    71,   -60,   -60,   -60,   -60,   -60,   -60,    30,
     -60,   -60,   -60,   -60,    27,    65,   -60,   -60,    44,    74,
     -60,    57,    60,   -60,   -60,   -60,   -60,   -60,   -60,    46,
     -60,   -60,    23,    63,    21,    57,    75,   -60,    -9,   -60,
     -60,    62,    21,    52,    23,   -60,   -60,   -60,   -60
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,    16,    47,     0,     0,     0,     0,     0,     3,
       0,     0,     0,     0,     0,    10,     0,     0,    42,    38,
      26,    39,    40,     0,     0,     1,     4,     9,     5,     7,
       6,     8,     0,     0,     0,     0,     0,    44,     0,    44,
       0,    50,    43,    28,    44,    41,     0,    55,    45,     0,
      12,    13,    11,     0,     0,    48,     0,    32,    58,    46,
       0,     0,     0,    20,     0,    18,     0,     0,    30,     0,
      35,    33,     0,    60,    64,    62,    65,    63,    61,     0,
      67,    66,    15,    14,     0,     0,    17,    51,     0,     0,
      29,     0,     0,    27,    36,    59,    56,    57,    22,    24,
      21,    19,     0,     0,    34,     0,     0,    23,     0,    52,
      54,     0,    37,     0,     0,    49,    31,    25,    53
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -60,   -60,   -60,    76,   -60,   -60,   -60,    24,   -60,   -60,
     -60,    -6,   -60,   -60,   -60,   -60,   -60,   -60,   -60,   -60,
     -60,   -60,   -60,   -60,   -60,   -54,    64,   -34,   -60,   -60,
     -60,   -60,   -60,   -60,   -27,   -60,   -22,     9,   -60,   -59
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     7,     8,     9,    10,    39,    50,    51,    11,    16,
      64,    65,    84,   100,   107,    12,    35,    44,    56,    90,
      70,    71,    93,    94,    20,    21,    22,    47,    48,    13,
      17,    67,    55,   108,   109,    14,    59,    60,    79,   110
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      66,    18,    82,    -2,     1,    52,    85,    15,    86,     2,
      57,     3,    23,    36,     4,    87,     1,     5,     6,    24,
      97,     2,   114,     3,   115,    25,     4,    80,    81,     5,
       6,    27,    19,    58,    80,    81,    28,   104,    73,    74,
      75,    76,    77,    78,    98,    99,    32,    29,    30,    31,
      33,   112,    36,    37,    38,    34,    40,    41,    42,    43,
      18,    46,    49,    53,    54,    58,    61,    62,    63,    68,
      69,    88,    72,    91,    95,    89,   102,   103,   106,   101,
     113,    92,   105,   111,    26,   117,    83,   118,    96,   116,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45
};

static const yytype_int8 yycheck[] =
{
      54,     3,    61,     0,     1,    39,    31,    30,    33,     6,
      44,     8,    12,    31,    11,    33,     1,    14,    15,     3,
      79,     6,    31,     8,    33,     0,    11,     4,     5,    14,
      15,    30,    34,     3,     4,     5,    30,    91,    24,    25,
      26,    27,    28,    29,    17,    18,     7,    30,    30,    30,
       9,   105,    31,     3,    16,    35,     3,     3,     3,    12,
       3,    13,     3,    32,    32,     3,    24,    31,     3,     3,
      21,    10,    35,    22,     3,    19,    32,     3,    32,    85,
       5,    23,    22,    20,     8,    33,    62,   114,    79,   111,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      36
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     6,     8,    11,    14,    15,    37,    38,    39,
      40,    44,    51,    65,    71,    30,    45,    66,     3,    34,
      60,    61,    62,    12,     3,     0,    39,    30,    30,    30,
      30,    30,     7,     9,    35,    52,    31,     3,    16,    41,
       3,     3,     3,    12,    53,    62,    13,    63,    64,     3,
      42,    43,    63,    32,    32,    68,    54,    63,     3,    72,
      73,    24,    31,     3,    46,    47,    61,    67,     3,    21,
      56,    57,    35,    24,    25,    26,    27,    28,    29,    74,
       4,     5,    75,    43,    48,    31,    33,    33,    10,    19,
      55,    22,    23,    58,    59,     3,    73,    75,    17,    18,
      49,    47,    32,     3,    61,    22,    32,    50,    69,    70,
      75,    20,    61,     5,    31,    33,    72,    33,    70
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    36,    37,    38,    38,    39,    39,    39,    39,    39,
      39,    40,    41,    42,    42,    43,    45,    44,    46,    46,
      48,    47,    49,    49,    50,    50,    52,    51,    54,    53,
      55,    55,    56,    56,    57,    58,    58,    59,    60,    60,
      61,    61,    62,    62,    63,    63,    64,    66,    67,    65,
      68,    68,    69,    69,    70,    71,    72,    72,    73,    73,
      74,    74,    74,    74,    74,    74,    75,    75
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     2,     2,     2,     2,     2,
       2,     4,     2,     1,     3,     3,     0,     7,     1,     3,
       0,     3,     1,     2,     0,     3,     0,     7,     0,     4,
       0,     4,     0,     1,     3,     0,     1,     3,     1,     1,
       1,     3,     1,     3,     0,     1,     2,     0,     0,    10,
       0,     3,     1,     3,     1,     4,     3,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: statements  */
#line 95 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
               {
        // 当整个程序解析完成，$1是sql_statements返回的根节点
        // 将它赋值给全局变量 ast_root
        ast_root = (yyvsp[0].node);
        std::cout << "[Parser] SQL parsing complete. AST root is set." << std::endl;
    }
#line 1633 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 3: /* statements: statement  */
#line 104 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
              {
        // 只有一个语句，创建一个列表节点，并把该语句作为子节点
        (yyval.node) = new ASTNode(ROOT_NODE, "", (yyloc));
        if ((yyvsp[0].node)) (yyval.node)->addChild((yyvsp[0].node));   // 在根节点下添加该语句
    }
#line 1643 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 4: /* statements: statements statement  */
#line 109 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           {
        if ((yyvsp[0].node)) (yyvsp[-1].node)->addChild((yyvsp[0].node));   // 已经有一个列表了($1)，把新的语句($2)加进去
        (yyval.node) = (yyvsp[-1].node); // 将更新后的列表向上传递
    }
#line 1652 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 5: /* statement: create_statement ';'  */
#line 117 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                         { current_parser_state = STATE_EXPECTING_COMMAND; (yyval.node) = (yyvsp[-1].node); }
#line 1658 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 6: /* statement: insert_statement ';'  */
#line 118 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           { current_parser_state = STATE_EXPECTING_COMMAND; (yyval.node) = (yyvsp[-1].node); }
#line 1664 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 7: /* statement: select_statement ';'  */
#line 119 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           { current_parser_state = STATE_EXPECTING_COMMAND; (yyval.node) = (yyvsp[-1].node); }
#line 1670 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 8: /* statement: delete_statement ';'  */
#line 120 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           { current_parser_state = STATE_EXPECTING_COMMAND; (yyval.node) = (yyvsp[-1].node); }
#line 1676 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 9: /* statement: update_statement ';'  */
#line 121 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           { current_parser_state = STATE_EXPECTING_COMMAND; (yyval.node) = (yyvsp[-1].node); }
#line 1682 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 10: /* statement: error ';'  */
#line 122 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           { current_parser_state = STATE_EXPECTING_COMMAND; (yyval.node) = nullptr; yyerrok; }
#line 1688 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 11: /* update_statement: K_UPDATE IDENTIFIER set_clause optional_where_clause  */
#line 130 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
    {
        (yyval.node) = new ASTNode(UPDATE_STMT, "", (yyloc));
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-2].str_val), (yylsp[-2]))); // 子节点0: 表名
        (yyval.node)->addChild((yyvsp[-1].node)); // 子节点1: SET_CLAUSE
        (yyval.node)->addChild((yyvsp[0].node)); // 子节点2: WHERE_CLAUSE
        free((yyvsp[-2].str_val));
    }
#line 1700 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 12: /* set_clause: K_SET set_list  */
#line 141 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
    {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1708 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 13: /* set_list: assignment  */
#line 148 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
    {
        (yyval.node) = new ASTNode(SET_CLAUSE, "", (yyloc));
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1717 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 14: /* set_list: set_list ',' assignment  */
#line 153 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
    {
        (yyvsp[-2].node)->addChild((yyvsp[0].node));
        (yyval.node) = (yyvsp[-2].node);
    }
#line 1726 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 15: /* assignment: IDENTIFIER OP_EQ literal  */
#line 161 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
    {
        // 创建一个 EQUAL_OPERATOR 节点来表示赋值
        (yyval.node) = new ASTNode(EQUAL_OPERATOR, "", (yylsp[-1]));
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-2].str_val), (yylsp[-2])));
        (yyval.node)->addChild((yyvsp[0].node));
        free((yyvsp[-2].str_val));
    }
#line 1738 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 16: /* $@1: %empty  */
#line 174 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
             { current_parser_state = STATE_EXPECTING_TABLE_AFTER_CREATE; }
#line 1744 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 17: /* create_statement: K_CREATE $@1 K_TABLE IDENTIFIER '(' column_definitions ')'  */
#line 175 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                                  {
        // 创建一个 CREATE_TABLE_STMT 节点
        (yyval.node) = new ASTNode(CREATE_TABLE_STMT, "", (yyloc));
        // 子节点1: 表名 (IDENTIFIER)
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-3].str_val), (yylsp[-3])));
        // 子节点2: 列定义列表(column_definitions)
        (yyval.node)->addChild((yyvsp[-1].node));

        free((yyvsp[-3].str_val)); // 释放由词法分析器中 strdup 分配的内存
    }
#line 1759 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 18: /* column_definitions: column_definition  */
#line 188 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                      {
        (yyval.node) = new ASTNode(COLUMN_DEFINITIONS_LIST, "", (yyloc));
        // 只有一个列定义(column_definition)，作为子节点添加
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1769 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 19: /* column_definitions: column_definitions ',' column_definition  */
#line 193 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                               {
        // 已经有一个列定义列表，把新的列定义加进去
        (yyvsp[-2].node)->addChild((yyvsp[0].node));
        (yyval.node) = (yyvsp[-2].node);
    }
#line 1779 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 20: /* $@2: %empty  */
#line 201 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
               { current_parser_state = STATE_EXPECTING_DATA_TYPE; }
#line 1785 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 21: /* column_definition: IDENTIFIER $@2 data_type  */
#line 202 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
              {
        (yyval.node) = new ASTNode(IDENTIFIER_NODE, (yyvsp[-2].str_val), (yylsp[-2]));
        // 将数据类型作为子节点添加
        (yyval.node)->addChild((yyvsp[0].node));
        free((yyvsp[-2].str_val));
    }
#line 1796 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 22: /* data_type: K_INT  */
#line 211 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
          {
        (yyval.node) = new ASTNode(DATA_TYPE_NODE, "INT", (yyloc));
    }
#line 1804 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 23: /* data_type: K_VARCHAR optional_varchar_length  */
#line 214 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                        {
        // 将长度信息存储在 data_type 节点的 value 中
        (yyval.node) = new ASTNode(DATA_TYPE_NODE, "VARCHAR", (yyloc));
        // 如果有长度定义 ($2 不为 nullptr)，可以将其作为子节点
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1815 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 24: /* optional_varchar_length: %empty  */
#line 223 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                { (yyval.node) = nullptr; }
#line 1821 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 25: /* optional_varchar_length: '(' INTEGER_CONST ')'  */
#line 224 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                            {
        (yyval.node) = new ASTNode(INTEGER_LITERAL_NODE, std::to_string((yyvsp[-1].int_val)), (yyloc));
    }
#line 1829 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 26: /* $@3: %empty  */
#line 234 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                          { current_parser_state = STATE_EXPECTING_FROM_AFTER_COLUMNS; }
#line 1835 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 27: /* select_statement: K_SELECT select_list $@3 from_clause optional_where_clause optional_group_by_clause optional_order_by_clause  */
#line 238 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
    {
        (yyval.node) = new ASTNode(SELECT_STMT, "", (yyloc));
        // 子节点1: 列表 (select_list)
        (yyval.node)->addChild((yyvsp[-5].node));
        // 子节点2: 表名 (IDENTIFIER)
        (yyval.node)->addChild((yyvsp[-3].node));
        // 子节点3: 可选的 WHERE 子句 (可能为nullptr)
        (yyval.node)->addChild((yyvsp[-2].node));
    }
#line 1849 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 28: /* $@4: %empty  */
#line 250 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
           { current_parser_state = STATE_EXPECTING_TABLE_AFTER_FROM; }
#line 1855 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 29: /* from_clause: K_FROM $@4 IDENTIFIER join_clause  */
#line 253 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
    {
        // 创建一个 FROM_CLAUSE 节点，并将表名作为其 value
        (yyval.node) = new ASTNode(FROM_CLAUSE, (yyvsp[-1].str_val), (yyloc));
        free((yyvsp[-1].str_val));
        // 如果有 JOIN 子句，将其作为 FROM_CLAUSE 的子节点
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1867 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 30: /* join_clause: %empty  */
#line 263 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                { (yyval.node) = nullptr; }
#line 1873 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 31: /* join_clause: K_JOIN IDENTIFIER K_ON expression  */
#line 265 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
    {
        // 創建 JOIN_CLAUSE 節點
        (yyval.node) = new ASTNode(JOIN_CLAUSE, "", (yylsp[-3]));
        // 子節點0: 被連接的表
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-2].str_val), (yylsp[-2])));
        free((yyvsp[-2].str_val));
        // 子節點1: ON 條件
        ASTNode* on_cond = new ASTNode(ON_CONDITION, "", (yylsp[-1]));
        on_cond->addChild((yyvsp[0].node));
        (yyval.node)->addChild(on_cond);
    }
#line 1889 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 32: /* optional_group_by_clause: %empty  */
#line 279 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                { (yyval.node) = nullptr; }
#line 1895 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 33: /* optional_group_by_clause: group_by_clause  */
#line 280 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 1901 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 34: /* group_by_clause: K_GROUP K_BY column_list  */
#line 285 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
    {
        (yyval.node) = new ASTNode(GROUP_BY_CLAUSE, "", (yyloc));
        for (auto child : (yyvsp[0].node)->children) {
            (yyval.node)->addChild(child);
        }
        (yyvsp[0].node)->children.clear(); // 避免双重释放
        delete (yyvsp[0].node);
    }
#line 1914 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 35: /* optional_order_by_clause: %empty  */
#line 296 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                { (yyval.node) = nullptr; }
#line 1920 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 36: /* optional_order_by_clause: order_by_clause  */
#line 297 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 1926 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 37: /* order_by_clause: K_ORDER K_BY column_list  */
#line 302 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
    {
        (yyval.node) = new ASTNode(ORDER_BY_CLAUSE, "", (yyloc));
        for (auto child : (yyvsp[0].node)->children) {
            (yyval.node)->addChild(child);
        }
        (yyvsp[0].node)->children.clear(); // 避免双重释放
        delete (yyvsp[0].node);
    }
#line 1939 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 38: /* select_list: '*'  */
#line 313 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
        { 
        (yyval.node) = new ASTNode(SELECT_LIST, "", (yyloc));
        // 使用一个特殊的 IDENTIFIER_NODE 来表示 '*'
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, "*", (yylsp[0])));
    }
#line 1949 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 39: /* select_list: column_list  */
#line 318 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                  { 
        // 将 column_list 包装在 SELECT_LIST 节点中
        (yyval.node) = new ASTNode(SELECT_LIST, "", (yyloc));
        // column_list 的子节点是 IDENTIFIER_NODE, 把它们移过来
        for(auto child : (yyvsp[0].node)->children) {
            (yyval.node)->addChild(child);
        }
        (yyvsp[0].node)->children.clear(); // 防止双重释放
        delete (yyvsp[0].node);
    }
#line 1964 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 40: /* column_list: column_name  */
#line 331 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                {
        (yyval.node) = new ASTNode(COLUMN_LIST, "", (yyloc)); 
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1973 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 41: /* column_list: column_list ',' column_name  */
#line 335 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                  {
        (yyvsp[-2].node)->addChild((yyvsp[0].node));
        (yyval.node) = (yyvsp[-2].node);
    }
#line 1982 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 42: /* column_name: IDENTIFIER  */
#line 342 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
               {
        (yyval.node) = new ASTNode(IDENTIFIER_NODE, (yyvsp[0].str_val), (yylsp[0]));
        free((yyvsp[0].str_val));
    }
#line 1991 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 43: /* column_name: IDENTIFIER '.' IDENTIFIER  */
#line 346 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                {
        std::string qualified_name = std::string((yyvsp[-2].str_val)) + "." + std::string((yyvsp[0].str_val));
        (yyval.node) = new ASTNode(IDENTIFIER_NODE, qualified_name, (yyloc));
        free((yyvsp[-2].str_val));
        free((yyvsp[0].str_val));
    }
#line 2002 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 44: /* optional_where_clause: %empty  */
#line 355 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                                      { (yyval.node) = nullptr; }
#line 2008 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 45: /* optional_where_clause: where_clause  */
#line 356 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 2014 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 46: /* where_clause: K_WHERE expression  */
#line 360 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                       {
        (yyval.node) = new ASTNode(WHERE_CLAUSE, "", (yyloc));
        (yyval.node)->addChild((yyvsp[0].node)); // 将表达式作为 WHERE_CLAUSE 的子节点
    }
#line 2023 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 47: /* $@5: %empty  */
#line 370 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
             { current_parser_state = STATE_EXPECTING_INTO_AFTER_INSERT; }
#line 2029 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 48: /* $@6: %empty  */
#line 371 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                           { current_parser_state = STATE_EXPECTING_VALUES_AFTER_TABLE; }
#line 2035 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 49: /* insert_statement: K_INSERT $@5 K_INTO IDENTIFIER optional_column_list $@6 K_VALUES '(' value_list ')'  */
#line 372 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                {
        (yyval.node) = new ASTNode(INSERT_STMT, "", (yyloc));
        // 子节点1: 表名
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-6].str_val), (yylsp[-6])));
        // 子节点2: 可选的列列表 (可能为nullptr)
        (yyval.node)->addChild((yyvsp[-5].node));
        // 子节点3: 值列表
        (yyval.node)->addChild((yyvsp[-1].node));   
        free((yyvsp[-6].str_val));
    }
#line 2050 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 50: /* optional_column_list: %empty  */
#line 390 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                { (yyval.node) = nullptr; }
#line 2056 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 51: /* optional_column_list: '(' column_list ')'  */
#line 391 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                          { (yyval.node) = (yyvsp[-1].node); }
#line 2062 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 52: /* value_list: value  */
#line 395 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
          {
        (yyval.node) = new ASTNode(VALUES_LIST, "", (yyloc));
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 2071 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 53: /* value_list: value_list ',' value  */
#line 399 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           {
        (yyvsp[-2].node)->addChild((yyvsp[0].node));
        (yyval.node) = (yyvsp[-2].node);
    }
#line 2080 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 54: /* value: literal  */
#line 406 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 2086 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 55: /* delete_statement: K_DELETE K_FROM IDENTIFIER optional_where_clause  */
#line 412 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                                     {
        (yyval.node) = new ASTNode(DELETE_STMT, "", (yyloc));
        // 子节点1: 表名
        (yyval.node)->addChild(new ASTNode(FROM_CLAUSE, (yyvsp[-1].str_val), (yylsp[-1])));
        // 子节点2: 可选的 WHERE 子句 (可能为nullptr)
        (yyval.node)->addChild((yyvsp[0].node));
        free((yyvsp[-1].str_val));
    }
#line 2099 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 56: /* expression: column_ref comparison_operator column_ref  */
#line 426 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                              {
        (yyval.node) = (yyvsp[-1].node);
        (yyval.node)->location = (yyloc);
        (yyval.node)->addChild((yyvsp[-2].node));
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 2110 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 57: /* expression: column_ref comparison_operator literal  */
#line 432 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                             { // 也保留 列=字面量
        (yyval.node) = (yyvsp[-1].node);
        (yyval.node)->location = (yyloc);
        (yyval.node)->addChild((yyvsp[-2].node));
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 2121 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 58: /* column_ref: IDENTIFIER  */
#line 441 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
               {
        (yyval.node) = new ASTNode(IDENTIFIER_NODE, (yyvsp[0].str_val), (yylsp[0]));
        free((yyvsp[0].str_val));
    }
#line 2130 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 59: /* column_ref: IDENTIFIER '.' IDENTIFIER  */
#line 445 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                {
        std::string qualified_name = std::string((yyvsp[-2].str_val)) + "." + std::string((yyvsp[0].str_val));
        (yyval.node) = new ASTNode(IDENTIFIER_NODE, qualified_name, (yyloc));
        free((yyvsp[-2].str_val));
        free((yyvsp[0].str_val));
    }
#line 2141 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 60: /* comparison_operator: OP_EQ  */
#line 454 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
           { (yyval.node) = new ASTNode(EQUAL_OPERATOR, "=", (yylsp[0]));  }
#line 2147 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 61: /* comparison_operator: OP_NEQ  */
#line 455 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
             { (yyval.node) = new ASTNode(BINARY_EXPR, "!=", (yylsp[0])); }
#line 2153 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 62: /* comparison_operator: OP_GT  */
#line 456 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
             { (yyval.node) = new ASTNode(GREATER_THAN_OPERATOR, ">", (yylsp[0]));  }
#line 2159 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 63: /* comparison_operator: OP_GTE  */
#line 457 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
              { (yyval.node) = new ASTNode(GREATER_THAN_OR_EQUAL_OPERATOR, ">=", (yylsp[0])); }
#line 2165 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 64: /* comparison_operator: OP_LT  */
#line 458 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
             { (yyval.node) = new ASTNode(LESS_THAN_OPERATOR, "<", (yylsp[0]));  }
#line 2171 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 65: /* comparison_operator: OP_LTE  */
#line 459 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
              { (yyval.node) = new ASTNode(LESS_THAN_OR_EQUAL_OPERATOR, "<=", (yylsp[0])); }
#line 2177 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 66: /* literal: INTEGER_CONST  */
#line 463 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                  {
        (yyval.node) = new ASTNode(INTEGER_LITERAL_NODE, std::to_string((yyvsp[0].int_val)), (yylsp[0]));
    }
#line 2185 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 67: /* literal: STRING_CONST  */
#line 466 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                   {
        (yyval.node) = new ASTNode(STRING_LITERAL_NODE, (yyvsp[0].str_val), (yylsp[0]));
        free((yyvsp[0].str_val));
    }
#line 2194 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;


#line 2198 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 472 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"

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
    case GREATER_THAN_OPERATOR:
        return "GREATER_THAN_OPERATOR";
    case GREATER_THAN_OR_EQUAL_OPERATOR:
        return "GREATER_THAN_OR_EQUAL_OPERATOR";
    case LESS_THAN_OPERATOR:
        return "LESS_THAN_OPERATOR";
    case LESS_THAN_OR_EQUAL_OPERATOR:
        return "LESS_THAN_OR_EQUAL_OPERATOR";
    case BINARY_EXPR:
        return "BINARY_EXPR";
    case FROM_CLAUSE:
        return "FROM_CLAUSE";
    case JOIN_CLAUSE:
        return "JOIN_CLAUSE";
    case ON_CONDITION:
        return "ON_CONDITION";
    case GROUP_BY_CLAUSE:
        return "GROUP_BY_CLAUSE";
    case ORDER_BY_CLAUSE:
        return "ORDER_BY_CLAUSE";
    case UPDATE_STMT:
        return "UPDATE_STMT";
    case SET_CLAUSE:
        return "SET_CLAUSE";
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
