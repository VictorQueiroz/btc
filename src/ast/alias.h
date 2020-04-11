#ifndef BTC_ALIAS_H_
#define BTC_ALIAS_H_

#define BTC_ALIAS 13

typedef struct _btc_alias btc_alias;

#include "parser_identifier.h"

struct _btc_alias {
    btc_ast_identifier name;
    btc_ast_item* value;
};

#endif
