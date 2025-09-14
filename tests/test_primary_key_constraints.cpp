//
// Created by Huang_cj on 2025/9/14.
//
#include <gtest/gtest.h>
#include <cstdio>
#include <string>
#include <vector>

#include "../include/storage/async_aliases.h"
#include "../include/catalog/table_manager.h"
#include "../include/catalog/types.h"

// 为每次测试创建独立的临时文件，避免相互干扰
static std::string MakeTempDbFilePK(const std::string &name_hint) {
    std::string path = std::string("test_pk_") + name_hint + ".db";
    // 清理旧文件
    std::remove(path.c_str());
    return path;
}

// ========== 基本主键约束测试 ==========
TEST(PrimaryKeyConstraintTest, BasicPrimaryKeyInsertion) {
    std::string db = MakeTempDbFilePK("basic");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建带主键的表
        TableSchema schema("users");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.AddColumn(Column("age", DataType::Int, 4, true));
        schema.primary_key_index_ = 0;  // 第一列(id)为主键

        EXPECT_TRUE(tm.CreateTable(schema));

        // 插入第一条记录
        Record record1;
        record1.AddValue(1);
        record1.AddValue("Alice");
        record1.AddValue(25);
        EXPECT_TRUE(tm.InsertRecord("users", record1));

        // 插入第二条记录（不同主键）
        Record record2;
        record2.AddValue(2);
        record2.AddValue("Bob");
        record2.AddValue(30);
        EXPECT_TRUE(tm.InsertRecord("users", record2));

        // 验证插入成功
        auto records = tm.SelectRecords("users");
        EXPECT_EQ(records.size(), 2);
    }
    std::remove(db.c_str());
}

TEST(PrimaryKeyConstraintTest, DuplicatePrimaryKeyRejection) {
    std::string db = MakeTempDbFilePK("duplicate");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建带主键的表
        TableSchema schema("users");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.primary_key_index_ = 0;

        EXPECT_TRUE(tm.CreateTable(schema));

        // 插入第一条记录
        Record record1;
        record1.AddValue(1);
        record1.AddValue("Alice");
        EXPECT_TRUE(tm.InsertRecord("users", record1));

        // 尝试插入重复主键的记录（应该失败）
        Record record2;
        record2.AddValue(1);  // 重复的主键
        record2.AddValue("Bob");
        EXPECT_FALSE(tm.InsertRecord("users", record2));

        // 验证只有一条记录
        auto records = tm.SelectRecords("users");
        EXPECT_EQ(records.size(), 1);
        EXPECT_EQ(std::get<int>(records[0].values_[0]), 1);
        EXPECT_EQ(std::get<std::string>(records[0].values_[1]), "Alice");
    }
    std::remove(db.c_str());
}

TEST(PrimaryKeyConstraintTest, UpdateToUniquePrimaryKey) {
    std::string db = MakeTempDbFilePK("update_unique");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建带主键的表
        TableSchema schema("users");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.primary_key_index_ = 0;

        EXPECT_TRUE(tm.CreateTable(schema));

        // 插入两条记录
        Record record1, record2;
        record1.AddValue(1); record1.AddValue("Alice");
        record2.AddValue(2); record2.AddValue("Bob");
        EXPECT_TRUE(tm.InsertRecord("users", record1));
        EXPECT_TRUE(tm.InsertRecord("users", record2));

        // 更新第一条记录为新的主键（应该成功）
        Record update_record;
        update_record.AddValue(3);  // 新的主键
        update_record.AddValue("Charlie");
        EXPECT_TRUE(tm.UpdateRecord("users", 0, update_record));

        // 验证更新成功
        auto records = tm.SelectRecords("users");
        EXPECT_EQ(records.size(), 2);
        
        // 检查更新后的记录
        bool found_3 = false, found_2 = false;
        for (const auto& rec : records) {
            int id = std::get<int>(rec.values_[0]);
            if (id == 3) found_3 = true;
            if (id == 2) found_2 = true;
        }
        EXPECT_TRUE(found_3);
        EXPECT_TRUE(found_2);
    }
    std::remove(db.c_str());
}

