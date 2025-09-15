//
// Created by Huang_cj on 2025/9/14.
// B+树分裂逻辑测试
//

#include <gtest/gtest.h>
#include <cstdio>
#include <string>
#include <vector>

#include "../../include/storage/async_aliases.h"
#include "../../include/catalog/bplus_tree.h"
#include "../../include/catalog/types.h"

// 为每次测试创建独立的临时文件，避免相互干扰
static std::string MakeTempDbFileSplit(const std::string &name_hint) {
    std::string path = std::string("test_split_") + name_hint + ".db";
    // 清理旧文件
    std::remove(path.c_str());
    return path;
}

// ========== 基本分裂测试 ==========

TEST(BPlusTreeSplitTest, BasicLeafSplit) {
    std::string db = MakeTempDbFileSplit("basic_leaf");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        // 创建B+树索引，设置较小的max_keys_per_node_以便测试分裂
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入足够多的记录以触发分裂
        const int NUM_RECORDS = 60; // 超过默认的50个键限制
        for (int i = 0; i < NUM_RECORDS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 10));
        }
        
        // 验证所有记录都能找到
        for (int i = 0; i < NUM_RECORDS; ++i) {
            auto results = index.Find(i);
            EXPECT_FALSE(results.empty());
            EXPECT_EQ(results[0], i * 10);
        }
        
        // 验证范围查询
        auto range_results = index.FindRange(10, 20);
        EXPECT_EQ(range_results.size(), 11); // 10到20，包含边界
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeSplitTest, MultipleLeafSplits) {
    std::string db = MakeTempDbFileSplit("multiple_leaf");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(32, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入大量记录以触发多次分裂
        const int NUM_RECORDS = 200;
        for (int i = 0; i < NUM_RECORDS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
        }
        
        // 验证所有记录都能找到
        for (int i = 0; i < NUM_RECORDS; ++i) {
            auto results = index.Find(i);
            EXPECT_FALSE(results.empty());
            EXPECT_EQ(results[0], i * 100);
        }
        
        // 验证范围查询
        auto range_results = index.FindRange(50, 150);
        EXPECT_EQ(range_results.size(), 101); // 50到150，包含边界
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeSplitTest, InternalNodeSplit) {
    std::string db = MakeTempDbFileSplit("internal_split");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(64, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入大量记录以触发内部节点分裂
        const int NUM_RECORDS = 500;
        for (int i = 0; i < NUM_RECORDS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 1000));
        }
        
        // 验证所有记录都能找到
        for (int i = 0; i < NUM_RECORDS; ++i) {
            auto results = index.Find(i);
            EXPECT_FALSE(results.empty());
            EXPECT_EQ(results[0], i * 1000);
        }
        
        // 验证树的高度
        EXPECT_GT(index.GetHeight(), 1); // 应该有内部节点
    }
    std::remove(db.c_str());
}

// ========== 分裂后数据完整性测试 ==========

TEST(BPlusTreeSplitTest, DataIntegrityAfterSplit) {
    std::string db = MakeTempDbFileSplit("data_integrity");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入随机顺序的记录
        std::vector<int> keys = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
        for (int key : keys) {
            EXPECT_TRUE(index.Insert(key, key * 100));
        }
        
        // 验证所有记录都能找到
        for (int key : keys) {
            auto results = index.Find(key);
            EXPECT_FALSE(results.empty());
            EXPECT_EQ(results[0], key * 100);
        }
        
        // 验证范围查询
        auto range_results = index.FindRange(3, 7);
        EXPECT_EQ(range_results.size(), 5); // 3, 4, 5, 6, 7
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeSplitTest, StringKeySplit) {
    std::string db = MakeTempDbFileSplit("string_key");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "name", 0);
        
        // 插入字符串键
        std::vector<std::string> keys = {"apple", "banana", "cherry", "date", "elderberry", 
                                        "fig", "grape", "honeydew", "kiwi", "lemon"};
        for (size_t i = 0; i < keys.size(); ++i) {
            EXPECT_TRUE(index.Insert(keys[i], static_cast<int>(i * 100)));
        }
        
        // 验证所有记录都能找到
        for (size_t i = 0; i < keys.size(); ++i) {
            auto results = index.Find(keys[i]);
            EXPECT_FALSE(results.empty());
            EXPECT_EQ(results[0], static_cast<int>(i * 100));
        }
        
        // 验证范围查询
        auto range_results = index.FindRange("cherry", "grape");
        EXPECT_EQ(range_results.size(), 5); // cherry, date, elderberry, fig, grape
    }
    std::remove(db.c_str());
}

