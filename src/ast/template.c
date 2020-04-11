#include "template.h"
#include "ast_list.h"

#include <stdlib.h>

void btc_initialize_template(btc_template** tmpl_ptr) {
    *tmpl_ptr = calloc(1, sizeof(btc_template));
    (*tmpl_ptr)->arguments = btc_ast_list_alloc();
}

void btc_destroy_template(btc_template* tmpl) {
    btc_ast_list_free(tmpl->arguments);
    free(tmpl);
}
