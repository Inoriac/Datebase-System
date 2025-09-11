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
#line 2 "sql_parser.y"

#include <iostream>
#include <string>
#include "ast.h" // 包含 AST 节点定义

// 声明由 Flex 生成的词法分析函数
extern int yylex(); 

// 声明错误报告函数
void yyerror(const char *s);

// 存储最终AST的根节点
ASTNode* ast_root = nullptr;


#line 87 "sql_parser.tab.cpp"

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
  YYSYMBOL_column_definitions = 33,        /* column_definitions  */
  YYSYMBOL_column_definition = 34,         /* column_definition  */
  YYSYMBOL_select_statement = 35,          /* select_statement  */
  YYSYMBOL_select_list = 36,               /* select_list  */
  YYSYMBOL_column_list = 37,               /* column_list  */
  YYSYMBOL_column_name = 38,               /* column_name  */
  YYSYMBOL_optional_where_clause = 39,     /* optional_where_clause  */
  YYSYMBOL_where_clause = 40,              /* where_clause  */
  YYSYMBOL_insert_statement = 41,          /* insert_statement  */
  YYSYMBOL_optional_column_list = 42,      /* optional_column_list  */
  YYSYMBOL_value_list = 43,                /* value_list  */
  YYSYMBOL_value = 44,                     /* value  */
  YYSYMBOL_delete_statement = 45,          /* delete_statement  */
  YYSYMBOL_expression = 46,                /* expression  */
  YYSYMBOL_comparison_operator = 47,       /* comparison_operator  */
  YYSYMBOL_literal = 48                    /* literal  */
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

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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
#define YYLAST   62

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  28
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  39
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  75

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
       0,    67,    67,    76,    81,    90,    91,    92,    93,    94,
     101,   114,   118,   125,   129,   140,   150,   153,   157,   161,
     168,   175,   176,   180,   190,   208,   209,   213,   217,   224,
     230,   245,   256,   257,   258,   259,   260,   261,   265,   268
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
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
  "statements", "statement", "create_statement", "column_definitions",
  "column_definition", "select_statement", "select_list", "column_list",
  "column_name", "optional_where_clause", "where_clause",
  "insert_statement", "optional_column_list", "value_list", "value",
  "delete_statement", "expression", "comparison_operator", "literal", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-18)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-3)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       2,   -17,     4,    14,    -3,    15,    34,     1,   -18,    -9,
      12,    13,    16,   -18,    35,    37,   -18,   -18,    25,    17,
     -18,    38,   -18,   -18,   -18,   -18,   -18,   -18,    18,    20,
      42,    43,    36,    44,    43,    40,    36,   -18,    45,   -18,
     -18,   -11,     3,   -18,     5,    27,   -18,     0,   -18,   -18,
      28,   -18,    44,   -18,    21,   -18,   -18,   -18,   -18,   -18,
     -18,    21,    48,   -18,   -18,   -18,     7,   -18,   -18,   -18,
      29,   -18,    21,   -18,   -18
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     3,     0,
       0,     0,     0,     9,     0,     0,    20,    16,     0,    17,
      18,     0,     1,     4,     5,     7,     6,     8,     0,    25,
       0,     0,    21,     0,     0,     0,    21,    19,     0,    30,
      22,     0,     0,    11,     0,     0,    15,     0,    23,    13,
       0,    10,     0,    26,     0,    32,    36,    34,    37,    35,
      33,     0,     0,    12,    39,    38,     0,    27,    29,    31,
       0,    24,     0,    14,    28
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -18,   -18,   -18,    49,   -18,   -18,     6,   -18,   -18,    23,
      24,    26,   -18,   -18,   -18,   -18,   -13,   -18,   -18,   -18,
      -1
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     6,     7,     8,     9,    42,    43,    10,    18,    19,
      20,    39,    40,    11,    35,    66,    67,    12,    48,    61,
      68
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      16,    -2,     1,     1,    49,    50,    13,     2,     2,     3,
       3,    14,     4,     4,    24,     5,     5,    55,    56,    57,
      58,    59,    60,    15,    17,    64,    65,    21,    51,    52,
      53,    31,    71,    72,    22,    25,    26,    30,    28,    27,
      29,    32,    33,    31,    34,    36,    16,    41,    47,    38,
      45,    54,    62,    70,    73,    37,    23,    44,    63,    74,
      69,     0,    46
};

