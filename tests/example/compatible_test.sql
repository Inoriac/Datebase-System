-- 兼容性测试文件
-- 只使用解析器支持的语法

-- 1. 创建用户表
CREATE TABLE users (id INT, name VARCHAR(50), email VARCHAR(100), age INT, city VARCHAR(30));

-- 2. 插入测试数据
INSERT INTO users (id, name, email, age, city) VALUES (1, 'Alice', 'alice@example.com', 25, 'Beijing');
INSERT INTO users (id, name, email, age, city) VALUES (2, 'Bob', 'bob@example.com', 30, 'Shanghai');
INSERT INTO users (id, name, email, age, city) VALUES (3, 'Charlie', 'charlie@example.com', 28, 'Guangzhou');
INSERT INTO users (id, name, email, age, city) VALUES (4, 'Diana', 'diana@example.com', 22, 'Shenzhen');
INSERT INTO users (id, name, email, age, city) VALUES (5, 'Eve', 'eve@example.com', 35, 'Hangzhou');

-- 3. 基本查询测试
SELECT * FROM users;
SELECT name FROM users;
SELECT name, email FROM users;
SELECT id, name, age FROM users;

-- 4. 条件查询测试（只使用支持的语法）
SELECT name FROM users WHERE age >= 25;
SELECT name, city FROM users WHERE age <= 30;
SELECT * FROM users WHERE id = 3;
SELECT name, email FROM users WHERE city = 'Beijing';
SELECT name, age FROM users WHERE age > 25;

-- 5. 更新测试（UPDATE语句不支持，跳过）
-- UPDATE users SET age = 26 WHERE id = 1;
-- UPDATE users SET city = 'Shanghai' WHERE age > 30;
-- UPDATE users SET name = 'Alice Smith' WHERE id = 1;

-- 6. 删除测试
DELETE FROM users WHERE id = 5;
DELETE FROM users WHERE age < 25;

-- 7. 最终验证
SELECT * FROM users;
SELECT name, age FROM users WHERE age > 20;
