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
    std::cout << "TableManager: 开始初始化..." << std::endl;
    
    // 创建表结构管理器
    std::cout << "TableManager: 创建表结构管理器..." << std::endl;
    schema_manager_ = new TableSchemaManager(bpm, this);
    std::cout << "TableManager: 表结构管理器创建完成" << std::endl;
    
    // 创建索引管理器
    std::cout << "TableManager: 创建索引管理器..." << std::endl;
    index_manager_ = std::make_unique<IndexManager>(bpm);
    std::cout << "TableManager: 索引管理器创建完成" << std::endl;
    
    // 延迟加载所有表结构到内存，避免循环依赖
    // 加载根目录（页0）并恢复表首页映射，再加载所有表结构
    std::cout << "TableManager: 加载根目录..." << std::endl;
    LoadRootDirectory();
    std::cout << "TableManager: 根目录加载完成" << std::endl;
    
    // 暂时注释掉LoadAllTableSchemas，避免循环依赖
    // std::cout << "TableManager: 加载所有表结构..." << std::endl;
    // LoadAllTableSchemas();
    std::cout << "TableManager: 初始化完成" << std::endl;
}

TableManager::~TableManager() {
    // 保存所有表结构到磁盘
    SaveAllTableSchemas();
    // 保存根目录
    SaveRootDirectory();
    
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

    // 为主键自动创建索引
    if (schema.primary_key_index_ >= 0 && schema.primary_key_index_ < static_cast<int>(schema.columns_.size())) {
        const std::string& pk_column_name = schema.columns_[schema.primary_key_index_].column_name_;
        if (index_manager_->CreateIndex(schema.table_name_, pk_column_name)) {
            std::cout << "TableManager: 为主键列 " << pk_column_name << " 创建索引成功" << std::endl;
        } else {
            std::cout << "TableManager: 为主键列 " << pk_column_name << " 创建索引失败" << std::endl;
        }
    }

    // 更新根目录：记录表的第一页
    SaveRootDirectory();
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
    // 更新根目录
    SaveRootDirectory();
    
    return true;
}

// ========== 根目录持久化（页0）：表名长度+表名+第一页id，顺序存储 ==========
bool TableManager::LoadRootDirectory() {
    // 页0如不存在则视为空
    Page* page0 = buffer_pool_manager_->FetchPage(0);
    if (page0 == nullptr) {
        return true;
    }
    const char* data = page0->GetData();
    int offset = 0;
    table_pages.clear();
    while (offset + 8 <= PAGE_SIZE) {
        int name_len;
        std::memcpy(&name_len, data + offset, sizeof(int));
        if (name_len <= 0 || name_len > PAGE_SIZE - offset - 8) break;
        offset += 4;
        if (offset + name_len + 4 > PAGE_SIZE) break;
        std::string name(data + offset, name_len);
        offset += name_len;
        int first_page;
        std::memcpy(&first_page, data + offset, sizeof(int));
        offset += 4;
        if (first_page > 0) {
            table_pages[name] = std::vector<int>{ first_page };
        }
    }
    buffer_pool_manager_->UnpinPage(0);
    return true;
}

