// 简单交互式数据库入口：使用 TableManager + BufferPoolManager + DiskManager
#include <iostream>
#include <string>
#include <sstream>
#include "storage/async_aliases.h"
#include "catalog/table_manager.h"
#include "ast.h"
#include "log/log_config.h"

// Flex/Bison 解析接口
extern int yyparse();
extern ASTNode* ast_root;      // 由语法器设置
// 与 flex 生成文件保持一致的声明（使用 C++ linkage）
typedef struct yy_buffer_state* YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string(const char* str);
void yy_delete_buffer(YY_BUFFER_STATE b);

// 从 AST 生成条件字符串（简化：只支持二元比较）
static std::string build_condition_from_where(ASTNode* where) {
    auto sql_logger = DatabaseSystem::Log::LogConfig::GetSQLLogger();
    sql_logger->Debug("build_condition_from_where called");
    if (!where) {
        sql_logger->Debug("where is null");
        return "";
    }
    if (where->children.empty()) {
        sql_logger->Debug("where has no children");
        return "";
    }
    
    sql_logger->Debug("where type = {}, where children count = {}", (int)where->type, where->children.size());
    
    ASTNode* expr = where->children[0];
    if (!expr) {
        sql_logger->Debug("expr is null");
        return "";
    }
    
    sql_logger->Debug("expr type = {}, expr children count = {}", (int)expr->type, expr->children.size());
    
    // 根据语法规则，expression节点本身就是BINARY_EXPR类型
    if (expr->type != BINARY_EXPR) {
        sql_logger->Debug("expr is not BINARY_EXPR, type = {}", (int)expr->type);
        return "";
    }
    
    if (expr->children.size() < 2) {
        sql_logger->Debug("expr has less than 2 children");
        return "";
    }
    
    std::string lhs, op, rhs;
    
    // children[0]: IDENTIFIER_NODE (列名)
    sql_logger->Debug("children[0] type = {}", (int)expr->children[0]->type);
    if (expr->children[0]->type == IDENTIFIER_NODE) {
        if (std::holds_alternative<std::string>(expr->children[0]->value))
            lhs = std::get<std::string>(expr->children[0]->value);
    }
    sql_logger->Debug("lhs = '{}'", lhs);
    
    // 操作符在expr节点本身的value中
    if (std::holds_alternative<std::string>(expr->value))
        op = std::get<std::string>(expr->value);
    sql_logger->Debug("op = '{}'", op);
    
    // children[1]: INTEGER_LITERAL_NODE 或 STRING_LITERAL_NODE (值)
    sql_logger->Debug("children[1] type = {}", (int)expr->children[1]->type);
    if (expr->children[1]->type == INTEGER_LITERAL_NODE) {
        if (std::holds_alternative<int>(expr->children[1]->value))
            rhs = std::to_string(std::get<int>(expr->children[1]->value));
        else if (std::holds_alternative<std::string>(expr->children[1]->value))
            rhs = std::get<std::string>(expr->children[1]->value);
    } else if (expr->children[1]->type == STRING_LITERAL_NODE) {
        if (std::holds_alternative<std::string>(expr->children[1]->value)) {
            std::string s = std::get<std::string>(expr->children[1]->value);
            if (!s.empty() && (s.front() == '\'' || s.front() == '"') && s.back() == s.front()) {
                if (s.size() >= 2) s = s.substr(1, s.size()-2);
            }
            rhs = "'" + s + "'";
        }
    } else if (expr->children[1]->type == IDENTIFIER_NODE) {
        if (std::holds_alternative<std::string>(expr->children[1]->value))
            rhs = std::get<std::string>(expr->children[1]->value);
    }
    sql_logger->Debug("rhs = '{}'", rhs);
    
    if (lhs.empty() || op.empty() || rhs.empty()) {
        sql_logger->Debug("one of lhs/op/rhs is empty");
        return "";
    }
    
    std::string result = lhs + op + rhs;
    sql_logger->Debug("final condition = '{}'", result);
    return result;
}

