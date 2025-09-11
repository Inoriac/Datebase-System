//
// Created by Huang_cj on 2025/9/9.
//

#include "catalog/table_schema_manager.h"
#include "catalog/table_manager.h"
#include <iostream>
#include <sstream>

// 系统目录表名
const std::string TableSchemaManager::SYSTEM_CATALOG_TABLE_NAME = "__system_catalog__";

TableSchemaManager::TableSchemaManager(BufferPoolManager* bpm, TableManager* tm)
    : buffer_pool_manager_(bpm), table_manager_(tm) {
}

TableSchemaManager::~TableSchemaManager() {
    // 析构函数不需要特殊处理
}

bool TableSchemaManager::InitializeSystemCatalog() {
    // 检查系统目录表是否已存在
    if (table_manager_->TableExists(SYSTEM_CATALOG_TABLE_NAME)) {
        return true; // 已经存在，无需重新创建
    }
    
    // 创建系统目录表结构
    TableSchema catalog_schema = CreateSystemCatalogSchema();
    
    // 使用 TableManager 创建系统目录表
    if (!table_manager_->CreateTable(catalog_schema)) {
        return false;
    }
    
    return true;
}

bool TableSchemaManager::SaveTableSchema(const TableSchema& schema) {
    // 确保系统目录表已初始化
    if (!InitializeSystemCatalog()) {
        return false;
    }
    
    // 先删除旧的表结构记录（如果存在）
    DeleteTableSchema(schema.table_name_);
    
    // 序列化表结构
    std::string schema_data = SerializeSchema(schema);
    
    // 创建记录
    Record record;
    record.AddValue(Value(schema.table_name_));
    record.AddValue(Value(schema_data));
    
    // 使用 TableManager 插入记录到系统目录表
    return table_manager_->InsertRecord(SYSTEM_CATALOG_TABLE_NAME, record);
}

bool TableSchemaManager::LoadTableSchema(const std::string& table_name, TableSchema& schema) {
    // 确保系统目录表已初始化
    if (!InitializeSystemCatalog()) {
        return false;
    }
    
    // 使用 TableManager 查询系统目录表
    std::vector<Record> records = table_manager_->SelectRecords(SYSTEM_CATALOG_TABLE_NAME);
    
    // 查找指定表名的记录
    for (const Record& record : records) {
        if (record.values_.size() >= 2) {
            if (std::holds_alternative<std::string>(record.values_[0])) {
                std::string stored_table_name = std::get<std::string>(record.values_[0]);
                if (stored_table_name == table_name) {
                    // 找到匹配的表，反序列化表结构
                    if (std::holds_alternative<std::string>(record.values_[1])) {
                        std::string schema_data = std::get<std::string>(record.values_[1]);
                        return DeserializeSchema(schema_data, schema);
                    }
                }
            }
        }
    }
    
    return false; // 未找到表
}

bool TableSchemaManager::DeleteTableSchema(const std::string& table_name) {
    // 确保系统目录表已初始化
    if (!InitializeSystemCatalog()) {
        return false;
    }
    
    // 使用 TableManager 查询系统目录表
    std::vector<Record> records = table_manager_->SelectRecords(SYSTEM_CATALOG_TABLE_NAME);
    
    // 查找并删除指定表名的记录
    for (size_t i = 0; i < records.size(); ++i) {
        if (records[i].values_.size() >= 1) {
            if (std::holds_alternative<std::string>(records[i].values_[0])) {
                std::string stored_table_name = std::get<std::string>(records[i].values_[0]);
                if (stored_table_name == table_name) {
                    // 使用 TableManager 删除记录
                    return table_manager_->DeleteRecord(SYSTEM_CATALOG_TABLE_NAME, static_cast<int>(i));
                }
            }
        }
    }
    
    return false; // 未找到表
}

