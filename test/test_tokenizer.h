#ifndef TEST_TOKENIZER_H_
#define TEST_TOKENIZER_H_

#include "tokenizer.h"

void test_tokenizer();

void expect_token(btc_token* token, const char* string, int type);
void expect_identifier(btc_token* token, const char* string);
void expect_keyword(btc_token* token, const char* string);
void expect_punctuator(btc_token* token, const char* string);

#endif