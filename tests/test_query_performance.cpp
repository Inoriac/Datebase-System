//
// 查询性能测试
//

#include <gtest/gtest.h>
#include <cstdio>
#include <string>
#include <vector>
#include <chrono>
#include <random>

#include "../include/storage/async_aliases.h"
#include "../include/catalog/table_manager.h"
#include "../include/catalog/types.h"

// 为每次测试创建独立的临时文件，避免相互干扰
static std::string MakeTempDbFilePerf(const std::string &name_hint) {
    std::string path = std::string("test_perf_") + name_hint + ".db";
    // 清理旧文件
    std::remove(path.c_str());
    return path;
}

// 性能测试基类
class QueryPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        db_path_ = MakeTempDbFilePerf("performance");
        dm_ = std::make_unique<DiskManager>(db_path_, 1);
        bpm_ = std::make_unique<BufferPoolManager>(64, dm_.get());
        tm_ = std::make_unique<TableManager>(bpm_.get());
        
        // 创建测试表
        TableSchema schema("performance_test");
        schema.AddColumn(Column("id", DataType::Int, 4, false));
        schema.AddColumn(Column("name", DataType::Varchar, 50, true));
        schema.AddColumn(Column("age", DataType::Int, 4, true));
        schema.AddColumn(Column("salary", DataType::Int, 4, true));
        schema.primary_key_index_ = 0;  // id为主键
        
        EXPECT_TRUE(tm_->CreateTable(schema));
    }
    
    void TearDown() override {
        tm_.reset();
        bpm_.reset();
        dm_.reset();
        std::remove(db_path_.c_str());
    }
    
    // 插入测试数据
    void InsertTestData(int record_count) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> age_dist(18, 65);
        std::uniform_int_distribution<> salary_dist(30000, 150000);
        
        int success_count = 0;
        int fail_count = 0;
        
        for (int i = 0; i < record_count; ++i) {
            Record record;
            record.AddValue(i);  // id
            record.AddValue("User" + std::to_string(i));  // name
            record.AddValue(age_dist(gen));  // age
            record.AddValue(salary_dist(gen));  // salary
            
            bool success = tm_->InsertRecord("performance_test", record);
            if (success) {
                success_count++;
            } else {
                fail_count++;
                std::cout << "DEBUG: Failed to insert record " << i << std::endl;
            }
        }
        
        std::cout << "DEBUG: InsertTestData completed: " << success_count << " success, " << fail_count << " failed" << std::endl;
        EXPECT_EQ(success_count, record_count);
    }
    
    std::string db_path_;
    std::unique_ptr<DiskManager> dm_;
    std::unique_ptr<BufferPoolManager> bpm_;
    std::unique_ptr<TableManager> tm_;
};

// 测试全表扫描性能
TEST_F(QueryPerformanceTest, FullTableScanPerformance) {
    const int RECORD_COUNT = 200;  // 增加到200条记录，测试多级B+树
    InsertTestData(RECORD_COUNT);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 全表扫描
    auto records = tm_->SelectRecords("performance_test");
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_EQ(records.size(), RECORD_COUNT);
    std::cout << "全表扫描 " << RECORD_COUNT << " 条记录耗时: " << duration.count() << " 微秒" << std::endl;
}

// 测试条件查询性能
TEST_F(QueryPerformanceTest, ConditionalQueryPerformance) {
    const int RECORD_COUNT = 200;  // 增加到200条记录，测试多级B+树
    InsertTestData(RECORD_COUNT);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 条件查询：查找年龄大于30的记录
    auto records = tm_->SelectRecordsWithCondition("performance_test", "age > 30");
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "条件查询 " << RECORD_COUNT << " 条记录耗时: " << duration.count() << " 微秒" << std::endl;
    std::cout << "找到 " << records.size() << " 条匹配记录" << std::endl;
}

