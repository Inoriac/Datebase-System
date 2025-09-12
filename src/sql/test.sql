-- 创建一个学生表
CREATE TABLE students (
    id INT,
    name VARCHAR,
    age INT
);

-- 插入一些学生数据
INSERT INTO students (id, name, age) VALUES (1, 'Alice', 20);
INSERT INTO students (id, name, age) VALUES (2, 'Bob', 22);
INSERT INTO students (id, name, age) VALUES (3, 'Charlie', 21);

-- 查询所有学生
SELECT name FROM students WHERE age >= 21;

-- 删除年龄大于21岁的学生
DELETE FROM students WHERE age > 21;

-- 再次查询所有学生
SELECT name FROM students;

SELECT id, name FROM users WHERE id > 100;





-- CREATE TABLE student(id INT, name VARCHAR, age INT);
-- INSERT INTO student(id,name,age) VALUES (1,'Alice',20);
-- SELECT id,name FROM student WHERE age > 18;
-- DELETE FROM student WHERE id = 1;