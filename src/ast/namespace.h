#ifndef NAMESPACE_H_
#define NAMESPACE_H_

typedef struct _btc_namespace btc_namespace;

#include "ast_list.h"
#include "parser_identifier.h"

#define BTC_NAMESPACE 2

struct _btc_namespace {
    /**
     * Namespace identifier
     */
    btc_ast_identifier name;
    btc_ast_list* body;
};

void btc_destroy_namespace(btc_namespace* namespace_item);
void btc_initialize_namespace(btc_namespace** namespace_ptr);

#endif
