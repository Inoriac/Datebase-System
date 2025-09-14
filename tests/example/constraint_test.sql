-- 基础功能测试文件
-- 测试数据库系统的基本功能

-- 1. 创建员工表（简化版本）
CREATE TABLE employees (emp_id INT, emp_name VARCHAR(50), department VARCHAR(30), salary INT);

-- 2. 插入正常数据
INSERT INTO employees (emp_id, emp_name, department, salary) VALUES (101, 'John Doe', 'IT', 75000);
INSERT INTO employees (emp_id, emp_name, department, salary) VALUES (102, 'Jane Smith', 'HR', 65000);
INSERT INTO employees (emp_id, emp_name, department, salary) VALUES (103, 'Bob Johnson', 'Finance', 70000);

-- 3. 查询验证
SELECT * FROM employees;

-- 4. 测试基本操作
-- 4.1 插入更多数据
INSERT INTO employees (emp_id, emp_name, department, salary) VALUES (104, 'Alice', 'IT', 60000);

-- 4.2 插入不同部门数据
INSERT INTO employees (emp_id, emp_name, department, salary) VALUES (105, 'David', 'Marketing', 55000);

-- 5. 正常操作测试
-- 5.1 按ID查询
SELECT * FROM employees WHERE emp_id = 102;

-- 5.2 更新ID
UPDATE employees SET emp_id = 106 WHERE emp_name = 'Bob Johnson';

-- 5.3 更新薪资
UPDATE employees SET salary = 80000 WHERE emp_id = 101;

-- 5.4 删除记录
DELETE FROM employees WHERE emp_id = 103;

-- 6. 最终验证
SELECT * FROM employees ORDER BY emp_id;
