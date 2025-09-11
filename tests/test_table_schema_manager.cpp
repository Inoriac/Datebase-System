//
// Created by Huang_cj on 2025/9/11.
//
#include <gtest/gtest.h>
#include <cstdio>
#include <string>
#include <unordered_map>

#include "storage/disk_manager.h"
#include "storage/buffer_pool_manager.h"
#include "../include/catalog/table_manager.h"
#include "../include/catalog/table_schema_manager.h"

static std::string MakeTempDbFileTSM(const std::string &hint) {
    std::string path = std::string("test_tsm_") + hint + ".db";
    std::remove(path.c_str());
    return path;
}

// 基本初始化与系统目录表创建
TEST(TableSchemaManagerTest, InitializeSystemCatalog) {
    std::string db = MakeTempDbFileTSM("init");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);
        TableSchemaManager tsm(&bpm, &tm);

        // 初始未创建任何业务表，系统目录应初始化成功
        EXPECT_TRUE(tsm.InitializeSystemCatalog());
        EXPECT_TRUE(tsm.IsSystemCatalogInitialized());
    }
    std::remove(db.c_str());
}

// 保存、读取、删除单个表结构
TEST(TableSchemaManagerTest, SaveLoadDeleteSingleSchema) {
    std::string db = MakeTempDbFileTSM("single");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);
        TableSchemaManager tsm(&bpm, &tm);

        // 构造一个表结构
        TableSchema schema("users");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 64, true));
        schema.AddColumn(Column("active", DataType::Bool, 0, true));
        schema.primary_key_index_ = 0;

        // 保存
        EXPECT_TRUE(tsm.SaveTableSchema(schema));

        // 加载并校验
        TableSchema loaded;
        EXPECT_TRUE(tsm.LoadTableSchema("users", loaded));
        EXPECT_EQ(loaded.table_name_, schema.table_name_);
        ASSERT_EQ(loaded.columns_.size(), schema.columns_.size());
        EXPECT_EQ(loaded.columns_[0].column_name_, "id");
        EXPECT_EQ(loaded.columns_[0].type_, DataType::Int);
        EXPECT_EQ(loaded.columns_[1].column_name_, "name");
        EXPECT_EQ(loaded.columns_[1].type_, DataType::Varchar);
        EXPECT_EQ(loaded.columns_[2].column_name_, "active");
        EXPECT_EQ(loaded.columns_[2].type_, DataType::Bool);
        EXPECT_EQ(loaded.primary_key_index_, 0);

        // 删除并验证
        EXPECT_TRUE(tsm.DeleteTableSchema("users"));
        TableSchema missing;
        EXPECT_FALSE(tsm.LoadTableSchema("users", missing));
    }
    std::remove(db.c_str());
}

// 批量保存与加载
TEST(TableSchemaManagerTest, SaveAndLoadMultipleSchemas) {
    std::string db = MakeTempDbFileTSM("multi");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);
        TableSchemaManager tsm(&bpm, &tm);

        TableSchema s1("t1");
        s1.AddColumn(Column("id", DataType::Int, 4, false));
        s1.AddColumn(Column("val", DataType::Varchar, 32, true));
        s1.primary_key_index_ = 0;

        TableSchema s2("t2");
        s2.AddColumn(Column("k", DataType::Varchar, 16, false));
        s2.AddColumn(Column("flag", DataType::Bool, 0, true));
        s2.primary_key_index_ = 0;

        EXPECT_TRUE(tsm.SaveTableSchema(s1));
        EXPECT_TRUE(tsm.SaveTableSchema(s2));

        std::unordered_map<std::string, TableSchema> all;
        EXPECT_TRUE(tsm.LoadAllTableSchemas(all));
        ASSERT_GE(all.size(), 2u);
        ASSERT_TRUE(all.count("t1") > 0);
        ASSERT_TRUE(all.count("t2") > 0);

        const TableSchema &ls1 = all["t1"];
        const TableSchema &ls2 = all["t2"];
        EXPECT_EQ(ls1.table_name_, "t1");
        EXPECT_EQ(ls1.columns_[0].type_, DataType::Int);
        EXPECT_EQ(ls1.columns_[1].type_, DataType::Varchar);
        EXPECT_EQ(ls2.table_name_, "t2");
        EXPECT_EQ(ls2.columns_[0].type_, DataType::Varchar);
        EXPECT_EQ(ls2.columns_[1].type_, DataType::Bool);
    }
    std::remove(db.c_str());
}

// 覆盖保存（更新同名表的结构）
TEST(TableSchemaManagerTest, OverwriteExistingSchema) {
    std::string db = MakeTempDbFileTSM("overwrite");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);
        TableSchemaManager tsm(&bpm, &tm);

        TableSchema s("cfg");
        s.AddColumn(Column("k", DataType::Varchar, 32, false));
        s.AddColumn(Column("v", DataType::Varchar, 64, true));
        s.primary_key_index_ = 0;
        EXPECT_TRUE(tsm.SaveTableSchema(s));

        // 更新列定义
        TableSchema s_new("cfg");
        s_new.AddColumn(Column("k", DataType::Varchar, 64, false));
        s_new.AddColumn(Column("v", DataType::Varchar, 128, true));
        s_new.AddColumn(Column("ver", DataType::Int, 4, true));
        s_new.primary_key_index_ = 0;
        EXPECT_TRUE(tsm.SaveTableSchema(s_new));

        TableSchema loaded;
        EXPECT_TRUE(tsm.LoadTableSchema("cfg", loaded));
        EXPECT_EQ(loaded.GetColumnCount(), 3u);
        EXPECT_EQ(loaded.columns_[0].length_, 64);
        EXPECT_EQ(loaded.columns_[1].length_, 128);
        EXPECT_EQ(loaded.columns_[2].type_, DataType::Int);
    }
    std::remove(db.c_str());
}