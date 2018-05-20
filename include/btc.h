#ifdef __cplusplus
extern "C" {
#endif

#ifndef BTC_H_
#define BTC_H_

/**
 * Positive status
 */
#define BTC_OK 1
/**
 * Unexpected end of stream
 */
#define BTC_UNEXPECTED_END -2
/**
 * Received unexpected token during parsing
 */
#define BTC_UNEXPECTED_TOKEN -3
/**
 * There is no token available for consuming
 */
#define BTC_NO_TOKEN -4

typedef struct _btc_parser btc_parser;
typedef struct _btc_token btc_token;
typedef struct _btc_tokenizer btc_tokenizer;

void btc_tokenizer_init(btc_tokenizer** tokenizer);
void btc_tokenizer_scan(btc_tokenizer* tokenizer, const char* string);
void btc_tokenizer_destroy(btc_tokenizer* tokenizer);
int btc_tokenizer_check_option(btc_tokenizer* tokenizer, int flag);
void btc_tokenizer_set_option(btc_tokenizer* tokenizer, int flag, int on);

void btc_parser_init(btc_parser** parser_ptr, btc_tokenizer* tokenizer);
void btc_parser_destroy(btc_parser* parser);
void btc_parse(btc_parser* parser);

#endif

#ifdef __cplusplus
}
#endif