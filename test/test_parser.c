#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "parser.h"
#include "tokenizer/tokenizer.h"
#include "test_parser.h"

void print_failure(btc_parser* parser, int error) {
    btc_token* token = btc_tokens_list_get(parser->tokens_list, parser->current_token);
    fprintf(stderr, "failure at line %lu (token value = %s, error = %d)\n", token->range.end_line_number, token->value, error);
}

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

    int status = btc_parse(parser);
    if(status != BTC_OK) print_failure(parser, status);
    assert(status == BTC_OK);

    ast_item = CVECTOR_GET(parser->result, 0);
    assert(btc_ast_list_length(parser->result) == 1);
    assert(ast_item != NULL);
    assert(ast_item->type == BTC_CONTAINER_GROUP);
    btc_ast_container_group_declaration* group = ast_item->container_group;
    assert(strncmp(group->type.value, "User", 4) == 0);

    btc_ast_container_declaration* container = CVECTOR_GET(group->body, 0)->container;
    assert(strncmp(container->name.value, "user", 4) == 0);

    uint32_t param_ast_index = 0;
    btc_ast_container_param* param = CVECTOR_GET(container->body, param_ast_index++)->container_param;

    assert(strncmp(param->name.value, "id", 2)==0);
    assert(strncmp(param->type->identifier.value, "uint32", 6)==0);

    param = CVECTOR_GET(container->body, param_ast_index++)->container_param;

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

    assert(btc_parse(parser) == BTC_OK);

    btc_ast_list* ast_list = parser->result;
    uint32_t item_index = 0;
    btc_ast_item* item = ast_list->data[item_index++];

    assert(item->type == BTC_NAMESPACE);
    assert(strncmp(item->namespace_item->name.value, "users", 5) == 0);

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

    assert(btc_parse(parser) == BTC_OK);

    btc_ast_list* list = parser->result;
    btc_ast_item* item = btc_ast_list_get(list, 0);

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

    assert(btc_parse(parser) == BTC_OK);

    btc_ast_list* ast_list = parser->result;
    btc_ast_item* item = btc_ast_list_get(ast_list, 0);

    btc_ast_container_declaration* container = btc_ast_list_get(item->container_group->body, item->container_group->body->offset - 1)->container;

    btc_ast_item* param_type = container->body->data[container->body->offset - 1]->container_param->type;
    btc_template* template = param_type->template_item;
    assert(param_type->type == BTC_TEMPLATE);
    assert(strncmp(template->name.value, "Vector", strlen(template->name.value)) == 0);

    assert(strncmp(template->arguments->data[0]->identifier.value, "Post", strlen(template->arguments->data[0]->identifier.value)) == 0);

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

    assert(btc_parse(parser) == BTC_OK);

//    btc_ast_list* ast_list = parser->result;
    btc_ast_item* item = parser->result->data[0];

    btc_ast_container_group_declaration* group = item->container_group;

    btc_ast_container_declaration* container = group->body->data[group->body->offset-1]->container;
    btc_ast_item* param_type = container->body->data[container->body->offset-1]->container_param->type;
    btc_template* template = param_type->template_item;
    assert(param_type->type == BTC_TEMPLATE);
    assert(strncmp(template->name.value, "Vector", strlen(template->name.value)) == 0);

    uint32_t template_arg_index = 0;
    btc_ast_item* first_template_argument = btc_ast_list_get(template->arguments, template_arg_index++);
    btc_ast_item* second_template_argument = btc_ast_list_get(template->arguments, template_arg_index++);
    assert(strncmp(first_template_argument->identifier.value, "Post", strlen(first_template_argument->identifier.value)) == 0);
    assert(strncmp(second_template_argument->identifier.value, "Statistic", strlen(second_template_argument->identifier.value)) == 0);

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

    assert(btc_parse(parser) == BTC_OK);

    btc_ast_list* ast_list = parser->result;
    btc_ast_item* item = btc_ast_list_get(ast_list, 0);

    btc_ast_container_declaration* container = item->container_group->body->data[0]->container;
    btc_ast_container_param* param = container->body->data[container->body->offset - 1]->container_param;
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

    assert(btc_parse(parser) == BTC_OK);

    btc_ast_list* ast_item_list = parser->result;

    // container group body
    ast_item_list = btc_ast_list_get(ast_item_list, 0)->container_group->body;

    // container body
    ast_item_list = btc_ast_list_get(ast_item_list, 0)->container->body;

    // first container param
    uint32_t params_index = 0;
    btc_ast_container_param* param = btc_ast_list_get(ast_item_list, params_index++)->container_param;

    assert(strncmp(param->name.value, "id", 2) == 0);
    assert(strncmp(param->type->identifier.value, "uint32", 6) == 0);

    // second linked container param
    param = btc_ast_list_get(ast_item_list, params_index++)->container_param;

    assert(strncmp(param->name.value, "lastPost", 2) == 0);
    assert(strncmp(param->type->identifier.value, "Post", 4) == 0);

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

    assert(btc_parse(parser) == BTC_OK);

    // Get container group
    btc_ast_list* container_params = btc_ast_list_get(parser->result, 0)->container_group->body;

    // Get container params
    container_params = btc_ast_list_get(container_params, 0)->container->body;

    uint32_t param_index = 0;
    btc_ast_container_param* container_param = btc_ast_list_get(container_params, param_index++)->container_param;
    assert(container_param->default_value->number.value == 1000);

    container_param = btc_ast_list_get(container_params, param_index++)->container_param;
    assert(container_param->default_value->number.value == 26.5);

    container_param = btc_ast_list_get(container_params, param_index++)->container_param;
    assert(strncmp(container_param->default_value->string.value, "empty name", strlen("empty name")) == 0);

    container_param = btc_ast_list_get(container_params, param_index++)->container_param;
    assert(strncmp(container_param->default_value->string.value, "", strlen("")) == 0);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_container_alias() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\n\
        alias Buffer = Vector<Uint8>;\n\
        alias ObjectId = StrictSize<Buffer, 12>;\n\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    assert(btc_parse(parser) == BTC_OK);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_node_offsets() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\n\
        alias Buffer = Vector<Uint8>;\n\
        alias ObjectId = StrictSize<Buffer, 12>;\n\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    assert(btc_parse(parser) == BTC_OK);

    btc_ast_item* node = parser->result->data[0];
    assert(node->range.start_offset == 9);
    assert(node->range.end_offset == 38);
    assert(node->range.start_line_number == 1);
    assert(node->range.end_line_number == 1);

    node = parser->result->data[1];
    assert(node->range.start_offset == 47);
    assert(node->range.end_offset == 87);
    assert(node->range.start_line_number == 2);
    assert(node->range.end_line_number == 2);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

