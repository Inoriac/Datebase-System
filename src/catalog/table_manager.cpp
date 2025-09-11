//
// Created by Huang_cj on 2025/9/9.
//

#include "../../include/catalog/table_manager.h"
#include "../../include/catalog/page.h"
#include "../../include/catalog/table_schema_manager.h"
#include <cstring>
#include <algorithm>
#include <cctype>
#include <iostream>

#include "../../include/catalog/record_serializer.h"

using Header = TableManager::PageHeader;

// ========= 表结构持久化管理方法定义 ===========
bool TableManager::LoadAllTableSchemas() {
    // 使用 TableSchemaManager 加载所有表结构
    if (schema_manager_ == nullptr) {
        return false;
    }
    return schema_manager_->LoadAllTableSchemas(table_schemas_);
}

bool TableManager::SaveAllTableSchemas() {
    // 保存所有表结构到持久化存储
    if (schema_manager_ == nullptr) {
        return false;
    }
    bool success = true;
    for (const auto& pair : table_schemas_) {
        if (!schema_manager_->SaveTableSchema(pair.second)) {
            success = false;
        }
    }
    return success;
}

// 简单条件解析与比较辅助
enum class CmpOp { EQ, NE, LT, LE, GT, GE };

static std::string TrimSpaces(const std::string& s) {
    size_t i = 0, j = s.size();
    while (i < j && std::isspace(static_cast<unsigned char>(s[i]))) i++;
    while (j > i && std::isspace(static_cast<unsigned char>(s[j-1]))) j--;
    return s.substr(i, j - i);
}

static bool ParseCondition(const std::string& cond,
                           std::string& col,
                           CmpOp& op,
                           std::string& literal,
                           bool& is_string,
                           bool& is_bool,
                           int& int_value,
                           bool& bool_value) {
    std::string c = TrimSpaces(cond);
    if (c.empty()) return false;

    // 支持的运算符按长度匹配：>= <= != > < =
    const std::pair<std::string, CmpOp> ops[] = {
        {">=", CmpOp::GE}, {"<=", CmpOp::LE}, {"!=", CmpOp::NE},
        {">", CmpOp::GT}, {"<", CmpOp::LT}, {"=", CmpOp::EQ}
    };

    size_t pos_op = std::string::npos;
    CmpOp found = CmpOp::EQ;
    std::string op_text;
    for (const auto& kv : ops) {
        size_t p = c.find(kv.first);
        if (p != std::string::npos) {
            pos_op = p; found = kv.second; op_text = kv.first; break;
        }
    }
    if (pos_op == std::string::npos) return false;

    col = TrimSpaces(c.substr(0, pos_op));
    std::string rhs = TrimSpaces(c.substr(pos_op + op_text.size()));
    if (col.empty() || rhs.empty()) return false;

    // 解析字面量：字符串（'...'或"..."）、bool、int
    is_string = false; is_bool = false; literal.clear();
    if ((rhs.front() == '\'' && rhs.back() == '\'') || (rhs.front() == '"' && rhs.back() == '"')) {
        is_string = true;
        literal = rhs.substr(1, rhs.size() - 2);
    } else {
        std::string l = rhs;
        for (auto& ch : l) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        if (l == "true" || l == "false") {
            is_bool = true; bool_value = (l == "true");
        } else {
            // 尝试解析为int
            try {
                size_t idx = 0; long long v = std::stoll(rhs, &idx, 10);
                if (idx != rhs.size()) return false;
                int_value = static_cast<int>(v);
            } catch (...) { return false; }
        }
    }
    op = found;
    return true;
}

static int FindColumnIndexByName(const TableSchema& schema, const std::string& name) {
    for (size_t i = 0; i < schema.columns_.size(); ++i) {
        if (schema.columns_[i].column_name_ == name) return static_cast<int>(i);
    }
    return -1;
}

static int StringCompare(const std::string& a, const std::string& b) {
    if (a < b) return -1; if (a > b) return 1; return 0;
}

