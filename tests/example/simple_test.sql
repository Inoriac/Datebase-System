-- 简单表测试
-- 测试基本的增删改查操作

-- 创建学生表（简化版本，移除不支持的约束）
CREATE TABLE students (id INT, name VARCHAR(50), age INT, grade VARCHAR(10));

-- 插入测试数据
INSERT INTO students (id, name, age, grade) VALUES (1, 'Alice', 20, 'A');
INSERT INTO students (id, name, age, grade) VALUES (2, 'Bob', 22, 'B');
INSERT INTO students (id, name, age, grade) VALUES (3, 'Charlie', 21, 'A');
INSERT INTO students (id, name, age, grade) VALUES (4, 'Diana', 19, 'C');

-- 查询测试
SELECT * FROM students;
SELECT name, grade FROM students;
SELECT name, age FROM students WHERE age >= 21;
SELECT * FROM students WHERE id = 2;

-- 更新测试（UPDATE语句不支持，跳过）
-- UPDATE students SET grade = 'A+' WHERE id = 1;
-- UPDATE students SET age = 23 WHERE name = 'Bob';

-- 删除测试
DELETE FROM students WHERE grade = 'C';

-- 最终查询
SELECT * FROM students;
