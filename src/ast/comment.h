#ifndef BTC_COMMENT_H_
#define BTC_COMMENT_H_

#include "tokenizer/token.h"

#include <stdint.h>

typedef struct _btc_comment btc_comment;

struct _btc_comment {
    const char* value;
    /**
     * Could be either BTC_TOKEN_SINGLE_LINE_COMMENT or BTC_TOKEN_MULTI_LINE_COMMENT
     */
    int token_type;
};

btc_comment* btc_comment_alloc();
btc_comment* btc_create_comment_from_token(btc_token*);
void btc_comment_free(btc_comment*);

#endif