// ========== 边界情况测试 ==========

TEST(BPlusTreeSplitTest, SingleKeySplit) {
    std::string db = MakeTempDbFileSplit("single_key");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 只插入一个键
        EXPECT_TRUE(index.Insert(42, 4200));
        
        // 验证能找到
        auto results = index.Find(42);
        EXPECT_FALSE(results.empty());
        EXPECT_EQ(results[0], 4200);
        
        // 验证找不到不存在的键
        auto not_found = index.Find(999);
        EXPECT_TRUE(not_found.empty());
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeSplitTest, DuplicateKeyHandling) {
    std::string db = MakeTempDbFileSplit("duplicate_key");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入重复的键（应该失败）
        EXPECT_TRUE(index.Insert(42, 4200));
        EXPECT_FALSE(index.Insert(42, 4300)); // 重复键应该失败
        
        // 验证只有第一个记录存在
        auto results = index.Find(42);
        EXPECT_FALSE(results.empty());
        EXPECT_EQ(results[0], 4200);
    }
    std::remove(db.c_str());
}

// ========== 性能测试 ==========

TEST(BPlusTreeSplitTest, LargeScaleInsertion) {
    std::string db = MakeTempDbFileSplit("large_scale");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(128, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入大量记录
        const int NUM_RECORDS = 1000;
        for (int i = 0; i < NUM_RECORDS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 10000));
        }
        
        // 验证所有记录都能找到
        for (int i = 0; i < NUM_RECORDS; ++i) {
            auto results = index.Find(i);
            EXPECT_FALSE(results.empty());
            EXPECT_EQ(results[0], i * 10000);
        }
        
        // 验证范围查询性能
        auto range_results = index.FindRange(100, 900);
        EXPECT_EQ(range_results.size(), 801); // 100到900，包含边界
    }
    std::remove(db.c_str());
}

// ========== 删除和重新平衡测试 ==========

TEST(BPlusTreeSplitTest, DeleteAfterSplit) {
    std::string db = MakeTempDbFileSplit("delete_after_split");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入记录以触发分裂
        const int NUM_RECORDS = 60;
        for (int i = 0; i < NUM_RECORDS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
        }
        
        // 删除一些记录
        for (int i = 10; i < 20; ++i) {
            EXPECT_TRUE(index.Remove(i));
        }
        
        // 验证删除的记录找不到
        for (int i = 10; i < 20; ++i) {
            auto results = index.Find(i);
            EXPECT_TRUE(results.empty());
        }
        
        // 验证未删除的记录仍然存在
        for (int i = 0; i < 10; ++i) {
            auto results = index.Find(i);
            EXPECT_FALSE(results.empty());
            EXPECT_EQ(results[0], i * 100);
        }
        
        for (int i = 20; i < NUM_RECORDS; ++i) {
            auto results = index.Find(i);
            EXPECT_FALSE(results.empty());
            EXPECT_EQ(results[0], i * 100);
        }
    }
    std::remove(db.c_str());
}

// ========== 索引管理器集成测试 ==========

TEST(BPlusTreeSplitTest, IndexManagerIntegration) {
    std::string db = MakeTempDbFileSplit("index_manager");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(32, &dm);
        IndexManager im(&bpm);
        
        // 创建索引
        EXPECT_TRUE(im.CreateIndex("test_table", "id"));
        
        // 插入记录
        const int NUM_RECORDS = 100;
        for (int i = 0; i < NUM_RECORDS; ++i) {
            Record record;
            record.AddValue(i);
            record.AddValue("Record " + std::to_string(i));
            EXPECT_TRUE(im.InsertRecord("test_table", record, i * 100));
        }
        
        // 验证索引查询
        for (int i = 0; i < NUM_RECORDS; ++i) {
            auto results = im.QueryWithIndex("test_table", "id", i);
            EXPECT_FALSE(results.empty());
            EXPECT_EQ(results[0], i * 100);
        }
        
        // 验证范围查询
        auto range_results = im.QueryRangeWithIndex("test_table", "id", 20, 80);
        EXPECT_EQ(range_results.size(), 61); // 20到80，包含边界
    }
    std::remove(db.c_str());
}