static bool CompareValues(const Value& lhs, DataType type, CmpOp op,
                          const std::string& lit, bool is_string,
                          bool is_bool, int int_value, bool bool_value) {
    switch (type) {
        case DataType::Int: {
            if (!std::holds_alternative<int>(lhs)) return false;
            int lv = std::get<int>(lhs);
            int rv = int_value;
            switch (op) {
                case CmpOp::EQ: return lv == rv;
                case CmpOp::NE: return lv != rv;
                case CmpOp::LT: return lv < rv;
                case CmpOp::LE: return lv <= rv;
                case CmpOp::GT: return lv > rv;
                case CmpOp::GE: return lv >= rv;
            }
            return false;
        }
        case DataType::Varchar: {
            if (!std::holds_alternative<std::string>(lhs) || !is_string) return false;
            int cmp = StringCompare(std::get<std::string>(lhs), lit);
            switch (op) {
                case CmpOp::EQ: return cmp == 0;
                case CmpOp::NE: return cmp != 0;
                case CmpOp::LT: return cmp < 0;
                case CmpOp::LE: return cmp <= 0;
                case CmpOp::GT: return cmp > 0;
                case CmpOp::GE: return cmp >= 0;
            }
            return false;
        }
        case DataType::Bool: {
            if (!std::holds_alternative<bool>(lhs) || !is_bool) return false;
            bool lv = std::get<bool>(lhs);
            bool rv = bool_value;
            switch (op) {
                case CmpOp::EQ: return lv == rv;
                case CmpOp::NE: return lv != rv;
                case CmpOp::LT: return static_cast<int>(lv) < static_cast<int>(rv);
                case CmpOp::LE: return static_cast<int>(lv) <= static_cast<int>(rv);
                case CmpOp::GT: return static_cast<int>(lv) > static_cast<int>(rv);
                case CmpOp::GE: return static_cast<int>(lv) >= static_cast<int>(rv);
            }
            return false;
        }
    }
    return false;
}

static void WriteInt(char* dst, int value) {
    std::memcpy(dst, &value, sizeof(int));
}

static int ReadInt(const char* src) {
    int v;
    std::memcpy(&v, src, sizeof(int));
    return v;
}

TableManager::TableManager(BufferPoolManager *bpm): buffer_pool_manager_(bpm) {
    // 创建表结构管理器
    schema_manager_ = new TableSchemaManager(bpm, this);
    
    // 延迟加载所有表结构到内存，避免循环依赖
    // LoadAllTableSchemas(); // 暂时注释掉，在第一次使用时加载
}

TableManager::~TableManager() {
    // 保存所有表结构到磁盘
    SaveAllTableSchemas();
    
    // 删除表结构管理器
    delete schema_manager_;
}

// ========= 表管理 ===========
bool TableManager::CreateTable(const TableSchema &schema) {
    if (table_schemas_.count(schema.table_name_)) return false;

    // 先更新内存状态
    table_schemas_.emplace(schema.table_name_, schema);
    
    // 持久化表结构（系统目录表本身除外，避免递归创建）
    if (schema.table_name_ != TableSchemaManager::SystemCatalogTableName()) {
        if (!schema_manager_->SaveTableSchema(schema)) {
            // 持久化失败，回滚内存状态
            table_schemas_.erase(schema.table_name_);
            return false;
        }
    }

    // 分配页
    int first_page_id = AllocatePageForTable(schema.table_name_);
    if (first_page_id < 0) {
        // 分配失败，回滚内存状态和持久化状态
        table_schemas_.erase(schema.table_name_);
        schema_manager_->DeleteTableSchema(schema.table_name_);
        return false;
    }

    // 初始化页头
    Header header;
    header.record_count = 0;
    header.free_space_offset = Header::HEADER_SIZE;
    // 对表名做哈希，并清除最高位
    header.table_id = static_cast<int>(std::hash<std::string>{}(schema.table_name_) & 0x7fffffff);

    if (!WritePageHeader(first_page_id, header)) {
        // 页头写入失败，回滚
        DeallocatePageForTable(schema.table_name_, first_page_id);
        table_schemas_.erase(schema.table_name_);
        schema_manager_->DeleteTableSchema(schema.table_name_);
        return false;
    }

    return true;
}

bool TableManager::DropTable(const std::string& table_name) {
    if (!table_schemas_.count(table_name)) return false;

    // 删除所有页
    for (int page_id : table_pages[table_name]) {
        buffer_pool_manager_->DeletePage(page_id);
    }
    table_pages.erase(table_name);
    table_schemas_.erase(table_name);
    
    // 从持久化存储中删除表结构
    schema_manager_->DeleteTableSchema(table_name);
    
    return true;
}

bool TableManager::TableExists(const std::string& table_name) {
    // 仅检查内存中是否存在，避免与 TableSchemaManager 相互调用导致递归
    return table_schemas_.count(table_name) > 0;
}

