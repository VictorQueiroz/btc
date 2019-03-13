#include "comment.h"
#include "tokenizer/token.h"
#include "btc.h"

#include <stdlib.h>

btc_comment* btc_comment_alloc() {
    btc_comment* comment = malloc(sizeof(btc_comment)*1);

    comment->value = NULL;
    return comment;
}

btc_comment* btc_create_comment_from_token(btc_token* token) {
    if(token->type != BTC_TOKEN_MULTI_LINE_COMMENT && token->type != BTC_TOKEN_SINGLE_LINE_COMMENT) {
        return NULL;
    }

    btc_comment* comment = btc_comment_alloc();
    comment->value = token->value;
    comment->token_type = token->type;
    return comment;
}

void btc_comment_free(btc_comment* comment) {
    free(comment);
}
