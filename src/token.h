#ifndef BTC_TOKEN_H_
#define BTC_TOKEN_H_

typedef struct _btc_token btc_token;

struct _btc_token {
    int type;

    const char* value;
    char* allocated;
    double number;
};

void btc_destroy_token(btc_token* token);

#endif