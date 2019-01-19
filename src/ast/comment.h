#ifndef BTC_COMMENT_H_
#define BTC_COMMENT_H_

typedef struct _btc_comment btc_comment;

struct _btc_comment {
    const char* value;
};

btc_comment* btc_comment_alloc();
void btc_comment_free(btc_comment*);

#endif
