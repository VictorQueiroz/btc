#ifndef BTC_MEMBER_EXPRESSION_H_
#define BTC_MEMBER_EXPRESSION_H_

#define BTC_MEMBER_EXPRESSION 4

typedef struct _btc_member_expression btc_member_expression;

#include "ast_item.h"
#include "parser_identifier.h"

struct _btc_member_expression {
    btc_ast_item* left;
    btc_ast_identifier right;
};

void btc_initialize_member_expression(btc_member_expression** expr_ptr);
void btc_destroy_member_expression(btc_member_expression* expr);

#endif