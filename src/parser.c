#include "tokenizer.h"
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void btc_parser_init(btc_parser** parser_ptr, btc_tokenizer* tokenizer) {
    *parser_ptr = calloc(1, sizeof(btc_parser));

    btc_parser* parser = *parser_ptr;
    parser->current_token = tokenizer->first_token;
}

void btc_parser_destroy(btc_parser* parser){

}

/**
 * Check if current token has the following value
 */
int btc_parser_peek(btc_parser* parser, const char* value) {
    if(strncmp(parser->current_token->value, value, strlen(value)) == 0)
        return 1;

    return 0;
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

void btc_parser_expect(btc_parser* parser, const char* string) {
    btc_token* token;
    btc_parser_consume(parser, &token);

    if(strncmp(string, token->value, strlen(string)) != 0)
        fprintf(stderr, "expected %s but got %s instead\n", string, token->value);
}

int btc_parser_eof(btc_parser* parser) {
    if(parser->current_token != NULL)
        return 0;
    return 1;
}

void btc_parser_scan_namespace(btc_parser* parser) {

}

/**
 * Add new ast item to linked list
 */
void btc_parser_add_ast_item(btc_parser* parser, btc_ast_item* item) {
    if(parser->first_item == NULL)
        parser->first_item = item;

    if(parser->last_item != NULL){
        parser->last_item->next_item = item;
        item->previous_item = parser->last_item;
    }

    parser->last_item = item;
}

void btc_parser_initialize_ast_item(btc_ast_item** ast_item_ptr) {
    *ast_item_ptr = malloc(sizeof(btc_ast_item));

    btc_ast_item* ast_item = *ast_item_ptr;
    ast_item->next_item = NULL;
    ast_item->previous_item = NULL;
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

void btc_parser_create_container_params(btc_ast_container_params** body_ptr) {
    *body_ptr = malloc(sizeof(btc_ast_container_params));

    btc_ast_container_params* body = *body_ptr;
    body->first_param = NULL;
    body->last_param = NULL;
}

void btc_parser_add_container_param(btc_ast_container_params* body, btc_ast_container_param* param) {
    if(body->first_param == NULL)
        body->first_param = param;

    if(body->last_param != NULL) {
        body->last_param->next_param = param;
        param->previous_param = body->last_param;
    }

    body->last_param = param;
}

void btc_parser_scan_container_short_body(btc_parser* parser, btc_ast_container_params** body_ptr) {
    btc_parser_create_container_params(body_ptr);
    btc_ast_container_params* body = *body_ptr;

    do {
        btc_ast_container_param* param = malloc(sizeof(btc_ast_container_param));
        param->name = btc_parser_consume_identifier(parser);
        btc_parser_expect(parser, ":");

        param->type = btc_parser_consume_identifier(parser);

        btc_parser_add_container_param(body, param);
    } while(!btc_parser_eof(parser) && (
        btc_parser_peek_and_consume(parser, ",") ||
        !btc_parser_peek_and_consume(parser, ";")
    ));
}

void btc_parser_scan_container_body(btc_parser* parser, btc_ast_container_params** body_ptr) {
    if(btc_parser_peek_and_consume(parser, "->")) {
        btc_parser_scan_container_short_body(parser, body_ptr);
    }
}

void btc_parser_scan_type_group_body(btc_parser* parser, btc_ast_container_params** body_ptr) {
    btc_parser_scan_container_body(parser, body_ptr);
}

void btc_create_containers_list(btc_ast_containers_list** containers_ptr) {
    *containers_ptr = malloc(sizeof(btc_ast_containers_list));
    btc_ast_containers_list* list = *containers_ptr;

    list->first_item = NULL;
    list->last_item = NULL;
}

void btc_add_container_declaration(btc_ast_containers_list* list, btc_ast_container_declaration container) {
    btc_linked_container_declaration* linked = malloc(sizeof(btc_linked_container_declaration));

    linked->container = container;

    if(list->first_item == NULL)
        list->first_item = linked;

    if(list->last_item != NULL) {
        list->last_item->next_item = linked;
        linked->previous_item = list->last_item;
    }

    list->last_item = linked;
}

void btc_parser_scan_type_group_definition(btc_parser* parser) {
    if(!btc_parser_peek(parser, "type"))
        return btc_parser_scan_namespace(parser);

    btc_ast_container_group_declaration group;
    btc_parser_consume(parser, NULL);
    group.type = btc_parser_consume_identifier(parser);
    btc_parser_expect(parser, "{");

    btc_ast_containers_list* containers;
    btc_create_containers_list(&containers);

    while(!btc_parser_eof(parser) && !btc_parser_peek_and_consume(parser, "}")) {
        btc_ast_container_declaration container;
        btc_ast_identifier container_name = btc_parser_consume_identifier(parser);
        btc_ast_container_params* body;

        btc_parser_scan_container_body(parser, &body);

        container.name = container_name;
        container.body = body;

        btc_add_container_declaration(containers, container);
    }

    group.body = containers;

    btc_ast_item* item = malloc(sizeof(btc_ast_item));
    item->type = BTC_CONTAINER_GROUP;
    item->container_group = group;

    btc_parser_add_ast_item(parser, item);
}

/**
 * consume tokens and fulfill ast
 */
void btc_parse(btc_parser* parser) {
    while(!btc_parser_eof(parser)) {
        btc_parser_scan_type_group_definition(parser);
    }
}