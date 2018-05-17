#ifndef BTC_TEMPLATE_H_
#define BTC_TEMPLATE_H_

#define BTC_TEMPLATE 8

typedef struct _btc_template btc_template;

#include "ast_list.h"

struct _btc_template {
    btc_ast_identifier name;
    btc_ast_list* arguments;
};

void btc_initialize_template(btc_template** tmpl_ptr);
void btc_destroy_template(btc_template* tmpl);

#endif