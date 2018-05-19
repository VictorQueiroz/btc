#include "container_declaration.h"
#include "parser_identifier.h"

#include <stdlib.h>

void btc_create_container_declaration(btc_ast_container_declaration** container_ptr){
    *container_ptr = calloc(1, sizeof(btc_ast_container_declaration));

    btc_ast_container_declaration* container = *container_ptr;
    btc_create_ast_list(&container->body);
}

void btc_destroy_container_declaration(btc_ast_container_declaration* container) {
    btc_destroy_ast_list(container->body);
    free(container);
}