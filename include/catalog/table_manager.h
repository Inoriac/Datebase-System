//
// Created by Huang_cj on 2025/9/9.
//

#pragma once
#include "types.h"
#include "../storage/buffer_pool_manager.h"
#include "table_schema_manager.h"
#include <unordered_map>
#include <vector>

class TableManager {
public:
    explicit TableManager(BufferPoolManager* bpm);
    ~TableManager();

    // 表管理
    bool CreateTable(const TableSchema& schema);
    bool DropTable(const std::string& table_name);
    bool TableExists(const std::string& table_name);
    TableSchema* GetTableSchema(const std::string& table_name);

    // 记录操作
    bool InsertRecord(const std::string& table_name, const Record& record);
    bool DeleteRecord(const std::string& table_name, int record_id);
    std::vector<Record> SelectRecords(const std::string& table_name);
    std::vector<Record> SelectRecordsWithCondition(const std::string& table_name, const std::string& condition);
    
    // 投影操作（Project算子功能）
    std::vector<Record> SelectColumns(const std::string& table_name, const std::vector<std::string>& column_names);
    std::vector<Record> SelectColumnsWithCondition(const std::string& table_name, 
                                                  const std::vector<std::string>& column_names,
                                                  const std::string& condition);

    // 更新操作
    bool UpdateRecord(const std::string& table_name, int record_id, const Record& new_record);
    int UpdateRecordsWithCondition(const std::string& table_name, const std::string& condition, const Record& new_record);

    // 页管理
    int AllocatePageForTable(const std::string& table_name);
    bool DeallocatePageForTable(const std::string& table_name, int page_id);

    // 统计信息
    int GetRecordCount(const std::string& table_name);
    int GetPageCount(const std::string& table_name);

    // 调试信息
    void PrintTableInfo(const std::string& table_name);
    void PrintAllTables();
    
    // 表结构持久化管理
    bool LoadAllTableSchemas();
    bool SaveAllTableSchemas();

    // 页头信息(存储在页的前几个字节)
    struct PageHeader {
        int record_count;   // 页中记录数量
        int free_space_offset;  // 空闲空间起始偏移
        int table_id;       // 表ID 用于验证

        static const int HEADER_SIZE = 12;   // 3个int，每个4字节
    };

private:
    BufferPoolManager* buffer_pool_manager_;
    TableSchemaManager* schema_manager_;                            // 表结构管理器
    std::unordered_map<std::string, TableSchema> table_schemas_;    // 表名 -> 表对象
    std::unordered_map<std::string, std::vector<int>> table_pages;  // 表名 -> 页ID列表

    // 内部方法
    int FindFreeSlotInPage(int page_id, const TableSchema& schema);     // 返回下一个可写的位置
    bool WriteRecordToPage(int page_id, int offset, const Record& record, const TableSchema& schema);
    bool ReadRecordFromPage(int page_id, int offset, Record& record, const TableSchema& schema);

    bool WritePageHeader(int page_id, const PageHeader& header);
    bool ReadPageHeader(int page_id, PageHeader& header);
    
    // 投影辅助方法
    std::vector<Record> ProjectRecords(const std::vector<Record>& records, 
                                      const std::string& table_name,
                                      const std::vector<std::string>& column_names);
    std::vector<int> GetColumnIndices(const std::string& table_name, const std::vector<std::string>& column_names);
    std::vector<std::string> ValidateColumns(const std::string& table_name, const std::vector<std::string>& column_names);
};