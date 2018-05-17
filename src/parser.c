#include "btc.h"
#include "tokenizer.h"
#include "parser.h"
#include "ast/container_params.h"
#include "ast/container_declaration.h"
#include "ast/namespace.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void btc_parser_init(btc_parser** parser_ptr, btc_tokenizer* tokenizer) {
    *parser_ptr = calloc(1, sizeof(btc_parser));

    btc_parser* parser = *parser_ptr;
    btc_ast_list* list;
    btc_create_ast_list(&list);

    parser->result = list;
    parser->status = BTC_OK;
    parser->current_token = tokenizer->first_token;
}

void btc_parser_destroy(btc_parser* parser){
    btc_destroy_ast_list(parser->result);
    free(parser);
}

/**
 * Check if current token has the following value starting from `index`
 */
int btc_parser_peek_from_index(btc_parser* parser, const char* value, size_t index) {
    btc_token* token = parser->current_token;
    size_t current_index = 0;

    while(current_index < index) {
        if(token->next_token == NULL)
            return 0;

        token = token->next_token;
        ++current_index;
    }

    if(strncmp(token->value, value, strlen(value)) == 0)
        return 1;

    return 0;
}

/**
 * Check if current token has the following value
 */
int btc_parser_peek(btc_parser* parser, const char* value) {
    return btc_parser_peek_from_index(parser, value, 0);
}

/**
 * Consume current token and skip to next. 
 * if `token` argument is provided, it'll be fulfilled 
 * with current token before it's flushed
 */
void btc_parser_consume(btc_parser* parser, btc_token** token) {
    if(token != NULL) {
        *token = parser->current_token;
    }

    parser->current_token = parser->current_token->next_token;
}

/**
 * return 1 if `string` matches and 0 if no
 */
int btc_parser_expect(btc_parser* parser, const char* string) {
    btc_token* token;
    btc_parser_consume(parser, &token);

    if(strncmp(string, token->value, strlen(string)) != 0) {
        fprintf(stderr, "expected %s but got %s instead\n", string, token->value);
        return 0;
    }
    return 1;
}

int btc_parser_eof(btc_parser* parser) {
    if(parser->current_token != NULL)
        return 0;
    return 1;
}

btc_string btc_consume_string(btc_parser* parser, int* error) {
    btc_token* token;
    btc_parser_consume(parser, &token);
    btc_string result = {};

    if(token->type != BTC_TOKEN_LITERAL_STRING) {
        if(error != NULL) {
            *error = BTC_UNEXPECTED_TOKEN;
        }
        return result;
    }

    if(error != NULL) {
        *error = BTC_OK;
    }

    result.value = token->value;

    return result;
}

int btc_parser_status(btc_parser* parser){
    return parser->status;
}

int btc_parser_scan_import(btc_parser* parser, btc_ast_item* result) {
    btc_parser_consume(parser, NULL);

    btc_import_declaration import = { btc_consume_string(parser, &parser->status) };

    if(btc_parser_status(parser) != BTC_OK)
        return parser->status;

    btc_parser_expect(parser, ";");

    result->type = BTC_IMPORT_DECLARATION;
    result->import_declaration = import;

    return BTC_OK;
}

/**
 * scan ast into `result` 
 */
int btc_parser_scan(btc_parser* parser, btc_ast_item* result) {
    if(btc_parser_peek(parser, "type"))
        btc_parser_scan_type_group_definition(parser, result);
    else if(btc_parser_peek(parser, "namespace"))
        btc_parser_scan_namespace(parser, result);
    else if(btc_parser_peek(parser, "import"))
        btc_parser_scan_import(parser, result);
    else {
        fprintf(stderr, "unexpected token \"%s\"\n", parser->current_token->value);
        return 1;
    }
    return BTC_OK;
}

void btc_parser_scan_namespace(btc_parser* parser, btc_ast_item* ast_item) {
    btc_parser_consume(parser, NULL);
    btc_ast_identifier name = btc_parser_consume_identifier(parser);

    if(!btc_parser_expect(parser, "{"))
        return;

    btc_namespace* namespace;
    btc_initialize_namespace(&namespace);

    namespace->name = name;

    ast_item->type = BTC_NAMESPACE;
    ast_item->namespace = namespace;

    while(!btc_parser_peek_and_consume(parser, "}")) {
        btc_ast_item* result;
        btc_initialize_ast_item(&result);

        if(btc_parser_scan(parser, result) != BTC_OK)
            break;

        btc_add_ast_item(namespace->body, result);
    }
}

btc_ast_identifier btc_parser_consume_identifier(btc_parser* parser) {
    btc_token* token;
    btc_parser_consume(parser, &token);

    if(token->type != BTC_TOKEN_IDENTIFIER) {
        fprintf(stderr, "expected identifier but got \"%s\" instead\n", token->value);
    }

    btc_ast_identifier identifier = { token->value };

    return identifier;
}

