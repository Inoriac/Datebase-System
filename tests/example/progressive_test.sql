-- 渐进式测试文件
-- 从简单到复杂逐步测试数据库功能

-- ===== 阶段1：基本表创建和插入 =====
CREATE TABLE test_table (id INT, name VARCHAR(50), value INT);

-- 插入测试数据
INSERT INTO test_table (id, name, value) VALUES (1, 'First', 100);
INSERT INTO test_table (id, name, value) VALUES (2, 'Second', 200);
INSERT INTO test_table (id, name, value) VALUES (3, 'Third', 300);

-- 验证插入
SELECT * FROM test_table;

-- ===== 阶段2：基本查询测试 =====
-- 查询所有列
SELECT * FROM test_table;

-- 查询特定列
SELECT name FROM test_table;
SELECT id, value FROM test_table;

-- 条件查询
SELECT * FROM test_table WHERE id = 2;
SELECT name FROM test_table WHERE value > 150;

-- ===== 阶段3：更新操作测试 =====
-- 更新单条记录
UPDATE test_table SET value = 250 WHERE id = 2;

-- 更新多条记录
UPDATE test_table SET name = 'Updated' WHERE value > 200;

-- 验证更新
SELECT * FROM test_table;

-- ===== 阶段4：删除操作测试 =====
-- 删除单条记录
DELETE FROM test_table WHERE id = 3;

-- 条件删除
DELETE FROM test_table WHERE value < 200;

-- 验证删除
SELECT * FROM test_table;

-- ===== 阶段5：重新插入和最终测试 =====
-- 重新插入数据
INSERT INTO test_table (id, name, value) VALUES (4, 'Fourth', 400);
INSERT INTO test_table (id, name, value) VALUES (5, 'Fifth', 500);

-- 最终查询
SELECT * FROM test_table ORDER BY id;
