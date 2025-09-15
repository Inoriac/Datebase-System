// 语义分析自定义封装异常类
#ifndef SEMANTIC_ERROR_H
#define SEMANTIC_ERROR_H

#include <stdexcept>
#include <string>
#include <sstream>
#include <utility>

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

    // 只需要一个构造函数
    SemanticError(ErrorType type, const std::string &reason, const ASTNode *node,
                  const std::string &token_value = "", const std::vector<std::string> &candidates = std::vector<std::string>())
        : std::runtime_error(reason), error_type(type), error_node(node), token_value_(token_value), candidate_columns_(candidates)
    {
        std::stringstream ss;
        ss << reason;

        // 检查节点是否有效，以及它是否有位置信息
        if (node && node->location.first_line > 0)
        {
            ss << " (行: " << node->location.first_line
               << ", 列: " << node->location.first_column << ")";
        }

        // 将 token_value 也加入到描述中，以便调试
        if (!token_value.empty())
        {
            ss << " [Token: " << token_value << "]";
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
    std::pair<int, int> getLocation() const
    {
        if (error_node)
        {
            return {error_node->location.first_line, error_node->location.first_column};
        }
        // 如果没有节点信息，返回一个表示“无”的特殊值，例如 {0, 0} 或 {-1, -1}
        return {0, 0};
    }
    const std::string &getTokenValue() const
    {
        return token_value_;
    }
    const std::vector<std::string> &getCandidateColumns() const { return candidate_columns_; }

private:
    ErrorType error_type;
    std::string description_;
    const ASTNode *error_node;
    std::string token_value_;
    std::vector<std::string> candidate_columns_;
};

#endif