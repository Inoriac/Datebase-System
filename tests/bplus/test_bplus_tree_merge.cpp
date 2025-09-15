//
// Created by Huang_cj on 2025/9/14.
// B+树节点合并逻辑测试
//

#include <gtest/gtest.h>
#include <cstdio>
#include <string>
#include <vector>
#include <set>

#include "../../include/storage/async_aliases.h"
#include "../../include/bplus/bplus_tree.h"
#include "../../include/catalog/types.h"

// 为每次测试创建独立的临时文件，避免相互干扰
static std::string MakeTempDbFileMerge(const std::string &name_hint) {
    std::string path = std::string("test_merge_") + name_hint + ".db";
    // 清理旧文件
    std::remove(path.c_str());
    return path;
}

// 辅助函数：验证B+树的完整性
static bool ValidateBPlusTreeIntegrity(BPlusTreeIndex& index, const std::set<int>& expected_keys) {
    // 验证所有期望的键都能找到
    for (int key : expected_keys) {
        auto results = index.Find(key);
        if (results.empty()) {
            std::cout << "ERROR: Key " << key << " not found" << std::endl;
            return false;
        }
    }
    
    // 验证范围查询
    if (!expected_keys.empty()) {
        int min_key = *expected_keys.begin();
        int max_key = *expected_keys.rbegin();
        auto range_results = index.FindRange(min_key, max_key);
        
        if (range_results.size() != expected_keys.size()) {
            std::cout << "ERROR: Range query returned " << range_results.size() 
                      << " results, expected " << expected_keys.size() << std::endl;
            return false;
        }
    }
    
    return true;
}

// ========== 基本合并测试 ==========

TEST(BPlusTreeMergeTest, BasicLeafMerge) {
    std::string db = MakeTempDbFileMerge("basic_leaf_merge");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入键以创建多个叶子节点
        const int NUM_KEYS = 100;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
            expected_keys.insert(i);
        }
        
        // 删除一些键以触发合并
        for (int i = 20; i < 80; ++i) {
            EXPECT_TRUE(index.Remove(i));
            expected_keys.erase(i);
        }
        
        // 验证删除后的完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
        
        // 验证删除的键确实不存在
        for (int i = 20; i < 80; ++i) {
            auto results = index.Find(i);
            EXPECT_TRUE(results.empty());
        }
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeMergeTest, LeafMergeWithStringKeys) {
    std::string db = MakeTempDbFileMerge("leaf_merge_string");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "name", 0);
        
        // 插入字符串键
        std::vector<std::string> keys = {
            "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
            "k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
            "u", "v", "w", "x", "y", "z", "aa", "bb", "cc", "dd",
            "ee", "ff", "gg", "hh", "ii", "jj", "kk", "ll", "mm", "nn",
            "oo", "pp", "qq", "rr", "ss", "tt", "uu", "vv", "ww", "xx"
        };
        
        std::set<std::string> expected_keys_set;
        for (size_t i = 0; i < keys.size(); ++i) {
            EXPECT_TRUE(index.Insert(keys[i], static_cast<int>(i * 100)));
            expected_keys_set.insert(keys[i]);
        }
        
        // 删除一些键以触发合并
        for (size_t i = 10; i < 30; ++i) {
            EXPECT_TRUE(index.Remove(keys[i]));
            expected_keys_set.erase(keys[i]);
        }
        
        // 验证剩余键的完整性
        for (const std::string& key : expected_keys_set) {
            auto results = index.Find(key);
            EXPECT_FALSE(results.empty());
        }
    }
    std::remove(db.c_str());
}

// ========== 内部节点合并测试 ==========

TEST(BPlusTreeMergeTest, InternalNodeMerge) {
    std::string db = MakeTempDbFileMerge("internal_merge");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(64, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入大量键以创建多层结构
        const int NUM_KEYS = 500;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 1000));
            expected_keys.insert(i);
        }
        
        // 记录初始高度
        int initial_height = index.GetHeight();
        
        // 删除大量键以触发内部节点合并
        for (int i = 100; i < 400; ++i) {
            EXPECT_TRUE(index.Remove(i));
            expected_keys.erase(i);
        }
        
        // 验证删除后的完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
        
        // 验证树高度可能减少（合并后）
        int final_height = index.GetHeight();
        EXPECT_LE(final_height, initial_height);
    }
    std::remove(db.c_str());
}

// ========== 重新分布测试 ==========

TEST(BPlusTreeMergeTest, NodeRedistribution) {
    std::string db = MakeTempDbFileMerge("redistribution");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(32, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入键以创建多个节点
        const int NUM_KEYS = 150;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
            expected_keys.insert(i);
        }
        
        // 删除一些键以创建不平衡的节点
        for (int i = 50; i < 100; ++i) {
            EXPECT_TRUE(index.Remove(i));
            expected_keys.erase(i);
        }
        
        // 验证删除后的完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
        
        // 继续插入新键以测试重新分布
        for (int i = 200; i < 250; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
            expected_keys.insert(i);
        }
        
        // 验证最终完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
    }
    std::remove(db.c_str());
}