TEST(PrimaryKeyConstraintTest, UpdateToDuplicatePrimaryKey) {
    std::string db = MakeTempDbFilePK("update_duplicate");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建带主键的表
        TableSchema schema("users");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.primary_key_index_ = 0;

        EXPECT_TRUE(tm.CreateTable(schema));

        // 插入两条记录
        Record record1, record2;
        record1.AddValue(1); record1.AddValue("Alice");
        record2.AddValue(2); record2.AddValue("Bob");
        EXPECT_TRUE(tm.InsertRecord("users", record1));
        EXPECT_TRUE(tm.InsertRecord("users", record2));

        // 尝试更新第一条记录为重复的主键（应该失败）
        Record update_record;
        update_record.AddValue(2);  // 与第二条记录重复的主键
        update_record.AddValue("Charlie");
        EXPECT_FALSE(tm.UpdateRecord("users", 0, update_record));

        // 验证记录没有被更新
        auto records = tm.SelectRecords("users");
        EXPECT_EQ(records.size(), 2);
        
        // 检查原始记录保持不变
        bool found_1 = false, found_2 = false;
        for (const auto& rec : records) {
            int id = std::get<int>(rec.values_[0]);
            if (id == 1) found_1 = true;
            if (id == 2) found_2 = true;
        }
        EXPECT_TRUE(found_1);
        EXPECT_TRUE(found_2);
    }
    std::remove(db.c_str());
}

TEST(PrimaryKeyConstraintTest, NoPrimaryKeyAllowsDuplicates) {
    std::string db = MakeTempDbFilePK("no_pk");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建无主键的表
        TableSchema schema("logs");
        schema.AddColumn(Column("message", DataType::Varchar, 100, true));
        schema.AddColumn(Column("level", DataType::Int, 4, true));
        schema.primary_key_index_ = -1;  // 无主键

        EXPECT_TRUE(tm.CreateTable(schema));

        // 插入相同数据两次（应该都成功）
        Record record1, record2;
        record1.AddValue("Error occurred");
        record1.AddValue(1);
        record2.AddValue("Error occurred");  // 相同数据
        record2.AddValue(1);

        EXPECT_TRUE(tm.InsertRecord("logs", record1));
        EXPECT_TRUE(tm.InsertRecord("logs", record2));

        // 验证两条记录都插入成功
        auto records = tm.SelectRecords("logs");
        EXPECT_EQ(records.size(), 2);
    }
    std::remove(db.c_str());
}

TEST(PrimaryKeyConstraintTest, BatchUpdatePrimaryKeyConstraint) {
    std::string db = MakeTempDbFilePK("batch_update");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建带主键的表
        TableSchema schema("users");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.primary_key_index_ = 0;

        EXPECT_TRUE(tm.CreateTable(schema));

        // 插入测试数据
        Record record1, record2;
        record1.AddValue(1); record1.AddValue("Alice");
        record2.AddValue(2); record2.AddValue("Bob");
        EXPECT_TRUE(tm.InsertRecord("users", record1));
        EXPECT_TRUE(tm.InsertRecord("users", record2));

        // 尝试批量更新为重复主键（应该失败）
        Record update_record;
        update_record.AddValue(1);  // 重复的主键
        update_record.AddValue("Charlie");

        // 更新所有记录（应该失败，因为会产生重复主键）
        int updated_count = tm.UpdateRecordsWithCondition("users", "", update_record);
        EXPECT_EQ(updated_count, 0);  // 应该没有记录被更新

        // 验证原始数据没有被改变
        auto records = tm.SelectRecords("users");
        EXPECT_EQ(records.size(), 2);
    }
    std::remove(db.c_str());
}

TEST(PrimaryKeyConstraintTest, DirectPrimaryKeyCheckMethods) {
    std::string db = MakeTempDbFilePK("direct_check");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建带主键的表
        TableSchema schema("users");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.primary_key_index_ = 0;

        EXPECT_TRUE(tm.CreateTable(schema));

        // 插入一条记录
        Record record1;
        record1.AddValue(1);
        record1.AddValue("Alice");
        EXPECT_TRUE(tm.InsertRecord("users", record1));

        // 测试CheckPrimaryKeyUnique方法
        Record unique_record;
        unique_record.AddValue(2);  // 唯一的主键
        unique_record.AddValue("Bob");
        EXPECT_TRUE(tm.CheckPrimaryKeyUnique("users", unique_record));

        Record duplicate_record;
        duplicate_record.AddValue(1);  // 重复的主键
        duplicate_record.AddValue("Charlie");
        EXPECT_FALSE(tm.CheckPrimaryKeyUnique("users", duplicate_record));
    }
    std::remove(db.c_str());
}

