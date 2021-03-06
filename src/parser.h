#ifndef PARSER_H_
#define PARSER_H_

typedef struct _btc_parser btc_parser;

#include "btc.h"
#include "tokenizer/tokenizer.h"
#include "tokenizer/tokens_list.h"
#include "ast/ast_item.h"
#include "ast/container_group.h"
#include "ast/parser_identifier.h"

#include <stddef.h>

struct _btc_parser {
    int status;
    btc_ast_list* result;
    uint32_t current_token;
    btc_tokens_list* tokens_list;
    btc_tokens_list* comments_list;
};

int btc_parser_eof(btc_parser* parser);

int btc_parser_peek_from_index(btc_parser* parser, const char* value, size_t index);
int btc_parser_peek(btc_parser* parser, const char* value);
int btc_parser_peek_and_consume(btc_parser* parser, const char* string);

int btc_parser_consume_identifier(btc_parser*, btc_ast_identifier*);
int btc_parser_scan_namespace(btc_parser* parser, btc_ast_item* ast_item);
int btc_parser_scan_type_group_definition(btc_parser* parser, btc_ast_item* result);
int btc_parser_scan_param_type(btc_parser*, btc_ast_item*);
int btc_parser_scan_literal_expression(btc_parser*, btc_ast_item*);
int btc_parser_scan_template_declaration(btc_parser*, btc_ast_item*);

void btc_parser_init(btc_parser** parser_ptr, btc_tokenizer* tokenizer);
void btc_parser_destroy(btc_parser* parser);
int btc_parse(btc_parser* parser);

#endif
