//
// Created by Huang_cj on 2025/9/9.
//

#pragma once
#include "types.h"
#include "../storage/buffer_pool_manager.h"
#include <unordered_map>
#include <string>

// 前向声明
class TableManager;

/**
 * 表结构管理器(Table Schema Manager)
 * 专门负责表结构的持久化管理，与 TableManager 协作
 */
class TableSchemaManager {
public:
    TableSchemaManager(BufferPoolManager* bpm, TableManager* tm);
    ~TableSchemaManager();

    // 核心功能
    bool InitializeSystemCatalog();
    bool SaveTableSchema(const TableSchema& schema);
    bool LoadTableSchema(const std::string& table_name, TableSchema& schema);
    bool DeleteTableSchema(const std::string& table_name);
    bool LoadAllTableSchemas(std::unordered_map<std::string, TableSchema>& schemas);
    
    // 检查系统目录表是否存在
    bool IsSystemCatalogInitialized() const;

    // 获取系统目录表名（对外只暴露只读访问）
    static const std::string& SystemCatalogTableName();

private:
    BufferPoolManager* buffer_pool_manager_;
    TableManager* table_manager_;
    
    // 系统目录表名
    static const std::string SYSTEM_CATALOG_TABLE_NAME;
    
    // 序列化/反序列化
    std::string SerializeSchema(const TableSchema& schema);
    bool DeserializeSchema(const std::string& data, TableSchema& schema);
    
    // 创建系统目录表结构
    TableSchema CreateSystemCatalogSchema();
};
