// 语义分析自定义封装异常类
#ifndef SEMANTIC_ERROR_H
#define SEMANTIC_ERROR_H

#include <stdexcept>
#include <string>
#include <sstream>

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

    SemanticError(ErrorType type, const std::string &reason, const ASTNode *node)
        : std::runtime_error(reason), error_type(type)
    {
        std::stringstream ss;
        ss << reason;
        // 检查节点是否有效，以及它是否有位置信息
        if (node && node->location.first_line > 0)
        {
            ss << " (行: " << node->location.first_line
               << ", 列: " << node->location.first_column << ")";
        }
        description_ = ss.str();
    }

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
    std::string description_;
};

#endif