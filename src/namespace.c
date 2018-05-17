#include "namespace.h"
#include "ast_list.h"
#include <stdlib.h>

void btc_initialize_namespace(btc_namespace** namespace_ptr) {
    *namespace_ptr = malloc(sizeof(btc_namespace));
    btc_namespace* namespace = *namespace_ptr;

    btc_ast_list* body;
    btc_create_ast_list(&body);

    namespace->body = body;
}

void btc_destroy_namespace(btc_namespace* namespace) {
    btc_destroy_ast_list(namespace->body);
    free(namespace);
}