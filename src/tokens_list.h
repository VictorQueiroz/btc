#ifndef BTC_TOKENS_LIST_H_
#define BTC_TOKENS_LIST_H_

typedef struct _btc_tokens_list btc_tokens_list;
typedef struct _btc_linked_token btc_linked_token;

#include "token.h"
#include "tokenizer.h"

struct _btc_linked_token {
	btc_token* value;
    struct _btc_linked_token* next_item;
    struct _btc_linked_token* previous_item;
};

struct _btc_tokens_list {
	struct _btc_linked_token* first_item;
	struct _btc_linked_token* last_item;
};

void btc_destroy_tokens_list(btc_tokens_list* list);
void btc_initialize_tokens_list(btc_tokens_list** list_ptr);
void btc_add_token(btc_tokens_list* list, btc_token* token);

#endif