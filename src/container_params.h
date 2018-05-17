#ifndef CONTAINER_PARAMS_H_
#define CONTAINER_PARAMS_H_

typedef struct _btc_ast_container_param btc_ast_container_param;
typedef struct _btc_ast_container_params btc_ast_container_params;

#include "parser_identifier.h"

struct _btc_ast_container_param {
    btc_ast_identifier name;
    btc_ast_identifier type;
    btc_ast_container_param* next_param;
    btc_ast_container_param* previous_param;
};

struct _btc_ast_container_params {
    btc_ast_container_param* first_param;
    btc_ast_container_param* last_param;
};

void btc_create_container_params(btc_ast_container_params** body_ptr);
void btc_destroy_container_params(btc_ast_container_params* params);
void btc_create_container_params(btc_ast_container_params** body_ptr);
void btc_add_container_param(btc_ast_container_params* body, btc_ast_container_param* param);
void btc_initialize_container_param(btc_ast_container_param** param_ptr);

#endif