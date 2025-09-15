//
// Created by Huang_cj on 2025/9/14.
// B+树分裂逻辑详细测试
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
static std::string MakeTempDbFileSplitDetailed(const std::string &name_hint) {
    std::string path = std::string("test_split_detailed_") + name_hint + ".db";
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

// ========== 叶子节点分裂详细测试 ==========

TEST(BPlusTreeSplitDetailedTest, LeafSplitExactBoundary) {
    std::string db = MakeTempDbFileSplitDetailed("leaf_exact_boundary");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入恰好触发分裂的键数量
        const int NUM_KEYS = 51; // 超过默认的50个键限制
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
            expected_keys.insert(i);
        }
        
        // 验证树结构完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
        
        // 验证所有键都能找到
        for (int i = 0; i < NUM_KEYS; ++i) {
            auto results = index.Find(i);
            EXPECT_FALSE(results.empty());
            EXPECT_EQ(results[0], i * 100);
        }
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeSplitDetailedTest, LeafSplitMultipleTimes) {
    std::string db = MakeTempDbFileSplitDetailed("leaf_multiple_splits");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(32, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入大量键以触发多次分裂
        const int NUM_KEYS = 200;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 1000));
            expected_keys.insert(i);
        }
        
        // 验证树结构完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
        
        // 验证树的高度
        EXPECT_GT(index.GetHeight(), 1);
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeSplitDetailedTest, LeafSplitWithStringKeys) {
    std::string db = MakeTempDbFileSplitDetailed("leaf_string_keys");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "name", 0);
        
        // 插入字符串键（按字母顺序排列，移除重复键）
        std::vector<std::string> keys = {
            "alef", "aleph", "alpha", "ayin", "bet", "beta", "beth", "chet", "chet2",
            "chi", "dalet", "daleth", "delta", "epsilon", "eta", "gamma", "gimel", "gimel2",
            "he", "he2", "iota", "kaf", "kaf2", "kappa", "lambda", "lamed", "mem",
            "mu", "nu", "nun", "omega", "omicron", "pe", "phi", "pi", "psi",
            "qof", "resh", "rho", "samekh", "shin", "sigma", "tau", "tav", "tet",
            "tet2", "theta", "tsade", "upsilon", "vav", "vav2", "xi", "yod", "yod2",
            "zayin", "zayin2", "zeta"
        };
        
        std::set<std::string> expected_keys_set;
        for (size_t i = 0; i < keys.size(); ++i) {
            EXPECT_TRUE(index.Insert(keys[i], static_cast<int>(i * 100)));
            expected_keys_set.insert(keys[i]);
        }
        
        // 验证所有键都能找到
        for (size_t i = 0; i < keys.size(); ++i) {
            auto results = index.Find(keys[i]);
            EXPECT_FALSE(results.empty());
            EXPECT_EQ(results[0], static_cast<int>(i * 100));
        }
        
        // 验证范围查询
        auto range_results = index.FindRange("alpha", "zeta");
        // 从"alpha"到"zeta"应该包含55个键（索引2到56）
        EXPECT_EQ(range_results.size(), 55);
    }
    std::remove(db.c_str());
}

// ========== 内部节点分裂详细测试 ==========

TEST(BPlusTreeSplitDetailedTest, InternalNodeSplit) {
    std::string db = MakeTempDbFileSplitDetailed("internal_split");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(64, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入大量键以触发内部节点分裂
        const int NUM_KEYS = 1000;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 10000));
            expected_keys.insert(i);
        }
        
        // 验证树结构完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
        
        // 验证树的高度（应该有内部节点）
        EXPECT_GT(index.GetHeight(), 2);
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeSplitDetailedTest, InternalNodeSplitWithRandomKeys) {
    std::string db = MakeTempDbFileSplitDetailed("internal_random_keys");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(64, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入随机顺序的键
        std::vector<int> keys;
        for (int i = 0; i < 500; ++i) {
            keys.push_back(i);
        }
        
        // 随机打乱
        std::random_shuffle(keys.begin(), keys.end());
        
        std::set<int> expected_keys;
        for (int key : keys) {
            EXPECT_TRUE(index.Insert(key, key * 1000));
            expected_keys.insert(key);
        }
        
        // 验证树结构完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
    }
    std::remove(db.c_str());
}