// 测试索引查询性能
TEST_F(QueryPerformanceTest, IndexedQueryPerformance) {
    const int RECORD_COUNT = 200;  // 增加到200条记录，测试多级B+树
    InsertTestData(RECORD_COUNT);
    
    // 验证数据插入是否成功
    auto all_records = tm_->SelectRecords("performance_test");
    std::cout << "DEBUG: 实际插入了 " << all_records.size() << " 条记录" << std::endl;
    EXPECT_EQ(all_records.size(), RECORD_COUNT);
    
    // 测试主键索引查询
    auto start = std::chrono::high_resolution_clock::now();
    
    Value key = 150;  // 查找id=150的记录（在0-199范围内）
    auto records = tm_->SelectRecordsWithIndex("performance_test", "id", key);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "索引查询耗时: " << duration.count() << " 微秒" << std::endl;
    EXPECT_EQ(records.size(), 1);
    if (!records.empty()) {
        EXPECT_EQ(std::get<int>(records[0].values_[0]), 150);
    }
}

// 测试范围查询性能
TEST_F(QueryPerformanceTest, RangeQueryPerformance) {
    const int RECORD_COUNT = 200;  // 增加到200条记录，测试多级B+树
    InsertTestData(RECORD_COUNT);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 范围查询：查找id在50-70之间的记录（在0-199范围内）
    Value start_key = 50;
    Value end_key = 70;
    auto records = tm_->SelectRecordsRangeWithIndex("performance_test", "id", start_key, end_key);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "范围查询耗时: " << duration.count() << " 微秒" << std::endl;
    std::cout << "找到 " << records.size() << " 条记录" << std::endl;
    EXPECT_EQ(records.size(), 21);  // 50到70，包含两端
}

// 测试大规模数据性能
TEST_F(QueryPerformanceTest, LargeScalePerformance) {
    const int RECORD_COUNT = 200;  // 增加到200条记录，测试多级B+树
    InsertTestData(RECORD_COUNT);
    
    // 测试全表扫描
    auto start = std::chrono::high_resolution_clock::now();
    auto records = tm_->SelectRecords("performance_test");
    auto end = std::chrono::high_resolution_clock::now();
    auto scan_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 测试索引查询
    start = std::chrono::high_resolution_clock::now();
    Value key = 150;  // 查找key=150（在0-199范围内）
    auto indexed_records = tm_->SelectRecordsWithIndex("performance_test", "id", key);
    end = std::chrono::high_resolution_clock::now();
    auto index_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "大规模数据测试 (" << RECORD_COUNT << " 条记录):" << std::endl;
    std::cout << "  全表扫描耗时: " << scan_duration.count() << " 毫秒" << std::endl;
    std::cout << "  索引查询耗时: " << index_duration.count() << " 微秒" << std::endl;
    
    EXPECT_EQ(records.size(), RECORD_COUNT);
    EXPECT_EQ(indexed_records.size(), 1);
}

// 测试投影查询性能
TEST_F(QueryPerformanceTest, ProjectionQueryPerformance) {
    const int RECORD_COUNT = 200;  // 增加到200条记录，测试多级B+树
    InsertTestData(RECORD_COUNT);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 投影查询：只选择id和name列
    std::vector<std::string> columns = {"id", "name"};
    auto records = tm_->SelectColumns("performance_test", columns);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "投影查询耗时: " << duration.count() << " 微秒" << std::endl;
    EXPECT_EQ(records.size(), RECORD_COUNT);
    
    // 验证投影结果
    if (!records.empty()) {
        EXPECT_EQ(records[0].values_.size(), 2);  // 只有id和name两列
    }
}

// 测试复合条件查询性能
TEST_F(QueryPerformanceTest, ComplexConditionQueryPerformance) {
    const int RECORD_COUNT = 200;  // 增加到200条记录，测试多级B+树
    InsertTestData(RECORD_COUNT);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 复合条件查询：年龄大于30且工资大于50000
    auto records = tm_->SelectRecordsWithCondition("performance_test", "age > 30");
    auto filtered_records = tm_->SelectRecordsWithCondition("performance_test", "salary > 50000");
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "复合条件查询耗时: " << duration.count() << " 微秒" << std::endl;
    std::cout << "年龄>30的记录: " << records.size() << " 条" << std::endl;
    std::cout << "工资>50000的记录: " << filtered_records.size() << " 条" << std::endl;
}
