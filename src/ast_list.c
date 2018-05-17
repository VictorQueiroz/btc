#include "ast_list.h"
#include <stdlib.h>

void btc_create_ast_list(btc_ast_list** list_ptr) {
    *list_ptr = calloc(1, sizeof(btc_ast_list));
}

/**
 * Add new ast item to linked list
 */
void btc_add_ast_item(btc_ast_list* list, btc_ast_item* item) {
    btc_linked_ast_item* linked_list = calloc(1, sizeof(btc_linked_ast_item));
    linked_list->value = item;

    if(list->first_item == NULL)
        list->first_item = linked_list;

    if(list->last_item != NULL){
        list->last_item->next_item = linked_list;
        linked_list->previous_item = list->last_item;
    }

    list->last_item = linked_list;
    ++list->length;
}

void btc_destroy_ast_list(btc_ast_list* list) {
    btc_linked_ast_item* item = list->last_item;
    btc_linked_ast_item* previous_item;

    while(item) {
        previous_item = item->previous_item;
        btc_destroy_ast_item(item->value);
        free(item);
        item = previous_item;
    }

    free(list);
}
