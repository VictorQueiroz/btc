#ifndef AST_ITEM_H_
#define AST_ITEM_H_

typedef struct _btc_ast_item btc_ast_item;

#include "parser_identifier.h"
#include "namespace.h"
#include "container_group.h"

struct _btc_ast_item {
    int type;

    btc_ast_container_group_declaration* container_group;
    btc_namespace* namespace;
};

void btc_initialize_ast_item(btc_ast_item** item_ptr);
void btc_destroy_ast_item(btc_ast_item* item);

#endif