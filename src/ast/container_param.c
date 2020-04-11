#include "container_param.h"
#include "ast_item.h"

#include <stdlib.h>

void btc_initialize_container_param(btc_ast_container_param** param_ptr) {
    *param_ptr = calloc(1, sizeof(btc_ast_container_param));
    btc_ast_container_param* param = *param_ptr;
    btc_initialize_ast_item(&param->type);
}

void btc_destroy_container_param(btc_ast_container_param* param) {
    btc_destroy_ast_item(param->type);

    if(param->default_value != NULL)
        btc_destroy_ast_item(param->default_value);

    free(param);
}
