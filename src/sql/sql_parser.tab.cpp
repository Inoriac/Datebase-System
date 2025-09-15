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
  YYSYMBOL_K_INT = 15,                     /* K_INT  */
  YYSYMBOL_K_VARCHAR = 16,                 /* K_VARCHAR  */
  YYSYMBOL_OP_EQ = 17,                     /* OP_EQ  */
  YYSYMBOL_OP_LT = 18,                     /* OP_LT  */
  YYSYMBOL_OP_GT = 19,                     /* OP_GT  */
  YYSYMBOL_OP_LTE = 20,                    /* OP_LTE  */
  YYSYMBOL_OP_GTE = 21,                    /* OP_GTE  */
  YYSYMBOL_OP_NEQ = 22,                    /* OP_NEQ  */
  YYSYMBOL_23_ = 23,                       /* ';'  */
  YYSYMBOL_24_ = 24,                       /* '('  */
  YYSYMBOL_25_ = 25,                       /* ')'  */
  YYSYMBOL_26_ = 26,                       /* ','  */
  YYSYMBOL_27_ = 27,                       /* '*'  */
  YYSYMBOL_YYACCEPT = 28,                  /* $accept  */
  YYSYMBOL_program = 29,                   /* program  */
  YYSYMBOL_statements = 30,                /* statements  */
  YYSYMBOL_statement = 31,                 /* statement  */
  YYSYMBOL_create_statement = 32,          /* create_statement  */
  YYSYMBOL_33_1 = 33,                      /* $@1  */
  YYSYMBOL_column_definitions = 34,        /* column_definitions  */
  YYSYMBOL_column_definition = 35,         /* column_definition  */
  YYSYMBOL_36_2 = 36,                      /* $@2  */
  YYSYMBOL_data_type = 37,                 /* data_type  */
  YYSYMBOL_optional_varchar_length = 38,   /* optional_varchar_length  */
  YYSYMBOL_select_statement = 39,          /* select_statement  */
  YYSYMBOL_40_3 = 40,                      /* $@3  */
  YYSYMBOL_select_list = 41,               /* select_list  */
  YYSYMBOL_column_list = 42,               /* column_list  */
  YYSYMBOL_column_name = 43,               /* column_name  */
  YYSYMBOL_optional_where_clause = 44,     /* optional_where_clause  */
  YYSYMBOL_where_clause = 45,              /* where_clause  */
  YYSYMBOL_insert_statement = 46,          /* insert_statement  */
  YYSYMBOL_47_4 = 47,                      /* $@4  */
  YYSYMBOL_48_5 = 48,                      /* $@5  */
  YYSYMBOL_optional_column_list = 49,      /* optional_column_list  */
  YYSYMBOL_value_list = 50,                /* value_list  */
  YYSYMBOL_value = 51,                     /* value  */
  YYSYMBOL_delete_statement = 52,          /* delete_statement  */
  YYSYMBOL_expression = 53,                /* expression  */
  YYSYMBOL_comparison_operator = 54,       /* comparison_operator  */
  YYSYMBOL_literal = 55                    /* literal  */
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
#define YYFINAL  22
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   63

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  28
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  47
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  82

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   277


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
      24,    25,    27,     2,    26,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    23,
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
      15,    16,    17,    18,    19,    20,    21,    22
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    86,    86,    95,   100,   108,   109,   110,   111,   112,
     119,   119,   133,   138,   146,   146,   156,   159,   168,   169,
     179,   179,   193,   198,   211,   215,   222,   229,   230,   234,
     244,   245,   244,   264,   265,   269,   273,   280,   286,   301,
     310,   311,   312,   313,   314,   315,   319,   322
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
  "K_INT", "K_VARCHAR", "OP_EQ", "OP_LT", "OP_GT", "OP_LTE", "OP_GTE",
  "OP_NEQ", "';'", "'('", "')'", "','", "'*'", "$accept", "program",
  "statements", "statement", "create_statement", "$@1",
  "column_definitions", "column_definition", "$@2", "data_type",
  "optional_varchar_length", "select_statement", "$@3", "select_list",
  "column_list", "column_name", "optional_where_clause", "where_clause",
  "insert_statement", "$@4", "$@5", "optional_column_list", "value_list",
  "value", "delete_statement", "expression", "comparison_operator",
  "literal", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-22)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-3)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       2,   -17,   -22,   -22,    -3,    -1,    14,     1,   -22,     4,
      11,    12,    13,   -22,    16,    28,   -22,   -22,   -22,    17,
     -22,    35,   -22,   -22,   -22,   -22,   -22,   -22,    36,    37,
      29,    39,    31,    22,    23,    42,   -22,    45,   -22,   -22,
      46,    39,   -22,    31,     0,   -22,   -22,   -21,   -22,     3,
      40,   -22,   -22,   -22,   -22,   -22,   -22,   -22,    21,    15,
     -22,    46,   -22,    27,   -22,   -22,   -22,   -22,    30,   -22,
     -22,    21,    47,   -22,     7,   -22,   -22,    32,   -22,    21,
     -22,   -22
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,    10,    30,     0,     0,     0,     0,     3,     0,
       0,     0,     0,     9,     0,     0,    26,    22,    20,    23,
      24,     0,     1,     4,     5,     7,     6,     8,     0,     0,
       0,     0,    27,     0,    33,     0,    25,     0,    38,    28,
       0,     0,    31,    27,     0,    29,    14,     0,    12,     0,
       0,    21,    40,    44,    42,    45,    43,    41,     0,     0,
      11,     0,    34,     0,    47,    46,    39,    16,    18,    15,
      13,     0,     0,    17,     0,    35,    37,     0,    32,     0,
      19,    36
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -22,   -22,   -22,    48,   -22,   -22,   -22,    -8,   -22,   -22,
     -22,   -22,   -22,   -22,    18,    25,    19,   -22,   -22,   -22,
     -22,   -22,   -22,   -19,   -22,   -22,   -22,     5
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     6,     7,     8,     9,    14,    47,    48,    59,    69,
      73,    10,    30,    18,    19,    20,    38,    39,    11,    15,
      50,    42,    74,    75,    12,    45,    58,    76
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      16,    -2,     1,     1,    60,    61,    13,     2,     2,     3,
       3,    21,     4,     4,    22,     5,     5,    52,    53,    54,
      55,    56,    57,    28,    17,    64,    65,    24,    62,    31,
      67,    68,    78,    79,    25,    26,    27,    29,    32,    33,
      34,    35,    16,    31,    37,    43,    40,    41,    44,    46,
      63,    71,    77,    70,    72,    23,    36,    80,     0,    49,
      81,     0,    51,    66
};

