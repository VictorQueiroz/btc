#ifndef CONTAINER_DECLARATION_H_
#define CONTAINER_DECLARATION_H_

typedef struct _btc_ast_container_declaration btc_ast_container_declaration;

#include "parser_identifier.h"
#include "container_params.h"

struct _btc_ast_container_declaration {
    btc_ast_identifier name;
    btc_ast_identifier type;
    btc_ast_container_params* body;
};

void btc_create_container_declaration(btc_ast_container_declaration** container_ptr);
void btc_destroy_container_declaration(btc_ast_container_declaration* container);

#endif