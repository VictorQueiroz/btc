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
            user -> uint32 id, string name;\
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

    btc_ast_container_declaration* container = group->body->first_item->value->container;
    assert(strncmp(container->name.value, "user", 4) == 0);

    btc_linked_ast_item* param_ast = container->body->first_item;
    btc_ast_container_param* param = param_ast->value->container_param;

    assert(strncmp(param->name.value, "id", 2)==0);
    assert(strncmp(param->type->identifier.value, "uint32", 6)==0);

    param = param_ast->next_item->value->container_param;

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
                user -> uint32 id\
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
                user -> uint32 id;\
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

void test_container_template_single_argument() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
        type User {\
            user -> uint32 id, Vector<Post> posts\
        }\
        type Post {\
            post -> string title\
        }\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    btc_parse(parser);

    btc_linked_ast_item* linked_item = parser->result->first_item;
    btc_ast_item* item = linked_item->value;

    btc_ast_container_declaration* container = item->container_group->body->last_item->value->container;
    btc_ast_item* param_type = container->body->last_item->value->container_param->type;
    btc_template* template = param_type->template;
    assert(param_type->type == BTC_TEMPLATE);
    assert(strncmp(template->name.value, "Vector", strlen(template->name.value)) == 0);

    assert(strncmp(template->arguments->first_item->value->identifier.value, "Post", strlen(template->arguments->first_item->value->identifier.value)) == 0);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_container_template_multiple_arguments() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
        type User {\
            user -> uint32 id, Vector<Post, Statistic> postsAndStatistics\
        }\
        type Statistic {\
            statisticPost -> uint32 hitsCount\
        }\
        type Post {\
            post -> string title\
        }\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    btc_parse(parser);

    btc_linked_ast_item* linked_item = parser->result->first_item;
    btc_ast_item* item = linked_item->value;

    btc_ast_container_declaration* container = item->container_group->body->last_item->value->container;
    btc_ast_item* param_type = container->body->last_item->value->container_param->type;
    btc_template* template = param_type->template;
    assert(param_type->type == BTC_TEMPLATE);
    assert(strncmp(template->name.value, "Vector", strlen(template->name.value)) == 0);

    btc_linked_ast_item* first_template_argument = template->arguments->first_item;
    btc_linked_ast_item* second_template_argument = first_template_argument->next_item;
    assert(strncmp(first_template_argument->value->identifier.value, "Post", strlen(first_template_argument->value->identifier.value)) == 0);
    assert(strncmp(second_template_argument->value->identifier.value, "Statistic", strlen(second_template_argument->value->identifier.value)) == 0);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_container_template() {
    test_container_template_single_argument();
    test_container_template_multiple_arguments();
}

void test_container_member_expression() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
        type User {\
            user -> uint32 id, posts.Post lastPost\
        }\
        namespace posts {\
            type Post {\
                post -> string title\
            }\
        }\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    btc_parse(parser);

    btc_linked_ast_item* linked_item = parser->result->first_item;
    btc_ast_item* item = linked_item->value;

    btc_ast_container_declaration* container = item->container_group->body->first_item->value->container;
    btc_ast_container_param* param = container->body->last_item->value->container_param;
    btc_member_expression* expr = param->type->member_expression;

    assert(param->type->type == BTC_MEMBER_EXPRESSION);
    assert(strncmp(expr->left->identifier.value, "posts", 5) == 0);
    assert(strncmp(expr->right.value, "Post", 4) == 0);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_container_declaration() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
        type User {\
            user {\
                uint32 id;\
                Post lastPost;\
            }\
        }\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    btc_parse(parser);

    btc_linked_ast_item* linked = parser->result->first_item;

    // container group body
    linked = linked->value->container_group->body->first_item;

    // first linked container param
    linked = linked->value->container->body->first_item;

    assert(strncmp(linked->value->container_param->name.value, "id", 2) == 0);
    assert(strncmp(linked->value->container_param->type->identifier.value, "uint32", 6) == 0);

    // second linked container param
    linked = linked->next_item;

    assert(strncmp(linked->value->container_param->name.value, "lastPost", 2) == 0);
    assert(strncmp(linked->value->container_param->type->identifier.value, "Post", 4) == 0);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_container_param_default() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\
        type User {\
            user {\
                uint32 id = 1000;\
                double temperature = 26.5;\
                string name = \"empty name\";\
                string description = \"\";\
            }\
        }\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    btc_parse(parser);

    btc_linked_ast_item* container_params = parser->result->first_item->value->container_group->body->first_item->value->container->body->first_item;
    assert(container_params->value->container_param->default_value->number.value == 1000);

    container_params = container_params->next_item;
    assert(container_params->value->container_param->default_value->number.value == 26.5);

    container_params = container_params->next_item;
    assert(strncmp(container_params->value->container_param->default_value->string.value, "empty name", strlen("empty name")) == 0);

    container_params = container_params->next_item;
    assert(strncmp(container_params->value->container_param->default_value->string.value, "", strlen("")) == 0);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_parser() {
    test_container_group();
    test_container_import();
    test_container_template();
    test_container_declaration();
    test_container_param_default();
    test_container_member_expression();
    test_container_namespace();
}