-- 产品表测试
-- 测试基本的产品管理功能

-- 创建产品表（简化版本）
CREATE TABLE products (product_id INT, product_name VARCHAR(100), price INT, category VARCHAR(50));

-- 插入正常数据
INSERT INTO products (product_id, product_name, price, category) VALUES (1, 'Laptop', 999, 'Electronics');
INSERT INTO products (product_id, product_name, price, category) VALUES (2, 'Mouse', 30, 'Electronics');
INSERT INTO products (product_id, product_name, price, category) VALUES (3, 'Keyboard', 80, 'Electronics');

-- 查询验证
SELECT * FROM products;

-- 插入更多数据
INSERT INTO products (product_id, product_name, price, category) VALUES (4, 'Monitor', 300, 'Electronics');
INSERT INTO products (product_id, product_name, price, category) VALUES (5, 'Headphones', 150, 'Electronics');

-- 更新产品ID
UPDATE products SET product_id = 6 WHERE product_name = 'Monitor';

-- 更新价格
UPDATE products SET price = 1200 WHERE product_id = 1;

-- 查询最终结果
SELECT * FROM products ORDER BY product_id;
