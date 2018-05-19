#include "container_group.h"
#include "containers_list.h"

#include <stdlib.h>

void btc_create_container_group(btc_ast_container_group_declaration** group_ptr) {
    *group_ptr = calloc(1, sizeof(btc_ast_container_group_declaration));
    btc_ast_container_group_declaration* group = *group_ptr;
    btc_create_ast_list(&group->body);
}

void btc_destroy_container_group(btc_ast_container_group_declaration* group) {
    btc_destroy_ast_list(group->body);
    free(group);
}