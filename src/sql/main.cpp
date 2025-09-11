#include "ast.h"
#include "symbol_table.h"
#include "sql_semantic_analyzer.h" // 假设这是你的语义分析函数声明文件
#include <iostream>

// Bison/Flex 生成的函数和全局变量的外部声明
extern int yyparse();
extern FILE* yyin;
extern ASTNode* ast_root; // 在.y文件中定义的全局AST根节点

// 声明 mock 数据生成函数
ASTNode *createMockCreateTableAst();
ASTNode *createMockInsertAst();
ASTNode *createMockSelectAst();
ASTNode *createMockDeleteAst();

ASTNode *createMockInsertAst_TableDoesNotExist();

void printAST(ASTNode *node, int depth);

int main()
{
    try
    {
        File* inputFile = fopen("test.sql", "r");
        if (!inputFile)
        {
            std::cerr << "无法打开输入文件 test.sql" << std::endl;
            return 1;
        }
        yyin = inputFile;
        if (yyparse() != 0)
        {
            std::cerr << "解析错误，无法生成 AST。" << std::endl;
            fclose(inputFile);
            return 1;
        }
        fclose(inputFile);
        std::cout << "--- 解析成功，生成 AST。 ---\n";
        printAST(ast_root,0);


        // 创建一个根节点，用于包含所有语句
        ASTNode *root_node = new ASTNode(ROOT_NODE, "");

        // 1. 添加 CREATE TABLE 语句
        ASTNode *createAst = createMockCreateTableAst();
        root_node->addChild(createAst);

        // 2. 添加 INSERT INTO 语句
        ASTNode *insertAst = createMockInsertAst();
        root_node->addChild(insertAst);

        // 3. 添加 SELECT 语句
        ASTNode *selectAst = createMockSelectAst();
        root_node->addChild(selectAst);

        // 4. 添加 DELETE 语句
        ASTNode *deleteAst = createMockDeleteAst();
        root_node->addChild(deleteAst);

        std::cout << "--- 正在对所有语句进行语义分析 ---\n";
        printAST(root_node,0);

        semantic_analysis(root_node);
        std::cout << "所有语句语义分析通过。\n";

        // 在程序结束时清理内存
        // delete root_node 会自动删除所有子节点，无需单独删除
        delete root_node;
    }
    catch (const std::exception &e)
    {
        std::cerr << "语义分析失败: " << e.what() << std::endl;
    }
    return 0;
}
ASTNode *createMockCreateTableAst()
{
    // 根节点（CREATE_TABLE_STMT）
    // 你之前这里写了 ASTNode(..., "")，这是正确的用法，我帮你保留了
    ASTNode *create_stmt = new ASTNode(CREATE_TABLE_STMT, "");

    // 表名节点
    ASTNode *table_name = new ASTNode(IDENTIFIER_NODE, "users");
    create_stmt->addChild(table_name);

    // 列定义列表节点
    ASTNode *col_definitions = new ASTNode(COLUMN_DEFINITIONS_LIST, "");
    create_stmt->addChild(col_definitions);

    // id 列节点：列名是父节点
    ASTNode *id_col = new ASTNode(IDENTIFIER_NODE, "id");
    col_definitions->addChild(id_col);

    // id 列的类型节点：类型是子节点
    ASTNode *id_type = new ASTNode(DATA_TYPE_NODE, "INT");
    id_col->addChild(id_type);

    // name 列节点：列名是父节点
    ASTNode *name_col = new ASTNode(IDENTIFIER_NODE, "name");
    col_definitions->addChild(name_col);

    // name 列的类型节点：类型是子节点
    ASTNode *name_type = new ASTNode(DATA_TYPE_NODE, "STRING");
    name_col->addChild(name_type);

    return create_stmt;
}

ASTNode *createMockInsertAst()
{
    // 根节点（INSERT_STMT）
    ASTNode *insert_stmt = new ASTNode(INSERT_STMT, "");

    // 表名节点
    ASTNode *table_name = new ASTNode(IDENTIFIER_NODE, "users");
    insert_stmt->addChild(table_name);

    // 值列表节点
    ASTNode *values_list = new ASTNode(VALUES_LIST, "");
    insert_stmt->addChild(values_list);

    // 整型常量值节点
    ASTNode *int_val = new ASTNode(INTEGER_LITERAL_NODE, 101);
    values_list->addChild(int_val);

    // 字符串常量值节点
    ASTNode *string_val = new ASTNode(STRING_LITERAL_NODE, "Alice");
    values_list->addChild(string_val);

    return insert_stmt;
}

/* 
SELECT id, name FROM users;
 */
ASTNode *createMockSelectAst()
{
    ASTNode *select_stmt = new ASTNode(SELECT_STMT, "");

    // SELECT 列表
    ASTNode *select_list = new ASTNode(SELECT_LIST, "");
    select_stmt->addChild(select_list);
    ASTNode *id_col = new ASTNode(IDENTIFIER_NODE, "id");
    select_list->addChild(id_col);
    ASTNode *name_col = new ASTNode(IDENTIFIER_NODE, "name");
    select_list->addChild(name_col);

    // FROM 子句
    ASTNode *from_node = new ASTNode(IDENTIFIER_NODE, "users");
    select_stmt->addChild(from_node);

    return select_stmt;
}
/*
DELETE FROM users WHERE id = 101;
*/
ASTNode *createMockDeleteAst()
{
    ASTNode *delete_stmt = new ASTNode(DELETE_STMT, "");

    // FROM 子句
    ASTNode *from_node = new ASTNode(IDENTIFIER_NODE, "users");
    delete_stmt->addChild(from_node);

    // WHERE 子句
    ASTNode *where_clause = new ASTNode(WHERE_CLAUSE, "");
    delete_stmt->addChild(where_clause);

    // 条件: id = 101
    ASTNode *eq_operator = new ASTNode(EQUAL_OPERATOR, "");
    where_clause->addChild(eq_operator);

    ASTNode *col_name = new ASTNode(IDENTIFIER_NODE, "id");
    eq_operator->addChild(col_name);

    ASTNode *value = new ASTNode(INTEGER_LITERAL_NODE, 101);
    eq_operator->addChild(value);

    return delete_stmt;
}

// 失败语句
ASTNode *createMockInsertAst_TableDoesNotExist()
{
    // 根节点 (INSERT_STMT)
    ASTNode *insert_stmt = new ASTNode(INSERT_STMT, "");

    // 表名节点，使用一个不存在的表名
    ASTNode *table_name = new ASTNode(IDENTIFIER_NODE, "non_existent_table");
    insert_stmt->addChild(table_name);

    // 值列表节点
    ASTNode *values_list = new ASTNode(VALUES_LIST, "");
    insert_stmt->addChild(values_list);

    // 整型常量值节点
    ASTNode *int_val = new ASTNode(INTEGER_LITERAL_NODE, "101");
    values_list->addChild(int_val);

    return insert_stmt;
}





// 辅助函数，用于将 ASTNodeType 转换为字符串
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

// 递归打印 AST
void printAST(ASTNode *node, int depth = 0)
{
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
    std::cout << std::endl;

    // 递归打印所有子节点
    for (ASTNode *child : node->children)
    {
        printAST(child, depth + 1);
    }
}