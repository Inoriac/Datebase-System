//
// Created by Huang_cj on 2025/9/9.
//

#include "storage/table_manager.h"
#include "storage/page.h"
#include <cstring>
#include <iostream>

using Header = TableManager::PageHeader;

static void WriteInt(char* dst, int value) {
    std::memcpy(dst, &value, sizeof(int));
}

static int ReadInt(const char* src) {
    int v;
    std::memcpy(&v, src, sizeof(int));
    return v;
}

TableManager::TableManager(BufferPoolManager *bpm): buffer_pool_manager_(bpm) {}

TableManager::~TableManager() = default;

bool TableManager::CreateTable(const TableSchema &schema) {

}

bool TableManager::DropTable(const TableSchema &schema) {
}

bool TableManager::TableExists(const TableSchema &schema) {
}

TableSchema * TableManager::GetTableSchema(const std::string &table_name) {
}

bool TableManager::InsertRecord(const std::string &table_name, const Record &record) {
}

bool TableManager::DeleteRecord(const std::string &table_name, int record_id) {
}

std::vector<Record> TableManager::SelectRecords(const std::string &table_name) {
}

std::vector<Record> TableManager::SelectRecordsWithCondition(const std::string &table_name,
    const std::string &condition) {
}

int TableManager::AllocatePageForTable(const std::string &table_name) {
}

bool TableManager::DeallocatePageForTable(const std::string &table_name, int page_id) {
}

int TableManager::GetRecordCount(const std::string &table_name) {
}

int TableManager::GetPageCount(const std::string &table_name) {
}

void TableManager::PrintTableInfo(const std::string &table_name) {
}

void TableManager::PrintAllTables() {
}

bool TableManager::WriteTableSchemaToPage(const TableSchema &schema, int page_id) {
}

bool TableManager::ReadTableSchemaFromPage(TableSchema &schema, int page_id) {
}

int TableManager::FindFreeSlotInPage(int page_id, const TableSchema &schema) {
}

bool TableManager::WriteRecordToPage(int page_id, int offset, const Record &record, const TableSchema &schema) {
}

bool TableManager::ReadRecordFromPage(int page_id, int offset, Record &record, const TableSchema &schema) {
}

// 页头读写
bool TableManager::WritePageHeader(int page_id, const PageHeader &header) {
    Page* page =buffer_pool_manager_->FetchPage(page_id);
    if (page == nullptr) return false;
    char* data = page->GetData();

    // 往页头写内容
    WriteInt(data + 0, header.record_count);
    WriteInt(data + 4, header.free_space_offset);
    WriteInt(data + 8, header.table_id);

    page->SetDirty(true);
    return buffer_pool_manager_->UnpinPage(page_id);
}

bool TableManager::ReadPageHeader(int page_id, PageHeader &header) {
    Page* page = buffer_pool_manager_->FetchPage(page_id);
    if (page == nullptr) return false;
    const char* data = page->GetData();

    header.record_count = ReadInt(data + 0);
    header.free_space_offset = ReadInt(data + 4);
    header.table_id = ReadInt(data + 8);

    // 读取不修改，无需写回
    buffer_pool_manager_->UnpinPage(page_id);
    return true;
}
