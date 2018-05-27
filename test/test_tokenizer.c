#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "tokenizer.h"
#include "test_tokenizer.h"

void expect_token(btc_linked_token* linked, const char* string, int type) {
    btc_token* token = linked->value;
    assert(token->type == type);
    assert(strncmp(token->value, string, strlen(string)) == 0);
}

void expect_identifier(btc_linked_token* linked, const char* string) {
    expect_token(linked, string, BTC_TOKEN_IDENTIFIER);
}

void expect_keyword(btc_linked_token* linked, const char* string) {
    expect_token(linked, string, BTC_TOKEN_KEYWORD);
}

void expect_punctuator(btc_linked_token* linked, const char* string) {
    expect_token(linked, string, BTC_TOKEN_PUNCTUATOR);
}

void expect_literal_string(btc_linked_token* linked, const char* string) {
    expect_token(linked, string, BTC_TOKEN_LITERAL_STRING);
}

void expect_comment(btc_linked_token* linked, const char* string){
    expect_token(linked, string, BTC_TOKEN_COMMENT);
}

void expect_number(btc_linked_token* linked, double number) {
    btc_token* token = linked->value;
    assert(token->type == BTC_TOKEN_LITERAL_NUMBER);
    assert(token->number == number);
}

void test_tokenizer_simple_container_group() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    int status = btc_tokenizer_scan(tokenizer, "type User {\
        user -> uint32 id;\
    }");

    assert(status == BTC_OK);

    btc_linked_token* token = tokenizer->tokens_list->first_item;
    expect_keyword(token, "type");

    token = token->next_item;
    expect_identifier(token, "User");

    token = token->next_item;
    expect_punctuator(token, "{");

    token = token->next_item;
    expect_identifier(token, "user");

    token = token->next_item;
    expect_punctuator(token, "->");

    token = token->next_item;
    expect_identifier(token, "uint32");

    token = token->next_item;
    expect_identifier(token, "id");

    token = token->next_item;
    expect_punctuator(token, ";");

    token = token->next_item;
    expect_punctuator(token, "}");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_namespace() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    int status = btc_tokenizer_scan(tokenizer, "\
        namespace users {\
            type User {\
                user -> uint32 id, string name\
            }\
        }\
    ");

    assert(status == BTC_OK);

    btc_linked_token* token = tokenizer->tokens_list->first_item;
    expect_keyword(token, "namespace");

    token = token->next_item;
    expect_identifier(token, "users");

    token = token->next_item;
    expect_punctuator(token, "{");

    token = token->next_item;
    expect_keyword(token, "type");

    token = token->next_item;
    expect_identifier(token, "User");

    token = token->next_item;
    expect_punctuator(token, "{");

    token = token->next_item;
    expect_identifier(token, "user");

    token = token->next_item;
    expect_punctuator(token, "->");

    token = token->next_item;
    expect_identifier(token, "uint32");

    token = token->next_item;
    expect_identifier(token, "id");

    token = token->next_item;
    expect_punctuator(token, ",");

    token = token->next_item;
    expect_identifier(token, "string");

    token = token->next_item;
    expect_identifier(token, "name");

    token = token->next_item;
    expect_punctuator(token, "}");

    token = token->next_item;
    expect_punctuator(token, "}");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_string() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    int status = btc_tokenizer_scan(tokenizer, "\
        import \"./default.schema.txt\";\
    ");

    assert(status == BTC_OK);

    btc_linked_token* token = tokenizer->tokens_list->first_item;
    expect_keyword(token, "import");

    token = token->next_item;
    expect_literal_string(token, "./default.schema.txt");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_test_template() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    assert(btc_tokenizer_scan(tokenizer, "vector<vector<uint32>>") == BTC_OK);

    btc_linked_token* token = tokenizer->tokens_list->first_item;
    expect_identifier(token, "vector");

    token = token->next_item;
    expect_punctuator(token, "<");

    token = token->next_item;
    expect_identifier(token, "vector");

    token = token->next_item;
    expect_punctuator(token, "<");

    token = token->next_item;
    expect_identifier(token, "uint32");

    token = token->next_item;
    expect_punctuator(token, ">");

    token = token->next_item;
    expect_punctuator(token, ">");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_member_expression() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "posts.Post");

    btc_linked_token* token = tokenizer->tokens_list->first_item;
    expect_identifier(token, "posts");

    token = token->next_item;
    expect_punctuator(token, ".");

    token = token->next_item;
    expect_identifier(token, "Post");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_test_comment() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    int status = btc_tokenizer_scan(tokenizer, "\
    // one line comment \n\
    /**\
     * test comment \
     */\
    // last one line comment \n\
    ");

    assert(status == BTC_OK);

    btc_linked_token* token = tokenizer->comments_list->first_item;
    expect_comment(token, " one line comment ");

    token = token->next_item;
    expect_comment(token, "*\
     * test comment \
     ");

    token = token->next_item;
    expect_comment(token, " last one line comment ");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_test_number() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);
    btc_tokenizer_set_option(tokenizer, BTC_TOKENIZER_CONFIG_IGNORE_COMMENTS, 1);

    int status = btc_tokenizer_scan(tokenizer, "\
        1010101010\
        -1.0\
        21321654\
        -0.0000000123\
    ");

    assert(status == BTC_OK);

    btc_linked_token* token = tokenizer->tokens_list->first_item;
    expect_number(token, 1010101010);

    token = token->next_item;
    expect_number(token, -1.0);

    token = token->next_item;
    expect_number(token, 21321654);

    token = token->next_item;
    expect_number(token, -0.0000000123);

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_test_ignore_comment() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);
    btc_tokenizer_set_option(tokenizer, BTC_TOKENIZER_CONFIG_IGNORE_COMMENTS, 1);

    int status = btc_tokenizer_scan(tokenizer, "\
    /**\
     * test comment \
     */\
    ");

    assert(status == BTC_OK);
    assert(tokenizer->tokens_list->first_item == NULL);

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_alias() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    int status = btc_tokenizer_scan(tokenizer, "\
        alias Buffer = Vector<Uint8>;\
        alias ObjectId = StrictSize<Buffer, 12>;\
    ");

    assert(status == BTC_OK);

    btc_linked_token* token = tokenizer->tokens_list->first_item;
    expect_keyword(token, "alias");

    token = token->next_item;
    expect_identifier(token, "Buffer");

    token = token->next_item;
    expect_punctuator(token, "=");

    token = token->next_item;
    expect_identifier(token, "Vector");

    token = token->next_item;
    expect_punctuator(token, "<");

    token = token->next_item;
    expect_identifier(token, "Uint8");

    token = token->next_item;
    expect_punctuator(token, ">");

    token = token->next_item;
    expect_punctuator(token, ";");

    token = token->next_item;
    expect_keyword(token, "alias");

    token = token->next_item;
    expect_identifier(token, "ObjectId");

    token = token->next_item;
    expect_punctuator(token, "=");

    token = token->next_item;
    expect_identifier(token, "StrictSize");

    token = token->next_item;
    expect_punctuator(token, "<");

    token = token->next_item;
    expect_identifier(token, "Buffer");

    token = token->next_item;
    expect_punctuator(token, ",");

    token = token->next_item;
    expect_number(token, 12);

    token = token->next_item;
    expect_punctuator(token, ">");

    token = token->next_item;
    expect_punctuator(token, ";");

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
    test_tokenizer_test_number();
    test_tokenizer_flags();
    test_tokenizer_alias();
    test_tokenizer_member_expression();
    test_tokenizer_test_template();
    test_tokenizer_test_comment();
    test_tokenizer_test_ignore_comment();
}