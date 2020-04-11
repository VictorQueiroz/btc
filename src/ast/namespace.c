#include "namespace.h"
#include "ast_list.h"
#include <stdlib.h>

void btc_initialize_namespace(btc_namespace** namespace_ptr) {
    *namespace_ptr = calloc(1, sizeof(btc_namespace));
    btc_namespace* namespace_item = *namespace_ptr;

    btc_ast_list* body = btc_ast_list_alloc();

    namespace_item->body = body;
}

void btc_destroy_namespace(btc_namespace* namespace_item) {
    btc_ast_list_free(namespace_item->body);
    free(namespace_item);
}