static const yytype_int8 yycheck[] =
{
       3,     0,     1,     1,    25,    26,    23,     6,     6,     8,
       8,    12,    11,    11,     0,    14,    14,    17,    18,    19,
      20,    21,    22,     7,    27,     4,     5,    23,    25,    26,
      15,    16,    25,    26,    23,    23,    23,     9,     3,     3,
       3,    12,     3,    26,    13,     3,    24,    24,     3,     3,
      10,    24,     5,    61,    24,     7,    31,    25,    -1,    41,
      79,    -1,    43,    58
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     6,     8,    11,    14,    29,    30,    31,    32,
      39,    46,    52,    23,    33,    47,     3,    27,    41,    42,
      43,    12,     0,    31,    23,    23,    23,    23,     7,     9,
      40,    26,     3,     3,     3,    12,    43,    13,    44,    45,
      24,    24,    49,     3,     3,    53,     3,    34,    35,    42,
      48,    44,    17,    18,    19,    20,    21,    22,    54,    36,
      25,    26,    25,    10,     4,     5,    55,    15,    16,    37,
      35,    24,    24,    38,    50,    51,    55,     5,    25,    26,
      25,    51
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    28,    29,    30,    30,    31,    31,    31,    31,    31,
      33,    32,    34,    34,    36,    35,    37,    37,    38,    38,
      40,    39,    41,    41,    42,    42,    43,    44,    44,    45,
      47,    48,    46,    49,    49,    50,    50,    51,    52,    53,
      54,    54,    54,    54,    54,    54,    55,    55
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     2,     2,     2,     2,     2,
       0,     7,     1,     3,     0,     3,     1,     2,     0,     3,
       0,     6,     1,     1,     1,     3,     1,     0,     1,     2,
       0,     0,    10,     0,     3,     1,     3,     1,     4,     3,
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
#line 86 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
               {
        // 当整个程序解析完成，$1是sql_statements返回的根节点
        // 将它赋值给全局变量 ast_root
        ast_root = (yyvsp[0].node);
        std::cout << "[Parser] SQL parsing complete. AST root is set." << std::endl;
    }
#line 1583 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 3: /* statements: statement  */
#line 95 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
              {
        // 只有一个语句，创建一个列表节点，并把该语句作为子节点
        (yyval.node) = new ASTNode(ROOT_NODE, "", (yyloc));
        if ((yyvsp[0].node)) (yyval.node)->addChild((yyvsp[0].node));   // 在根节点下添加该语句
    }
#line 1593 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 4: /* statements: statements statement  */
#line 100 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           {
        if ((yyvsp[0].node)) (yyvsp[-1].node)->addChild((yyvsp[0].node));   // 已经有一个列表了($1)，把新的语句($2)加进去
        (yyval.node) = (yyvsp[-1].node); // 将更新后的列表向上传递
    }
