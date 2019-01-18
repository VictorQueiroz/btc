#ifndef TEST_TOKENIZER_H_
#define TEST_TOKENIZER_H_

#include "token.h"

void test_tokenizer();

void expect_token(btc_token*, const char*, int);
void expect_identifier(btc_token*, const char*);
void expect_keyword(btc_token*, const char*);
void expect_punctuator(btc_token*, const char*);

#endif