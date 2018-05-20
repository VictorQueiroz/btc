#ifdef __cplusplus
extern "C" {
#endif

#ifndef BTC_TOKENIZER_H_
#define BTC_TOKENIZER_H_

#include "btc.h"
#include <stdint.h>
#include <stddef.h>

#define BTC_TOKEN_PUNCTUATOR 1
#define BTC_TOKEN_IDENTIFIER 2
#define BTC_TOKEN_KEYWORD 3
#define BTC_TOKEN_LITERAL_STRING 4
#define BTC_TOKEN_COMMENT 5
#define BTC_TOKEN_LITERAL_NUMBER 6

#define BTC_TOKENIZER_CONFIG_IGNORE_COMMENTS 1

struct _btc_token {
    int type;

    const char* value;
    char* allocated;
    double number;

    struct _btc_token* next_token;
    struct _btc_token* previous_token;
};

struct _btc_tokenizer {
    int flags;
    btc_token* first_token;
    btc_token* last_token;
    uint8_t* buffer;
    const char* string;
    size_t offset;
    size_t string_length;
};

#endif

#ifdef __cplusplus
}
#endif