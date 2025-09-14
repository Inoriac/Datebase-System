#include <gtest/gtest.h>
#include <cstdio>
#include <string>
#include <vector>

#include "../include/storage/async_aliases.h"
#include "../include/catalog/table_manager.h"
#include "../include/catalog/types.h"

// 为每次测试创建独立的临时文件，避免相互干扰
static std::string MakeTempDbFile(const std::string &name_hint) {
    std::string path = std::string("test_") + name_hint + ".db";
    // 清理旧文件
    std::remove(path.c_str());
    return path;
}

// 测试TableManager基本初始化
TEST(SimpleInitTest, TableManagerInitialization) {
    std::string db = MakeTempDbFile("init");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);
        
        // 如果程序能执行到这里，说明初始化成功
        EXPECT_TRUE(true);
    }
    std::remove(db.c_str());
}

// 测试创建简单表
TEST(SimpleInitTest, CreateSimpleTable) {
    std::string db = MakeTempDbFile("create");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        TableManager tm(&bpm);
        
        // 创建简单表
        TableSchema schema("test_table");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.primary_key_index_ = 0;
        
        EXPECT_TRUE(tm.CreateTable(schema));
        
        // 验证表存在
        EXPECT_TRUE(tm.TableExists("test_table"));
    }
    std::remove(db.c_str());
}