// 执行解析得到的 AST：按顺序对接 TableManager
static void execute_ast(ASTNode* root, TableManager& tm) {
    if (!root) return;
    
    auto sql_logger = DatabaseSystem::Log::LogConfig::GetSQLLogger();
    for (ASTNode* stmt : root->children) {
        try {
        switch (stmt->type) {
            case CREATE_TABLE_STMT: {
                // CREATE TABLE table_name (col defs)
                std::string table;
                if (std::holds_alternative<std::string>(stmt->children[0]->value))
                    table = std::get<std::string>(stmt->children[0]->value);
                ASTNode* col_list = stmt->children[1];
                TableSchema schema(table);
                for (ASTNode* col_node : col_list->children) {
                    std::string col_name;
                    if (std::holds_alternative<std::string>(col_node->value))
                        col_name = std::get<std::string>(col_node->value);
                    std::string t;
                    if (std::holds_alternative<std::string>(col_node->children[0]->value))
                        t = std::get<std::string>(col_node->children[0]->value);
                    // 映射字符串到内部类型
                    DataType dt = (t == "INT" || t == "INTEGER") ? DataType::Int : DataType::Varchar;
                    int len = (dt == DataType::Varchar ? 255 : 4);
                    schema.AddColumn(Column(col_name, dt, len, true));
                }
                schema.primary_key_index_ = 0;
                if (tm.TableExists(table)) {
                    std::cout << "ERR: table already exists: " << table << "\n";
                    break;
                }
                bool ok = tm.CreateTable(schema);
                if (!ok) std::cout << "ERR: create table failed: " << table << "\n"; else std::cout << "OK\n";
                break;
            }
            case INSERT_STMT: {
                std::string table;
                if (std::holds_alternative<std::string>(stmt->children[0]->value))
                    table = std::get<std::string>(stmt->children[0]->value);
                ASTNode* values = stmt->children.back();
                Record rec;
                for (ASTNode* v : values->children) {
                    if (v->type == INTEGER_LITERAL_NODE) {
                        if (std::holds_alternative<int>(v->value)) rec.AddValue(std::get<int>(v->value));
                        else if (std::holds_alternative<std::string>(v->value)) {
                            try { rec.AddValue(std::stoi(std::get<std::string>(v->value))); } catch (...) { rec.AddValue(0); }
                        }
                    } else if (v->type == STRING_LITERAL_NODE) {
                        std::string s;
                        if (std::holds_alternative<std::string>(v->value)) s = std::get<std::string>(v->value);
                        if (!s.empty() && (s.front() == '\'' || s.front() == '"') && s.back() == s.front()) {
                            if (s.size() >= 2) s = s.substr(1, s.size()-2);
                        }
                        rec.AddValue(s);
                    } else if (v->type == IDENTIFIER_NODE) {
                        // 容错：把未加引号的标识符当作字符串值
                        if (std::holds_alternative<std::string>(v->value))
                            rec.AddValue(std::get<std::string>(v->value));
                    }
                }
                if (!tm.TableExists(table)) {
                    std::cout << "ERR: table not found: " << table << "\n"; break;
                }
                if (TableSchema* s = tm.GetTableSchema(table)) {
                    if (rec.values_.size() != s->columns_.size()) {
                        std::cout << "ERR: column count mismatch: expected " << s->columns_.size()
                                  << ", got " << rec.values_.size() << "\n"; break;
                    }
                }
                bool ok = tm.InsertRecord(table, rec);
                if (!ok) {
                    // 检查是否是主键重复错误
                    TableSchema* schema = tm.GetTableSchema(table);
                    if (schema && schema->primary_key_index_ >= 0) {
                        std::cout << "ERR: Primary key constraint violation in table '" << table << "'" << std::endl;
                    } else {
                        std::cout << "ERR: insert failed into " << table << "\n";
                    }
                } else {
                    std::cout << "OK\n";
                }
                break;
            }
            case SELECT_STMT: {
                // SELECT ... FROM table [WHERE ...]
                ASTNode* select_list = stmt->children[0];
                ASTNode* from = stmt->children[1];
                std::string table;
                if (std::holds_alternative<std::string>(from->value)) table = std::get<std::string>(from->value);
                
                // 解析SELECT列表
                std::vector<std::string> columns;
                if (select_list->children.size() == 1 && 
                    select_list->children[0]->type == IDENTIFIER_NODE &&
                    std::holds_alternative<std::string>(select_list->children[0]->value) &&
                    std::get<std::string>(select_list->children[0]->value) == "*") {
                    // SELECT * - 获取所有列
                    // 这里我们需要获取表结构来知道所有列名
                    TableSchema* schema = tm.GetTableSchema(table);
                    if (schema) {
                        for (const auto& col : schema->columns_) {
                            columns.push_back(col.column_name_);
                        }
                    }
                } else {
                    // SELECT column1, column2, ... - 获取指定列
                    for (ASTNode* col_node : select_list->children) {
                        if (col_node->type == IDENTIFIER_NODE && 
                            std::holds_alternative<std::string>(col_node->value)) {
                            columns.push_back(std::get<std::string>(col_node->value));
                        }
                    }
                }
                
                std::string cond;
                if (stmt->children.size() > 2) cond = build_condition_from_where(stmt->children[2]);
                sql_logger->Debug("condition = '{}'", cond);
                std::string columns_str = "[";
                for (size_t i = 0; i < columns.size(); ++i) {
                    columns_str += "'" + columns[i] + "'";
                    if (i + 1 < columns.size()) columns_str += ", ";
                }
                columns_str += "]";
                sql_logger->Debug("columns = {}", columns_str);
                
                if (!tm.TableExists(table)) { std::cout << "ERR: table not found: " << table << "\n"; break; }
                
                // 使用投影查询
                std::vector<Record> rows;
                if (cond.empty()) {
                    rows = tm.SelectColumns(table, columns);
                } else {
                    rows = tm.SelectColumnsWithCondition(table, columns, cond);
                }
                
                std::cout << "rows=" << rows.size() << "\n";
                for (const auto& r : rows) {
                    std::cout << "{";
                    for (size_t i = 0; i < r.values_.size(); ++i) {
                        if (std::holds_alternative<int>(r.values_[i])) std::cout << std::get<int>(r.values_[i]);
                        else if (std::holds_alternative<std::string>(r.values_[i])) std::cout << std::get<std::string>(r.values_[i]);
                        else if (std::holds_alternative<bool>(r.values_[i])) std::cout << (std::get<bool>(r.values_[i]) ? "true" : "false");
                        if (i + 1 < r.values_.size()) std::cout << ", ";
                    }
                    std::cout << "}\n";
                }
                break;
            }
            case DELETE_STMT: {
                ASTNode* from = stmt->children[0];
                std::string table;
                if (std::holds_alternative<std::string>(from->value)) table = std::get<std::string>(from->value);
                std::string cond;
                if (stmt->children.size() > 1) cond = build_condition_from_where(stmt->children[1]);
                if (!tm.TableExists(table)) { std::cout << "ERR: table not found: " << table << "\n"; break; }
                int n = tm.DeleteRecordsWithCondition(table, cond);
                std::cout << "deleted=" << n << "\n";
                break;
            }
            default:
                std::cout << "WARN: unsupported statement type: " << (int)stmt->type << "\n";
        }
        } catch (const std::bad_variant_access&) {
            std::cout << "ERR: bad value type in statement (variant access)\n";
        } catch (const std::exception& e) {
            std::cout << "ERR: " << e.what() << "\n";
        }
    }
}

