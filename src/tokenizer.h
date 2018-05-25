#ifndef BTC_TOKENIZER_H_
#define BTC_TOKENIZER_H_

typedef struct _btc_token btc_token;
typedef struct _btc_tokenizer btc_tokenizer;

#include "btc.h"
#include <stdint.h>
#include <stddef.h>
#include "tokens_list.h"

#define BTC_TOKEN_PUNCTUATOR 1
#define BTC_TOKEN_IDENTIFIER 2
#define BTC_TOKEN_KEYWORD 3
#define BTC_TOKEN_LITERAL_STRING 4
#define BTC_TOKEN_COMMENT 5
#define BTC_TOKEN_LITERAL_NUMBER 6

#define BTC_TOKENIZER_CONFIG_IGNORE_COMMENTS 1

struct _btc_tokenizer {
    int flags;
    btc_tokens_list* tokens_list;
    uint8_t* buffer;
    const char* string;
    size_t offset;
    size_t string_length;
    size_t line_number;
};

void btc_tokenizer_init(btc_tokenizer** tokenizer);
void btc_tokenizer_scan(btc_tokenizer* tokenizer, const char* string);
void btc_tokenizer_destroy(btc_tokenizer* tokenizer);
int btc_tokenizer_check_option(btc_tokenizer* tokenizer, int flag);
void btc_tokenizer_set_option(btc_tokenizer* tokenizer, int flag, int on);

#endif