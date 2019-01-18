#include "tokenizer/tokenizer.h"
#include "test_tokenizer.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

void expect_token(btc_token* token, const char* string, int type) {
    if(token->type != type) {
        fprintf(
            stderr,
            "expected %s \"%s\" but got %s \"%s\" instead\n",
            btc_token_type_to_readable(type),
            string,
            btc_token_type_to_readable(token->type),
            token->value
        );
    }
    assert(token->type == type);
    assert(strncmp(token->value, string, strlen(string)) == 0);
}

/**
 * Assert that token has the appropriate range
 */
void expect_token_range(btc_token* token, int start_ln, int end_ln, int start_o, int end_o) {
    assert(token->range.start_line_number == start_ln);
    assert(token->range.end_line_number == end_ln);
    assert(token->range.start_offset == start_o);
    assert(token->range.end_offset == end_o);
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

void expect_number(btc_token* token, double number) {
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

    btc_tokens_list* tokens_list = tokenizer->tokens_list;
    size_t current_index = 0;
    btc_token* token = btc_tokens_list_get(tokens_list, current_index++);
    expect_keyword(token, "type");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_identifier(token, "User");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_punctuator(token, "{");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_identifier(token, "user");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_punctuator(token, "->");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_identifier(token, "uint32");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_identifier(token, "id");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_punctuator(token, ";");

    token = btc_tokens_list_get(tokens_list, current_index);
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

    btc_tokens_list* tokens_list = tokenizer->tokens_list;
    btc_token* token;
    size_t current_token = 0;

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_keyword(token, "namespace");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_identifier(token, "users");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_punctuator(token, "{");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_keyword(token, "type");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_identifier(token, "User");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_punctuator(token, "{");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_identifier(token, "user");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_punctuator(token, "->");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_identifier(token, "uint32");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_identifier(token, "id");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_punctuator(token, ",");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_identifier(token, "string");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_identifier(token, "name");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_punctuator(token, "}");

    token = btc_tokens_list_get(tokens_list, current_token);
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

    btc_tokens_list* tokens_list = tokenizer->tokens_list;
    btc_token* token = NULL;
    size_t tokens_index = 0;

    token = btc_tokens_list_get(tokens_list, tokens_index++);
    expect_keyword(token, "import");

    token = btc_tokens_list_get(tokens_list, tokens_index);
    expect_literal_string(token, "./default.schema.txt");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_test_template() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    assert(btc_tokenizer_scan(tokenizer, "vector<vector<uint32>>") == BTC_OK);

    btc_tokens_list* tokens_list = tokenizer->tokens_list;
    btc_token* token;
    size_t current_token = 0;
    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_identifier(token, "vector");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_punctuator(token, "<");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_identifier(token, "vector");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_punctuator(token, "<");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_identifier(token, "uint32");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_punctuator(token, ">");

    token = btc_tokens_list_get(tokens_list, current_token);
    expect_punctuator(token, ">");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_member_expression() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "posts.Post");

    btc_tokens_list* tokens_list = tokenizer->tokens_list;
    size_t current_token = 0;
    btc_token* token = btc_tokens_list_get(tokens_list, current_token++);
    expect_identifier(token, "posts");

    token = btc_tokens_list_get(tokens_list, current_token++);
    expect_punctuator(token, ".");

    token = btc_tokens_list_get(tokens_list, current_token);
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

    btc_tokens_list* tokens_list = tokenizer->comments_list;
    btc_token* token = btc_tokens_list_get(tokens_list, 0);
    expect_comment(token, " one line comment ");

    token = btc_tokens_list_get(tokens_list, 1);
    expect_comment(token, "*\
     * test comment \
     ");

    token = btc_tokens_list_get(tokens_list, 2);
    expect_comment(token, " last one line comment ");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_test_number() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    int status = btc_tokenizer_scan(tokenizer, "\
        1010101010\
        -1.0\
        21321654\
        -0.0000000123\
    ");

    assert(status == BTC_OK);

    btc_tokens_list* tokens_list = tokenizer->tokens_list;
    btc_token* token = btc_tokens_list_get(tokens_list, 0);
    expect_number(token, 1010101010);

    token = btc_tokens_list_get(tokens_list, 1);
    expect_number(token, -1.0);

    token = btc_tokens_list_get(tokens_list, 2);
    expect_number(token, 21321654);

    token = btc_tokens_list_get(tokens_list, 3);
    expect_number(token, -0.0000000123);

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

    btc_tokens_list* tokens_list = tokenizer->tokens_list;
    size_t current_index = 0;
    btc_token* token = btc_tokens_list_get(tokens_list, current_index++);
    expect_keyword(token, "alias");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_identifier(token, "Buffer");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_punctuator(token, "=");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_identifier(token, "Vector");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_punctuator(token, "<");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_identifier(token, "Uint8");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_punctuator(token, ">");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_punctuator(token, ";");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_keyword(token, "alias");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_identifier(token, "ObjectId");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_punctuator(token, "=");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_identifier(token, "StrictSize");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_punctuator(token, "<");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_identifier(token, "Buffer");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_punctuator(token, ",");

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_number(token, 12);

    token = btc_tokens_list_get(tokens_list, current_index++);
    expect_punctuator(token, ">");

    token = btc_tokens_list_get(tokens_list, current_index);
    expect_punctuator(token, ";");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_line_number() {
    int status;
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    status = btc_tokenizer_scan(tokenizer, "\
    // comment 1\n\
    // comment 2\n\
    /**\n\
     * This is a simple testing \n\
     * comment\n\
     */\
    ");
    assert(status == BTC_OK);

    btc_tokens_list* tokens_list = tokenizer->comments_list;
    btc_token* linked = btc_tokens_list_get(tokens_list, 0);
    expect_comment(linked, " comment 1");
    expect_token_range(linked, 0, 0, 4, 16);

    linked = btc_tokens_list_get(tokens_list, 1);
    expect_comment(linked, " comment 2");
    expect_token_range(linked, 1, 1, 21, 33);

    linked = btc_tokens_list_get(tokens_list, 2);
    expect_comment(linked, "*\n\
     * This is a simple testing \n\
     * comment\n\
     ");
    expect_token_range(linked, 2, 5, 38, 97);

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer_identifier() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    assert(btc_tokenizer_scan(tokenizer, "_user _user_ us_er") == BTC_OK);

    btc_tokens_list* tokens_list = tokenizer->tokens_list;
    size_t current_offset = 0;
    btc_token* linked = btc_tokens_list_get(tokens_list, current_offset++);
    expect_identifier(linked, "_user");

    linked = btc_tokens_list_get(tokens_list, current_offset++);
    expect_identifier(linked, "_user_");

    linked = btc_tokens_list_get(tokens_list, current_offset);
    expect_identifier(linked, "us_er");

    btc_tokenizer_destroy(tokenizer);
}

void test_tokenizer() {
    test_tokenizer_namespace();
    test_tokenizer_simple_container_group();
    test_tokenizer_string();
    test_tokenizer_test_number();
    test_tokenizer_alias();
    test_tokenizer_member_expression();
    test_tokenizer_test_template();
    test_tokenizer_test_comment();
    test_tokenizer_line_number();
    test_tokenizer_identifier();
}