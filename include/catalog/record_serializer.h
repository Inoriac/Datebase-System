//
// Created by Huang_cj on 2025/9/9.
//

#pragma once
#include "types.h"
#include "page.h"
#include <cstring>

class RecordSerializer {
public:
    // 将记录序列化到页中
    static bool SerializeRecord(const Record& record, const TableSchema& schema,
                        char* page_data, int offset);
    // 从页中反序列化记录
    static bool DeserializeRecord(Record& record, const TableSchema& schema,
                        const char* page_data, int offset);
    // 计算记录在页中占用的字节数
    static int CalculateRecordSize(const TableSchema& schema);
    // 计算记录在页中的偏移量
    static int CalculateRecordOffset(int record_index, const TableSchema& schema);
    //检查页中是否有足够的存储空间记录
    static bool HasEnoughSpace(const char* page_data, int offset, const TableSchema& schema);

private:
    // 序列化单个值 返回值为读取的字节数
    static int SerializeValue(const Value& value, DataType type, char* buffer);
    // 反序列化单个值
    static int DeserializeValue(Value& value, DataType type, const char* buffer);
    // 获取值类型的字节大小
    static int GetValueSize(const Value& value, DataType type);
    // 获取数据类型的固定字节大小
    static int GetDataTypeSize(DataType type, int length = 0);
};