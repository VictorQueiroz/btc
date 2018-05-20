#ifndef PARSER_H_
#define PARSER_H_

#include "btc.h"
#include "tokenizer.h"
#include "ast/ast_item.h"
#include "ast/container_group.h"
#include "ast/parser_identifier.h"

struct _btc_parser {
    int status;
    btc_ast_list* result;
    btc_token* current_token;
};

int btc_parser_eof(btc_parser* parser);

int btc_parser_peek_from_index(btc_parser* parser, const char* value, size_t index);
int btc_parser_peek(btc_parser* parser, const char* value);
int btc_parser_peek_and_consume(btc_parser* parser, const char* string);

btc_ast_identifier btc_parser_consume_identifier(btc_parser* parser);
int btc_parser_scan_namespace(btc_parser* parser, btc_ast_item* ast_item);
int btc_parser_scan_type_group_definition(btc_parser* parser, btc_ast_item* result);
void btc_parser_scan_param_type(btc_parser* parser, btc_ast_item* result);

#endif