#line 1602 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 5: /* statement: create_statement ';'  */
#line 108 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                         { current_parser_state = STATE_EXPECTING_COMMAND; (yyval.node) = (yyvsp[-1].node); }
#line 1608 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 6: /* statement: insert_statement ';'  */
#line 109 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           { current_parser_state = STATE_EXPECTING_COMMAND; (yyval.node) = (yyvsp[-1].node); }
#line 1614 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 7: /* statement: select_statement ';'  */
#line 110 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           { current_parser_state = STATE_EXPECTING_COMMAND; (yyval.node) = (yyvsp[-1].node); }
#line 1620 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 8: /* statement: delete_statement ';'  */
#line 111 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           { current_parser_state = STATE_EXPECTING_COMMAND; (yyval.node) = (yyvsp[-1].node); }
#line 1626 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 9: /* statement: error ';'  */
#line 112 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           { current_parser_state = STATE_EXPECTING_COMMAND; (yyval.node) = nullptr; yyerrok; }
#line 1632 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 10: /* $@1: %empty  */
#line 119 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
             { current_parser_state = STATE_EXPECTING_TABLE_AFTER_CREATE; }
#line 1638 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 11: /* create_statement: K_CREATE $@1 K_TABLE IDENTIFIER '(' column_definitions ')'  */
#line 120 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                                  {
        // 创建一个 CREATE_TABLE_STMT 节点
        (yyval.node) = new ASTNode(CREATE_TABLE_STMT, "", (yyloc));
        // 子节点1: 表名 (IDENTIFIER)
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-3].str_val), (yylsp[-3])));
        // 子节点2: 列定义列表(column_definitions)
        (yyval.node)->addChild((yyvsp[-1].node));

        free((yyvsp[-3].str_val)); // 释放由词法分析器中 strdup 分配的内存
    }
#line 1653 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 12: /* column_definitions: column_definition  */
#line 133 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                      {
        (yyval.node) = new ASTNode(COLUMN_DEFINITIONS_LIST, "", (yyloc));
        // 只有一个列定义(column_definition)，作为子节点添加
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1663 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 13: /* column_definitions: column_definitions ',' column_definition  */
#line 138 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                               {
        // 已经有一个列定义列表，把新的列定义加进去
        (yyvsp[-2].node)->addChild((yyvsp[0].node));
        (yyval.node) = (yyvsp[-2].node);
    }