static const yytype_int8 yycheck[] =
{
       3,     0,     1,     1,    15,    16,    23,     6,     6,     8,
       8,     7,    11,    11,    23,    14,    14,    17,    18,    19,
      20,    21,    22,     9,    27,     4,     5,    12,    25,    26,
      25,    26,    25,    26,     0,    23,    23,    12,     3,    23,
       3,     3,    24,    26,    24,     3,     3,     3,     3,    13,
      10,    24,    24,     5,    25,    31,     7,    34,    52,    72,
      61,    -1,    36
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     6,     8,    11,    14,    29,    30,    31,    32,
      35,    41,    45,    23,     7,     9,     3,    27,    36,    37,
      38,    12,     0,    31,    23,    23,    23,    23,     3,     3,
      12,    26,     3,    24,    24,    42,     3,    38,    13,    39,
      40,     3,    33,    34,    37,    10,    39,     3,    46,    15,
      16,    25,    26,    25,    24,    17,    18,    19,    20,    21,
      22,    47,    24,    34,     4,     5,    43,    44,    48,    48,
       5,    25,    26,    25,    44
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    28,    29,    30,    30,    31,    31,    31,    31,    31,
      32,    33,    33,    34,    34,    35,    36,    36,    37,    37,
      38,    39,    39,    40,    41,    42,    42,    43,    43,    44,
      45,    46,    47,    47,    47,    47,    47,    47,    48,    48
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     2,     2,     2,     2,     2,
       6,     1,     3,     2,     5,     5,     1,     1,     1,     3,
       1,     0,     1,     2,     8,     0,     3,     1,     3,     1,
       4,     3,     1,     1,     1,     1,     1,     1,     1,     1
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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
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
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
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
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
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

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

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

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
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
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: statements  */
#line 67 "sql_parser.y"
               {
        // 当整个程序解析完成，$1是sql_statements返回的根节点
        // 将它赋值给全局变量 ast_root
        ast_root = (yyvsp[0].node);
        std::cout << "[Parser] SQL parsing complete. AST root is set." << std::endl;
    }
#line 1169 "sql_parser.tab.cpp"
    break;

  case 3: /* statements: statement  */
#line 76 "sql_parser.y"
              {
        // 只有一个语句，创建一个列表节点，并把该语句作为子节点
        (yyval.node) = new ASTNode(SQL_STATEMENTS_LIST);
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1179 "sql_parser.tab.cpp"
    break;

  case 4: /* statements: statements statement  */
#line 81 "sql_parser.y"
                           {
        // 已经有一个列表了($1)，把新的语句($2)加进去
        (yyvsp[-1].node)->addChild((yyvsp[0].node));
        (yyval.node) = (yyvsp[-1].node); // 将更新后的列表向上传递
    }
#line 1189 "sql_parser.tab.cpp"
    break;

  case 5: /* statement: create_statement ';'  */
#line 90 "sql_parser.y"
                         { (yyval.node) = (yyvsp[-1].node); }
#line 1195 "sql_parser.tab.cpp"
    break;

  case 6: /* statement: insert_statement ';'  */
#line 91 "sql_parser.y"
                           { (yyval.node) = (yyvsp[-1].node); }
#line 1201 "sql_parser.tab.cpp"
    break;

  case 7: /* statement: select_statement ';'  */
#line 92 "sql_parser.y"
                           { (yyval.node) = (yyvsp[-1].node); }
#line 1207 "sql_parser.tab.cpp"
    break;

  case 8: /* statement: delete_statement ';'  */
#line 93 "sql_parser.y"
                           { (yyval.node) = (yyvsp[-1].node); }
#line 1213 "sql_parser.tab.cpp"
    break;

  case 9: /* statement: error ';'  */
#line 94 "sql_parser.y"
                { yyerrok; }
#line 1219 "sql_parser.tab.cpp"
    break;

  case 10: /* create_statement: K_CREATE K_TABLE IDENTIFIER '(' column_definitions ')'  */
#line 101 "sql_parser.y"
                                                           {
        // 创建一个 CREATE_TABLE_STMT 节点
        (yyval.node) = new ASTNode(CREATE_TABLE_STMT);
        // 子节点1: 表名 (IDENTIFIER)
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-3].str_val)));
        // 子节点2: 列定义列表
        (yyval.node)->addChild((yyvsp[-1].node));

        free((yyvsp[-3].str_val)); // 释放由词法分析器中 strdup 分配的内存
    }
#line 1234 "sql_parser.tab.cpp"
    break;

  case 11: /* column_definitions: column_definition  */