TEST(PrimaryKeyConstraintTest, EmptyTablePrimaryKeyCheck) {
    std::string db = MakeTempDbFilePK("empty_table");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建新表但不插入数据
        TableSchema schema("empty_table");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.primary_key_index_ = 0;

        EXPECT_TRUE(tm.CreateTable(schema));

        // 在空表中插入记录应该成功
        Record record;
        record.AddValue(1);
        record.AddValue("Alice");
        EXPECT_TRUE(tm.InsertRecord("empty_table", record));

        // 验证插入成功
        auto records = tm.SelectRecords("empty_table");
        EXPECT_EQ(records.size(), 1);
    }
    std::remove(db.c_str());
}

// ========== 边界情况测试 ==========
TEST(PrimaryKeyConstraintTest, PrimaryKeyWithDifferentDataTypes) {
    std::string db = MakeTempDbFilePK("different_types");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建主键为字符串类型的表
        TableSchema schema("config");
        schema.AddColumn(Column("key", DataType::Varchar, 50, false));
        schema.AddColumn(Column("value", DataType::Varchar, 100, true));
        schema.primary_key_index_ = 0;  // 字符串主键

        EXPECT_TRUE(tm.CreateTable(schema));

        // 插入记录
        Record record1, record2;
        record1.AddValue("app_name");
        record1.AddValue("MyApp");
        record2.AddValue("version");
        record2.AddValue("1.0.0");
        EXPECT_TRUE(tm.InsertRecord("config", record1));
        EXPECT_TRUE(tm.InsertRecord("config", record2));

        // 尝试插入重复的字符串主键（应该失败）
        Record duplicate_record;
        duplicate_record.AddValue("app_name");  // 重复的主键
        duplicate_record.AddValue("AnotherApp");
        EXPECT_FALSE(tm.InsertRecord("config", duplicate_record));

        // 验证只有两条记录
        auto records = tm.SelectRecords("config");
        EXPECT_EQ(records.size(), 2);
    }
    std::remove(db.c_str());
}

TEST(PrimaryKeyConstraintTest, PrimaryKeyWithBooleanType) {
    std::string db = MakeTempDbFilePK("boolean_pk");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);

        // 创建主键为布尔类型的表
        TableSchema schema("flags");
        schema.AddColumn(Column("enabled", DataType::Bool, 0, false));
        schema.AddColumn(Column("description", DataType::Varchar, 100, true));
        schema.primary_key_index_ = 0;  // 布尔主键

        EXPECT_TRUE(tm.CreateTable(schema));

        // 插入记录
        Record record1, record2;
        record1.AddValue(true);
        record1.AddValue("Feature enabled");
        record2.AddValue(false);
        record2.AddValue("Feature disabled");
        EXPECT_TRUE(tm.InsertRecord("flags", record1));
        EXPECT_TRUE(tm.InsertRecord("flags", record2));

        // 尝试插入重复的布尔主键（应该失败）
        Record duplicate_record;
        duplicate_record.AddValue(true);  // 重复的主键
        duplicate_record.AddValue("Another description");
        EXPECT_FALSE(tm.InsertRecord("flags", duplicate_record));

        // 验证只有两条记录
        auto records = tm.SelectRecords("flags");
        EXPECT_EQ(records.size(), 2);
    }
    std::remove(db.c_str());
}

// ========== 性能测试 ==========
TEST(PrimaryKeyConstraintTest, LargeScalePrimaryKeyOperations) {
    std::string db = MakeTempDbFilePK("large_scale");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(64, &dm);
        TableManager tm(&bpm);

        // 创建带主键的表
        TableSchema schema("large_table");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("data", DataType::Varchar, 100, true));
        schema.primary_key_index_ = 0;

        EXPECT_TRUE(tm.CreateTable(schema));

        // 插入大量记录
        const int NUM_RECORDS = 100;
        for (int i = 0; i < NUM_RECORDS; ++i) {
            Record record;
            record.AddValue(i);
            record.AddValue("Data" + std::to_string(i));
            EXPECT_TRUE(tm.InsertRecord("large_table", record));
        }

        // 验证记录数量
        EXPECT_EQ(tm.GetRecordCount("large_table"), NUM_RECORDS);

        // 尝试插入重复主键（应该失败）
        Record duplicate_record;
        duplicate_record.AddValue(50);  // 重复的主键
        duplicate_record.AddValue("Duplicate data");
        EXPECT_FALSE(tm.InsertRecord("large_table", duplicate_record));

        // 验证记录数量没有变化
        EXPECT_EQ(tm.GetRecordCount("large_table"), NUM_RECORDS);
    }
    std::remove(db.c_str());
}
