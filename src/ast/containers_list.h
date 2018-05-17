#ifndef CONTAINERS_LIST_H_
#define CONTAINERS_LIST_H_

typedef struct _btc_linked_container_declaration btc_linked_container_declaration;
typedef struct _btc_ast_containers_list btc_ast_containers_list;

#include "container_declaration.h"

struct _btc_linked_container_declaration {
    btc_ast_container_declaration* container;
    struct _btc_linked_container_declaration* next_item;
    struct _btc_linked_container_declaration* previous_item;
};

struct _btc_ast_containers_list {
    struct _btc_linked_container_declaration* first_item;
    struct _btc_linked_container_declaration* last_item;
};

void btc_add_container_declaration(btc_ast_containers_list* list, btc_ast_container_declaration* container);
void btc_create_containers_list(btc_ast_containers_list** containers_ptr);
void btc_destroy_containers_list(btc_ast_containers_list* list);

#endif