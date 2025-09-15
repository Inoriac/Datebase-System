#ifndef SUGGESTION_H
#define SUGGESTION_H

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <limits>

// 所有 SQL 关键字的列表 (统一大写)
const std::vector<std::string> keywords = {
    "CREATE", "TABLE", "INSERT", "INTO", "VALUES", "SELECT", 
    "FROM", "WHERE", "DELETE", "INT", "VARCHAR", "JOIN", "ON",
    "UPDATE", "SET", "GROUP", "BY", "ORDER"
};

enum ParserState {
    // 默认状态
    STATE_EXPECTING_COMMAND,         // 期待一个语句的起始命令 (CREATE, SELECT, etc.)

    // CREATE 语句状态
    STATE_EXPECTING_TABLE_AFTER_CREATE,
    STATE_EXPECTING_LPAREN_AFTER_TABLE_NAME,
    STATE_EXPECTING_COLUMN_DEFINITION,
    STATE_EXPECTING_DATA_TYPE,

    // INSERT 语句状态
    STATE_EXPECTING_INTO_AFTER_INSERT,
    STATE_EXPECTING_TABLE_AFTER_INTO,
    STATE_EXPECTING_VALUES_AFTER_TABLE,

    // SELECT 语句状态
    STATE_EXPECTING_COLUMNS_AFTER_SELECT,
    STATE_EXPECTING_FROM_AFTER_COLUMNS,
    STATE_EXPECTING_TABLE_AFTER_FROM
};

const std::vector<std::string> keywords_for_command = {"CREATE", "INSERT", "SELECT", "DELETE"};
const std::vector<std::string> keywords_for_create = {"TABLE"};
const std::vector<std::string> keywords_for_insert = {"INTO", "VALUES"};
const std::vector<std::string> keywords_for_select = {"FROM"};
const std::vector<std::string> keywords_for_data_type = {"INT", "VARCHAR"};



/**
 * @brief 计算两个字符串之间的莱文斯坦距离
 * @param s1 字符串1
 * @param s2 字符串2
 * @return int 它们之间的编辑距离
 */
inline int levenshteinDistance(const std::string& s1, const std::string& s2) {
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));

    d[0][0] = 0;
    for(size_t i = 1; i <= len1; ++i) d[i][0] = i;
    for(size_t i = 1; i <= len2; ++i) d[0][i] = i;

    for(size_t i = 1; i <= len1; ++i) {
        for(size_t j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost });
        }
    }
    return d[len1][len2];
}

/**
 * @brief 在关键字列表中查找与输入最相似的词
 * @param input 用户输入的、可能是错误的词
 * @return std::string 最佳建议，如果没有好的建议则返回空字符串
 */
inline std::string findClosestKeyword(const std::string& mistyped_word, ParserState current_state) {
    const std::vector<std::string>* keywords_to_search = nullptr;

    // 根据当前状态，选择正确的、范围更小的关键字列表
    switch (current_state) {
        case STATE_EXPECTING_COMMAND: // 期待一个语句的起始命令
            keywords_to_search = &keywords_for_command;
            break;
        case STATE_EXPECTING_TABLE_AFTER_CREATE: // CREATE 后期待 TABLE
            keywords_to_search = &keywords_for_create;
            break;
        case STATE_EXPECTING_INTO_AFTER_INSERT: // INSERT 后期待 INTO
            keywords_to_search = &keywords_for_insert;
            break;
        case STATE_EXPECTING_VALUES_AFTER_TABLE: // INSERT INTO table 后期待 VALUES
            keywords_to_search = &keywords_for_insert;
            break;
        case STATE_EXPECTING_FROM_AFTER_COLUMNS: // SELECT columns 后期待 FROM
             keywords_to_search = &keywords_for_select;
             break;
        case STATE_EXPECTING_DATA_TYPE: // 期待数据类型
            keywords_to_search = &keywords_for_data_type;
            break;
        default:
            // 对于其他状态,使用通用关键字
            keywords_to_search = &keywords;
            break;
    }

    std::string best_suggestion = "";
    int min_distance = std::numeric_limits<int>::max();
    int threshold = 2; // 只建议相似度非常高的词

    for (const auto& keyword : *keywords_to_search) {
        int distance = levenshteinDistance(mistyped_word, keyword);
        if (distance < min_distance && distance <= threshold) {
            min_distance = distance;
            best_suggestion = keyword;
        }
    }

    return best_suggestion;
}

#endif // SUGGESTION_H
