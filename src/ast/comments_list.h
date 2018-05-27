#ifndef COMMENTS_LIST_H_
#define COMMENTS_LIST_H_

typedef struct _btc_comments_list btc_comments_list;
typedef struct _btc_linked_comment btc_linked_comment;

#include "comment.h"

struct _btc_linked_comment {
	struct _btc_comment comment;
	struct _btc_linked_comment* next_item;
	struct _btc_linked_comment* previous_item;
};

struct _btc_comments_list {
	struct _btc_linked_comment* first_item;
	struct _btc_linked_comment* last_item;
};

void btc_add_comment(btc_comments_list* list, btc_comment comment);
void btc_initialize_comments_list(btc_comments_list** list_ptr);

#endif