//
// 测试异步IO集成的简单程序
//

#include "storage/async_aliases.h"
#include "catalog/table_manager.h"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "=== 测试异步IO集成 ===" << std::endl;
    
    try {
        // 测试异步磁盘管理器
        std::cout << "1. 创建异步磁盘管理器..." << std::endl;
        DiskManager disk_manager("async_integration_test.db", 2);
        std::cout << "   总页数: " << disk_manager.GetTotalPages() << std::endl;
        
        // 测试异步缓冲池管理器
        std::cout << "2. 创建异步缓冲池管理器..." << std::endl;
        BufferPoolManager buffer_pool(10, &disk_manager);
        
        // 测试表管理器
        std::cout << "3. 创建表管理器..." << std::endl;
        TableManager table_manager(&buffer_pool);
        
        // 测试创建表
        std::cout << "4. 测试创建表..." << std::endl;
        TableSchema schema;
        schema.table_name_ = "test_table";
        schema.columns_.push_back({"id", DataType::Int, true});
        schema.columns_.push_back({"name", DataType::Varchar, false});
        
        bool create_result = table_manager.CreateTable(schema);
        std::cout << "   创建表结果: " << (create_result ? "成功" : "失败") << std::endl;
        
        // 测试插入记录
        std::cout << "5. 测试插入记录..." << std::endl;
        Record record;
        record.values_ = {1, std::string("Alice")};
        record.is_deleted_ = false;
        
        int insert_result = table_manager.InsertRecord("test_table", record);
        std::cout << "   插入记录结果: " << insert_result << std::endl;
        
        // 测试查询记录
        std::cout << "6. 测试查询记录..." << std::endl;
        auto records = table_manager.SelectRecords("test_table");
        std::cout << "   查询到 " << records.size() << " 条记录" << std::endl;
        
        for (const auto& rec : records) {
            std::cout << "   记录: ";
            for (const auto& val : rec.values_) {
                if (std::holds_alternative<int>(val)) {
                    std::cout << std::get<int>(val) << " ";
                } else if (std::holds_alternative<std::string>(val)) {
                    std::cout << std::get<std::string>(val) << " ";
                }
            }
            std::cout << std::endl;
        }
        
        // 测试异步操作
        std::cout << "7. 测试异步操作..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        
        // 异步写入一些页面
        std::vector<std::future<bool>> write_futures;
        for (int i = 0; i < 5; ++i) {
            int page_id = disk_manager.AllocatePage();
            char* data = new char[PAGE_SIZE];
            memset(data, 'A' + i, PAGE_SIZE);
            
            auto future = disk_manager.WritePageAsync(page_id, data);
            write_futures.push_back(std::move(future));
            delete[] data;
        }
        
        // 等待所有写入完成
        for (auto& future : write_futures) {
            bool success = future.get();
            std::cout << "   异步写入: " << (success ? "成功" : "失败") << std::endl;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "   异步操作耗时: " << duration.count() << "ms" << std::endl;
        
        // 测试表信息
        std::cout << "8. 测试表信息..." << std::endl;
        table_manager.PrintTableInfo("test_table");
        
        std::cout << "\n=== 异步IO集成测试完成 ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