#line 1673 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 14: /* $@2: %empty  */
#line 146 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
               { current_parser_state = STATE_EXPECTING_DATA_TYPE; }
#line 1679 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 15: /* column_definition: IDENTIFIER $@2 data_type  */
#line 147 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
              {
        (yyval.node) = new ASTNode(IDENTIFIER_NODE, (yyvsp[-2].str_val), (yylsp[-2]));
        // 将数据类型作为子节点添加
        (yyval.node)->addChild((yyvsp[0].node));
        free((yyvsp[-2].str_val));
    }
#line 1690 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 16: /* data_type: K_INT  */
#line 156 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
          {
        (yyval.node) = new ASTNode(DATA_TYPE_NODE, "INT", (yyloc));
    }
#line 1698 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 17: /* data_type: K_VARCHAR optional_varchar_length  */
#line 159 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                        {
        // 将长度信息存储在 data_type 节点的 value 中
        (yyval.node) = new ASTNode(DATA_TYPE_NODE, "VARCHAR", (yyloc));
        // 如果有长度定义 ($2 不为 nullptr)，可以将其作为子节点
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1709 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 18: /* optional_varchar_length: %empty  */
#line 168 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                { (yyval.node) = nullptr; }
#line 1715 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 19: /* optional_varchar_length: '(' INTEGER_CONST ')'  */
#line 169 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                            {
        (yyval.node) = new ASTNode(INTEGER_LITERAL_NODE, std::to_string((yyvsp[-1].int_val)), (yyloc));
    }
#line 1723 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 20: /* $@3: %empty  */
#line 179 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                          { current_parser_state = STATE_EXPECTING_FROM_AFTER_COLUMNS; }
#line 1729 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 21: /* select_statement: K_SELECT select_list $@3 K_FROM IDENTIFIER optional_where_clause  */
#line 180 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                            {
        (yyval.node) = new ASTNode(SELECT_STMT, "", (yyloc));
        // 子节点1: 列表 (select_list)
        (yyval.node)->addChild((yyvsp[-4].node));
        // 子节点2: 表名 (IDENTIFIER)
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-1].str_val), (yylsp[-1])));
        // 子节点3: 可选的 WHERE 子句 (可能为nullptr)
        (yyval.node)->addChild((yyvsp[0].node));
        free((yyvsp[-1].str_val));
    }
#line 1744 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 22: /* select_list: '*'  */
#line 193 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
        { 
        (yyval.node) = new ASTNode(SELECT_LIST, "", (yyloc));
        // 使用一个特殊的 IDENTIFIER_NODE 来表示 '*'
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, "*", (yylsp[0])));
    }
#line 1754 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 23: /* select_list: column_list  */
#line 198 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
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
#line 1769 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 24: /* column_list: column_name  */
#line 211 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                {
        (yyval.node) = new ASTNode(COLUMN_LIST, "", (yyloc)); 
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1778 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 25: /* column_list: column_list ',' column_name  */
#line 215 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                  {
        (yyvsp[-2].node)->addChild((yyvsp[0].node));
        (yyval.node) = (yyvsp[-2].node);
    }
#line 1787 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 26: /* column_name: IDENTIFIER  */
#line 222 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
               {
        (yyval.node) = new ASTNode(IDENTIFIER_NODE, (yyvsp[0].str_val), (yylsp[0]));
        free((yyvsp[0].str_val));
    }
#line 1796 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 27: /* optional_where_clause: %empty  */
#line 229 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                                      { (yyval.node) = nullptr; }
#line 1802 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 28: /* optional_where_clause: where_clause  */
#line 230 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 1808 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 29: /* where_clause: K_WHERE expression  */
#line 234 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                       {
        (yyval.node) = new ASTNode(WHERE_CLAUSE, "", (yyloc));
        (yyval.node)->addChild((yyvsp[0].node)); // 将表达式作为 WHERE_CLAUSE 的子节点
    }
