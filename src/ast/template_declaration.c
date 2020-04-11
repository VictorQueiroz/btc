#include "template_declaration.h"
#include "ast_list.h"
#include "ast_item.h"

#include <stdlib.h>

#define BTC_RETURN_IF_NULL(expression) if(expression == NULL) return NULL;

btc_template_declaration* btc_template_declaration_alloc() {
    btc_template_declaration* template_declaration = calloc(1, sizeof(btc_template_declaration));
    BTC_RETURN_IF_NULL(template_declaration)

    template_declaration->arguments = btc_ast_list_alloc();
    template_declaration->body = btc_ast_item_alloc();

    return template_declaration;
}

void btc_template_declaration_free(btc_template_declaration* declaration) {
    btc_ast_list_free(declaration->arguments);
    btc_ast_item_free(declaration->body);
    free(declaration);
}