#line 114 "sql_parser.y"
                      {
        (yyval.node) = new ASTNode(COLUMN_DEFINITIONS_LIST);
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1243 "sql_parser.tab.cpp"
    break;

  case 12: /* column_definitions: column_definitions ',' column_definition  */
#line 118 "sql_parser.y"
                                               {
        (yyvsp[-2].node)->addChild((yyvsp[0].node));
        (yyval.node) = (yyvsp[-2].node);
    }
#line 1252 "sql_parser.tab.cpp"
    break;

  case 13: /* column_definition: IDENTIFIER K_INT  */
#line 125 "sql_parser.y"
                     {
        (yyval.node) = new ASTNode(IDENTIFIER_NODE, (yyvsp[-1].str_val));
        free((yyvsp[-1].str_val));
    }
#line 1261 "sql_parser.tab.cpp"
    break;

  case 14: /* column_definition: IDENTIFIER K_VARCHAR '(' INTEGER_CONST ')'  */
#line 129 "sql_parser.y"
                                                 {
        (yyval.node) = new ASTNode(IDENTIFIER_NODE, (yyvsp[-4].str_val));
        free((yyvsp[-4].str_val));
    }
#line 1270 "sql_parser.tab.cpp"
    break;

  case 15: /* select_statement: K_SELECT select_list K_FROM IDENTIFIER optional_where_clause  */
#line 140 "sql_parser.y"
                                                                 {
        (yyval.node) = new ASTNode(SELECT_STMT);
        (yyval.node)->addChild((yyvsp[-3].node));
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-1].str_val)));
        (yyval.node)->addChild((yyvsp[0].node));
        free((yyvsp[-1].str_val));
    }
#line 1282 "sql_parser.tab.cpp"
    break;

  case 16: /* select_list: '*'  */
#line 150 "sql_parser.y"
        { 
        (yyval.node) = new ASTNode(IDENTIFIER_NODE, "*");
    }
#line 1290 "sql_parser.tab.cpp"
    break;

  case 17: /* select_list: column_list  */
#line 153 "sql_parser.y"
                  {  (yyval.node) = (yyvsp[0].node); }
#line 1296 "sql_parser.tab.cpp"
    break;

  case 18: /* column_list: column_name  */
#line 157 "sql_parser.y"
                {
        (yyval.node) = new ASTNode(COLUMN_LIST);
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1305 "sql_parser.tab.cpp"
    break;

  case 19: /* column_list: column_list ',' column_name  */
#line 161 "sql_parser.y"
                                  {
        (yyvsp[-2].node)->addChild((yyvsp[0].node));
        (yyval.node) = (yyvsp[-2].node);
    }
#line 1314 "sql_parser.tab.cpp"
    break;

  case 20: /* column_name: IDENTIFIER  */
#line 168 "sql_parser.y"
               {
        (yyval.node) = new ASTNode(IDENTIFIER_NODE, (yyvsp[0].str_val));
        free((yyvsp[0].str_val));
    }
#line 1323 "sql_parser.tab.cpp"
    break;

  case 21: /* optional_where_clause: %empty  */
#line 175 "sql_parser.y"
                                                      { (yyval.node) = nullptr; }
#line 1329 "sql_parser.tab.cpp"
    break;

  case 22: /* optional_where_clause: where_clause  */
#line 176 "sql_parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 1335 "sql_parser.tab.cpp"
    break;

  case 23: /* where_clause: K_WHERE expression  */
#line 180 "sql_parser.y"
                       {
        // 直接返回表达式子树
        (yyval.node) = (yyvsp[0].node);
    }
#line 1344 "sql_parser.tab.cpp"
    break;

  case 24: /* insert_statement: K_INSERT K_INTO IDENTIFIER optional_column_list K_VALUES '(' value_list ')'  */
#line 190 "sql_parser.y"
                                                                                {
        (yyval.node) = new ASTNode(INSERT_STMT);
        // 子节点1: 表名
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-5].str_val)));
        // 子节点2: 可选的列列表 (可能为nullptr)
        (yyval.node)->addChild((yyvsp[-4].node));
        // 子节点3: 值列表
        (yyval.node)->addChild((yyvsp[-1].node));
        free((yyvsp[-5].str_val));
    }
#line 1359 "sql_parser.tab.cpp"
    break;

  case 25: /* optional_column_list: %empty  */
#line 208 "sql_parser.y"
                { (yyval.node) = nullptr; }
#line 1365 "sql_parser.tab.cpp"
    break;

  case 26: /* optional_column_list: '(' column_list ')'  */
