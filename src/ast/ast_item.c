#include "ast_item.h"
#include "container_group.h"
#include "template.h"

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
        case BTC_MEMBER_EXPRESSION:
            btc_destroy_member_expression(item->member_expression);
            break;
        case BTC_NAMESPACE:
            btc_destroy_namespace(item->namespace_item);
            break;
        case BTC_ALIAS:
            btc_destroy_ast_item(item->alias.value);
            break;
        case BTC_TEMPLATE:
            btc_destroy_template(item->template_item);
            break;
        case BTC_CONTAINER_DECLARATION:
            btc_destroy_container_declaration(item->container);
            break;
        case BTC_CONTAINER_PARAM:
            btc_destroy_container_param(item->container_param);
            break;
        case BTC_IMPORT_DECLARATION:
        case BTC_IDENTIFIER:
        case BTC_NUMBER:
        case BTC_STRING:
            break;
        default:
            fprintf(stderr, "unrecognized ast item type of %d\n", item->type);
    }

    btc_comments_list_free(item->leading_comments);
    btc_comments_list_free(item->trailing_comments);

    free(item);
}

void btc_initialize_ast_item(btc_ast_item** ast_item_ptr) {
    *ast_item_ptr = calloc(1, sizeof(btc_ast_item));
    (*ast_item_ptr)->leading_comments = btc_comments_list_alloc();
    (*ast_item_ptr)->trailing_comments = btc_comments_list_alloc();
}
