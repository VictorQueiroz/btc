#include "container_params.h"
#include <stdlib.h>

void btc_create_container_params(btc_ast_container_params** body_ptr) {
    *body_ptr = calloc(1, sizeof(btc_ast_container_params));

    btc_ast_container_params* body = *body_ptr;
    body->first_param = NULL;
    body->last_param = NULL;
}

void btc_add_container_param(btc_ast_container_params* body, btc_ast_container_param* param) {
    if(body->first_param == NULL)
        body->first_param = param;

    if(body->last_param != NULL) {
        body->last_param->next_param = param;
        param->previous_param = body->last_param;
    }

    body->last_param = param;
}

void btc_initialize_container_param(btc_ast_container_param** param_ptr) {
    *param_ptr = calloc(1, sizeof(btc_ast_container_param));
}

void btc_destroy_container_params(btc_ast_container_params* params) {
    btc_ast_container_param* param = params->last_param;
    btc_ast_container_param* previous_param;

    while(param) {
        previous_param = param->previous_param;
        free(param);
        param = previous_param;
    }

    free(params);
}