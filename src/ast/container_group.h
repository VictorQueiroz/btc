#ifndef CONTAINER_GROUP_H_
#define CONTAINER_GROUP_H_

typedef struct _btc_ast_container_group_declaration btc_ast_container_group_declaration;

#include "containers_list.h"
#include "parser_identifier.h"

#define BTC_CONTAINER_GROUP 1

struct _btc_ast_container_group_declaration {
    btc_ast_identifier type;
    /**
     * Common list of container declarations without `type` 
     * param
     */
    btc_ast_containers_list* body;
};

void btc_destroy_container_group(btc_ast_container_group_declaration* group);
void btc_create_container_group(btc_ast_container_group_declaration** group_ptr);

#endif