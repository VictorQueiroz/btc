#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "tokenizer.h"
#include "test_tokenizer.h"

void expect_token(btc_token* token, const char* string, int type) {
    assert(token->type == type);
    assert(strncmp(token->value, string, strlen(string)) == 0);
}

void expect_identifier(btc_token* token, const char* string) {
    expect_token(token, string, BTC_TOKEN_IDENTIFIER);
}

void expect_keyword(btc_token* token, const char* string) {
    expect_token(token, string, BTC_TOKEN_KEYWORD);
}

void expect_punctuator(btc_token* token, const char* string) {
    expect_token(token, string, BTC_TOKEN_PUNCTUATOR);
}

void test_tokenizer() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "type User {\
        user -> id: uint;\
    }");

    btc_token* token = tokenizer->first_token;
    expect_keyword(token, "type");

    token = token->next_token;
    expect_identifier(token, "User");

    token = token->next_token;
    expect_punctuator(token, "{");

    token = token->next_token;
    expect_identifier(token, "user");

    token = token->next_token;
    expect_punctuator(token, "->");

    token = token->next_token;
    expect_identifier(token, "id");

    token = token->next_token;
    expect_punctuator(token, ":");

    token = token->next_token;
    expect_identifier(token, "uint");

    token = token->next_token;
    expect_punctuator(token, ";");

    token = token->next_token;
    expect_punctuator(token, "}");

    btc_tokenizer_destroy(tokenizer);
}