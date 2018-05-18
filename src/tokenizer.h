#ifdef __cplusplus
extern "C" {
#endif

#ifndef BTC_TOKENIZER_H_
#define BTC_TOKENIZER_H_

#include <stdint.h>
#include <stddef.h>

#define BTC_TOKEN_PUNCTUATOR 1
#define BTC_TOKEN_IDENTIFIER 2
#define BTC_TOKEN_KEYWORD 3
#define BTC_TOKEN_LITERAL_STRING 4
#define BTC_TOKEN_COMMENT 5

struct _btc_token {
    int type;
    const char* value;
    char* allocated;
    struct _btc_token* next_token;
    struct _btc_token* previous_token;
};

typedef struct _btc_token btc_token;

struct _btc_tokenizer {
    btc_token* first_token;
    btc_token* last_token;
    uint8_t* buffer;
    const char* string;
    size_t offset;
    size_t string_length;
};

typedef struct _btc_tokenizer btc_tokenizer;


void btc_tokenizer_init(btc_tokenizer** tokenizer);
void btc_tokenizer_scan(btc_tokenizer* tokenizer, const char* string);
void btc_tokenizer_destroy(btc_tokenizer* tokenizer);

#endif

#ifdef __cplusplus
}
#endif