/**
 * almost like `btc_parser_expect` but doesn't throw an exception 
 * when peek does not returns a positive value
 */
int btc_parser_peek_and_consume(btc_parser* parser, const char* string) {
    if(btc_parser_peek(parser, string)) {
        btc_parser_consume(parser, NULL);
        return 1;
    }
    return 0;
}

void btc_parser_scan_member_expression(btc_parser* parser, btc_ast_item* output) {
    btc_ast_item* left;
    btc_initialize_ast_item(&left);
    left->type = BTC_IDENTIFIER;
    left->identifier = btc_parser_consume_identifier(parser);

    while(!btc_parser_eof(parser) && btc_parser_peek_and_consume(parser, ".")) {
        btc_member_expression* expr;
        btc_initialize_member_expression(&expr);

        btc_destroy_ast_item(expr->left);
        expr->left = left;
        expr->right = btc_parser_consume_identifier(parser);

        if(!btc_parser_peek(parser, ".")) {
            output->type = BTC_MEMBER_EXPRESSION;
            output->member_expression = expr;
            break;
        }

        btc_ast_item* next;
        btc_initialize_ast_item(&next);
        next->type = BTC_MEMBER_EXPRESSION;
        next->member_expression = expr;
        left = next;
    }
}

void btc_parser_scan_template(btc_parser* parser, btc_ast_item* result) {
    btc_ast_identifier name = btc_parser_consume_identifier(parser);
    btc_template* template;
    btc_initialize_template(&template);

    template->name = name;

    btc_parser_expect(parser, "<");

    while(btc_parser_peek_and_consume(parser, ",") || !btc_parser_peek_and_consume(parser, ">")) {
        btc_ast_item* argument;
        btc_initialize_ast_item(&argument);

        btc_parser_scan_param_type(parser, argument);
        btc_add_ast_item(template->arguments, argument);
    }

    result->type = BTC_TEMPLATE;
    result->template = template;
}

void btc_parser_scan_param_type(btc_parser* parser, btc_ast_item* result) {
    if(btc_parser_peek_from_index(parser, ".", 1)) {
        btc_parser_scan_member_expression(parser, result);
        return;
    } else if(btc_parser_peek_from_index(parser, "<", 1)) {
        btc_parser_scan_template(parser, result);
        return;
    }

    result->type = BTC_IDENTIFIER;
    result->identifier = btc_parser_consume_identifier(parser);
}

void btc_parser_scan_container_short_body(btc_parser* parser, btc_ast_container_params* body) {
    do {
        btc_ast_container_param* param;
        btc_initialize_container_param(&param);

        param->name = btc_parser_consume_identifier(parser);
        btc_parser_expect(parser, ":");

        btc_parser_scan_param_type(parser, param->type);

        btc_add_container_param(body, param);
    } while(!btc_parser_eof(parser) && btc_parser_peek_and_consume(parser, ","));
}

void btc_parser_scan_container_body(btc_parser* parser, btc_ast_container_params* params) {
    if(btc_parser_peek_and_consume(parser, "->")) {
        btc_parser_scan_container_short_body(parser, params);
        btc_parser_peek_and_consume(parser, ";");
    } else {
        fprintf(stderr, "unexpected container declaration %s\n", parser->current_token->value);
    }
}

void btc_parser_scan_type_group_definition(btc_parser* parser, btc_ast_item* result) {
    btc_ast_container_group_declaration* group;
    btc_create_container_group(&group);

    btc_parser_consume(parser, NULL);
    group->type = btc_parser_consume_identifier(parser);
    btc_parser_expect(parser, "{");

    btc_ast_containers_list* containers = group->body;

    while(!btc_parser_eof(parser) && !btc_parser_peek(parser, "}")) {
        btc_ast_container_declaration* container;
        btc_create_container_declaration(&container);

        btc_ast_identifier container_name = btc_parser_consume_identifier(parser);

        btc_parser_scan_container_body(parser, container->body);

        container->name = container_name;

        btc_add_container_declaration(containers, container);
    }

    btc_parser_expect(parser, "}");

    result->type = BTC_CONTAINER_GROUP;
    result->container_group = group;
}

/**
 * consume tokens and fulfill ast
 */
void btc_parse(btc_parser* parser) {
    while(!btc_parser_eof(parser)) {
        btc_ast_item* result;
        btc_initialize_ast_item(&result);

        if(btc_parser_scan(parser, result) != BTC_OK) {
            btc_destroy_ast_item(result);
            break;
        }

        btc_add_ast_item(parser->result, result);
    }
}