TableSchema * TableManager::GetTableSchema(const std::string &table_name) {
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return nullptr;
    return &(it->second);
}

// ========= 对外记录接口 ===========
bool TableManager::InsertRecord(const std::string &table_name, const Record &record) {
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return false;   // 存在性检查
    const TableSchema &schema = it->second;

    // 找到可写页和偏移
    int target_page = -1;
    int offset = -1;

    auto& pages = table_pages[table_name];
    for (int page_id : pages) {
        int off = FindFreeSlotInPage(page_id, schema);
        if (off >= 0) {
            target_page = page_id;
            offset = off;
            break;
        }
    }

    // 如果没有空间，则分配新页并初始化页头
    if (target_page < 0) {
        int page_id = AllocatePageForTable(table_name);
        if (page_id < 0) return false;

        // 创建并写入页头
        Header header;
        header.record_count = 0;
        header.free_space_offset = Header::HEADER_SIZE;
        header.table_id = static_cast<int>(std::hash<std::string>{}(table_name) & 0x7fffffff);
        if (!WritePageHeader(page_id, header)) return false;

        target_page = page_id;
        offset = header.free_space_offset;
    }

    // 写入记录
    if (!WriteRecordToPage(target_page, offset, record, schema)) return false;

    // 更新页头
    Header header;
    if (!ReadPageHeader(target_page, header)) return false;

    header.record_count++;
    header.free_space_offset += RecordSerializer::CalculateRecordSize(schema);
    return WritePageHeader(target_page, header);
}

// 软删除
bool TableManager::DeleteRecord(const std::string &table_name, int record_id) {
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return false;
    const TableSchema &schema = it->second;
    const int record_size = RecordSerializer::CalculateRecordSize(schema);

    auto& pages = table_pages[table_name];
    int base = 0;
    for (int page_id : pages) {
        Header header;
        if (!ReadPageHeader(page_id, header)) return false;
        if (record_id < base + header.record_count) {   // 在该页内
            int slot_index = record_id - base;  // 页内位置
            int offset = Header::HEADER_SIZE + slot_index * record_size;

            // 读出 标记删除 写回
            Record rec;
            if (!ReadRecordFromPage(page_id, offset, rec, schema)) return false;
            rec.is_deleted_ = true;
            return WriteRecordToPage(page_id, offset, rec, schema);
        }
        // 在下一页中寻找
        base += header.record_count;
    }
    // 未找到
    return false;
}

std::vector<Record> TableManager::SelectRecords(const std::string &table_name) {
    std::vector<Record> out;
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return out;
    const TableSchema &schema = it->second;

    const int record_size = RecordSerializer::CalculateRecordSize(schema);

    for (int page_id : table_pages[table_name]) {
        Header header;
        if (!ReadPageHeader(page_id, header)) continue;

        for (int i = 0; i< header.record_count; i++) {
            int offset = Header::HEADER_SIZE + i * record_size;
            Record rec;
            if (!ReadRecordFromPage(page_id, offset ,rec, schema)) continue;
            if (!rec.is_deleted_) out.push_back(rec);   // 检查删除标记
        }
    }
    return out;
}

std::vector<Record> TableManager::SelectRecordsWithCondition(const std::string &table_name,
    const std::string &condition) {
    std::vector<Record> out;

    // 空条件返回全部
    std::string cond = TrimSpaces(condition);
    if (cond.empty()) return SelectRecords(table_name);

    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return out;
    const TableSchema &schema = it->second;

    // 解析条件
    std::string col; CmpOp op; std::string lit; bool is_str = false; bool is_b = false; int iv = 0; bool bv = false;
    if (!ParseCondition(cond, col, op, lit, is_str, is_b, iv, bv)) return out;

    int col_idx = FindColumnIndexByName(schema, col);
    if (col_idx < 0) return out;

    const DataType col_type = schema.columns_[static_cast<size_t>(col_idx)].type_;
    const int record_size = RecordSerializer::CalculateRecordSize(schema);

    for (int page_id : table_pages[table_name]) {
        Header header;
        if (!ReadPageHeader(page_id, header)) continue;

        for (int i = 0; i < header.record_count; i++) {
            int offset = Header::HEADER_SIZE + i * record_size;
            Record rec;
            if (!ReadRecordFromPage(page_id, offset, rec, schema)) continue;
            if (rec.is_deleted_) continue;

            if (static_cast<size_t>(col_idx) >= rec.values_.size()) continue;
            const Value& v = rec.values_[static_cast<size_t>(col_idx)];
            if (CompareValues(v, col_type, op, lit, is_str, is_b, iv, bv)) {
                out.push_back(rec);
            }
        }
    }
    return out;
}

