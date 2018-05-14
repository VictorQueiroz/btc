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

    ast_item = parser->first_item;
    assert(ast_item->type == BTC_CONTAINER_GROUP);
    btc_ast_container_group_declaration group = ast_item->container_group;
    assert(strncmp(group.type.value, "User", 4) == 0);

    btc_linked_container_declaration* linked_container = group.body->first_item;
    assert(strncmp(linked_container->container.name.value, "user", 4) == 0);
    assert(linked_container->next_item == NULL);

    btc_ast_container_param* param = linked_container->container.body->first_param;

    assert(strncmp(param->name.value, "id", 2)==0);
    assert(strncmp(param->type.value, "uint32", 6)==0);

    param = param->next_param;

    assert(strncmp(param->name.value, "name", 4)==0);
    assert(strncmp(param->type.value, "string", 6)==0);    

    assert(btc_parser_eof(parser) == 1);

    btc_tokenizer_destroy(tokenizer);
    btc_parser_destroy(parser);
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

    btc_tokenizer_destroy(tokenizer);
}

void test_parser() {
    test_container_group();
    test_container_namespace();
}