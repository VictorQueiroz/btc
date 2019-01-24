#include "comment.h"

#include <stdlib.h>

btc_comment* btc_comment_alloc() {
    btc_comment* comment = malloc(sizeof(btc_comment)*1);

    return comment;
}

void btc_comment_free(btc_comment* comment) {
    free(comment);
}
