-- 主键表测试SQL文件
-- 包含基本的增删改查操作

-- 1. 创建用户表
CREATE TABLE users (id INT, name VARCHAR(50), age INT, email VARCHAR(100));

-- 2. 插入用户数据
INSERT INTO users VALUES (1, 'Alice', 25, 'alice@example.com');
INSERT INTO users VALUES (2, 'Bob', 30, 'bob@example.com');
INSERT INTO users VALUES (3, 'Charlie', 28, 'charlie@example.com');
INSERT INTO users VALUES (4, 'Diana', 35, 'diana@example.com');
INSERT INTO users VALUES (5, 'Eve', 22, 'eve@example.com');

-- 3. 查询操作
-- 查询所有用户
SELECT * FROM users;

-- 查询特定用户
SELECT * FROM users WHERE id = 1;

-- 查询年龄大于25的用户
SELECT name, age FROM users WHERE age > 25;

-- 6. 更新操作
-- 更新用户信息
UPDATE users SET age = 26 WHERE id = 1;
UPDATE users SET email = 'alice.new@example.com' WHERE id = 1;

-- 7. 删除操作
-- 删除特定用户
DELETE FROM users WHERE id = 4;
