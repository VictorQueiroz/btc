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

void expect_literal_string(btc_token* token, const char* string) {
    expect_token(token, string, BTC_TOKEN_LITERAL_STRING);
}

void expect_comment(btc_token* token, const char* string){
    expect_token(token, string, BTC_TOKEN_COMMENT);
}

void test_tokenizer_simple_container_group() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "type User {\
        user -> uint32 id;\
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
    expect_identifier(token, "uint32");

    token = token->next_token;
    expect_identifier(token, "id");

    token = token->next_token;
    expect_punctuator(token, ";");

    token = token->next_token;
    expect_punctuator(token, "}");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_namespace() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
        namespace users {\
            type User {\
                user -> uint32 id, string name\
            }\
        }\
    ");

    btc_token* token = tokenizer->first_token;
    expect_keyword(token, "namespace");

    token = token->next_token;
    expect_identifier(token, "users");

    token = token->next_token;
    expect_punctuator(token, "{");

    token = token->next_token;
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
    expect_identifier(token, "uint32");

    token = token->next_token;
    expect_identifier(token, "id");

    token = token->next_token;
    expect_punctuator(token, ",");

    token = token->next_token;
    expect_identifier(token, "string");

    token = token->next_token;
    expect_identifier(token, "name");

    token = token->next_token;
    expect_punctuator(token, "}");

    token = token->next_token;
    expect_punctuator(token, "}");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_string() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
        import \"./default.schema.txt\";\
    ");

    btc_token* token = tokenizer->first_token;
    expect_keyword(token, "import");

    token = token->next_token;
    expect_literal_string(token, "./default.schema.txt");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_test_template() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "vector<vector<uint32>>");

    btc_token* token = tokenizer->first_token;
    expect_identifier(token, "vector");

    token = token->next_token;
    expect_punctuator(token, "<");

    token = token->next_token;
    expect_identifier(token, "vector");

    token = token->next_token;
    expect_punctuator(token, "<");

    token = token->next_token;
    expect_identifier(token, "uint32");

    token = token->next_token;
    expect_punctuator(token, ">");

    token = token->next_token;
    expect_punctuator(token, ">");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_member_expression() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "posts.Post");

    btc_token* token = tokenizer->first_token;
    expect_identifier(token, "posts");

    token = token->next_token;
    expect_punctuator(token, ".");

    token = token->next_token;
    expect_identifier(token, "Post");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_test_comment() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
    /**\
     * test comment \
     */\
    ");

    btc_token* token = tokenizer->first_token;
    expect_comment(token, "*\
     * test comment \
     ");

    btc_tokenizer_destroy(tokenizer);
}


void test_tokenizer_test_ignore_comment() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);
    btc_tokenizer_set_option(tokenizer, BTC_TOKENIZER_CONFIG_IGNORE_COMMENTS, 1);

    btc_tokenizer_scan(tokenizer, "\
    /**\
     * test comment \
     */\
    ");

    assert(tokenizer->first_token == NULL);

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_flags() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    assert(btc_tokenizer_check_option(tokenizer, BTC_TOKENIZER_CONFIG_IGNORE_COMMENTS) == 0);

    btc_tokenizer_set_option(tokenizer, BTC_TOKENIZER_CONFIG_IGNORE_COMMENTS, 1);
    assert(btc_tokenizer_check_option(tokenizer, BTC_TOKENIZER_CONFIG_IGNORE_COMMENTS) == 1);

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer() {
    test_tokenizer_namespace();
    test_tokenizer_simple_container_group();
    test_tokenizer_string();
    test_tokenizer_flags();
    test_tokenizer_member_expression();
    test_tokenizer_test_template();
    test_tokenizer_test_comment();
    test_tokenizer_test_ignore_comment();
}