#line 1817 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 30: /* $@4: %empty  */
#line 244 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
             { current_parser_state = STATE_EXPECTING_INTO_AFTER_INSERT; }
#line 1823 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 31: /* $@5: %empty  */
#line 245 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                           { current_parser_state = STATE_EXPECTING_VALUES_AFTER_TABLE; }
#line 1829 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 32: /* insert_statement: K_INSERT $@4 K_INTO IDENTIFIER optional_column_list $@5 K_VALUES '(' value_list ')'  */
#line 246 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
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
#line 1844 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 33: /* optional_column_list: %empty  */
#line 264 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                { (yyval.node) = nullptr; }
#line 1850 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 34: /* optional_column_list: '(' column_list ')'  */
#line 265 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                          { (yyval.node) = (yyvsp[-1].node); }
#line 1856 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 35: /* value_list: value  */
#line 269 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
          {
        (yyval.node) = new ASTNode(VALUES_LIST, "", (yyloc));
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1865 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 36: /* value_list: value_list ',' value  */
#line 273 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                           {
        (yyvsp[-2].node)->addChild((yyvsp[0].node));
        (yyval.node) = (yyvsp[-2].node);
    }
#line 1874 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 37: /* value: literal  */
#line 280 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 1880 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 38: /* delete_statement: K_DELETE K_FROM IDENTIFIER optional_where_clause  */
#line 286 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                                     {
        (yyval.node) = new ASTNode(DELETE_STMT, "", (yyloc));
        // 子节点1: 表名
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-1].str_val), (yylsp[-1])));
        // 子节点2: 可选的 WHERE 子句 (可能为nullptr)
        (yyval.node)->addChild((yyvsp[0].node));
        free((yyvsp[-1].str_val));
    }
#line 1893 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 39: /* expression: IDENTIFIER comparison_operator literal  */
#line 301 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                                           {
        (yyval.node) = (yyvsp[-1].node);
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-2].str_val), (yylsp[-2])));
        (yyval.node)->addChild((yyvsp[0].node));
        free((yyvsp[-2].str_val));
    }
#line 1904 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 40: /* comparison_operator: OP_EQ  */
#line 310 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
           { (yyval.node) = new ASTNode(BINARY_EXPR, "=", (yylsp[0]));  }
#line 1910 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 41: /* comparison_operator: OP_NEQ  */
#line 311 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
             { (yyval.node) = new ASTNode(BINARY_EXPR, "!=", (yylsp[0])); }
#line 1916 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 42: /* comparison_operator: OP_GT  */
#line 312 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
             { (yyval.node) = new ASTNode(BINARY_EXPR, ">", (yylsp[0]));  }
#line 1922 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 43: /* comparison_operator: OP_GTE  */
#line 313 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
              { (yyval.node) = new ASTNode(BINARY_EXPR, ">=", (yylsp[0])); }
#line 1928 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 44: /* comparison_operator: OP_LT  */
#line 314 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
             { (yyval.node) = new ASTNode(BINARY_EXPR, "<", (yylsp[0]));  }
#line 1934 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 45: /* comparison_operator: OP_LTE  */
#line 315 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
              { (yyval.node) = new ASTNode(BINARY_EXPR, "<=", (yylsp[0])); }
#line 1940 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 46: /* literal: INTEGER_CONST  */
#line 319 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                  {
        (yyval.node) = new ASTNode(INTEGER_LITERAL_NODE, std::to_string((yyvsp[0].int_val)), (yylsp[0]));
    }
#line 1948 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;

  case 47: /* literal: STRING_CONST  */
#line 322 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"
                   {
        (yyval.node) = new ASTNode(STRING_LITERAL_NODE, (yyvsp[0].str_val), (yylsp[0]));
        free((yyvsp[0].str_val));
    }
#line 1957 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"
    break;


#line 1961 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.tab.cpp"

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

#line 328 "/home/cao/Desktop/Database-Sys/Datebase-System/src/sql/sql_parser.y"

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
