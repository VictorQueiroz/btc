#include "comments_list.h"

#include <stdlib.h>

void btc_initialize_comments_list(btc_comments_list** list_ptr) {
	*list_ptr = calloc(1, sizeof(btc_comments_list));
}

void btc_add_comment(btc_comments_list* list, btc_comment comment) {
    btc_linked_comment* linked = calloc(1, sizeof(btc_linked_comment));

    linked->comment = comment;

    if(list->first_item == NULL)
        list->first_item = linked;

    if(list->last_item != NULL) {
        list->last_item->next_item = linked;
        linked->previous_item = list->last_item;
    }

    list->last_item = linked;
}