#include "ast.h"
#include "symbol_table.h"

void semantic_analysis(ASTNode* root_node) {
    if (!root_node) {
        return; // 空 AST
    }

    // 遍历 AST
    for (ASTNode* statement_node : root_node->children) {
        switch (statement_node->type) {
            case CREATE_TABLE_STMT:
                check_create_statement(statement_node);
                break;
            case INSERT_STMT:
                check_insert_statement(statement_node);
                break;
            case SELECT_STMT:
                check_select_statement(statement_node);
                break;
            // ...
        }
    }
}