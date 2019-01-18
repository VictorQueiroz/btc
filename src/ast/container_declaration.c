#include "container_declaration.h"
#include "parser_identifier.h"

#include <stdlib.h>

void btc_create_container_declaration(btc_ast_container_declaration** container_ptr){
    *container_ptr = calloc(1, sizeof(btc_ast_container_declaration));

    btc_ast_container_declaration* container = *container_ptr;
    btc_ast_identifier type = { "" };

    container->type = type;
    container->body = btc_ast_list_alloc();
}

void btc_destroy_container_declaration(btc_ast_container_declaration* container) {
    btc_ast_list_free(container->body);
    free(container);
}