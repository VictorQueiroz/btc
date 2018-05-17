#include "container_group.h"
#include "containers_list.h"

#include <stdlib.h>

void btc_create_container_group(btc_ast_container_group_declaration** group_ptr) {
    *group_ptr = malloc(sizeof(btc_ast_container_group_declaration));
    btc_ast_container_group_declaration* group = *group_ptr;
    btc_create_containers_list(&group->body);
}

void btc_destroy_container_group(btc_ast_container_group_declaration* group) {
    btc_destroy_containers_list(group->body);
    free(group);
}