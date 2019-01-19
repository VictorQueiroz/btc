#include "ast_list.h"
#include "ast_item.h"
#include <stdlib.h>

CVECTOR_NEW(btc_ast_list, btc_ast_item*)

void btc_ast_list_free_items(btc_ast_list* list) {
    vector_foreach(list, i) {
        btc_ast_item_free(CVECTOR_GET(list, i));
    }
}

void btc_destroy_ast_list(btc_ast_list* list) {
    btc_ast_list_free_items(list);
    btc_ast_list_destroy(list);
}