int compare_strings(char* c1, char* c2) {
    if(strlen(c1) != strlen(c2)) {
        return 0;
    }
    if(strncmp(c1, c2, strlen(c1)) == 0) {
        return 1;
    }
    return 0;
}

void expect_comment(btc_comments_list* list, size_t i, const char* comment) {
    int result = compare_strings((char*)list->data[i]->value, (char*)comment);
    if(result != 1) {
        fprintf(stderr, "comparison failed %s != %s\n", comment, list->data[i]->value);
    }
    assert(result == 1);
}

void test_container_comments() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\n\
        /* user container group */\
        type User {\n\
            /* user container */\n\
            user -> /* user id */ string id, /* after user id */\n\
                    /* user name */ string name /* after user name */;\n\
            // after user container\n\
        }\n\
        // after user container group\n\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    assert(btc_parse(parser) == BTC_OK);

    btc_ast_item* node = parser->result->data[0];
    assert(node->type == BTC_CONTAINER_GROUP);
    assert(node->leading_comments->offset == 1);
    expect_comment(node->leading_comments, 0, " user container group ");
    expect_comment(node->trailing_comments, 0, " after user container group");

    node = node->container_group->body->data[0];
    assert(node->leading_comments->offset == 1);
    expect_comment(node->leading_comments, 0, " user container ");
    expect_comment(node->trailing_comments, 0, " after user container");

    btc_ast_list* container_params = node->container->body;
    assert(container_params->data[0]->leading_comments->offset == 1);
    expect_comment(container_params->data[0]->leading_comments, 0, " user id ");
    expect_comment(container_params->data[0]->trailing_comments, 0, " after user id ");

    assert(container_params->data[1]->leading_comments->offset == 2);
    expect_comment(container_params->data[1]->leading_comments, 0, " after user id ");
    expect_comment(container_params->data[1]->leading_comments, 1, " user name ");

    assert(container_params->data[1]->trailing_comments->offset == 1);
    expect_comment(container_params->data[1]->trailing_comments, 0, " after user name ");

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_template_declaration() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\n\
        type UserItem_t {\n\
            template<typename T, typename B>\n\n\
            UserItem -> /* item id */ int id, /* item type */ T itemA, B itemB;\
        }\n\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    assert(btc_parse(parser) == BTC_OK);

    btc_ast_item* node = parser->result->data[0]->container_group->body->data[0];
    btc_template_declaration* template_decl = node->template_declaration;
    assert(compare_strings((char*)template_decl->arguments->data[0]->identifier.value, "T"));
    assert(compare_strings((char*)template_decl->arguments->data[1]->identifier.value, "B"));

    node = template_decl->body;
    btc_ast_list* params = node->container->body;
    assert(compare_strings((char*) params->data[0]->container_param->name.value, "id"));
    assert(compare_strings((char*) params->data[1]->container_param->name.value, "itemA"));
    assert(compare_strings((char*) params->data[2]->container_param->name.value, "itemB"));

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(tokenizer);
}

void test_bodyless_containers() {
    btc_tokenizer* tokenizer;
    btc_tokenizer_init(&tokenizer);

    btc_tokenizer_scan(tokenizer, "\n\
        type User {\
            user\
        }\
    ");

    btc_parser* parser;
    btc_parser_init(&parser, tokenizer);

    assert(btc_parse(parser) == BTC_OK);

    btc_parser_destroy(parser);
}

void test_parser() {
    test_container_group();
    test_container_alias();
    test_container_import();
    test_container_template();
    test_container_declaration();
    test_container_param_default();
    test_container_member_expression();
    test_container_namespace();
    test_container_comments();
    test_node_offsets();
    test_template_declaration();
    test_bodyless_containers();
}