-- 主键表测试SQL文件
-- 包含基本的增删改查操作

-- 1. 创建带主键的用户表
-- CREATE TABLE users (id INT PRIMARY KEY, name VARCHAR(50), age INT, email VARCHAR(100));
CREATE TABLE users (id INT, name VARCHAR(50), age INT, email VARCHAR(100));

-- 2. 创建带主键的订单表
CREATE TABLE orders (order_id INT PRIMARY KEY, user_id INT, product_name VARCHAR(100), price DECIMAL(10,2), order_date VARCHAR(20));

-- 3. 插入用户数据
INSERT INTO users VALUES (1, 'Alice', 25, 'alice@example.com');
INSERT INTO users VALUES (2, 'Bob', 30, 'bob@example.com');
INSERT INTO users VALUES (3, 'Charlie', 28, 'charlie@example.com');
INSERT INTO users VALUES (4, 'Diana', 35, 'diana@example.com');
INSERT INTO users VALUES (5, 'Eve', 22, 'eve@example.com');

-- 4. 插入订单数据
INSERT INTO orders VALUES (1001, 1, 'Laptop', 999.99, '2024-01-15');
INSERT INTO orders VALUES (1002, 2, 'Mouse', 29.99, '2024-01-16');
INSERT INTO orders VALUES (1003, 1, 'Keyboard', 79.99, '2024-01-17');
INSERT INTO orders VALUES (1004, 3, 'Monitor', 299.99, '2024-01-18');
INSERT INTO orders VALUES (1005, 2, 'Headphones', 149.99, '2024-01-19');

-- 5. 查询操作
-- 查询所有用户
SELECT * FROM users;

-- 查询特定用户
SELECT * FROM users WHERE id = 1;

-- 查询年龄大于25的用户
SELECT name, age FROM users WHERE age > 25;

-- 查询所有订单
SELECT * FROM orders;

-- 查询特定用户的订单
SELECT * FROM orders WHERE user_id = 1;

-- 查询价格大于100的订单
SELECT order_id, product_name, price FROM orders WHERE price > 100;

-- 6. 更新操作
-- 更新用户信息
UPDATE users SET age = 26 WHERE id = 1;
UPDATE users SET email = 'alice.new@example.com' WHERE id = 1;

-- 更新订单价格
UPDATE orders SET price = 1099.99 WHERE order_id = 1001;

-- 批量更新年龄
UPDATE users SET age = age + 1 WHERE age < 30;

-- 7. 删除操作
-- 删除特定订单
DELETE FROM orders WHERE order_id = 1005;

-- 删除特定用户的所有订单
DELETE FROM orders WHERE user_id = 4;

-- 删除特定用户
DELETE FROM users WHERE id = 4;

-- 8. 复杂查询
-- 查询用户及其订单信息（模拟JOIN）
SELECT u.name, u.email, o.product_name, o.price 
FROM users u, orders o 
WHERE u.id = o.user_id;

-- 查询每个用户的订单数量
SELECT u.name, COUNT(o.order_id) as order_count
FROM users u, orders o 
WHERE u.id = o.user_id
GROUP BY u.id, u.name;

-- 查询最贵的订单
SELECT * FROM orders WHERE price = (SELECT MAX(price) FROM orders);

-- 9. 测试主键约束
-- 尝试插入重复主键（应该失败）
-- INSERT INTO users VALUES (1, 'Duplicate', 30, 'duplicate@example.com');

-- 尝试插入NULL主键（应该失败）
-- INSERT INTO users VALUES (NULL, 'NullKey', 30, 'null@example.com');

-- 10. 清理测试数据
-- DELETE FROM orders;
-- DELETE FROM users;
-- DROP TABLE orders;
-- DROP TABLE users;
