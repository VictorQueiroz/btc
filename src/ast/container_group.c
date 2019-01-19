#include "container_group.h"
#include "containers_list.h"

#include <stdlib.h>

void btc_create_container_group(btc_ast_container_group_declaration** group_ptr) {
    *group_ptr = calloc(1, sizeof(btc_ast_container_group_declaration));
    btc_ast_container_group_declaration* group = *group_ptr;
    group->body = btc_ast_list_alloc();
}

void btc_destroy_container_group(btc_ast_container_group_declaration* group) {
    btc_ast_list_free(group->body);
    free(group);
}