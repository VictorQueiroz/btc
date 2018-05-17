#include "member_expression.h"
#include "ast_item.h"

#include <stdlib.h>

void btc_initialize_member_expression(btc_member_expression** expr_ptr) {
    *expr_ptr = calloc(1, sizeof(btc_member_expression));
    btc_initialize_ast_item(&(*expr_ptr)->left);
}

void btc_destroy_member_expression(btc_member_expression* expr) {
    btc_destroy_ast_item(expr->left);
    free(expr);
}