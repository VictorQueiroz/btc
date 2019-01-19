#include "tokens_list.h"
#include "token.h"

#include <stdlib.h>

CVECTOR_NEW(btc_tokens_list, btc_token*)

void btc_tokens_list_free_items(btc_tokens_list* list) {
	btc_token* token;
	vector_foreach(list, i) {
		token = btc_tokens_list_get(list, i);
		btc_destroy_token(token);
	}
}
