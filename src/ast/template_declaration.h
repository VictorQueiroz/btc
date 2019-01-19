#ifndef BTC_TEMPLATE_DECLARATION_H
#define BTC_TEMPLATE_DECLARATION_H

typedef struct _btc_template_declaration btc_template_declaration;

#include "ast_list.h"
#include "ast_item.h"

struct _btc_template_declaration {
    btc_ast_list* arguments;
    btc_ast_item* body;
};

#define BTC_TEMPLATE_DECLARATION 15

btc_template_declaration* btc_template_declaration_alloc();
void btc_template_declaration_free(btc_template_declaration* declaration);

#endif //BTC_TEMPLATE_DECLARATION_H
