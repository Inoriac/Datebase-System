#include <gtest/gtest.h>
#include <cstdio>
#include <cstring>
#include <string>
#include "storage/page.h"
#include "storage/disk_manager.h"
#include "storage/buffer_pool_manager.h"

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
