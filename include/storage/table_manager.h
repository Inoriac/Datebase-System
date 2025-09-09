//
// Created by Huang_cj on 2025/9/9.
//

#pragma once
#include "types.h"
#include "buffer_pool_manager.h"
#include "record_serializer.h"
#include <unordered_map>
#include <vector>

class TableManager {
public:
    explicit TableManager(BufferPoolManager* bpm);
    ~TableManager();

    // 表管理
    bool CreateTable(const TableSchema& schema);
    bool DropTable(const TableSchema& schema);
    bool TableExists(const TableSchema& schema);
    TableSchema* GetTableSchema(const std::string& table_name);

    // 记录操作
    bool InsertRecord(const std::string& table_name, const Record& record);
    bool DeleteRecord(const std::string& table_name, int record_id);
    std::vector<Record> SelectRecords(const std::string& table_name);
    std::vector<Record> SelectRecordsWithCondition(const std::string& table_name, const std::string& condition);

    // 页管理
    int AllocatePageForTable(const std::string& table_name);
    bool DeallocatePageForTable(const std::string& table_name, int page_id);

    // 统计信息
    int GetRecordCount(const std::string& table_name);
    int GetPageCount(const std::string& table_name);

    // 调试信息
    void PrintTableInfo(const std::string& table_name);
    void PrintAllTables();

    // 页头信息(存储在页的前几个字节)
    struct PageHeader {
        int record_count;   // 页中记录数量
        int free_space_offset;  // 空闲空间起始偏移
        int table_id;       // 表ID 用于验证

        static const int HEADER_SIZE = 3;   // 3个int
    };

private:
    BufferPoolManager* buffer_pool_manager_;
    std::unordered_map<std::string, TableSchema> table_schemas_;
    std::unordered_map<std::string, std::vector<int>> table_pages;  // 表名 -> 页ID列表

    // 内部方法
    bool WriteTableSchemaToPage(const TableSchema& schema, int page_id);
    bool ReadTableSchemaFromPage(TableSchema& schema, int page_id);
    int FindFreeSlotInPage(int page_id, const TableSchema& schema);
    bool WriteRecordToPage(int page_id, int offset, const Record& record, const TableSchema& schema);
    bool ReadRecordFromPage(int page_id, int offset, Record& record, const TableSchema& schema);

    bool WritePageHeader(int page_id, const PageHeader& header);
    bool ReadPageHeader(int page_id, PageHeader& header);
};