#line 209 "sql_parser.y"
                          { (yyval.node) = (yyvsp[-1].node); }
#line 1371 "sql_parser.tab.cpp"
    break;

  case 27: /* value_list: value  */
#line 213 "sql_parser.y"
          {
        (yyval.node) = new ASTNode(VALUES_LIST);
        (yyval.node)->addChild((yyvsp[0].node));
    }
#line 1380 "sql_parser.tab.cpp"
    break;

  case 28: /* value_list: value_list ',' value  */
#line 217 "sql_parser.y"
                           {
        (yyvsp[-2].node)->addChild((yyvsp[0].node));
        (yyval.node) = (yyvsp[-2].node);
    }
#line 1389 "sql_parser.tab.cpp"
    break;

  case 29: /* value: literal  */
#line 224 "sql_parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 1395 "sql_parser.tab.cpp"
    break;

  case 30: /* delete_statement: K_DELETE K_FROM IDENTIFIER optional_where_clause  */
#line 230 "sql_parser.y"
                                                     {
        (yyval.node) = new ASTNode(DELETE_STMT);
        // 子节点1: 表名
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-1].str_val)));
        // 子节点2: 可选的 WHERE 子句 (可能为nullptr)
        (yyval.node)->addChild((yyvsp[0].node));
        free((yyvsp[-1].str_val));
    }
#line 1408 "sql_parser.tab.cpp"
    break;

  case 31: /* expression: IDENTIFIER comparison_operator literal  */
#line 245 "sql_parser.y"
                                           {
        // $2 是 comparison_operator 返回的节点，其value已设为操作符
        (yyval.node) = (yyvsp[-1].node); 
        // 将列名和字面量作为操作符节点的子节点
        (yyval.node)->addChild(new ASTNode(IDENTIFIER_NODE, (yyvsp[-2].str_val)));
        (yyval.node)->addChild((yyvsp[0].node));
        free((yyvsp[-2].str_val));
    }
#line 1421 "sql_parser.tab.cpp"
    break;

  case 32: /* comparison_operator: OP_EQ  */
#line 256 "sql_parser.y"
           { (yyval.node) = new ASTNode(EXPRESSION_NODE, "=");  }
#line 1427 "sql_parser.tab.cpp"
    break;

  case 33: /* comparison_operator: OP_NEQ  */
#line 257 "sql_parser.y"
             { (yyval.node) = new ASTNode(EXPRESSION_NODE, "!="); }
#line 1433 "sql_parser.tab.cpp"
    break;

  case 34: /* comparison_operator: OP_GT  */
#line 258 "sql_parser.y"
             { (yyval.node) = new ASTNode(EXPRESSION_NODE, ">");  }
#line 1439 "sql_parser.tab.cpp"
    break;

  case 35: /* comparison_operator: OP_GTE  */
#line 259 "sql_parser.y"
              { (yyval.node) = new ASTNode(EXPRESSION_NODE, ">="); }
#line 1445 "sql_parser.tab.cpp"
    break;

  case 36: /* comparison_operator: OP_LT  */
#line 260 "sql_parser.y"
             { (yyval.node) = new ASTNode(EXPRESSION_NODE, "<");  }
#line 1451 "sql_parser.tab.cpp"
    break;

  case 37: /* comparison_operator: OP_LTE  */
#line 261 "sql_parser.y"
              { (yyval.node) = new ASTNode(EXPRESSION_NODE, "<="); }
#line 1457 "sql_parser.tab.cpp"
    break;

  case 38: /* literal: INTEGER_CONST  */
#line 265 "sql_parser.y"
                  {
        (yyval.node) = new ASTNode(INTEGER_LITERAL_NODE, std::to_string((yyvsp[0].int_val)));
    }
#line 1465 "sql_parser.tab.cpp"
    break;

  case 39: /* literal: STRING_CONST  */
#line 268 "sql_parser.y"
                   {
        (yyval.node) = new ASTNode(STRING_LITERAL_NODE, (yyvsp[0].str_val));
        free((yyvsp[0].str_val));
    }
#line 1474 "sql_parser.tab.cpp"
    break;


#line 1478 "sql_parser.tab.cpp"

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
      yyerror (YY_("syntax error"));
    }

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
                      yytoken, &yylval);
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


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 274 "sql_parser.y"

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
void print_ast(ASTNode* node, int indent = 0) {
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
}

// 主函数 (用于独立测试)
int main() {
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
}
