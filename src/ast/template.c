#include "template.h"
#include "ast_list.h"

#include <stdlib.h>

void btc_initialize_template(btc_template** tmpl_ptr) {
    *tmpl_ptr = calloc(1, sizeof(tmpl_ptr));
    btc_create_ast_list(&(*tmpl_ptr)->arguments);
}

void btc_destroy_template(btc_template* tmpl) {
    btc_destroy_ast_list(tmpl->arguments);
}