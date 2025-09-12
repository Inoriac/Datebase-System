//
// Created by Assistant on 2025/1/27.
//

#include <gtest/gtest.h>
#include <cstdio>
#include <string>
#include <vector>

#include "storage/disk_manager.h"
#include "storage/buffer_pool_manager.h"
#include "../include/catalog/table_manager.h"
#include "../include/catalog/table_schema_manager.h"

// 为每次测试创建独立的临时文件
static std::string MakeTempDbFileProjection(const std::string& hint) {
    std::string path = std::string("test_projection_") + hint + ".db";
    std::remove(path.c_str());
    return path;
}

// 基本投影功能测试
TEST(TableManagerProjectionTest, BasicProjection) {
    std::string db = MakeTempDbFileProjection("basic");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建测试表
        TableSchema schema("students");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.AddColumn(Column("age", DataType::Int, 4, true));
        schema.AddColumn(Column("email", DataType::Varchar, 100, true));
        schema.AddColumn(Column("phone", DataType::Varchar, 20, true));
        schema.primary_key_index_ = 0;

        ASSERT_TRUE(tm.CreateTable(schema));

        // 插入测试数据
        Record record1;
        record1.AddValue(Value(1));
        record1.AddValue(Value("Alice"));
        record1.AddValue(Value(20));
        record1.AddValue(Value("alice@email.com"));
        record1.AddValue(Value("123-456-7890"));

        Record record2;
        record2.AddValue(Value(2));
        record2.AddValue(Value("Bob"));
        record2.AddValue(Value(22));
        record2.AddValue(Value("bob@email.com"));
        record2.AddValue(Value("987-654-3210"));

        ASSERT_TRUE(tm.InsertRecord("students", record1));
        ASSERT_TRUE(tm.InsertRecord("students", record2));

        // 测试投影：只选择name和age列
        std::vector<std::string> columns = {"name", "age"};
        auto projected_records = tm.SelectColumns("students", columns);

        // 验证结果
        ASSERT_EQ(projected_records.size(), 2);
        
        // 验证第一条记录
        ASSERT_EQ(projected_records[0].GetValueCount(), 2);
        EXPECT_EQ(std::get<std::string>(projected_records[0].GetValue(0)), "Alice");
        EXPECT_EQ(std::get<int>(projected_records[0].GetValue(1)), 20);
        
        // 验证第二条记录
        ASSERT_EQ(projected_records[1].GetValueCount(), 2);
        EXPECT_EQ(std::get<std::string>(projected_records[1].GetValue(0)), "Bob");
        EXPECT_EQ(std::get<int>(projected_records[1].GetValue(1)), 22);
    }
    std::remove(db.c_str());
}

// 条件查询+投影测试
TEST(TableManagerProjectionTest, ProjectionWithCondition) {
    std::string db = MakeTempDbFileProjection("condition");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建测试表
        TableSchema schema("students");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.AddColumn(Column("age", DataType::Int, 4, true));
        schema.primary_key_index_ = 0;

        ASSERT_TRUE(tm.CreateTable(schema));

        // 插入测试数据
        for (int i = 1; i <= 5; ++i) {
            Record record;
            record.AddValue(Value(i));
            record.AddValue(Value("Student" + std::to_string(i)));
            record.AddValue(Value(18 + i)); // age: 19, 20, 21, 22, 23
            ASSERT_TRUE(tm.InsertRecord("students", record));
        }

        // 测试：选择年龄大于20的学生的姓名和年龄
        std::vector<std::string> columns = {"name", "age"};
        auto projected_records = tm.SelectColumnsWithCondition("students", columns, "age > 20");

        // 验证结果：应该有3条记录（age: 21, 22, 23）
        ASSERT_EQ(projected_records.size(), 3);
        
        // 验证所有记录的age都大于20
        for (const auto& record : projected_records) {
            ASSERT_EQ(record.GetValueCount(), 2);
            int age = std::get<int>(record.GetValue(1));
            EXPECT_GT(age, 20);
        }
    }
    std::remove(db.c_str());
}

// 单列投影测试
TEST(TableManagerProjectionTest, SingleColumnProjection) {
    std::string db = MakeTempDbFileProjection("single");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建测试表
        TableSchema schema("users");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.AddColumn(Column("active", DataType::Bool, 0, true));
        schema.primary_key_index_ = 0;

        ASSERT_TRUE(tm.CreateTable(schema));

        // 插入测试数据
        Record record;
        record.AddValue(Value(1));
        record.AddValue(Value("Alice"));
        record.AddValue(Value(true));
        ASSERT_TRUE(tm.InsertRecord("users", record));

        // 测试：只选择name列
        std::vector<std::string> columns = {"name"};
        auto projected_records = tm.SelectColumns("users", columns);

        // 验证结果
        ASSERT_EQ(projected_records.size(), 1);
        ASSERT_EQ(projected_records[0].GetValueCount(), 1);
        EXPECT_EQ(std::get<std::string>(projected_records[0].GetValue(0)), "Alice");
    }
    std::remove(db.c_str());
}