// ========== 分裂后操作测试 ==========

TEST(BPlusTreeSplitDetailedTest, OperationsAfterSplit) {
    std::string db = MakeTempDbFileSplitDetailed("operations_after_split");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(32, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入键以触发分裂
        const int NUM_KEYS = 100;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
            expected_keys.insert(i);
        }
        
        // 分裂后继续插入
        for (int i = NUM_KEYS; i < NUM_KEYS + 50; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
            expected_keys.insert(i);
        }
        
        // 验证树结构完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
        
        // 分裂后删除一些键
        for (int i = 10; i < 30; ++i) {
            EXPECT_TRUE(index.Remove(i));
            expected_keys.erase(i);
        }
        
        // 验证删除后的完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
    }
    std::remove(db.c_str());
}

// ========== 边界情况测试 ==========

TEST(BPlusTreeSplitDetailedTest, SplitWithMinimumKeys) {
    std::string db = MakeTempDbFileSplitDetailed("split_minimum_keys");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(16, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入恰好达到分裂阈值的键
        const int NUM_KEYS = 50; // 恰好达到默认限制
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
            expected_keys.insert(i);
        }
        
        // 再插入一个键应该触发分裂
        EXPECT_TRUE(index.Insert(NUM_KEYS, NUM_KEYS * 100));
        expected_keys.insert(NUM_KEYS);
        
        // 验证分裂后的完整性
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeSplitDetailedTest, SplitWithDuplicateKeys) {
    std::string db = MakeTempDbFileSplitDetailed("split_duplicate_keys");
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
        
        // 尝试插入重复键（应该失败）
        for (int i = 0; i < 10; ++i) {
            EXPECT_FALSE(index.Insert(i, i * 200)); // 重复键应该失败
        }
        
        // 验证原始键仍然存在
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
    }
    std::remove(db.c_str());
}

// ========== 性能压力测试 ==========

TEST(BPlusTreeSplitDetailedTest, StressTestWithManySplits) {
    std::string db = MakeTempDbFileSplitDetailed("stress_many_splits");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(128, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入大量键以触发多次分裂
        const int NUM_KEYS = 2000;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 1000));
            expected_keys.insert(i);
            
            // 每插入100个键验证一次完整性
            if (i % 100 == 0) {
                EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
            }
        }
        
        // 最终验证
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
        
        // 验证树的高度
        EXPECT_GT(index.GetHeight(), 2);
    }
    std::remove(db.c_str());
}

TEST(BPlusTreeSplitDetailedTest, MixedOperationsAfterSplit) {
    std::string db = MakeTempDbFileSplitDetailed("mixed_operations");
    {
        DiskManager dm(db, 1);
        BufferPoolManager bpm(32, &dm);
        
        BPlusTreeIndex index(&bpm, "test_table", "id", 0);
        
        // 插入键以触发分裂
        const int NUM_KEYS = 100;
        std::set<int> expected_keys;
        
        for (int i = 0; i < NUM_KEYS; ++i) {
            EXPECT_TRUE(index.Insert(i, i * 100));
            expected_keys.insert(i);
        }
        
        // 混合操作：插入、删除、查找
        for (int i = 0; i < 50; ++i) {
            // 插入新键
            int new_key = NUM_KEYS + i;
            EXPECT_TRUE(index.Insert(new_key, new_key * 100));
            expected_keys.insert(new_key);
            
            // 删除一些旧键
            if (i % 3 == 0) {
                int delete_key = i * 2;
                if (expected_keys.count(delete_key) > 0) {
                    EXPECT_TRUE(index.Remove(delete_key));
                    expected_keys.erase(delete_key);
                }
            }
            
            // 验证随机键
            int check_key = (i * 7) % (NUM_KEYS + i + 1);
            if (expected_keys.count(check_key) > 0) {
                auto results = index.Find(check_key);
                EXPECT_FALSE(results.empty());
            }
        }
        
        // 最终验证
        EXPECT_TRUE(ValidateBPlusTreeIntegrity(index, expected_keys));
    }
    std::remove(db.c_str());
}
