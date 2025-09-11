#include <gtest/gtest.h>
#include <cstdio>
#include <cstring>
#include <string>
#include "../include/catalog/page.h"
#include "storage/disk_manager.h"
#include "storage/buffer_pool_manager.h"
#include "../include/catalog/types.h"
#include "../include/catalog/table_manager.h"

// 为每次测试创建独立的临时文件，避免相互干扰
static std::string MakeTempDbFile(const std::string &name_hint) {
    std::string path = std::string("test_") + name_hint + ".db";
    // 清理旧文件
    std::remove(path.c_str());
    return path;
}

TEST(PageTest, BasicReadWrite) {
    Page page;
    EXPECT_EQ(page.GetPageId(), -1);
    EXPECT_FALSE(page.IsDirty());

    const char *msg = "hello page";
    std::memcpy(page.GetData(), msg, std::strlen(msg) + 1);
    page.SetPageId(123);
    page.SetDirty(true);

    EXPECT_EQ(page.GetPageId(), 123);
    EXPECT_TRUE(page.IsDirty());
    EXPECT_STREQ(page.GetData(), msg);
}

TEST(DiskManagerTest, AllocateWriteReadDeallocate) {
    std::string db = MakeTempDbFile("disk_manager");
    {
        DiskManager dm(db);
        // 分配两个页
        int p0 = dm.AllocatePage();
        int p1 = dm.AllocatePage();
        EXPECT_EQ(p0, 0);
        EXPECT_EQ(p1, 1);

        // 写入固定内容
        char buf0[PAGE_SIZE];
        char buf1[PAGE_SIZE];
        std::memset(buf0, 0, sizeof(buf0));
        std::memset(buf1, 0, sizeof(buf1));
        std::string s0 = "page0 data";
        std::string s1 = "page1 data";
        std::memcpy(buf0, s0.c_str(), s0.size() + 1);
        std::memcpy(buf1, s1.c_str(), s1.size() + 1);

        dm.WritePage(p0, buf0);
        dm.WritePage(p1, buf1);

        // 读取验证
        char r0[PAGE_SIZE];
        char r1[PAGE_SIZE];
        dm.ReadPage(p0, r0);
        dm.ReadPage(p1, r1);
        EXPECT_STREQ(r0, s0.c_str());
        EXPECT_STREQ(r1, s1.c_str());

        // 释放页，再次分配应复用
        dm.DeallocatePage(p0);
        int p2 = dm.AllocatePage();
        EXPECT_EQ(p2, p0);
    }
    // 测试结束后清理
    std::remove(db.c_str());
}

TEST(BufferPoolManagerTest, NewFetchUnpinFlushAndLRU) {
    std::string db = MakeTempDbFile("bpm");
    {
        DiskManager dm(db);
        // 缓冲池大小设为2，便于触发淘汰
        BufferPoolManager bpm(2, &dm);

        int p0, p1, p2;
        Page *page0 = bpm.NewPage(&p0);
        ASSERT_NE(page0, nullptr);
        std::string s0 = "buffer page 0";
        std::memcpy(page0->GetData(), s0.c_str(), s0.size() + 1);
        page0->SetDirty(true);
        EXPECT_TRUE(bpm.UnpinPage(p0));

        Page *page1 = bpm.NewPage(&p1);
        ASSERT_NE(page1, nullptr);
        std::string s1 = "buffer page 1";
        std::memcpy(page1->GetData(), s1.c_str(), s1.size() + 1);
        page1->SetDirty(true);
        EXPECT_TRUE(bpm.UnpinPage(p1));

        // 触发淘汰：新建第三页，池大小为2
        Page *page2 = bpm.NewPage(&p2);
        ASSERT_NE(page2, nullptr);
        std::string s2 = "buffer page 2";
        std::memcpy(page2->GetData(), s2.c_str(), s2.size() + 1);
        page2->SetDirty(true);
        EXPECT_TRUE(bpm.UnpinPage(p2));

        // 再次抓取 p0，应能从磁盘读取到原数据（如果被淘汰且正确写回）
        Page *fp0 = bpm.FetchPage(p0);
        ASSERT_NE(fp0, nullptr);
        EXPECT_STREQ(fp0->GetData(), s0.c_str());

        // 刷盘某页
        EXPECT_TRUE(bpm.FlushPage(p0));

        // 删除页
        EXPECT_TRUE(bpm.DeletePage(p1));
    }
    std::remove(db.c_str());
}

