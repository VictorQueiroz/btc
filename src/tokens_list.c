#include "tokens_list.h"
#include "token.h"

#include <stdlib.h>

void btc_initialize_tokens_list(btc_tokens_list** list_ptr) {
	*list_ptr = calloc(1, sizeof(btc_tokens_list));
}

void btc_add_token(btc_tokens_list* list, btc_token* token) {
	btc_linked_token* linked = calloc(1, sizeof(btc_linked_token));
	linked->value = token;

	if(list->first_item == NULL)
		list->first_item = linked;

	if(list->last_item != NULL) {
		list->last_item->next_item = linked;
		linked->previous_item = list->last_item;
	}

	list->last_item = linked;
}

void btc_destroy_tokens_list(btc_tokens_list* list) {
	btc_linked_token* linked = list->last_item;
	btc_linked_token* previous_linked;

	while(linked) {
		previous_linked = linked->previous_item;
		btc_destroy_token(linked->value);
		free(linked);
		linked = previous_linked;
	}

	free(list);
}