// ========== 边界情况测试 ==========

TEST(BPlusTreeMergeTest, MergeWithMinimumKeys) {
    std::string db = MakeTempDbFileMerge("merge_minimum_keys");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入恰好达到分裂阈值的键
        const int NUM_KEYS = 51;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
            expected_keys.insert(i);
        }
        
        // 删除一个键
        EXPECT_TRUE(index.Remove(25));
        expected_keys.erase(25);
        
        // 验证删除后的完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
        
        // 验证删除的键不存在
        auto results = index.Find(25);
        EXPECT_TRUE(results.empty());
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeMergeTest, MergeWithEmptyNodes) {
    std::string db = MakeTempDbFileMerge("merge_empty_nodes");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入一些键
        const int NUM_KEYS = 30;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
            expected_keys.insert(i);
        }
        
        // 删除大部分键
        for (int i = 5; i < 25; ++i) {
            EXPECT_TRUE(index.Remove(i));
            expected_keys.erase(i);
        }
        
        // 验证删除后的完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
        
        // 验证剩余的键
        for (int i = 0; i < 5; ++i) {
            auto results = index.Find(i);
            EXPECT_FALSE(results.empty());
        }
        
        for (int i = 25; i < NUM_KEYS; ++i) {
            auto results = index.Find(i);
            EXPECT_FALSE(results.empty());
        }
    }
    std::remove(db.c_str());
}

// ========== 性能测试 ==========

TEST(BPlusTreeMergeTest, LargeScaleMerge) {
    std::string db = MakeTempDbFileMerge("large_scale_merge");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(128, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入大量键
        const int NUM_KEYS = 1000;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 1000));
            expected_keys.insert(i);
        }
        
        // 记录初始高度
        int initial_height = index.GetHeight();
        
        // 删除大量键以触发合并
        for (int i = 200; i < 800; ++i) {
            EXPECT_TRUE(index.Remove(i));
            expected_keys.erase(i);
        }
        
        // 验证删除后的完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
        
        // 验证树高度可能减少
        int final_height = index.GetHeight();
        EXPECT_LE(final_height, initial_height);
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeMergeTest, MixedOperationsWithMerge) {
    std::string db = MakeTempDbFileMerge("mixed_operations_merge");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(32, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入键以创建多节点结构
        const int NUM_KEYS = 200;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
            expected_keys.insert(i);
        }
        
        // 混合操作：删除、插入、查找
        for (int i = 0; i < 50; ++i) {
            // 删除一些键
            if (i % 3 == 0) {
                int delete_key = i * 4;
                if (expected_keys.count(delete_key) > 0) {
                    EXPECT_TRUE(index.Remove(delete_key));
                    expected_keys.erase(delete_key);
                }
            }
            
            // 插入新键
            int new_key = NUM_KEYS + i;
            EXPECT_TRUE(index.Insert(new_key, new_key * 100));
            expected_keys.insert(new_key);
            
            // 验证随机键
            int check_key = (i * 7) % (NUM_KEYS + i + 1);
            if (expected_keys.count(check_key) > 0) {
                auto results = index.Find(check_key);
                EXPECT_FALSE(results.empty());
            }
            
            // 每10次操作验证一次完整性
            if (i % 10 == 0) {
                EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
            }
        }
        
        // 最终验证
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
    }
    std::remove(db.c_str());
}

// ========== 索引管理器集成测试 ==========

TEST(BPlusTreeMergeTest, IndexManagerMergeIntegration) {
    std::string db = MakeTempDbFileMerge("index_manager_merge");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(32, &dm);
        IndexManager im(&bpm);
        
        // 创建索引
        EXPECT_TRUE(im.CreateIndex("test_table", "id"));
        
        // 插入记录
        const int NUM_RECORDS = 200;
        for (int i = 0; i < NUM_RECORDS; ++i) {
            Record record;
            record.AddValue(i);
            record.AddValue("Record " + std::to_string(i));
            EXPECT_TRUE(im.InsertRecord("test_table", record, i * 100));
        }
        
        // 删除一些记录
        for (int i = 50; i < 150; ++i) {
            Record record;
            record.AddValue(i);
            record.AddValue("Record " + std::to_string(i));
            EXPECT_TRUE(im.DeleteRecord("test_table", record, i * 100));
        }
        
        // 验证剩余记录
        for (int i = 0; i < 50; ++i) {
            auto results = im.QueryWithIndex("test_table", "id", i);
            EXPECT_FALSE(results.empty());
        }
        
        for (int i = 150; i < NUM_RECORDS; ++i) {
            auto results = im.QueryWithIndex("test_table", "id", i);
            EXPECT_FALSE(results.empty());
        }
        
        // 验证删除的记录不存在
        for (int i = 50; i < 150; ++i) {
            auto results = im.QueryWithIndex("test_table", "id", i);
            EXPECT_TRUE(results.empty());
        }
    }
    std::remove(db.c_str());
}
