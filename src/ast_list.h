#ifndef AST_LIST_H_
#define AST_LIST_H_

typedef struct _btc_ast_list btc_ast_list;

#include "ast_item.h"

typedef struct _btc_linked_ast_item {
    btc_ast_item* value;
    struct _btc_linked_ast_item* next_item;
    struct _btc_linked_ast_item* previous_item;
} btc_linked_ast_item;

struct _btc_ast_list {
    btc_linked_ast_item* first_item;
    btc_linked_ast_item* last_item;
    int length;
};

void btc_add_ast_item(btc_ast_list* list, btc_ast_item* item);
void btc_create_ast_list(btc_ast_list** list);
void btc_destroy_ast_list(btc_ast_list* list);

#endif