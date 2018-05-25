#include "comments_list.h"

#include <stdlib.h>

void btc_initialize_comments_list(btc_comments_list** list_ptr) {
	*list_ptr = calloc(1, sizeof(btc_comments_list));
}