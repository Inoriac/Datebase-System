-- 用户表测试SQL文件
-- 测试用户表的增删改查操作

-- 1. 创建用户表（简化版本）
CREATE TABLE users (id INT, name VARCHAR(50), email VARCHAR(100), age INT, city VARCHAR(30));

-- 2. 插入测试数据
INSERT INTO users (id, name, email, age, city) VALUES (1, 'Alice', 'alice@example.com', 25, 'Beijing');
INSERT INTO users (id, name, email, age, city) VALUES (2, 'Bob', 'bob@example.com', 30, 'Shanghai');
INSERT INTO users (id, name, email, age, city) VALUES (3, 'Charlie', 'charlie@example.com', 28, 'Guangzhou');
INSERT INTO users (id, name, email, age, city) VALUES (4, 'Diana', 'diana@example.com', 22, 'Shenzhen');
INSERT INTO users (id, name, email, age, city) VALUES (5, 'Eve', 'eve@example.com', 35, 'Hangzhou');

-- 3. 查询测试
-- 3.1 查询所有数据
SELECT * FROM users;

-- 3.2 查询特定列
SELECT name, email FROM users;

-- 3.3 条件查询
SELECT name, age FROM users WHERE age > 25;

-- 3.4 主键查询
SELECT * FROM users WHERE id = 3;

-- 3.5 范围查询（拆分为两个简单查询）
SELECT name, city FROM users WHERE age >= 25;
SELECT name, city FROM users WHERE age <= 30;

-- 3.6 字符串查询
SELECT name, email FROM users WHERE city = 'Beijing';

-- 4. 更新测试
-- 4.1 更新单条记录
UPDATE users SET age = 26 WHERE id = 1;

-- 4.2 更新多条记录
UPDATE users SET city = 'Shanghai' WHERE age > 30;

-- 4.3 更新多个字段
UPDATE users SET name = 'Alice Smith', email = 'alice.smith@example.com' WHERE id = 1;

-- 5. 删除测试
-- 5.1 删除单条记录
DELETE FROM users WHERE id = 5;

-- 5.2 条件删除
DELETE FROM users WHERE age < 25;

-- 5.3 删除所有记录
-- DELETE FROM users;

-- 6. 验证操作结果
SELECT * FROM users;

-- 7. 重新插入一些数据测试
INSERT INTO users (id, name, email, age, city) VALUES (6, 'Frank', 'frank@example.com', 27, 'Nanjing');
INSERT INTO users (id, name, email, age, city) VALUES (7, 'Grace', 'grace@example.com', 24, 'Wuhan');

-- 8. 最终查询验证
SELECT id, name, age, city FROM users ORDER BY id;