// ========== 引用管理测试 ==========
TEST(ReferenceManagementTest, PageReferenceCounting) {
    std::string db = MakeTempDbFile("ref_mgmt");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(10, &dm);

        // 分配新页
        int page_id;
        Page* page1 = bpm.NewPage(&page_id);
        ASSERT_NE(page1, nullptr);
        std::cout << "Allocated page_id: " << page_id << std::endl;

        // 获取页的引用
        Page* page2 = bpm.FetchPage(page_id);
        ASSERT_NE(page2, nullptr);
        ASSERT_EQ(page1, page2);
        std::cout << "Fetched page_id: " << page_id << std::endl;

        // 释放第一个引用
        bool result1 = bpm.UnpinPage(page_id);
        std::cout << "First UnpinPage result: " << result1 << std::endl;
        EXPECT_TRUE(result1);

        // 页应该还在缓冲池中（因为还有引用）
        Page* page3 = bpm.FetchPage(page_id);
        ASSERT_NE(page3, nullptr);
        std::cout << "Second fetch successful" << std::endl;

        // 释放第二个引用
        bool result2 = bpm.UnpinPage(page_id);
        std::cout << "Second UnpinPage result: " << result2 << std::endl;
        EXPECT_TRUE(result2);

        // 释放第三个引用
        bool result3 = bpm.UnpinPage(page_id);
        std::cout << "Third UnpinPage result: " << result3 << std::endl;
        EXPECT_TRUE(result3);

        // 现在可以安全删除页
        EXPECT_TRUE(bpm.DeletePage(page_id));
    }
    std::remove(db.c_str());
}

TEST(ReferenceManagementTest, PreventDeleteWithActiveReferences) {
    std::string db = MakeTempDbFile("ref_mgmt_prevent");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(10, &dm);

        // 分配新页
        int page_id;
        Page* page = bpm.NewPage(&page_id);
        ASSERT_NE(page, nullptr);

        // 获取页的引用
        Page* ref_page = bpm.FetchPage(page_id);
        ASSERT_NE(ref_page, nullptr);

        // 尝试删除页（应该失败，因为有活跃引用）
        EXPECT_FALSE(bpm.DeletePage(page_id));

        // 释放引用
        EXPECT_TRUE(bpm.UnpinPage(page_id));
        EXPECT_TRUE(bpm.UnpinPage(page_id));

        // 现在可以安全删除
        EXPECT_TRUE(bpm.DeletePage(page_id));
    }
    std::remove(db.c_str());
}

// ========== 文件压缩测试 ==========
TEST(FileCompactionTest, BasicCompaction) {
    std::string db = MakeTempDbFile("compaction");
    {
        DiskManager dm(db);

        // 分配多个页
        std::vector<int> page_ids;
        for (int i = 0; i < 5; ++i) {
            int page_id = dm.AllocatePage();
            page_ids.push_back(page_id);
        }

        // 验证文件大小
        EXPECT_EQ(dm.GetTotalPages(), 5);

        // 释放后面的页
        dm.DeallocatePage(page_ids[3]); // 释放页3
        dm.DeallocatePage(page_ids[4]); // 释放页4

        // 执行压缩
        EXPECT_TRUE(dm.CompactDatabase());

        // 验证压缩后的文件大小
        EXPECT_EQ(dm.GetTotalPages(), 3); // 只保留前3页

        // 验证空闲页数量
        EXPECT_EQ(dm.GetFreePages(), 0); // 压缩后应该没有空闲页
    }
    std::remove(db.c_str());
}

TEST(FileCompactionTest, CompactionWithGaps) {
    std::string db = MakeTempDbFile("compaction_gaps");
    {
        DiskManager dm(db);

        // 分配页：0, 1, 2, 3, 4
        std::vector<int> page_ids;
        for (int i = 0; i < 5; ++i) {
            int page_id = dm.AllocatePage();
            page_ids.push_back(page_id);
        }

        // 释放中间的页：保留0, 2, 4，释放1, 3
        dm.DeallocatePage(page_ids[1]); // 释放页1
        dm.DeallocatePage(page_ids[3]); // 释放页3

        // 执行压缩
        EXPECT_TRUE(dm.CompactDatabase());

        // 验证压缩后的文件大小（应该保留到页4）
        EXPECT_EQ(dm.GetTotalPages(), 5);

        // 验证空闲页数量
        EXPECT_EQ(dm.GetFreePages(), 2); // 页1和页3是空闲的
    }
    std::remove(db.c_str());
}

// ========== 页ID重用测试 ==========
TEST(PageReuseTest, PageIdReuse) {
    std::string db = MakeTempDbFile("page_reuse");
    {
        DiskManager dm(db);

        // 分配页
        int page1 = dm.AllocatePage();
        int page2 = dm.AllocatePage();
        int page3 = dm.AllocatePage();

        EXPECT_EQ(page1, 0);
        EXPECT_EQ(page2, 1);
        EXPECT_EQ(page3, 2);

        // 释放页
        dm.DeallocatePage(page2);
        dm.DeallocatePage(page3);

        // 重新分配页（应该重用页ID）
        int page4 = dm.AllocatePage();
        int page5 = dm.AllocatePage();

        EXPECT_EQ(page4, 2); // 重用页2
        EXPECT_EQ(page5, 1); // 重用页1

        // 验证空闲页数量
        EXPECT_EQ(dm.GetFreePages(), 0);
    }
    std::remove(db.c_str());
}

