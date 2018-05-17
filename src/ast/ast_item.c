#include "ast_item.h"
#include "container_group.h"

#include <stdlib.h>
#include <stdio.h>

void btc_destroy_ast_item(btc_ast_item* item) {
    switch(item->type) {
        /**
         * This ast is empty, therefore we assume there's no 
         * data allocated in it
         */
        case 0:
            break;
        case BTC_CONTAINER_GROUP:
            btc_destroy_container_group(item->container_group);
            break;
        case BTC_NAMESPACE:
            btc_destroy_namespace(item->namespace);
        case BTC_IMPORT_DECLARATION:
            break;
    }
    free(item);
}

void btc_initialize_ast_item(btc_ast_item** ast_item_ptr) {
    *ast_item_ptr = calloc(1, sizeof(btc_ast_item));
}
