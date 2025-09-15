-- 创建一个学生表
CREATE TABLE students (id INT, name VARCHAR(50), age INT);

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

UPDATE user SET name = 'Alice' WHERE id = 1;



-- CREATE TABLE student(id INT, name VARCHAR, age INT);
-- INSERT INTO student(id,name,age) VALUES (1,'Alice',20);
-- SELECT id,name FROM student WHERE age > 18;
-- DELETE FROM student WHERE id = 1;


-- 语义分析的功能测试语句
CREATE TABLE users (
    id INT,
    name STRING,
    age INT
);

---

CREATE TABLE orders (
    order_id INT,
    user_id INT
);

---

INSERT INTO users VALUES (101, 'Alice', 19);

---

INSERT INTO orders VALUES (10001, 101);

---
-- 1表/列存在性检查；
-- INSERT INTO users111 (name, id, age) VALUES ('Bob', 102, 25);
-- 2类型一致性检查（列类型与输入值是否匹配）；
-- INSERT INTO users (name, id, age) VALUES ('Bob', '102', 25);
-- 3列数/列序检查（INSERT值的数量与列数是否一致）。
-- INSERT INTO users (name, id) VALUES ('Bob', 102);
-- 4构建并维护模式目录（Catalog）。
-- 5出错时，输出错误类型、位置与原因说明。
-- 6列名拼写错误、类型不匹配、值个数不一致
INSERT INTO users (name, id, age) VALUES ('Bob', 102, 25);

---
-- 错误诊断增强：如智能纠错提示:列的智能纠错
-- INSERT INTO orders (id, user_id) VALUES (10001, 101);
INSERT INTO orders (order_id, user_id) VALUES (10001, 101);

---

INSERT INTO non_existent_table VALUES (101);

---

SELECT id, name FROM users WHERE id >= 100;

---


-- 语法扩展：支持UPDATE、JOIN、ORDER BY、GROUP BY;
SELECT
    users.name,
    orders.order_id
FROM
    users
JOIN
    orders ON users.id = orders.user_id
WHERE
    users.age > 25
GROUP BY
    users.name
ORDER BY
    orders.order_id;

---

DELETE FROM users WHERE id >= 100;

---

UPDATE users SET name = 'MOLIDIS' WHERE id = 1;