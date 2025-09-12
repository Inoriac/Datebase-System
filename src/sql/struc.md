AST 结构设计蓝图

本文档展示了四种基本 SQL 语句在解析后，应形成的抽象语法树 (AST) 的理想结构。
后续的语义分析器和执行计划生成器将依赖于此结构。
1. CREATE TABLE 语句

SQL 示例:
CREATE TABLE users (id INT, name VARCHAR(50));

AST 结构:

    STATEMENTS_LIST

        CREATE_TABLE_STMT

            [子节点 0] IDENTIFIER, Value: 'users'  (表名)

            [子节点 1] COLUMN_DEFINITION_LIST (列定义列表)

                [子] COLUMN_DEFINITION

                    [子] IDENTIFIER, Value: 'id'

                    [子] DATA_TYPE, Value: 'INT'

                [子] COLUMN_DEFINITION

                    [子] IDENTIFIER, Value: 'name'

                    [子] DATA_TYPE, Value: 'VARCHAR'

                    [子] INTEGER_LITERAL, Value: '50' (可选的类型参数)

2. INSERT 语句

SQL 示例 1 (带列名):
INSERT INTO users (id, name) VALUES (1, 'Alice');

AST 结构:

    STATEMENTS_LIST

        INSERT_STMT

            [子节点 0] IDENTIFIER, Value: 'users' (表名)

            [子节点 1] COLUMN_LIST (指定的列列表)

                [子] IDENTIFIER, Value: 'id'

                [子] IDENTIFIER, Value: 'name'

            [子节点 2] VALUES_LIST (值列表)

                [子] INTEGER_LITERAL, Value: '1'

                [子] STRING_LITERAL, Value: "'Alice'"

SQL 示例 2 (不带列名):
INSERT INTO users VALUES (2, 'Bob');

AST 结构:

    STATEMENTS_LIST

        INSERT_STMT

            [子节点 0] IDENTIFIER, Value: 'users' (表名)

            [子节点 1] nullptr (可选的列列表为空)

            [子节点 2] VALUES_LIST (值列表)

                [子] INTEGER_LITERAL, Value: '2'

                [子] STRING_LITERAL, Value: "'Bob'"

3. SELECT 语句

SQL 示例:
SELECT id, name FROM users WHERE age >= 20;

AST 结构:

    STATEMENTS_LIST

        SELECT_STMT

            [子节点 0] SELECT_LIST (选择项列表)

                [子] IDENTIFIER, Value: 'id'

                [子] IDENTIFIER, Value: 'name'

            [子节点 1] IDENTIFIER, Value: 'users' (表名)

            [子节点 2] WHERE_CLAUSE (WHERE 子句)

                [子] BINARY_EXPR, Value: '>=' (二元表达式)

                    [子] IDENTIFIER, Value: 'age' (左操作数)

                    [子] INTEGER_LITERAL, Value: '20' (右操作数)

*SQL 示例 (SELECT ):
SELECT * FROM users;

AST 结构:

    STATEMENTS_LIST

        SELECT_STMT

            [子节点 0] SELECT_LIST

                [子] IDENTIFIER, Value: '*'

            [子节点 1] IDENTIFIER, Value: 'users'

            [子节点 2] nullptr (可选的 WHERE 子句为空)

4. DELETE 语句

SQL 示例:
DELETE FROM users WHERE id = 1;

AST 结构:

    STATEMENTS_LIST

        DELETE_STMT

            [子节点 0] IDENTIFIER, Value: 'users' (表名)

            [子节点 1] WHERE_CLAUSE (WHERE 子句)

                [子] BINARY_EXPR, Value: '='

                    [子] IDENTIFIER, Value: 'id'

                    [子] INTEGER_LITERAL, Value: '1'