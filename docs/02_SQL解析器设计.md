# SQL解析器设计

## 概述
基于Flex+Bison的SQL解析器，将SQL语句转换为抽象语法树(AST)。

## 架构
```
SQL文本 → 词法分析(Flex) → Token流 → 语法分析(Bison) → AST
```

## 组件结构
```
src/sql/
├── sql_lexer.l          # Flex词法分析器
├── sql_parser.y         # Bison语法分析器
├── sql_lexer.yy.cpp     # 生成的词法分析器代码
├── sql_parser.tab.cpp   # 生成的语法分析器代码
└── sql_parser.tab.hpp   # 生成的语法分析器头文件
```

## 词法分析器 (Flex)

### 主要功能
- 识别SQL关键字 (CREATE, INSERT, SELECT, DELETE等)
- 识别标识符 (表名、列名)
- 识别字面量 (整数、字符串)
- 识别操作符 (=, >, <, >=, <=, !=)

### 关键设计
```c
"CREATE"        { return CREATE; }
"SELECT"        { return SELECT; }
"INSERT"        { return INSERT; }
"DELETE"        { return DELETE; }

[a-zA-Z_][a-zA-Z0-9_]*  { 
    yylval.string_val = new std::string(yytext);
    return IDENTIFIER; 
}

[0-9]+          { 
    yylval.int_val = atoi(yytext);
    return INTEGER_LITERAL; 
}
```

### 特点
- 自动行号跟踪
- 动态字符串内存管理
- 错误位置精确定位

## 语法分析器 (Bison)

### 语法规则
```yacc
// CREATE TABLE语句
create_table_stmt:
    CREATE TABLE IDENTIFIER LPAREN column_definitions RPAREN

// INSERT语句  
insert_stmt:
    INSERT INTO IDENTIFIER VALUES LPAREN value_list RPAREN

// SELECT语句
select_stmt:
    SELECT select_list FROM IDENTIFIER where_clause

// WHERE子句
where_clause:
    WHERE expression
```

### 错误处理
```yacc
void yyerror(const char* msg) {
    std::cerr << "[Parser] Error at line " << yylineno << ": " << msg << std::endl;
}
```
```

## 抽象语法树 (AST)

### AST节点结构
```cpp
class ASTNode {
public:
    ASTNodeType type;
    std::variant<std::string, int, bool> value;
    std::vector<ASTNode*> children;
    Location location;

    void addChild(ASTNode* child);
    void print(int depth = 0) const;
};
```

### 主要节点类型
- `CREATE_TABLE_STMT`: 创建表语句
- `INSERT_STMT`: 插入语句  
- `SELECT_STMT`: 查询语句
- `DELETE_STMT`: 删除语句
- `IDENTIFIER_NODE`: 标识符节点
- `INTEGER_LITERAL_NODE`: 整数字面量
- `STRING_LITERAL_NODE`: 字符串字面量

## 使用方式
```cpp
// 解析SQL语句
ASTNode* ast = parseSQL("CREATE TABLE users (id INT, name VARCHAR(50));");

// 遍历AST
ast->print();  // 打印AST结构

// 访问节点
if (ast->type == CREATE_TABLE_STMT) {
    std::string table_name = std::get<std::string>(ast->value);
    // 处理创建表逻辑
}
```

## 错误处理
- **词法错误**: 无法识别的字符或关键字
- **语法错误**: 不符合语法规则的语句结构  
- **语义错误**: 表不存在、列不存在、类型不匹配等

## 性能优化
- 智能指针管理AST节点生命周期
- 预分配内存池减少动态分配
- 语法规则优化和预测分析表优化
