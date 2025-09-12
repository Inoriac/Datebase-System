// 语义分析自定义封装异常类
#ifndef SEMANTIC_ERROR_H
#define SEMANTIC_ERROR_H

#include <stdexcept>
#include <string>

class SemanticError : public std::runtime_error
{
public:
    enum ErrorType
    {
        TABLE_NOT_FOUND,
        COLUMN_NOT_FOUND,
        TYPE_MISMATCH,
        DUPLICATE_TABLE,
        DUPLICATE_COLUMN,
        SYNTAX_ERROR, // 用于AST结构不正确的情况
        UNKNOWN_ERROR
    };

    SemanticError(ErrorType type, const std::string &reason)
        : std::runtime_error(reason), error_type(type) {}

    ErrorType getType() const { return error_type; }
    
    std::string getErrorDescription() const
    {
        switch (error_type)
        {
        case TABLE_NOT_FOUND:
            return "表不存在";
        case COLUMN_NOT_FOUND:
            return "列不存在";
        case TYPE_MISMATCH:
            return "类型不匹配";
        case DUPLICATE_TABLE:
            return "表重复";
        case DUPLICATE_COLUMN:
            return "列重复";
        case SYNTAX_ERROR:
            return "语法错误";
        case UNKNOWN_ERROR:
            return "未知错误";
        }
        return "未知错误";
    }

private:
    ErrorType error_type;
};

#endif