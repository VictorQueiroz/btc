#ifndef TEST_TOKENIZER_H_
#define TEST_TOKENIZER_H_

#include "tokenizer.h"

void test_tokenizer();

void expect_token(btc_linked_token* linked, const char* string, int type);
void expect_identifier(btc_linked_token* linked, const char* string);
void expect_keyword(btc_linked_token* linked, const char* string);
void expect_punctuator(btc_linked_token* linked, const char* string);

#endif