static void print_help() {
    std::cout << "Commands:\n"
              << "  help                         - show this help\n"
              << "  exit                         - quit\n"
              << "  create <table> id:int name:varchar(50) age:int\n"
              << "  insert <table> <id> <name> <age>\n"
              << "  select <table> [cond]        - cond example: age > 20\n"
              << "  delete <table> [cond]        - cond example: id = 1\n";
}

int main() {
    // 初始化日志系统
    DatabaseSystem::Log::LogConfig::Initialize();
    auto sql_logger = DatabaseSystem::Log::LogConfig::GetSQLLogger();
    
    // 初始化异步存储引擎
    DiskManager disk_manager("db.data", 2); // 2个工作线程
    BufferPoolManager buffer_pool(128, &disk_manager);
    TableManager table_manager(&buffer_pool);
    // 启动时加载所有已持久化的表结构，避免重启后重复创建同名表
    (void)table_manager.LoadAllTableSchemas();
    // 启动时打印最多6个已有表名
    {
        auto names = table_manager.ListTableNames();
        std::cout << "Existing tables (up to 6): ";
        int count = 0;
        for (const auto& n : names) {
            if (count >= 6) break;
            if (count) std::cout << ", ";
            std::cout << n;
            count++;
        }
        if (count == 0) std::cout << "<none>";
        std::cout << "\n";
    }

    std::cout << "MiniDB SQL mode. Type SQL. Statements end with ';'. Each line executes statements immediately. Type 'help' or 'exit'.\n";
    std::string buffer;
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "help") { print_help(); continue; }
        if (line == "exit") { break; }
        // 累积当前行
        buffer.append(line);
        buffer.push_back('\n');
        // 按 ';' 切分并依次执行完整语句
        size_t pos = 0;
        while (true) {
            size_t semi = buffer.find(';', pos);
            if (semi == std::string::npos) {
                // 没有完整语句，保留缓冲
                // 丢弃前面已处理部分
                if (pos > 0) buffer.erase(0, pos);
                break;
            }
            // 取出一个完整语句（包含到分号）
            std::string stmt = buffer.substr(0, semi + 1);
            // 特殊内置命令：SHOW TABLES;（不经由 SQL 解析器）
            {
                std::string trimmed = stmt;
                // 去空白
                size_t b = trimmed.find_first_not_of(" \t\r\n");
                size_t e = trimmed.find_last_not_of(" \t\r\n");
                if (b != std::string::npos) trimmed = trimmed.substr(b, e - b + 1); else trimmed.clear();
                // 大写
                std::string upper = trimmed; for (char &ch : upper) ch = (char)std::toupper((unsigned char)ch);
                if (upper == "SHOW TABLES;" || upper == "SHOW TABLES") {
                    table_manager.PrintAllTables();
                    // 移除已处理语句
                    buffer.erase(0, semi + 1);
                    size_t np2 = buffer.find_first_not_of(" \t\r\n");
                    if (np2 != std::string::npos) buffer.erase(0, np2); else buffer.clear();
                    pos = 0;
                    if (buffer.empty()) break;
                    continue;
                }
            }

            // 解析并执行
            YY_BUFFER_STATE bs = yy_scan_string(stmt.c_str());
            ast_root = nullptr;
            int rc = yyparse();
            if (rc == 0 && ast_root) {
                execute_ast(ast_root, table_manager);
            } else {
                std::cout << "Parse error\n";
            }
            yy_delete_buffer(bs);
            // 移除已处理语句
            buffer.erase(0, semi + 1);
            // 去除起始空白
            size_t np = buffer.find_first_not_of(" \t\r\n");
            if (np != std::string::npos) buffer.erase(0, np); else buffer.clear();
            // 继续检查下一个 ';'
            pos = 0;
            if (buffer.empty()) break;
        }
    }

    return 0;
}