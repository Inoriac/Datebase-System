//
// Created by Huang_cj on 2025/9/9.
//

#include "storage/record_serializer.h"
#include <iostream>

bool RecordSerializer::SerializeRecord(const Record &record, const TableSchema &schema, char *page_data, int offset) {
    if (!HasEnoughSpace(page_data, offset, schema)) return false;

    char* buffer = page_data + offset;
    int current_offset = 0; // 记录内部偏移量

    // 序列化删除标记(1字节)
    buffer[current_offset] = record.is_deleted_ ? 1 : 0;
    current_offset += 1;

    // 序列化每个值
    for (size_t i = 0; i < record.values_.size() && i < schema.columns_.size(); i++) {
        // 序列化每个字段值，转为二进制
        int bytes_written = SerializeValue(record.values_[i], schema.columns_[i].type_, buffer + current_offset);
        if (bytes_written <= 0) return false;

        current_offset += bytes_written;
    }
    return true;
}

bool RecordSerializer::DeserializeRecord(Record &record, const TableSchema &schema, const char *page_data, int offset) {
    const char* buffer = page_data + offset;
    int current_offset = 0;

    // 反序列化删除标记
    record.is_deleted_ = (buffer[current_offset] != 0);
    current_offset += 1;

    // 清空现有值
    record.values_.clear();

    // 反序列化每个值
    for (size_t i = 0; i < schema.columns_.size(); i++) {
        Value value;
        int bytes_read = DeserializeValue(value, schema.columns_[i].type_, buffer + current_offset);

        if (bytes_read <= 0) return false;
        record.values_.push_back(value);
        current_offset += bytes_read;
    }
    return true;
}

int RecordSerializer::CalculateRecordSize(const TableSchema &schema) {
    int total_size = 1; // 删除标记占1字节

    for (const auto& column : schema.columns_) {
        total_size += GetDataTypeSize(column.type_, column.length_);
    }

    return total_size;
}

int RecordSerializer::CalculateRecordOffset(int record_index, const TableSchema &schema) {
    int record_size = CalculateRecordSize(schema);
    return record_size * record_index;
}

bool RecordSerializer::HasEnoughSpace(const char *page_data, int offset, const TableSchema &schema) {
    int record_size = CalculateRecordSize(schema);
    return (offset + record_size) <= PAGE_SIZE;
}

int RecordSerializer::SerializeValue(const Value &value, DataType type, char *buffer) {
    switch (type) {
        case DataType::Int: {
            if (std::holds_alternative<int>(value)) {
                int int_val = std::get<int>(value);
                std::memcpy(buffer, &int_val, sizeof(int)); // buffer 为目标地址
                return sizeof(int);
            }
            break;
        }
        case DataType::Varchar: {
            if (std::holds_alternative<std::string>(value)) {
                const std::string& str_val = std::get<std::string>(value);
                int length = static_cast<int>(str_val.length());

                // 先后写入字符串长度与字符串内容
                std::memcpy(buffer, &length, sizeof(int));
                std::memcpy(buffer + sizeof(int), str_val.c_str(), length);

                return sizeof(int) + length;
            }
            break;
        }
        case DataType::Bool: {
            if (std::holds_alternative<bool>(value)) {
                bool bool_val = std::get<bool>(value);
                buffer[0] = bool_val ? 1 : 0;
                return sizeof(bool);
            }
            break;
        }
    }
    return 0;
}

int RecordSerializer::DeserializeValue(Value &value, DataType type, const char *buffer) {
    switch (type) {
        case DataType::Int: {
            int int_val;
            std::memcpy(&int_val, buffer, sizeof(int));
            value = int_val;
            return sizeof(int);
        }
        case DataType::Varchar: {
            // 获取字符串长度
            int length;
            std::memcpy(&length, buffer, sizeof(int));

            // 获取字符串内容
            std::string str_val(buffer + sizeof(int), length);
            value = str_val;

            return sizeof(int) + length;
        }
        case DataType::Bool: {
            bool bool_val = (buffer[0] != 0);
            value = bool_val;
            return sizeof(bool);
        }
    }
    return 0;
}

int RecordSerializer::GetValueSize(const Value &value, DataType type) {
    switch (type) {
        case DataType::Int: {
            if (std::holds_alternative<int>(value)) {
                return sizeof(int);
            }
            break;
        }
        case DataType::Varchar: {
            if (std::holds_alternative<std::string>(value)) {   // 检查是否真的包含 string 类型
                // 长度字段 + 内容长度
                return sizeof(int) + static_cast<int>(std::get<std::string>(value).length());
            }
            break;
        }
        case DataType::Bool: {
            if (std::holds_alternative<bool>(value)) {
                return sizeof(bool);
            }
            break;
        }
    }
    return 0;
}

int RecordSerializer::GetDataTypeSize(DataType type, int length) {
    switch (type) {
        case DataType::Int:
            return sizeof(int);
        case DataType::Varchar:
            return sizeof(int) + length;
        case DataType::Bool:
            return sizeof(bool);
    }
    return 0;
}
