#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "parser.h"
#include "tokenizer.h"
#include "test_parser.h"

void test_container_group() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
        type User {\
            user -> id: uint32, name: string;\
        }\
    ");

    btc_parser* parser;
    btc_ast_item* ast_item;
    btc_parser_init(&parser, tokenizer);

    btc_parse(parser);

    ast_item = parser->result->first_item->value;
    assert(ast_item->type == BTC_CONTAINER_GROUP);
    btc_ast_container_group_declaration* group = ast_item->container_group;
    assert(strncmp(group->type.value, "User", 4) == 0);

    btc_linked_container_declaration* linked_container = group->body->first_item;
    assert(strncmp(linked_container->container->name.value, "user", 4) == 0);
    assert(linked_container->next_item == NULL);

    btc_ast_container_param* param = linked_container->container->body->first_param;

    assert(strncmp(param->name.value, "id", 2)==0);
    assert(strncmp(param->type->identifier.value, "uint32", 6)==0);

    param = param->next_param;

    assert(strncmp(param->name.value, "name", 4)==0);
    assert(strncmp(param->type->identifier.value, "string", 6)==0);    

    assert(btc_parser_eof(parser) == 1);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_container_namespace() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
        namespace users {\
            type User {\
                user -> id: uint32\
            }\
        }\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    btc_parse(parser);

    btc_linked_ast_item* linked_item = parser->result->first_item;
    btc_ast_item* item = linked_item->value;

    assert(item->type == BTC_NAMESPACE);
    assert(strncmp(item->namespace->name.value, "users", 5) == 0);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_container_import() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
        import \"./default_schema.txt\";\
        namespace users {\
            type User {\
                user -> id: uint32;\
            }\
        }\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    btc_parse(parser);

    btc_linked_ast_item* linked_item = parser->result->first_item;
    btc_ast_item* item = linked_item->value;

    assert(item->type == BTC_IMPORT_DECLARATION);
    assert(strncmp(item->import_declaration.path.value, "./default_schema.txt", strlen("./default_schema.txt")) == 0);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_container_member_expression() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
        type User {\
            user -> id: uint32, lastPost: posts.Post\
        }\
        namespace posts {\
            type Post {\
                post -> title: string\
            }\
        }\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    btc_parse(parser);

    btc_linked_ast_item* linked_item = parser->result->first_item;
    btc_ast_item* item = linked_item->value;

    btc_ast_container_declaration* container = item->container_group->body->first_item->container;
    btc_ast_container_param* param = container->body->last_param;
    btc_member_expression* expr = param->type->member_expression;

    assert(param->type->type == BTC_MEMBER_EXPRESSION);
    assert(strncmp(expr->left->identifier.value, "posts", 5) == 0);
    assert(strncmp(expr->right.value, "Post", 4) == 0);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_parser() {
    test_container_group();
    test_container_import();
    test_container_member_expression();
    test_container_namespace();
}