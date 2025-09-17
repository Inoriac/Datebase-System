#include "ast.h"
// #include "symbol_table.h"
#include "sql_semantic_analyzer.h" // 假设这是你的语义分析函数声明文件
#include "plan_generator.h"        // 执行计划生成器
#include "executor.h"              // 执行器
#include "utils.h"
#include "log/log_config.h"

#include <iostream>
#include <cstdio>

// Bison/Flex 生成的函数和全局变量的外部声明
extern int yyparse();
extern FILE *yyin;
extern ASTNode *ast_root; // 在.y文件中定义的全局AST根节点

// 声明 mock 数据生成函数
ASTNode *createMockCreateTableAst();
ASTNode *createMockCreateTableAst_order();

ASTNode *createMockInsertAst();
ASTNode *createMockInsertAst_order();
ASTNode *createMockInsertWithColumnsAst();
ASTNode *createMockInsertWithColumnsAst_order();

// 新增
ASTNode *createMockSelectAllUsersAst();
ASTNode *createMockSelectAllOrdersAst();

ASTNode *createMockSelectAst();
ASTNode *createMockFullSelectAst();

ASTNode *createMockDeleteAst();
ASTNode *createMockUpdateAst();

ASTNode *createMockInsertAst_TableDoesNotExist();

void free_ast(ASTNode *node)
{
    if (!node)
    {
        return;
    }
    for (ASTNode *child : node->children)
    {
        free_ast(child);
    }
    delete node;
}

