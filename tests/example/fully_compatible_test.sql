-- 完全兼容测试文件
-- 只使用解析器支持的语法：CREATE, INSERT, SELECT, DELETE

-- 1. 创建学生表
CREATE TABLE students (id INT, name VARCHAR(50), age INT, grade VARCHAR(10));

-- 2. 插入测试数据
INSERT INTO students (id, name, age, grade) VALUES (1, 'Alice', 20, 'A');
INSERT INTO students (id, name, age, grade) VALUES (2, 'Bob', 22, 'B');
INSERT INTO students (id, name, age, grade) VALUES (3, 'Charlie', 21, 'A');
INSERT INTO students (id, name, age, grade) VALUES (4, 'Diana', 19, 'C');
INSERT INTO students (id, name, age, grade) VALUES (5, 'Eve', 23, 'B');

-- 3. 查询所有数据
SELECT * FROM students;

-- 4. 投影查询测试
SELECT name FROM students;
SELECT id, name FROM students;
SELECT name, age FROM students;
SELECT name, grade FROM students;

-- 5. 条件查询测试
SELECT name FROM students WHERE age >= 21;
SELECT * FROM students WHERE id = 3;
SELECT name, grade FROM students WHERE grade = 'A';
SELECT name, age FROM students WHERE age > 20;
SELECT * FROM students WHERE id = 2;

-- 6. 删除测试
DELETE FROM students WHERE grade = 'C';
DELETE FROM students WHERE age < 20;
DELETE FROM students WHERE id = 5;

-- 7. 最终验证
SELECT * FROM students;
SELECT name, age FROM students WHERE age > 20;