// 列重排序测试
TEST(TableManagerProjectionTest, ColumnReordering) {
    std::string db = MakeTempDbFileProjection("reorder");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建测试表
        TableSchema schema("students");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.AddColumn(Column("age", DataType::Int, 4, true));
        schema.primary_key_index_ = 0;

        ASSERT_TRUE(tm.CreateTable(schema));

        // 插入测试数据
        Record record;
        record.AddValue(Value(1));
        record.AddValue(Value("Alice"));
        record.AddValue(Value(20));
        ASSERT_TRUE(tm.InsertRecord("students", record));

        // 测试：按age, name, id的顺序选择列
        std::vector<std::string> columns = {"age", "name", "id"};
        auto projected_records = tm.SelectColumns("students", columns);

        // 验证结果
        ASSERT_EQ(projected_records.size(), 1);
        ASSERT_EQ(projected_records[0].GetValueCount(), 3);
        
        // 验证列的顺序：age, name, id
        EXPECT_EQ(std::get<int>(projected_records[0].GetValue(0)), 20);      // age
        EXPECT_EQ(std::get<std::string>(projected_records[0].GetValue(1)), "Alice"); // name
        EXPECT_EQ(std::get<int>(projected_records[0].GetValue(2)), 1);       // id
    }
    std::remove(db.c_str());
}

// 无效列名测试
TEST(TableManagerProjectionTest, InvalidColumnNames) {
    std::string db = MakeTempDbFileProjection("invalid");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建测试表
        TableSchema schema("test");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.primary_key_index_ = 0;

        ASSERT_TRUE(tm.CreateTable(schema));

        // 测试：使用无效列名
        std::vector<std::string> columns = {"invalid_column", "name"};
        auto projected_records = tm.SelectColumns("test", columns);

        // 应该返回空结果
        EXPECT_EQ(projected_records.size(), 0);
    }
    std::remove(db.c_str());
}

// 不存在的表测试
TEST(TableManagerProjectionTest, NonExistentTable) {
    std::string db = MakeTempDbFileProjection("nonexistent");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 测试：对不存在的表进行投影
        std::vector<std::string> columns = {"name"};
        auto projected_records = tm.SelectColumns("nonexistent_table", columns);

        // 应该返回空结果
        EXPECT_EQ(projected_records.size(), 0);
    }
    std::remove(db.c_str());
}

// 空列列表测试
TEST(TableManagerProjectionTest, EmptyColumnList) {
    std::string db = MakeTempDbFileProjection("empty");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建测试表
        TableSchema schema("test");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.primary_key_index_ = 0;

        ASSERT_TRUE(tm.CreateTable(schema));

        // 测试：空列列表
        std::vector<std::string> columns = {};
        auto projected_records = tm.SelectColumns("test", columns);

        // 应该返回空结果
        EXPECT_EQ(projected_records.size(), 0);
    }
    std::remove(db.c_str());
}

// 性能对比测试
TEST(TableManagerProjectionTest, PerformanceComparison) {
    std::string db = MakeTempDbFileProjection("performance");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建测试表
        TableSchema schema("large_table");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.AddColumn(Column("age", DataType::Int, 4, true));
        schema.AddColumn(Column("email", DataType::Varchar, 100, true));
        schema.AddColumn(Column("phone", DataType::Varchar, 20, true));
        schema.AddColumn(Column("address", DataType::Varchar, 200, true));
        schema.primary_key_index_ = 0;

        ASSERT_TRUE(tm.CreateTable(schema));

        // 插入多条测试数据
        for (int i = 1; i <= 10; ++i) {
            Record record;
            record.AddValue(Value(i));
            record.AddValue(Value("User" + std::to_string(i)));
            record.AddValue(Value(20 + i));
            record.AddValue(Value("user" + std::to_string(i) + "@email.com"));
            record.AddValue(Value("123-456-" + std::to_string(1000 + i)));
            record.AddValue(Value("Address " + std::to_string(i)));
            ASSERT_TRUE(tm.InsertRecord("large_table", record));
        }

        // 传统方法：获取所有列
        auto all_records = tm.SelectRecordsWithCondition("large_table", "age > 22");
        std::cout << "Traditional method: " << all_records.size() << " records, "
                  << (all_records.empty() ? 0 : all_records[0].GetValueCount()) << " columns each" << std::endl;

        // 投影方法：只获取需要的列
        std::vector<std::string> columns = {"name", "age"};
        auto projected_records = tm.SelectColumnsWithCondition("large_table", columns, "age > 22");
        std::cout << "Projection method: " << projected_records.size() << " records, "
                  << (projected_records.empty() ? 0 : projected_records[0].GetValueCount()) << " columns each" << std::endl;

        // 验证结果一致性
        EXPECT_EQ(all_records.size(), projected_records.size());
        
        // 验证投影结果的正确性
        for (size_t i = 0; i < projected_records.size(); ++i) {
            ASSERT_EQ(projected_records[i].GetValueCount(), 2);
            // 验证name列
            EXPECT_EQ(std::get<std::string>(projected_records[i].GetValue(0)), 
                     std::get<std::string>(all_records[i].GetValue(1)));
            // 验证age列
            EXPECT_EQ(std::get<int>(projected_records[i].GetValue(1)), 
                     std::get<int>(all_records[i].GetValue(2)));
        }
    }
    std::remove(db.c_str());
}