// ========= 页管理 ===========
int TableManager::AllocatePageForTable(const std::string &table_name) {
    if (!table_schemas_.count(table_name)) return -1;

    int page_id;
    Page* page = buffer_pool_manager_->NewPage(&page_id);
    if (page == nullptr) return -1;

    // 新页清0并写回
    std::memset(page->GetData(), 0, PAGE_SIZE);
    page->SetDirty(true);
    buffer_pool_manager_->UnpinPage(page_id);

    table_pages[table_name].push_back(page_id);
    return page_id;
}

bool TableManager::DeallocatePageForTable(const std::string &table_name, int page_id) {
    if (!table_schemas_.count(table_name)) return false;

    auto& vec = table_pages[table_name];
    vec.erase(std::remove(vec.begin(), vec.end(), page_id), vec.end());
    return buffer_pool_manager_->DeletePage(page_id);
}

int TableManager::GetRecordCount(const std::string &table_name) {
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return -1;
    const TableSchema &schema = it->second;
    const int record_size = RecordSerializer::CalculateRecordSize(schema);

    int total = 0;
    for (int page_id : table_pages[table_name]) {
        Header header;
        if (!ReadPageHeader(page_id, header)) continue;
        for (int i = 0; i< header.record_count; i++) {
            int offset = Header::HEADER_SIZE + i * record_size;
            Record rec;
            if (!ReadRecordFromPage(page_id, offset, rec, schema)) continue;
            if (!rec.is_deleted_) total++;
        }
    }
    return total;
}

int TableManager::GetPageCount(const std::string &table_name) {
    if (!table_schemas_.count(table_name)) return -1;
    return static_cast<int>(table_pages[table_name].size());
}

// ========= 统计/调试 ===========
void TableManager::PrintTableInfo(const std::string &table_name) {
    if (!table_schemas_.count(table_name)) {
        std::cout << "[TableManager] table not found: " << table_name << std::endl;
        return;
    }

    const auto& schema = table_schemas_[table_name];
    std::cout << "Table: " << schema.table_name_ << ", columns=" << schema.columns_.size()
              << ", pages=" << GetPageCount(table_name)
              << ", records=" << GetRecordCount(table_name) << std::endl;
}

void TableManager::PrintAllTables() {
    for (auto& [name, _] : table_schemas_) {
        PrintTableInfo(name);
    }
}

// TODO：表的持久化实现
bool TableManager::WriteTableSchemaToPage(const TableSchema &schema, int page_id) {
    // 暂时返回true，避免编译错误
    return true;
}

bool TableManager::ReadTableSchemaFromPage(TableSchema &schema, int page_id) {
    // 暂时返回true，避免编译错误
    return true;
}

// ========= 记录存取 ===========
int TableManager::FindFreeSlotInPage(int page_id, const TableSchema &schema) {
    Header header;
    if (!ReadPageHeader(page_id, header)) return -1;

    // 考虑当前页面塞不塞的下新的记录
    const int record_size = RecordSerializer::CalculateRecordSize(schema);
    if (header.free_space_offset + record_size > PAGE_SIZE) return -1;

    return header.free_space_offset;
}

bool TableManager::WriteRecordToPage(int page_id, int offset, const Record &record, const TableSchema &schema) {
    Page* page = buffer_pool_manager_->FetchPage(page_id);
    if (page == nullptr) return false;

    // 序列化后进行存储
    char* data = page->GetData();
    bool ok = RecordSerializer::SerializeRecord(record, schema, data, offset);
    if (ok) page->SetDirty(true);

    buffer_pool_manager_->UnpinPage(page_id);
    return ok;
}

bool TableManager::ReadRecordFromPage(int page_id, int offset, Record &record, const TableSchema &schema) {
    Page* page = buffer_pool_manager_->FetchPage(page_id);
    if (page == nullptr) return false;

    // 反序列化，进行解析
    const char* data = page->GetData();
    bool ok = RecordSerializer::DeserializeRecord(record, schema, data, offset);

    buffer_pool_manager_->UnpinPage(page_id);
    return ok;
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
    buffer_pool_manager_->UnpinPage(page_id);
    return true;
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

