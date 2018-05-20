#ifndef CONTAINER_PARAM_H_
#define CONTAINER_PARAM_H_

#define BTC_CONTAINER_PARAM 11

typedef struct _btc_ast_container_param btc_ast_container_param;

#include "ast_item.h"

struct _btc_ast_container_param {
    btc_ast_identifier name;
    btc_ast_item* type;
    /**
     * Parameter default value when 
     * no value is provided
     */
    btc_ast_item* default_value;
};

void btc_initialize_container_param(btc_ast_container_param** param_ptr);
void btc_destroy_container_param(btc_ast_container_param* param);

#endif
