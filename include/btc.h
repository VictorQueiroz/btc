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

#include "../src/tokenizer.h"
#include "../src/parser.h"

#endif

#ifdef __cplusplus
}
#endif