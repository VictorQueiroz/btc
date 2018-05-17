#include "containers_list.h"

#include <stdlib.h>
#include <string.h>

void btc_initialize_linked_container(btc_linked_container_declaration** linked_ptr) {
    *linked_ptr = malloc(sizeof(btc_linked_container_declaration));
    btc_linked_container_declaration* linked = *linked_ptr;
    linked->container = NULL;
    linked->previous_item = NULL;
    linked->next_item = NULL;
}

void btc_destroy_linked_container(btc_linked_container_declaration* linked) {
    btc_destroy_container_declaration(linked->container);
    free(linked);
}

void btc_add_container_declaration(btc_ast_containers_list* list, btc_ast_container_declaration* container) {
    btc_linked_container_declaration* linked;
    btc_initialize_linked_container(&linked);

    linked->container = container;

    if(list->first_item == NULL)
        list->first_item = linked;

    if(list->last_item != NULL) {
        list->last_item->next_item = linked;
        linked->previous_item = list->last_item;
    }

    list->last_item = linked;
}

void btc_create_containers_list(btc_ast_containers_list** containers_ptr) {
    *containers_ptr = calloc(1, sizeof(btc_ast_containers_list));
}

void btc_destroy_containers_list(btc_ast_containers_list* list) {
    btc_linked_container_declaration* linked = list->last_item;
    btc_linked_container_declaration* previous_item;

    while(linked) {
        previous_item = linked->previous_item;
        btc_destroy_linked_container(linked);
        linked = previous_item;
    }

    free(list);
}