int main()
{
    // try
    // {
    //     // FILE *inputFile = fopen("test.sql", "r");
    //     // if (!inputFile)
    //     // {
    //     //     std::cerr << "无法打开输入文件 test.sql" << std::endl;
    //     //     return 1;
    //     // }
    //     // yyin = inputFile;
    //     // if (yyparse() != 0)
    //     // {
    //     //     std::cerr << "解析错误，无法生成 AST。" << std::endl;
    //     //     fclose(inputFile);
    //     //     return 1;
    //     // }
    //     // fclose(inputFile);
    //     // std::cout << "--- 解析成功，生成 AST。 ---\n";
    //     // printAST(ast_root, 0);

    //     // std::cout << "--- 开始语义分析。 ---\n";
    //     // semantic_analysis(ast_root);
    //     // std::cout << "语义分析通过。\n";

    //     // 创建一个根节点，用于包含所有语句
    //     ASTNode *root_node = new ASTNode(ROOT_NODE, "");

    //     // 1. 添加 CREATE TABLE 语句
    //     ASTNode *createAst = createMockCreateTableAst();
    //     root_node->addChild(createAst);
    //     ASTNode *createAst_order = createMockCreateTableAst_order();
    //     root_node->addChild(createAst_order);

    //     // 2. 添加 INSERT INTO 语句
    //     ASTNode *insertAst = createMockInsertAst();
    //     root_node->addChild(insertAst);
    //     ASTNode *insertAst_order = createMockInsertAst_order();
    //     root_node->addChild(insertAst_order);
    //     ASTNode *insertAst_colume = createMockInsertWithColumnsAst();
    //     root_node->addChild(insertAst_colume);
    //     ASTNode *insertAst_colume_order = createMockInsertWithColumnsAst_order();
    //     root_node->addChild(insertAst_colume_order);

    //     // 3. 添加 SELECT 语句
    //     ASTNode *selectAst = createMockSelectAst();
    //     root_node->addChild(selectAst);

    //     // 3.5 select扩展测试语句
    //     ASTNode *fullselectAst = createMockFullSelectAst();
    //     root_node->addChild(fullselectAst);

    //     // 4. 添加 DELETE 语句
    //     ASTNode *deleteAst = createMockDeleteAst();
    //     root_node->addChild(deleteAst);

    //     // 5. 添加 UPDATE 语句
    //     ASTNode *updateAst = createMockUpdateAst();
    //     root_node->addChild(updateAst);

    //     std::cout << "--- 正在对所有语句进行语义分析 ---\n";
    //     printAST(root_node, 0);

    //     semantic_analysis(root_node);
    //     // std::cout << "所有语句语义分析通过。\n";

    //     // 在程序结束时清理内存
    //     delete root_node;
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "语义分析失败: " << e.what() << std::endl;
    // }
    // return 0;
    try
    {
        auto logger = DatabaseSystem::Log::LogConfig::GetExecutionLogger();

        // 批量执行模拟语句
        std::vector<ASTNode *> mock_statements;


        // mock_statements.push_back(createMockCreateTableAst());
        // mock_statements.push_back(createMockCreateTableAst_order());

        // mock_statements.push_back(createMockInsertWithColumnsAst_order());
        // mock_statements.push_back(createMockInsertAst());            // 插入 (101, "Alice", 19)
        // mock_statements.push_back(createMockInsertWithColumnsAst()); // 插入 (102, "Alice", 25)
        // mock_statements.push_back(createMockInsertAst_order());      // 插入 (10001, 101)

        // // 3. 检查数据 - 在 UPDATE 前执行
        // mock_statements.push_back(createMockSelectAllUsersAst());
        // mock_statements.push_back(createMockSelectAllOrdersAst());

        // mock_statements.push_back(createMockSelectAst());
        // mock_statements.push_back(createMockFullSelectAst());

        // mock_statements.push_back(createMockUpdateAst());
        // mock_statements.push_back(createMockSelectAllUsersAst());

        // mock_statements.push_back(createMockDeleteAst());


        FILE *inputFile = fopen("/home/molidis/Desktop/bianyi/Datebase-System/src/sql/test.sql", "r");
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
        printAST(ast_root, 0);

        //转化函数
        if (ast_root && ast_root->type == ROOT_NODE)
        {
            // 将 ast_root 的子节点所有权转移到 mock_statements
            // C++11 之后的 std::vector 支持移动语义
            mock_statements = std::move(ast_root->children);
        }



        for (ASTNode *current_statement_node : mock_statements)
        {
            if (!current_statement_node)
            {
                continue;
            }

            // 修复：为每个语句创建一个临时的 ROOT_NODE 父节点
            ASTNode *temp_root = new ASTNode(ROOT_NODE, "");
            temp_root->addChild(current_statement_node);

            // 1. 语义分析
            logger->Info("--- 开始语义分析 ---");
            semantic_analysis(temp_root);
            logger->Info("语义分析成功。");

            // 2. 生成执行计划
            logger->Info("	--- 正在生成执行计划 ---");
            PlanGenerator plan_gen;
            std::unique_ptr<Operator> plan_root = plan_gen.generatePlan(temp_root);
            plan_gen.printPlanTree(plan_root.get(), 0);
            logger->Info("执行计划生成成功。");

            // 3. 执行计划
            logger->Info("--- 正在执行计划 ---");
            Executor executor(std::move(plan_root));
            std::vector<Tuple> results = executor.execute();
            logger->Info("	计划执行成功。");

            // 4. 打印结果（仅针对 SELECT 语句）
            if (current_statement_node->type == SELECT_STMT)
            {
                logger->Info("	--- 查询结果 ---");
                for (const auto &tuple : results)
                {
                    for (const auto &val : tuple)
                    {
                        std::visit([](const auto &arg)
                                   { std::cout << arg << " "; }, val);
                    }
                    std::cout << std::endl;
                }
            }

            // 清理内存
            delete (temp_root);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "错误: " << e.what() << std::endl;
        // 如果出错，确保清理 AST
        if (ast_root)
        {
            delete (ast_root);
        }
    }
    return 0;
}
ASTNode *createMockCreateTableAst()
{
    // 根节点（CREATE_TABLE_STMT）
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
    ASTNode *name_type = new ASTNode(DATA_TYPE_NODE, "VARCHAR");
    name_col->addChild(name_type);

    // age 列节点：列名是父节点
    ASTNode *age_col = new ASTNode(IDENTIFIER_NODE, "age");
    col_definitions->addChild(age_col);
    // age 列的类型节点：类型是子节点
    ASTNode *age_type = new ASTNode(DATA_TYPE_NODE, "INT");
    age_col->addChild(age_type);

    return create_stmt;
}

ASTNode *createMockCreateTableAst_order()
{

    ASTNode *create_stmt = new ASTNode(CREATE_TABLE_STMT, "");

    // 表名节点
    ASTNode *table_name = new ASTNode(IDENTIFIER_NODE, "orders");
    create_stmt->addChild(table_name);

    // 列定义列表节点
    ASTNode *col_definitions = new ASTNode(COLUMN_DEFINITIONS_LIST, "");
    create_stmt->addChild(col_definitions);

    // id 列节点：列名是父节点
    ASTNode *id_col = new ASTNode(IDENTIFIER_NODE, "order_id");
    col_definitions->addChild(id_col);

    // id 列的类型节点：类型是子节点
    ASTNode *id_type = new ASTNode(DATA_TYPE_NODE, "INT");
    id_col->addChild(id_type);

    // name 列节点：列名是父节点
    ASTNode *name_col = new ASTNode(IDENTIFIER_NODE, "user_id");
    col_definitions->addChild(name_col);

    // name 列的类型节点：类型是子节点
    ASTNode *name_type = new ASTNode(DATA_TYPE_NODE, "INT");
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

    ASTNode *int_val_ = new ASTNode(INTEGER_LITERAL_NODE, 19);
    values_list->addChild(int_val_);

    return insert_stmt;
}
ASTNode *createMockInsertAst_order()
{
    // 根节点（INSERT_STMT）
    ASTNode *insert_stmt = new ASTNode(INSERT_STMT, "");

    // 表名节点
    ASTNode *table_name = new ASTNode(IDENTIFIER_NODE, "orders");
    insert_stmt->addChild(table_name);

    // 值列表节点
    ASTNode *values_list = new ASTNode(VALUES_LIST, "");
    insert_stmt->addChild(values_list);

    // 整型常量值节点
    ASTNode *int_val = new ASTNode(INTEGER_LITERAL_NODE, 10001);
    values_list->addChild(int_val);

    // user_id
    ASTNode *int_val_ = new ASTNode(INTEGER_LITERAL_NODE, 101);
    values_list->addChild(int_val_);

    return insert_stmt;
}
ASTNode *createMockInsertWithColumnsAst()
{
    // 根节点（INSERT_STMT）
    ASTNode *insert_stmt = new ASTNode(INSERT_STMT, "");

    // 表名节点
    ASTNode *table_name = new ASTNode(IDENTIFIER_NODE, "users");
    insert_stmt->addChild(table_name);

    // 显式列名列表节点
    ASTNode *column_list = new ASTNode(COLUMN_LIST, "");
    insert_stmt->addChild(column_list);

    // 向列名列表节点添加子节点

    ASTNode *id_col = new ASTNode(IDENTIFIER_NODE, "id");
    column_list->addChild(id_col);

    ASTNode *name_col = new ASTNode(IDENTIFIER_NODE, "name");
    column_list->addChild(name_col);

    ASTNode *age_col = new ASTNode(IDENTIFIER_NODE, "age");
    column_list->addChild(age_col);

    // 值列表节点
    ASTNode *values_list = new ASTNode(VALUES_LIST, "");
    insert_stmt->addChild(values_list);

    // 整型常量值节点
    ASTNode *int_val = new ASTNode(INTEGER_LITERAL_NODE, 102);
    values_list->addChild(int_val);

    // 字符串常量值节点
    ASTNode *string_val = new ASTNode(STRING_LITERAL_NODE, "Bob");
    values_list->addChild(string_val);

    ASTNode *int_val_ = new ASTNode(INTEGER_LITERAL_NODE, 25);
    values_list->addChild(int_val_);

    return insert_stmt;
}

ASTNode *createMockInsertWithColumnsAst_order()
{
    // 根节点（INSERT_STMT）
    ASTNode *insert_stmt = new ASTNode(INSERT_STMT, "");

    // 表名节点
    ASTNode *table_name = new ASTNode(IDENTIFIER_NODE, "orders");
    insert_stmt->addChild(table_name);

    // 显式列名列表节点
    ASTNode *column_list = new ASTNode(COLUMN_LIST, "");
    insert_stmt->addChild(column_list);

    // 向列名列表节点添加子节点
    // ASTNode *order_id_col = new ASTNode(IDENTIFIER_NODE, "id");//todo
    ASTNode *order_id_col = new ASTNode(IDENTIFIER_NODE, "order_id"); // todo
    column_list->addChild(order_id_col);

    ASTNode *user_id_col = new ASTNode(IDENTIFIER_NODE, "user_id");
    column_list->addChild(user_id_col);

    // 值列表节点
    ASTNode *values_list = new ASTNode(VALUES_LIST, "");
    insert_stmt->addChild(values_list);

    // 整型常量值节点
    ASTNode *int_val = new ASTNode(INTEGER_LITERAL_NODE, 10001);
    values_list->addChild(int_val);

    ASTNode *int_val_ = new ASTNode(INTEGER_LITERAL_NODE, 101);
    values_list->addChild(int_val_);

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
    ASTNode *from_node = new ASTNode(FROM_CLAUSE, "users");
    select_stmt->addChild(from_node);

    // WHERE
    ASTNode *where_node = new ASTNode(WHERE_CLAUSE, "");
    select_stmt->addChild(where_node);

    // OPERATOR
    ASTNode *gt_op = new ASTNode(GREATER_THAN_OR_EQUAL_OPERATOR, "");
    gt_op->addChild(new ASTNode(IDENTIFIER_NODE, std::string("id")));
    gt_op->addChild(new ASTNode(INTEGER_LITERAL_NODE, 100));
    where_node->addChild(gt_op);

    return select_stmt;
}

/*
SELECT
    users.name,
    COUNT(orders.order_id)
FROM
    users
JOIN
    orders
ON
    users.id = orders.user_id //拼成一条数据的条件 拼表本质是拼数据
WHERE
    users.age > 25
GROUP BY
    users.name
ORDER BY
    COUNT(orders.order_id) DESC;
*/
ASTNode *createMockFullSelectAst()
{
    // 根节点
    ASTNode *select_stmt = new ASTNode(SELECT_STMT, "");

    // 1. SELECT 列表
    ASTNode *select_list_node = new ASTNode(SELECT_LIST, "");
    select_stmt->addChild(select_list_node);

    // - 选择列: users.name
    ASTNode *user_name_node = new ASTNode(IDENTIFIER_NODE, "users.name");
    select_list_node->addChild(user_name_node);

    // - 选择列: COUNT(orders.order_id)
    // 假设 COUNT 函数也用一个特殊的节点类型表示
    // ASTNode* count_func_node = new ASTNode(FUNCTION_CALL_NODE, "COUNT");
    // count_func_node->addChild(new ASTNode(IDENTIFIER_NODE, "orders.order_id"));
    // select_list_node->addChild(count_func_node);

    // 为了简化，我们只使用列名节点来表示
    ASTNode *order_count_node = new ASTNode(IDENTIFIER_NODE, "orders.order_id");
    select_list_node->addChild(order_count_node);

    // 2. FROM 子句
    ASTNode *from_clause_node = new ASTNode(FROM_CLAUSE, "users");
    select_stmt->addChild(from_clause_node);

    // 3. JOIN 子句 (作为 FROM 的子节点)
    ASTNode *join_clause_node = new ASTNode(JOIN_CLAUSE, "");
    from_clause_node->addChild(join_clause_node);

    // - 连接的表名: orders
    ASTNode *orders_table_node = new ASTNode(IDENTIFIER_NODE, "orders");
    join_clause_node->addChild(orders_table_node);

    // - ON 条件
    ASTNode *on_condition_node = new ASTNode(ON_CONDITION, "");
    join_clause_node->addChild(on_condition_node);
    ASTNode *on_equal_op = new ASTNode(EQUAL_OPERATOR, "");
    on_equal_op->addChild(new ASTNode(IDENTIFIER_NODE, "users.id"));
    on_equal_op->addChild(new ASTNode(IDENTIFIER_NODE, "orders.user_id"));
    on_condition_node->addChild(on_equal_op);

    // 4. WHERE 子句 (作为 SELECT_STMT 的子节点)
    ASTNode *where_clause_node = new ASTNode(WHERE_CLAUSE, "");
    select_stmt->addChild(where_clause_node);
    ASTNode *where_gt_op = new ASTNode(GREATER_THAN_OPERATOR, "");
    where_gt_op->addChild(new ASTNode(IDENTIFIER_NODE, "users.age"));
    where_gt_op->addChild(new ASTNode(INTEGER_LITERAL_NODE, 25));
    where_clause_node->addChild(where_gt_op);

    // 5. GROUP BY 子句
    ASTNode *group_by_clause_node = new ASTNode(GROUP_BY_CLAUSE, "");
    select_stmt->addChild(group_by_clause_node);
    group_by_clause_node->addChild(new ASTNode(IDENTIFIER_NODE, "users.name"));

    // 6. ORDER BY 子句
    ASTNode *order_by_clause_node = new ASTNode(ORDER_BY_CLAUSE, "");
    select_stmt->addChild(order_by_clause_node);
    order_by_clause_node->addChild(new ASTNode(IDENTIFIER_NODE, "orders.order_id")); // 这里忽略 DESC

    return select_stmt;
}

//
ASTNode *createMockDeleteAst()
{
    ASTNode *delete_stmt = new ASTNode(DELETE_STMT, "");

    // FROM 子句
    ASTNode *from_node = new ASTNode(FROM_CLAUSE, "users");
    delete_stmt->addChild(from_node);

    // WHERE 子句
    ASTNode *where_node = new ASTNode(WHERE_CLAUSE, "");
    delete_stmt->addChild(where_node);

    // 条件: id = 101
    ASTNode *gt_op = new ASTNode(EQUAL_OPERATOR, "");
    gt_op->addChild(new ASTNode(IDENTIFIER_NODE, std::string("id")));
    gt_op->addChild(new ASTNode(INTEGER_LITERAL_NODE, 101));
    where_node->addChild(gt_op);

    return delete_stmt;
}

// Mocks the AST for `UPDATE students SET age = 21 WHERE id = 1;`
ASTNode *createMockUpdateAst()
{
    // Root node (UPDATE_STMT)
    ASTNode *update_stmt = new ASTNode(UPDATE_STMT, "");

    // Table name node: 'students'
    ASTNode *table_name_node = new ASTNode(IDENTIFIER_NODE, "users");
    update_stmt->addChild(table_name_node);

    // SET clause node
    ASTNode *set_clause_node = new ASTNode(SET_CLAUSE, "");
    update_stmt->addChild(set_clause_node);

    // Assignment expression: age = 21
    ASTNode *equal_op_node = new ASTNode(EQUAL_OPERATOR, "");
    set_clause_node->addChild(equal_op_node);

    // Column name: 'age'
    ASTNode *age_col_node = new ASTNode(IDENTIFIER_NODE, "name");
    equal_op_node->addChild(age_col_node);

    // Value: 21
    ASTNode *age_value_node = new ASTNode(STRING_LITERAL_NODE, "MOLIDIS");
    equal_op_node->addChild(age_value_node);

    // WHERE clause node
    ASTNode *where_clause_node = new ASTNode(WHERE_CLAUSE, "");
    update_stmt->addChild(where_clause_node);

    // Condition expression: id = 1
    ASTNode *condition_equal_op_node = new ASTNode(EQUAL_OPERATOR, "");
    where_clause_node->addChild(condition_equal_op_node);

    // Column name in WHERE: 'id'
    ASTNode *id_col_node = new ASTNode(IDENTIFIER_NODE, "users.id");
    condition_equal_op_node->addChild(id_col_node);

    // Value in WHERE: 1
    ASTNode *id_value_node = new ASTNode(INTEGER_LITERAL_NODE, 101);
    condition_equal_op_node->addChild(id_value_node);

    return update_stmt;
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

ASTNode *createMockSelectAllUsersAst()
{
    ASTNode *select_stmt = new ASTNode(SELECT_STMT, "");

    // SELECT 列表 - '*'
    ASTNode *select_list = new ASTNode(SELECT_LIST, "");
    select_stmt->addChild(select_list);
    select_list->addChild(new ASTNode(IDENTIFIER_NODE, "*"));

    // FROM 子句
    ASTNode *from_node = new ASTNode(FROM_CLAUSE, "users");
    select_stmt->addChild(from_node);

    return select_stmt;
}

/*
    模拟 SELECT * FROM orders;
*/
ASTNode *createMockSelectAllOrdersAst()
{
    ASTNode *select_stmt = new ASTNode(SELECT_STMT, "");

    // SELECT 列表 - '*'
    ASTNode *select_list = new ASTNode(SELECT_LIST, "");
    select_stmt->addChild(select_list);
    select_list->addChild(new ASTNode(IDENTIFIER_NODE, "*"));

    // FROM 子句
    ASTNode *from_node = new ASTNode(FROM_CLAUSE, "orders");
    select_stmt->addChild(from_node);

    return select_stmt;
}