bool TableSchemaManager::LoadAllTableSchemas(std::unordered_map<std::string, TableSchema>& schemas) {
    // 清空输出参数
    schemas.clear();
    
    // 确保系统目录表已初始化
    if (!InitializeSystemCatalog()) {
        return false;
    }
    
    // 使用 TableManager 查询系统目录表
    std::vector<Record> records = table_manager_->SelectRecords(SYSTEM_CATALOG_TABLE_NAME);
    
    // 反序列化所有表结构
    for (const Record& record : records) {
        if (record.values_.size() >= 2) {
            if (std::holds_alternative<std::string>(record.values_[0]) &&
                std::holds_alternative<std::string>(record.values_[1])) {
                
                std::string table_name = std::get<std::string>(record.values_[0]);
                std::string schema_data = std::get<std::string>(record.values_[1]);
                
                TableSchema schema;
                if (DeserializeSchema(schema_data, schema)) {
                    schemas.emplace(table_name, schema);
                }
            }
        }
    }
    
    return true;
}

bool TableSchemaManager::IsSystemCatalogInitialized() const {
    return table_manager_->TableExists(SYSTEM_CATALOG_TABLE_NAME);
}

const std::string& TableSchemaManager::SystemCatalogTableName() {
    return SYSTEM_CATALOG_TABLE_NAME;
}

std::string TableSchemaManager::SerializeSchema(const TableSchema& schema) {
    std::ostringstream oss;
    
    // 序列化表基本信息
    oss << schema.table_name_ << "\n";
    oss << schema.primary_key_index_ << "\n";
    oss << schema.columns_.size() << "\n";
    
    // 序列化每个列
    for (const Column& column : schema.columns_) {
        oss << column.column_name_ << "|";
        oss << static_cast<int>(column.type_) << "|";
        oss << column.length_ << "|";
        oss << (column.nullable_ ? 1 : 0) << "\n";
    }
    
    return oss.str();
}

bool TableSchemaManager::DeserializeSchema(const std::string& data, TableSchema& schema) {
    std::istringstream iss(data);
    std::string line;
    
    // 读取表基本信息
    if (!std::getline(iss, line)) return false;
    schema.table_name_ = line;
    
    if (!std::getline(iss, line)) return false;
    schema.primary_key_index_ = std::stoi(line);
    
    if (!std::getline(iss, line)) return false;
    size_t column_count = std::stoul(line);
    
    // 读取每个列
    schema.columns_.clear();
    schema.columns_.reserve(column_count);
    
    for (size_t i = 0; i < column_count; ++i) {
        if (!std::getline(iss, line)) return false;
        
        // 解析列信息：column_name|type|length|nullable
        size_t pos1 = line.find('|');
        if (pos1 == std::string::npos) return false;
        
        size_t pos2 = line.find('|', pos1 + 1);
        if (pos2 == std::string::npos) return false;
        
        size_t pos3 = line.find('|', pos2 + 1);
        if (pos3 == std::string::npos) return false;
        
        std::string column_name = line.substr(0, pos1);
        int type_int = std::stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
        int length = std::stoi(line.substr(pos2 + 1, pos3 - pos2 - 1));
        bool nullable = (std::stoi(line.substr(pos3 + 1)) != 0);
        
        schema.columns_.emplace_back(column_name, static_cast<DataType>(type_int), length, nullable);
    }
    
    return true;
}

TableSchema TableSchemaManager::CreateSystemCatalogSchema() {
    TableSchema schema(SYSTEM_CATALOG_TABLE_NAME);
    
    // 系统目录表的列结构：
    // 1. table_name (VARCHAR) - 表名
    // 2. schema_data (VARCHAR) - 序列化的表结构数据
    
    schema.AddColumn(Column("table_name", DataType::Varchar, 255, false));
    // 为了保证单条记录可写入一页，限制最大长度到 2048
    schema.AddColumn(Column("schema_data", DataType::Varchar, 2048, false));
    
    schema.primary_key_index_ = 0; // 表名作为主键
    
    return schema;
}