bool TableManager::SaveRootDirectory() {
    // 确保页0存在
    Page* page0 = buffer_pool_manager_->FetchPage(0);
    if (page0 == nullptr) {
        int pid;
        page0 = buffer_pool_manager_->NewPage(&pid);
        if (page0 == nullptr || pid != 0) {
            // 若无法分配到页0（例如已有文件排布），则放弃根页写入
            if (page0) buffer_pool_manager_->UnpinPage(pid);
            return false;
        }
    }
    char* data = page0->GetData();
    std::memset(data, 0, PAGE_SIZE);
    int offset = 0;
    for (const auto& kv : table_pages) {
        const std::string& name = kv.first;
        int first_page = kv.second.empty() ? -1 : kv.second.front();
        int name_len = static_cast<int>(name.size());
        if (name_len <= 0) continue;
        if (offset + 4 + name_len + 4 > PAGE_SIZE) break;
        std::memcpy(data + offset, &name_len, sizeof(int)); offset += 4;
        std::memcpy(data + offset, name.data(), name_len); offset += name_len;
        std::memcpy(data + offset, &first_page, sizeof(int)); offset += 4;
    }
    page0->SetDirty(true);
    buffer_pool_manager_->UnpinPage(0);
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

    // 检查主键唯一性
    if (!CheckPrimaryKeyUnique(table_name, record)) {
        return false; // 主键重复
    }

    // 找到可写页和偏移
    int target_page = -1;
    int offset = -1;

    // 检查表是否有页
    auto pages_it = table_pages.find(table_name);
    if (pages_it != table_pages.end()) {
        for (int page_id : pages_it->second) {
        int off = FindFreeSlotInPage(page_id, schema);
        if (off >= 0) {
            target_page = page_id;
            offset = off;
            break;
            }
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
    if (!WritePageHeader(target_page, header)) return false;
    
    // 计算当前表的记录ID（只计算当前表的记录）
    int record_id = 0;
    auto pages_it2 = table_pages.find(table_name);
    if (pages_it2 != table_pages.end()) {
        for (int page_id : pages_it2->second) {
            Header page_header;
            if (ReadPageHeader(page_id, page_header)) {
                // 检查页是否属于当前表
                int expected_table_id = static_cast<int>(std::hash<std::string>{}(table_name) & 0x7fffffff);
                if (page_header.table_id == expected_table_id) {
                    record_id += page_header.record_count;
                }
            }
        }
    }
    record_id--; // 减去刚刚添加的记录
    
    // 调试：只在记录ID异常时打印
    if (record_id != header.record_count - 1) {
        std::cout << "DEBUG: Record ID mismatch! Calculated=" << record_id << ", Expected=" << (header.record_count - 1) << " for table=" << table_name << std::endl;
    }
    
    // 只在记录ID异常时打印
    if (record_id > 100) {
        std::cout << "DEBUG: Calculated record_id=" << record_id << " for table=" << table_name << std::endl;
    }
    
    // 更新索引
    if (index_manager_) {
        
        // 只在记录ID异常时打印
        if (record_id > 100) {
            std::cout << "DEBUG: Updating index for record_id=" << record_id << ", primary_key_index=" << schema.primary_key_index_ << std::endl;
        }
        
        // 为主键列更新索引
        if (schema.primary_key_index_ >= 0 && schema.primary_key_index_ < static_cast<int>(record.values_.size())) {
            const Value& key_value = record.values_[schema.primary_key_index_];
            const std::string& pk_column_name = schema.columns_[schema.primary_key_index_].column_name_;
            
            // 安全地打印键值
            std::cout << "DEBUG: Inserting into index: table=" << table_name << ", column=" << pk_column_name << ", key=";
            if (std::holds_alternative<int>(key_value)) {
                std::cout << std::get<int>(key_value);
            } else if (std::holds_alternative<std::string>(key_value)) {
                std::cout << std::get<std::string>(key_value);
            } else if (std::holds_alternative<bool>(key_value)) {
                std::cout << (std::get<bool>(key_value) ? "true" : "false");
            } else {
                std::cout << "unknown type";
            }
            std::cout << ", record_id=" << record_id << std::endl;
            
            bool success = index_manager_->InsertRecord(table_name, record, record_id);
            std::cout << "DEBUG: Index insert result: " << (success ? "success" : "failed") << std::endl;
            
            if (!success) {
                std::cout << "DEBUG: Index insert failed for record " << record_id << std::endl;
            }
        } else {
            std::cout << "DEBUG: No primary key index or invalid index" << std::endl;
        }
    } else {
        std::cout << "DEBUG: No index manager available" << std::endl;
    }
    
    return true;
}

// 软删除
bool TableManager::DeleteRecord(const std::string &table_name, int record_id) {
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return false;
    const TableSchema &schema = it->second;
    const int record_size = RecordSerializer::CalculateRecordSize(schema);

    // 检查表是否有页
    auto pages_it = table_pages.find(table_name);
    if (pages_it == table_pages.end()) return false;
    
    int base = 0;
    for (int page_id : pages_it->second) {
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

    // 检查表是否有页
    auto pages_it = table_pages.find(table_name);
    if (pages_it == table_pages.end()) return out;
    
    for (int page_id : pages_it->second) {
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

    // 检查表是否有页
    auto pages_it = table_pages.find(table_name);
    if (pages_it == table_pages.end()) return out;
    
    for (int page_id : pages_it->second) {
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

// ========= 更新 =========
bool TableManager::UpdateRecord(const std::string &table_name, int record_id, const Record &new_record) {
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return false;
    const TableSchema &schema = it->second;
    
    // 检查主键唯一性（排除当前记录）
    if (!CheckPrimaryKeyUnique(table_name, new_record, record_id)) {
        return false; // 主键重复
    }
    
    const int record_size = RecordSerializer::CalculateRecordSize(schema);

    // 检查表是否有页
    auto pages_it = table_pages.find(table_name);
    if (pages_it == table_pages.end()) return false;
    
    int base = 0;
    for (int page_id : pages_it->second) {
        Header header;
        if (!ReadPageHeader(page_id, header)) return false;
        if (record_id < base + header.record_count) {
            int slot_index = record_id - base;
            int offset = Header::HEADER_SIZE + slot_index * record_size;
            // 直接覆盖写入新记录
            return WriteRecordToPage(page_id, offset, new_record, schema);
        }
        base += header.record_count;
    }
    return false;
}

int TableManager::UpdateRecordsWithCondition(const std::string &table_name, const std::string &condition, const Record &new_record) {
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return 0;
    const TableSchema &schema = it->second;

    // 空条件：更新全部
    std::string cond = TrimSpaces(condition);
    bool update_all = cond.empty();

    // 准备条件解析
    std::string col; CmpOp op; std::string lit; bool is_str=false, is_b=false; int iv=0; bool bv=false;
    int col_idx = -1; DataType col_type = DataType::Int;
    if (!update_all) {
        if (!ParseCondition(cond, col, op, lit, is_str, is_b, iv, bv)) return 0;
        col_idx = FindColumnIndexByName(schema, col);
        if (col_idx < 0) return 0;
        col_type = schema.columns_[static_cast<size_t>(col_idx)].type_;
    }

    const int record_size = RecordSerializer::CalculateRecordSize(schema);
    int current_record_id = 0;
    
    // 检查表是否有页
    auto pages_it = table_pages.find(table_name);
    if (pages_it == table_pages.end()) return 0;
    
    // 第一遍：检查所有匹配的记录是否都能成功更新（主键唯一性检查）
    for (int page_id : pages_it->second) {
        Header header;
        if (!ReadPageHeader(page_id, header)) continue;
        
        // 检查页是否属于正确的表
        int expected_table_id = static_cast<int>(std::hash<std::string>{}(table_name) & 0x7fffffff);
        if (header.table_id != expected_table_id) continue;
        
        for (int i = 0; i < header.record_count; i++) {
            int offset = Header::HEADER_SIZE + i * record_size;
            Record rec;
            if (!ReadRecordFromPage(page_id, offset, rec, schema)) {
                current_record_id++;
                continue;
            }
            if (rec.is_deleted_) {
                current_record_id++;
                continue;
            }

            bool matched = update_all;
            if (!update_all) {
                if (static_cast<size_t>(col_idx) < rec.values_.size()) {
                    const Value &v = rec.values_[static_cast<size_t>(col_idx)];
                    matched = CompareValues(v, col_type, op, lit, is_str, is_b, iv, bv);
                }
            }
            if (matched) {
                // 检查主键唯一性（排除当前记录）
                if (!CheckPrimaryKeyUnique(table_name, new_record, current_record_id)) {
                    // 如果任何一个匹配的记录不能更新，则整个批量更新失败
                    return 0;
                }
            }
            current_record_id++;
        }
    }
    
    // 第二遍：执行实际更新
    int updated = 0;
    current_record_id = 0;
    
    for (int page_id : pages_it->second) {
        Header header;
        if (!ReadPageHeader(page_id, header)) continue;
        
        // 检查页是否属于正确的表
        int expected_table_id = static_cast<int>(std::hash<std::string>{}(table_name) & 0x7fffffff);
        if (header.table_id != expected_table_id) continue;
        
        for (int i = 0; i < header.record_count; i++) {
            int offset = Header::HEADER_SIZE + i * record_size;
            Record rec;
            if (!ReadRecordFromPage(page_id, offset, rec, schema)) {
                current_record_id++;
                continue;
            }
            if (rec.is_deleted_) {
                current_record_id++;
                continue;
            }

            bool matched = update_all;
            if (!update_all) {
                if (static_cast<size_t>(col_idx) < rec.values_.size()) {
                    const Value &v = rec.values_[static_cast<size_t>(col_idx)];
                    matched = CompareValues(v, col_type, op, lit, is_str, is_b, iv, bv);
                }
            }
            if (matched) {
                // 执行更新
                if (WriteRecordToPage(page_id, offset, new_record, schema)) updated++;
            }
            current_record_id++;
        }
    }
    return updated;
}

int TableManager::DeleteRecordsWithCondition(const std::string &table_name, const std::string &condition) {
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return 0;
    const TableSchema &schema = it->second;

    std::string cond = TrimSpaces(condition);
    bool delete_all = cond.empty();

    std::string col; CmpOp op; std::string lit; bool is_str=false, is_b=false; int iv=0; bool bv=false;
    int col_idx = -1; DataType col_type = DataType::Int;
    if (!delete_all) {
        if (!ParseCondition(cond, col, op, lit, is_str, is_b, iv, bv)) return 0;
        col_idx = FindColumnIndexByName(schema, col);
        if (col_idx < 0) return 0;
        col_type = schema.columns_[static_cast<size_t>(col_idx)].type_;
    }

    const int record_size = RecordSerializer::CalculateRecordSize(schema);
    int deleted = 0;
    
    // 检查表是否有页
    auto pages_it = table_pages.find(table_name);
    if (pages_it == table_pages.end()) return 0;
    
    for (int page_id : pages_it->second) {
        Header header;
        if (!ReadPageHeader(page_id, header)) continue;
        for (int i = 0; i < header.record_count; i++) {
            int offset = Header::HEADER_SIZE + i * record_size;
            Record rec;
            if (!ReadRecordFromPage(page_id, offset, rec, schema)) continue;
            if (rec.is_deleted_) continue;

            bool matched = delete_all;
            if (!delete_all) {
                if (static_cast<size_t>(col_idx) < rec.values_.size()) {
                    const Value &v = rec.values_[static_cast<size_t>(col_idx)];
                    matched = CompareValues(v, col_type, op, lit, is_str, is_b, iv, bv);
                }
            }
            if (matched) {
                rec.is_deleted_ = true;
                if (WriteRecordToPage(page_id, offset, rec, schema)) deleted++;
            }
        }
    }
    return deleted;
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
    
    // 检查表是否有页
    auto pages_it = table_pages.find(table_name);
    if (pages_it == table_pages.end()) return 0;
    
    for (int page_id : pages_it->second) {
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
    
    auto pages_it = table_pages.find(table_name);
    if (pages_it == table_pages.end()) return 0;
    
    return static_cast<int>(pages_it->second.size());
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

std::vector<std::string> TableManager::ListTableNames() const {
    std::vector<std::string> names;
    names.reserve(table_schemas_.size());
    for (const auto& kv : table_schemas_) names.push_back(kv.first);
    return names;
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

// ========= 投影操作 =========
std::vector<Record> TableManager::SelectColumns(const std::string& table_name, 
                                               const std::vector<std::string>& column_names) {
    std::vector<Record> out;
    
    // 验证表是否存在
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return out;
    
    // 验证列名
    std::vector<std::string> invalid_columns = ValidateColumns(table_name, column_names);
    if (!invalid_columns.empty()) return out;
    
    // 获取所有记录
    std::vector<Record> all_records = SelectRecords(table_name);
    
    // 执行投影
    out = ProjectRecords(all_records, table_name, column_names);
    
    return out;
}

std::vector<Record> TableManager::SelectColumnsWithCondition(const std::string& table_name,
                                                           const std::vector<std::string>& column_names,
                                                           const std::string& condition) {
    std::vector<Record> out;
    
    // 验证表是否存在
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return out;
    
    // 验证列名
    std::vector<std::string> invalid_columns = ValidateColumns(table_name, column_names);
    if (!invalid_columns.empty()) return out;
    
    // 获取过滤后的记录
    std::vector<Record> filtered_records;
    if (condition.empty()) {
        filtered_records = SelectRecords(table_name);
    } else {
        filtered_records = SelectRecordsWithCondition(table_name, condition);
    }
    
    // 执行投影
    out = ProjectRecords(filtered_records, table_name, column_names);
    
    return out;
}

std::vector<Record> TableManager::ProjectRecords(const std::vector<Record>& records,
                                                const std::string& table_name,
                                                const std::vector<std::string>& column_names) {
    std::vector<Record> out;
    
    if (column_names.empty()) return out;
    
    // 获取列索引映射
    std::vector<int> column_indices = GetColumnIndices(table_name, column_names);
    
    // 对每条记录进行投影
    for (const Record& record : records) {
        Record projected_record;
        projected_record.is_deleted_ = record.is_deleted_;
        
        for (int col_idx : column_indices) {
            if (col_idx >= 0 && col_idx < static_cast<int>(record.values_.size())) {
                projected_record.AddValue(record.GetValue(col_idx));
            }
        }
        
        out.push_back(projected_record);
    }
    
    return out;
}

std::vector<int> TableManager::GetColumnIndices(const std::string& table_name, 
                                               const std::vector<std::string>& column_names) {
    std::vector<int> out;
    
    TableSchema* schema = GetTableSchema(table_name);
    if (!schema) return out;
    
    for (const std::string& col_name : column_names) {
        int col_idx = schema->GetColumnIndex(col_name);
        if (col_idx >= 0) {
            out.push_back(col_idx);
        }
    }
    
    return out;
}

std::vector<std::string> TableManager::ValidateColumns(const std::string& table_name,
                                                      const std::vector<std::string>& column_names) {
    std::vector<std::string> out;
    
    TableSchema* schema = GetTableSchema(table_name);
    if (!schema) return column_names;
    
    for (const std::string& col_name : column_names) {
        bool found = false;
        for (const auto& column : schema->columns_) {
            if (column.column_name_ == col_name) {
                found = true;
                break;
            }
        }
        if (!found) {
            out.push_back(col_name);
        }
    }
    
    return out;
}

// ========= 主键唯一性检查 =========
bool TableManager::CheckPrimaryKeyUnique(const std::string& table_name, const Record& record, int exclude_record_id) {
    TableSchema* schema = GetTableSchema(table_name);
    if (!schema || schema->primary_key_index_ < 0) {
        std::cout << "DEBUG: No primary key, returning true" << std::endl;
        return true; // 无主键，总是唯一
    }
    
    int primary_key_index = schema->primary_key_index_;
    if (primary_key_index >= static_cast<int>(record.values_.size())) {
        std::cout << "DEBUG: Primary key index out of range" << std::endl;
        return false; // 主键列不存在
    }
    
    const Value& key_value = record.values_[primary_key_index];
    bool exists = IsPrimaryKeyValueExists(table_name, primary_key_index, key_value, exclude_record_id);
    std::cout << "DEBUG: Primary key exists: " << exists << ", returning: " << !exists << std::endl;
    return !exists;
}

bool TableManager::IsPrimaryKeyValueExists(const std::string& table_name, int primary_key_index, const Value& key_value, int exclude_record_id) {
    TableSchema* schema = GetTableSchema(table_name);
    if (!schema) {
        std::cout << "DEBUG: Schema not found" << std::endl;
        return false;
    }
    
    // 首先尝试使用索引查找
    if (index_manager_ && schema->primary_key_index_ == primary_key_index) {
        const std::string& pk_column_name = schema->columns_[primary_key_index].column_name_;
        if (index_manager_->HasIndex(table_name, pk_column_name)) {
            std::cout << "DEBUG: Using index for primary key check" << std::endl;
            std::vector<int> record_ids = index_manager_->QueryWithIndex(table_name, pk_column_name, key_value);
            
            // 如果有结果且不是要排除的记录，则主键重复
            for (int record_id : record_ids) {
                if (record_id != exclude_record_id) {
                    std::cout << "DEBUG: Found duplicate primary key via index!" << std::endl;
                    return true;
                }
            }
            return false; // 主键唯一
        }
    }
    
    // 回退到全表扫描
    std::cout << "DEBUG: Falling back to full table scan for primary key check" << std::endl;
    
    // 检查表是否有页
    auto it = table_pages.find(table_name);
    if (it == table_pages.end() || it->second.empty()) {
        std::cout << "DEBUG: Table has no pages, key is unique" << std::endl;
        return false; // 表没有页，主键值唯一
    }
    
    std::cout << "DEBUG: Checking " << it->second.size() << " pages for primary key" << std::endl;
    
    const int record_size = RecordSerializer::CalculateRecordSize(*schema);
    int current_record_id = 0;
    
    for (int page_id : it->second) {
        Header header;
        if (!ReadPageHeader(page_id, header)) continue;
        
        // 检查页是否属于正确的表
        int expected_table_id = static_cast<int>(std::hash<std::string>{}(table_name) & 0x7fffffff);
        if (header.table_id != expected_table_id) {
            std::cout << "DEBUG: Skipping page " << page_id << " (table_id: " << header.table_id << ", expected: " << expected_table_id << ")" << std::endl;
            continue;
        }
        
        std::cout << "DEBUG: Page " << page_id << " has " << header.record_count << " records" << std::endl;
        
        for (int i = 0; i < header.record_count; i++) {
            // 跳过要排除的记录（用于更新操作）
            if (current_record_id == exclude_record_id) {
                current_record_id++;
                continue;
            }
            
            int offset = Header::HEADER_SIZE + i * record_size;
            Record existing_record;
            if (!ReadRecordFromPage(page_id, offset, existing_record, *schema)) {
                current_record_id++;
                continue;
            }
            
            // 跳过已删除的记录
            if (existing_record.is_deleted_) {
                current_record_id++;
                continue;
            }
            
            // 检查主键值是否相同
            if (primary_key_index < static_cast<int>(existing_record.values_.size())) {
                const Value& existing_key_value = existing_record.values_[primary_key_index];
                std::cout << "DEBUG: Comparing existing key: ";
                if (std::holds_alternative<int>(existing_key_value)) {
                    std::cout << std::get<int>(existing_key_value);
                } else if (std::holds_alternative<std::string>(existing_key_value)) {
                    std::cout << std::get<std::string>(existing_key_value);
                } else if (std::holds_alternative<bool>(existing_key_value)) {
                    std::cout << (std::get<bool>(existing_key_value) ? "true" : "false");
                }
                std::cout << " with new key: ";
                if (std::holds_alternative<int>(key_value)) {
                    std::cout << std::get<int>(key_value);
                } else if (std::holds_alternative<std::string>(key_value)) {
                    std::cout << std::get<std::string>(key_value);
                } else if (std::holds_alternative<bool>(key_value)) {
                    std::cout << (std::get<bool>(key_value) ? "true" : "false");
                }
                std::cout << std::endl;
                
                if (existing_key_value == key_value) {
                    std::cout << "DEBUG: Found duplicate primary key!" << std::endl;
                    return true; // 找到重复的主键值
                }
            }
            
            current_record_id++;
        }
    }
    
    return false; // 主键值唯一
}

// ========== 索引管理方法 ==========

bool TableManager::CreateIndex(const std::string& table_name, const std::string& column_name) {
    if (!index_manager_) return false;
    return index_manager_->CreateIndex(table_name, column_name);
}

bool TableManager::DropIndex(const std::string& table_name, const std::string& column_name) {
    if (!index_manager_) return false;
    return index_manager_->DropIndex(table_name, column_name);
}

bool TableManager::HasIndex(const std::string& table_name, const std::string& column_name) const {
    if (!index_manager_) return false;
    return index_manager_->HasIndex(table_name, column_name);
}

std::vector<Record> TableManager::SelectRecordsWithIndex(const std::string& table_name, 
                                                        const std::string& column_name,
                                                        const Value& key) const {
    std::vector<Record> result;
    
    if (!index_manager_) {
        // 如果没有索引管理器，回退到全表扫描
        std::string condition;
        if (std::holds_alternative<int>(key)) {
            condition = column_name + " = " + std::to_string(std::get<int>(key));
        } else if (std::holds_alternative<std::string>(key)) {
            condition = column_name + " = '" + std::get<std::string>(key) + "'";
        } else if (std::holds_alternative<bool>(key)) {
            condition = column_name + " = " + (std::get<bool>(key) ? "true" : "false");
        } else {
            return result; // 不支持的类型
        }
        return const_cast<TableManager*>(this)->SelectRecordsWithCondition(table_name, condition);
    }
    
    // 使用索引查找记录ID
    std::vector<int> record_ids = index_manager_->QueryWithIndex(table_name, column_name, key);
    
    std::cout << "DEBUG: Index query returned " << record_ids.size() << " record IDs" << std::endl;
    
    // 根据记录ID获取实际记录
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return result;
    const TableSchema &schema = it->second;
    
    const int record_size = RecordSerializer::CalculateRecordSize(schema);
    int current_record_id = 0;
    
    auto pages_it = table_pages.find(table_name);
    if (pages_it == table_pages.end()) return result;
    
    for (int page_id : pages_it->second) {
        Header header;
        if (!const_cast<TableManager*>(this)->ReadPageHeader(page_id, header)) continue;
        
        int expected_table_id = static_cast<int>(std::hash<std::string>{}(table_name) & 0x7fffffff);
        if (header.table_id != expected_table_id) continue;
        
        for (int i = 0; i < header.record_count; i++) {
            int offset = Header::HEADER_SIZE + i * record_size;
            Record rec;
            if (!const_cast<TableManager*>(this)->ReadRecordFromPage(page_id, offset, rec, schema)) {
                current_record_id++;
                continue;
            }
            if (rec.is_deleted_) {
                current_record_id++;
                continue;
            }
            
            // 检查是否在索引结果中
            if (std::find(record_ids.begin(), record_ids.end(), current_record_id) != record_ids.end()) {
                std::cout << "DEBUG: Found record with ID " << current_record_id << std::endl;
                result.push_back(rec);
            }
            
            current_record_id++;
        }
    }
    
    std::cout << "DEBUG: Index query returned " << result.size() << " records" << std::endl;
    return result;
}

std::vector<Record> TableManager::SelectRecordsRangeWithIndex(const std::string& table_name,
                                                            const std::string& column_name,
                                                            const Value& start_key,
                                                            const Value& end_key) const {
    std::vector<Record> result;
    
    if (!index_manager_) {
        // 如果没有索引管理器，回退到全表扫描
        return const_cast<TableManager*>(this)->SelectRecordsWithCondition(table_name, column_name + " >= " + std::to_string(std::get<int>(start_key)) + 
                                         " AND " + column_name + " <= " + std::to_string(std::get<int>(end_key)));
    }
    
    // 使用索引查找记录ID范围
    std::vector<int> record_ids = index_manager_->QueryRangeWithIndex(table_name, column_name, start_key, end_key);
    
    // 根据记录ID获取实际记录
    auto it = table_schemas_.find(table_name);
    if (it == table_schemas_.end()) return result;
    const TableSchema &schema = it->second;
    
    const int record_size = RecordSerializer::CalculateRecordSize(schema);
    int current_record_id = 0;
    
    auto pages_it = table_pages.find(table_name);
    if (pages_it == table_pages.end()) return result;
    
    for (int page_id : pages_it->second) {
        Header header;
        if (!const_cast<TableManager*>(this)->ReadPageHeader(page_id, header)) continue;
        
        int expected_table_id = static_cast<int>(std::hash<std::string>{}(table_name) & 0x7fffffff);
        if (header.table_id != expected_table_id) continue;
        
        for (int i = 0; i < header.record_count; i++) {
            int offset = Header::HEADER_SIZE + i * record_size;
            Record rec;
            if (!const_cast<TableManager*>(this)->ReadRecordFromPage(page_id, offset, rec, schema)) {
                current_record_id++;
                continue;
            }
            if (rec.is_deleted_) {
                current_record_id++;
                continue;
            }
            
            // 检查是否在索引结果中
            if (std::find(record_ids.begin(), record_ids.end(), current_record_id) != record_ids.end()) {
                result.push_back(rec);
            }
            
            current_record_id++;
        }
    }
    
    return result;
}

