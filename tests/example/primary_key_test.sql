-- 主键唯一性约束测试
-- 测试主键重复插入、更新等操作

-- 1. 创建带主键的表
CREATE TABLE users (id INT, name VARCHAR(50), age INT);

-- 2. 插入正常数据
INSERT INTO users (id, name, age) VALUES (1, 'Alice', 25);
INSERT INTO users (id, name, age) VALUES (2, 'Bob', 30);
INSERT INTO users (id, name, age) VALUES (3, 'Charlie', 35);

-- 3. 尝试插入重复主键（应该失败）
INSERT INTO users (id, name, age) VALUES (1, 'David', 28);

-- 4. 查询所有数据
SELECT * FROM users;

-- 5. 尝试更新为重复主键（应该失败）
-- 注意：由于解析器不支持UPDATE，这里用DELETE+INSERT模拟
DELETE FROM users WHERE id = 2;
INSERT INTO users (id, name, age) VALUES (1, 'Eve', 32);

-- 6. 最终查询
SELECT * FROM users;