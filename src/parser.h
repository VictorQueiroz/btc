#ifndef PARSER_H_
#define PARSER_H_

#include "tokenizer.h"
#include "parser_identifier.h"

#define BTC_CONTAINER_GROUP 1

struct _btc_ast_container_param {
    btc_ast_identifier name;
    btc_ast_identifier type;
    struct _btc_ast_container_param* next_param;
    struct _btc_ast_container_param* previous_param;
};

typedef struct _btc_ast_container_param btc_ast_container_param;

typedef struct {
    btc_ast_container_param* first_param;
    btc_ast_container_param* last_param;
} btc_ast_container_params;

struct _btc_ast_container_declaration {
    btc_ast_identifier name;
    btc_ast_identifier type;
    btc_ast_container_params* body;
};

typedef struct _btc_ast_container_declaration btc_ast_container_declaration;

typedef struct _btc_linked_container_declaration {
    btc_ast_container_declaration container;
    struct _btc_linked_container_declaration* next_item;
    struct _btc_linked_container_declaration* previous_item;
} btc_linked_container_declaration;

typedef struct {
    btc_linked_container_declaration* first_item;
    btc_linked_container_declaration* last_item;
} btc_ast_containers_list; 

typedef struct {
    btc_ast_identifier type;
    /**
     * Common list of container declarations without `type` 
     * param
     */
    btc_ast_containers_list* body;
} btc_ast_container_group_declaration;

struct _btc_ast_item {
    int type;

    btc_ast_container_group_declaration container_group;

    struct _btc_ast_item* next_item;
    struct _btc_ast_item* previous_item;
};

typedef struct _btc_ast_item btc_ast_item;

struct _btc_parser {
    btc_ast_item* last_item;
    btc_ast_item* first_item;
    btc_token* current_token;
};

typedef struct _btc_parser btc_parser;

void btc_parser_init(btc_parser** parser_ptr, btc_tokenizer* tokenizer);
void btc_parser_destroy(btc_parser* parser);
void btc_parse(btc_parser* parser);
int btc_parser_eof(btc_parser* parser);

int btc_parser_peek_and_consume(btc_parser* parser, const char* string);

#endif

#endif