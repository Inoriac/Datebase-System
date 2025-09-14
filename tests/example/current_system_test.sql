-- 当前系统功能测试
-- 基于你现有的数据库系统功能

-- 1. 创建学生表（简化版本）
CREATE TABLE students (id INT, name VARCHAR(50), age INT, grade VARCHAR(10));

-- 2. 插入测试数据
INSERT INTO students (id, name, age, grade) VALUES (1, 'Alice', 20, 'A');
INSERT INTO students (id, name, age, grade) VALUES (2, 'Bob', 22, 'B');
INSERT INTO students (id, name, age, grade) VALUES (3, 'Charlie', 21, 'A');
INSERT INTO students (id, name, age, grade) VALUES (4, 'Diana', 19, 'C');
INSERT INTO students (id, name, age, grade) VALUES (5, 'Eve', 23, 'B');

-- 3. 测试投影查询
SELECT name FROM students;
SELECT id, name FROM students;
SELECT name, age FROM students;

-- 4. 测试条件查询
SELECT name FROM students WHERE age >= 21;
SELECT * FROM students WHERE id = 3;
SELECT name, grade FROM students WHERE grade = 'A';

-- 5. 测试更新操作（UPDATE语句不支持，跳过）
-- UPDATE students SET grade = 'A+' WHERE id = 1;
-- UPDATE students SET age = 24 WHERE name = 'Bob';

-- 6. 测试删除操作
DELETE FROM students WHERE grade = 'C';
DELETE FROM students WHERE age < 20;

-- 7. 验证最终结果
SELECT * FROM students;
SELECT name, age FROM students WHERE age > 20;
