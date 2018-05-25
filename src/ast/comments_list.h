#ifndef COMMENTS_LIST_H_
#define COMMENTS_LIST_H_

typedef struct _btc_comments_list btc_comments_list;

struct _btc_comment {
	const char* value;
};

struct _btc_linked_comment {
	struct _btc_comment comment;
	struct _btc_linked_comment* next_item;
	struct _btc_linked_comment* previous_item;
};

struct _btc_comments_list {
	struct _btc_linked_comment* first_comment;
	struct _btc_linked_comment* last_comment;
};

#endif