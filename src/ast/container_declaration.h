#ifndef CONTAINER_DECLARATION_H_
#define CONTAINER_DECLARATION_H_

#define BTC_CONTAINER_DECLARATION 10

typedef struct _btc_ast_container_declaration btc_ast_container_declaration;

#include "parser_identifier.h"
#include "ast_item.h"

struct _btc_ast_container_declaration {
    btc_ast_identifier name;
    btc_ast_identifier type;
    btc_ast_list* body;
};

void btc_create_container_declaration(btc_ast_container_declaration** container_ptr);
void btc_destroy_container_declaration(btc_ast_container_declaration* container);

#endif