// ========== 表管理器集成测试 ==========
TEST(TableManagerIntegrationTest, CreateDropTableWithCompaction) {
    // 创建测试环境
    DiskManager dm("test_table_compaction.db");
    BufferPoolManager bpm(10, &dm);
    TableManager tm(&bpm);

    // 创建表结构
    TableSchema schema;
    schema.table_name_ = "test_table";
    schema.columns_ = {
        {"id", DataType::Int, 4, false},
        {"name", DataType::Varchar, 50, true},
        {"age", DataType::Int, 4, true}
    };
    schema.primary_key_index_ = 0;

    // 测试创建表
    EXPECT_TRUE(tm.CreateTable(schema));
    EXPECT_TRUE(tm.TableExists("test_table"));

    // 插入多条记录
    std::vector<Record> test_records;
    for (int i = 0; i < 100; ++i) {
        Record rec;
        rec.values_ = {i, "user" + std::to_string(i), 20 + i};
        rec.is_deleted_ = false;
        test_records.push_back(rec);
        EXPECT_TRUE(tm.InsertRecord("test_table", rec));
    }

    // 验证记录插入成功
    auto all_records = tm.SelectRecords("test_table");
    EXPECT_EQ(all_records.size(), 100);

    // 删除部分记录
    for (int i = 0; i < 50; ++i) {
        EXPECT_TRUE(tm.DeleteRecord("test_table", i));
    }

    // 验证删除成功
    auto remaining_records = tm.SelectRecords("test_table");
    EXPECT_EQ(remaining_records.size(), 50);

    // 测试条件查询
    auto age_records = tm.SelectRecordsWithCondition("test_table", "age > 70");
    EXPECT_EQ(age_records.size(), 49); // 记录71-119，共49条

    // 测试字符串条件查询
    auto name_records = tm.SelectRecordsWithCondition("test_table", "name = 'user75'");
    EXPECT_EQ(name_records.size(), 1);
    EXPECT_EQ(std::get<int>(name_records[0].values_[0]), 75);

    // 测试表统计信息
    EXPECT_EQ(tm.GetRecordCount("test_table"), 50);
    EXPECT_GT(tm.GetPageCount("test_table"), 0);

    // 删除表
    EXPECT_TRUE(tm.DropTable("test_table"));
    EXPECT_FALSE(tm.TableExists("test_table"));

    // 验证表删除后无法操作
    EXPECT_FALSE(tm.InsertRecord("test_table", test_records[0]));
    EXPECT_FALSE(tm.DeleteRecord("test_table", 0));
    EXPECT_TRUE(tm.SelectRecords("test_table").empty());

    // 清理测试文件
    std::remove("test_table_compaction.db");
}

// ========== 压力测试 ==========
TEST(StressTest, LargeScaleOperations) {
    std::string db = MakeTempDbFile("stress_test");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(50, &dm);
        TableManager tm(&bpm);

        // 创建表结构
        TableSchema schema("stress_table");
        schema.AddColumn(Column("id", DataType::Int));
        schema.AddColumn(Column("data", DataType::Varchar, 100));

        EXPECT_TRUE(tm.CreateTable(schema));

        // 插入大量记录
        const int NUM_RECORDS = 100;
        for (int i = 0; i < NUM_RECORDS; ++i) {
            Record record;
            record.AddValue(Value(i));
            record.AddValue(Value("Data" + std::to_string(i)));
            EXPECT_TRUE(tm.InsertRecord("stress_table", record));
        }

        // 验证记录数量
        EXPECT_EQ(tm.GetRecordCount("stress_table"), NUM_RECORDS);

        // 删除一半记录
        for (int i = 0; i < NUM_RECORDS / 2; ++i) {
            EXPECT_TRUE(tm.DeleteRecord("stress_table", i));
        }

        // 验证删除后的记录数量
        EXPECT_EQ(tm.GetRecordCount("stress_table"), NUM_RECORDS / 2);

        // 执行压缩
        EXPECT_TRUE(dm.CompactDatabase());

        // 验证压缩效果
        EXPECT_LT(dm.GetTotalPages(), NUM_RECORDS);
    }
    std::remove(db.c_str());
}

// ========== 错误处理测试 ==========
TEST(ErrorHandlingTest, InvalidOperations) {
    std::string db = MakeTempDbFile("error_handling");
    {
        DiskManager dm(db);
        BufferPoolManager bpm(10, &dm);

        // 尝试删除不存在的页
        EXPECT_FALSE(bpm.DeletePage(999));

        // 尝试获取不存在的页
        Page* page = bpm.FetchPage(999);
        ASSERT_NE(page, nullptr); // 应该返回空页

        // 尝试释放不存在的页
        EXPECT_FALSE(bpm.UnpinPage(999));

        // 尝试压缩空数据库
        EXPECT_FALSE(dm.CompactDatabase());
    }
    std::remove(db.c_str());
}