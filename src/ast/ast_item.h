#ifndef AST_ITEM_H_
#define AST_ITEM_H_

typedef struct _btc_ast_item btc_ast_item;

#include "parser_identifier.h"
#include "namespace.h"
#include "container_group.h"
#include "import_declaration.h"
#include "template.h"
#include "member_expression.h"
#include "container_declaration.h"
#include "container_param.h"
#include "string.h"
#include "number.h"

struct _btc_ast_item {
    int type;

    btc_string string;
    btc_number number;
    btc_ast_identifier identifier;
    btc_ast_container_group_declaration* container_group;
    btc_ast_container_declaration* container;
    btc_namespace* namespace_item;
    btc_template* template_item;
    btc_ast_container_param* container_param;
    btc_import_declaration import_declaration;
    btc_member_expression* member_expression;
};

void btc_initialize_ast_item(btc_ast_item** item_ptr);
void btc_destroy_ast_item(btc_ast_item* item);

#endif