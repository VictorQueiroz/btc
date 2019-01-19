#include "btc.h"
#include "tokenizer/tokenizer.h"
#include "parser.h"
#include "ast/container_declaration.h"
#include "ast/namespace.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int btc_parser_get_last_token(btc_parser* parser, btc_token** token) {
    if(token == NULL) {
        return BTC_UNEXPECTED_TOKEN;
    }
    *token = btc_tokens_list_get(parser->tokens_list, parser->current_token - 1);
    return BTC_OK;
}

int btc_set_node_end_token(btc_ast_item* node, btc_token* token) {
    if(token == NULL) {
        return BTC_UNEXPECTED_TOKEN;
    }

    node->range.end_line_number = token->range.end_line_number;
    node->range.end_offset = token->range.end_offset;

    return BTC_OK;
}

int btc_set_node_start_token(btc_ast_item* node, btc_token* token) {
    if(token == NULL) {
        return BTC_UNEXPECTED_TOKEN;
    }

    node->range.start_line_number = token->range.start_line_number;
    node->range.start_offset = token->range.start_offset;

    return BTC_OK;
}

void btc_parser_init(btc_parser** parser_ptr, btc_tokenizer* tokenizer) {
    *parser_ptr = calloc(1, sizeof(btc_parser));

    btc_parser* parser = *parser_ptr;

    parser->result = btc_ast_list_alloc();
    parser->status = BTC_OK;
    parser->tokens_list = tokenizer->tokens_list;
    parser->comments_list = tokenizer->comments_list;
}

void btc_parser_get_token(btc_parser* parser, btc_token** token_output) {
    *token_output = btc_tokens_list_get(parser->tokens_list, parser->current_token);
}

void btc_parser_destroy(btc_parser* parser){
    btc_ast_list_free(parser->result);
    free(parser);
}

/**
 * Check if current token has the following value starting from `index`
 */
int btc_parser_peek_from_index(btc_parser* parser, const char* value, size_t index) {
    btc_token* token = btc_tokens_list_get(parser->tokens_list, parser->current_token + index);

    if(strlen(value) != strlen(token->value)) {
        return 0;
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
int btc_parser_consume(btc_parser* parser, btc_token** token) {
    if(btc_parser_eof(parser)) {
        return BTC_UNEXPECTED_END;
    }

    if(token != NULL) {
        *token = btc_tokens_list_get(parser->tokens_list, parser->current_token);
    }

    parser->current_token++;

    return BTC_OK;
}

/**
 *  Return BTC_OK if matches and BTC_UNEXPECTED_TOKEN if not
 */
int btc_parser_expect(btc_parser* parser, const char* string) {
    btc_token* token = NULL;
    int status = btc_parser_consume(parser, &token);
    BTC_PASS_OR_RETURN_ERROR(status)

    if(strncmp(string, token->value, strlen(string)) != 0) {
        status = BTC_UNEXPECTED_TOKEN;
    }
    return status;
}

int btc_parser_eof(btc_parser* parser) {
    if(parser->current_token == btc_tokens_list_length(parser->tokens_list))
        return 1;
    return 0;
}

btc_string btc_consume_string(btc_parser* parser, int* error) {
    btc_token* token;
    btc_parser_consume(parser, &token);
    btc_string result = {0};

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

int btc_parser_scan_alias(btc_parser* parser, btc_ast_item* result) {
    int status = BTC_OK;

    btc_ast_item* value;
    btc_initialize_ast_item(&value);

    btc_parser_consume(parser, NULL);

    btc_ast_identifier name = btc_parser_consume_identifier(parser);

    btc_parser_expect(parser, "=");
    btc_parser_scan_param_type(parser, value);
    btc_parser_expect(parser, ";");

    btc_alias alias = { name, value };

    result->type = BTC_ALIAS;
    result->alias = alias;

    return status;
}

/**
 * Scan ast into `result` 
 */
int btc_parser_scan(btc_parser* parser, btc_ast_item* result) {
    int status = BTC_OK;

    btc_token* token = btc_tokens_list_get(parser->tokens_list, parser->current_token);

    if(btc_parser_peek(parser, "alias"))
        status = btc_parser_scan_alias(parser, result);
    else if(btc_parser_peek(parser, "type"))
        status = btc_parser_scan_type_group_definition(parser, result);
    else if(btc_parser_peek(parser, "namespace"))
        status = btc_parser_scan_namespace(parser, result);
    else if(btc_parser_peek(parser, "import"))
        status = btc_parser_scan_import(parser, result);
    else
        status = BTC_UNEXPECTED_TOKEN;

    if(status == BTC_OK) {
        if(token == NULL) {
            return status;
        }
        btc_set_node_start_token(result, token);
        token = btc_tokens_list_get(parser->tokens_list, parser->current_token-1);
        btc_set_node_end_token(result, token);
    }

    return status;
}

int btc_parser_scan_namespace(btc_parser* parser, btc_ast_item* ast_item) {
    btc_parser_consume(parser, NULL);
    btc_ast_identifier name = btc_parser_consume_identifier(parser);

    if(!btc_parser_expect(parser, "{"))
        return BTC_OK;

    btc_namespace* namespace_item;
    btc_initialize_namespace(&namespace_item);

    namespace_item->name = name;

    ast_item->type = BTC_NAMESPACE;
    ast_item->namespace_item = namespace_item;

    int status = BTC_OK;
    btc_ast_item* result;

    while(!btc_parser_peek_and_consume(parser, "}")) {
        btc_initialize_ast_item(&result);

        status = btc_parser_scan(parser, result);

        if(status != BTC_OK) {
            btc_destroy_ast_item(result);
            break;
        }
        else
            btc_ast_list_add(namespace_item->body, result);
    }

    return status;
}

// TODO: find a way to return an error in case no identifier is find
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

int btc_parser_scan_member_expression(btc_parser* parser, btc_ast_item* output) {
    btc_ast_item* left;
    btc_initialize_ast_item(&left);

    left->type = BTC_IDENTIFIER;
    left->identifier = btc_parser_consume_identifier(parser);

    btc_member_expression* expr = NULL;
    while(!btc_parser_eof(parser) && btc_parser_peek_and_consume(parser, ".")) {
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

    return BTC_OK;
}

void btc_parser_scan_template(btc_parser* parser, btc_ast_item* result) {
    btc_ast_identifier name = btc_parser_consume_identifier(parser);
    btc_template* template_item;
    btc_initialize_template(&template_item);

    template_item->name = name;

    btc_parser_expect(parser, "<");

    while(btc_parser_peek_and_consume(parser, ",") || !btc_parser_peek_and_consume(parser, ">")) {
        btc_ast_item* argument;
        btc_initialize_ast_item(&argument);

        if(btc_parser_scan_literal_expression(parser, argument) != BTC_OK) {
            btc_parser_scan_param_type(parser, argument);
        }
        btc_ast_list_add(template_item->arguments, argument);
    }

    result->type = BTC_TEMPLATE;
    result->template_item = template_item;
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

int btc_parser_scan_container_short_param(btc_parser* parser, btc_ast_item* body) {
    btc_ast_container_param* param;
    btc_initialize_container_param(&param);

    body->type = BTC_CONTAINER_PARAM;
    body->container_param = param;

    btc_parser_scan_param_type(parser, param->type);

    param->name = btc_parser_consume_identifier(parser);

    return BTC_OK;
}

int btc_parser_scan_container_short_body(btc_parser* parser, btc_ast_list* body) {
    int status = BTC_OK;
    btc_token* token = NULL;

    do {
        btc_ast_item* param;
        btc_initialize_ast_item(&param);

        btc_parser_get_token(parser, &token);
        status = btc_set_node_start_token(param, token);
        BTC_PASS_OR_BREAK(status)

        status = btc_parser_scan_container_short_param(parser, param);
        BTC_PASS_OR_BREAK(status)

        btc_parser_get_last_token(parser, &token);
        status = btc_set_node_end_token(param, token);
        BTC_PASS_OR_BREAK(status)

        btc_ast_list_add(body, param);
    } while(!btc_parser_eof(parser) && btc_parser_peek_and_consume(parser, ","));

    return status;
}

/**
 * Put current token into `token_ptr` argument
 * @param parser Current parser pointer
 * @param token_ptr
 */
void btc_get_token(btc_parser* parser, btc_token** token_ptr) {
    *token_ptr = btc_tokens_list_get(parser->tokens_list, parser->current_token);
}

int btc_parser_scan_literal_expression(btc_parser* parser, btc_ast_item* result) {
    btc_token* token;
    btc_get_token(parser, &token);
    
    if(token == NULL)
        return BTC_UNEXPECTED_END;

    if(token->type == BTC_TOKEN_LITERAL_STRING) {
        btc_string string = { token->value };

        string.value = token->value;

        result->type = BTC_STRING;
        result->string = string;
    } else if(token->type == BTC_TOKEN_LITERAL_NUMBER) {
        btc_number number = { token->number };

        result->type = BTC_NUMBER;
        result->number = number;
    } else {
        return BTC_UNEXPECTED_TOKEN;
    }

    return btc_parser_consume(parser, NULL);
}

int btc_parser_scan_container_param(btc_parser* parser, btc_ast_item* result) {
    int status = BTC_OK;

    btc_ast_container_param* param;
    btc_initialize_container_param(&param);

    btc_parser_scan_param_type(parser, param->type);

    param->name = btc_parser_consume_identifier(parser);

    if(btc_parser_peek_and_consume(parser, "=")) {
        btc_initialize_ast_item(&param->default_value);

        status = btc_parser_scan_literal_expression(parser, param->default_value);
        BTC_PASS_OR_RETURN_ERROR(status)
    }

    if(btc_parser_peek_and_consume(parser, ";") != 1) {
        return BTC_UNEXPECTED_TOKEN;
    }

    result->type = BTC_CONTAINER_PARAM;
    result->container_param = param;

    return status;
}

int btc_parser_scan_full_container_body(btc_parser* parser, btc_ast_list* params) {
    int status = BTC_OK;

    if(!btc_parser_expect(parser, "{"))
        return BTC_UNEXPECTED_TOKEN;

    btc_token* token = NULL;
    btc_ast_item* param = NULL;

    while(!btc_parser_peek_and_consume(parser, "}")) {
        btc_initialize_ast_item(&param);

        btc_parser_get_token(parser, &token);
        btc_set_node_start_token(param, token);

        status = btc_parser_scan_container_param(parser, param);

        btc_parser_get_last_token(parser, &token);
        btc_set_node_end_token(param, token);

        btc_ast_list_add(params, param);
    }

    return status;
}

int btc_parser_scan_container_body(btc_parser* parser, btc_ast_list* params) {
    int status = BTC_UNEXPECTED_TOKEN;

    if(btc_parser_peek_and_consume(parser, "->")) {
        status = btc_parser_scan_container_short_body(parser, params);
        BTC_PASS_OR_RETURN_ERROR(status)

        btc_parser_peek_and_consume(parser, ";");
    } else if(btc_parser_peek(parser, "{")) {
        status = btc_parser_scan_full_container_body(parser, params);
        BTC_PASS_OR_RETURN_ERROR(status);
    }

    return status;
}

int btc_parser_scan_container_declaration(btc_parser* parser, btc_ast_item* result) {
    btc_ast_container_declaration* container;
    btc_create_container_declaration(&container);

    result->type = BTC_CONTAINER_DECLARATION;
    result->container = container;

    btc_token* token = NULL;
    BTC_PASS_OR_RETURN_ERROR(btc_parser_consume(parser, &token));
    container->name = (btc_ast_identifier) { .value = token->value };

    BTC_PASS_OR_RETURN_ERROR(btc_parser_scan_container_body(parser, container->body));

    return BTC_OK;
}

int btc_parser_scan_template_declaration(btc_parser* parser, btc_ast_item* node) {
    int status = btc_parser_expect(parser, "template");
    BTC_PASS_OR_RETURN_ERROR(status)

    status = btc_parser_expect(parser, "<");
    BTC_PASS_OR_RETURN_ERROR(status)

    btc_token* token = NULL;
    btc_template_declaration* declaration = btc_template_declaration_alloc();

    btc_ast_item* body = declaration->body;

    node->type = BTC_TEMPLATE_DECLARATION;
    node->template_declaration = declaration;

    do {
        btc_ast_item* argument = btc_ast_item_alloc();

        btc_parser_get_token(parser, &token);
        btc_set_node_start_token(argument, token);

        status = btc_parser_expect(parser, "typename");
        BTC_PASS_OR_RETURN_ERROR(status)

        status = btc_parser_consume(parser, &token);
        BTC_PASS_OR_RETURN_ERROR(status)

        if(token->type != BTC_TOKEN_IDENTIFIER) {
            return BTC_UNEXPECTED_TOKEN;
        }

        argument->type = BTC_IDENTIFIER;
        argument->identifier = (btc_ast_identifier) { .value = token->value };

        btc_ast_list_add(declaration->arguments, argument);
    } while(!btc_parser_eof(parser) && !btc_parser_peek(parser, ">") && btc_parser_peek_and_consume(parser, ","));

    status = btc_parser_expect(parser, ">");
    BTC_PASS_OR_RETURN_ERROR(status)

    btc_parser_scan_container_declaration(parser, body);

    return BTC_OK;
}

int btc_parser_scan_type_group_definition(btc_parser* parser, btc_ast_item* result) {
    btc_ast_container_group_declaration* group;
    btc_create_container_group(&group);

    int status = BTC_OK;

    status = btc_parser_consume(parser, NULL);
    BTC_PASS_OR_RETURN_ERROR(status)

    group->type = btc_parser_consume_identifier(parser);
    status = btc_parser_expect(parser, "{");
    BTC_PASS_OR_RETURN_ERROR(status)

    btc_ast_item* output_ast_item = NULL;
    btc_token* token = NULL;
    while(!btc_parser_eof(parser) && !btc_parser_peek(parser, "}")) {
        btc_initialize_ast_item(&output_ast_item);

        btc_parser_get_token(parser, &token);
        btc_set_node_start_token(output_ast_item, token);

        if(btc_parser_peek(parser, "template")) {
            status = btc_parser_scan_template_declaration(parser, output_ast_item);
        } else {
            status = btc_parser_scan_container_declaration(parser, output_ast_item);
        }
        BTC_PASS_OR_BREAK(status)

        if(status != BTC_OK) {
            btc_destroy_ast_item(output_ast_item);
        }
        BTC_PASS_OR_RETURN_ERROR(status)

        btc_parser_get_last_token(parser, &token);
        btc_set_node_end_token(output_ast_item, token);

        btc_ast_list_add(group->body, output_ast_item);
    }

    BTC_PASS_OR_RETURN_ERROR(btc_parser_expect(parser, "}"));

    result->type = BTC_CONTAINER_GROUP;
    result->container_group = group;

    return status;
}

int btc_parser_attach_comments(btc_parser* parser, btc_ast_list* list, btc_ast_item* parent_node) {
    btc_ast_item* item = NULL;
    btc_token* comment = NULL;
    btc_ast_item* previous_node = NULL;
    int status = BTC_OK;

    /**
     * Should be set to true when comment is beyond the start offset of parent node
     * or end offset of previous node.
     *
     * We want to be able to get comments that are inside parent node and avoid comments that are
     * before previous node. Comments that are present before previous node must be processed by
     * them as trailing comments.
     */
    int inside_parent_or_after_last_node = 0;

    vector_foreach(list, j) {
        item = btc_ast_list_get(list, j);
        previous_node = j == 0 ? NULL : btc_ast_list_get(list,j-1);
        vector_foreach(parser->comments_list, i) {
            comment = btc_tokens_list_get(parser->comments_list, i);
            if(previous_node == NULL) {
                if(parent_node != NULL) {
                    inside_parent_or_after_last_node = comment->range.start_offset >= parent_node->range.start_offset;
                } else {
                    inside_parent_or_after_last_node = 1;
                }
            } else {
                inside_parent_or_after_last_node = comment->range.start_offset >= previous_node->range.end_offset;
            }
            if(!inside_parent_or_after_last_node) {
                continue;
            }
            if(comment->range.end_offset <= item->range.start_offset) {
                btc_comment* leading_comment = btc_comment_alloc();
                leading_comment->value = comment->value;
                btc_comments_list_add(item->leading_comments, leading_comment);
            }
            if(comment->range.start_offset >= item->range.end_offset && (parent_node == NULL ? 1 : comment->range.start_offset <= parent_node->range.end_offset)) {
                btc_comment* trailing_comment = btc_comment_alloc();
                trailing_comment->value = comment->value;
                btc_comments_list_add(item->trailing_comments, trailing_comment);
            }
        }
        switch(item->type) {
            case BTC_CONTAINER_GROUP:
                status = btc_parser_attach_comments(parser, item->container_group->body, item);
                break;
            case BTC_CONTAINER_DECLARATION:
                status = btc_parser_attach_comments(parser, item->container->body, item);
                break;
            case BTC_NAMESPACE:
                status = btc_parser_attach_comments(parser, item->namespace_item->body, item);
                break;
            case BTC_TEMPLATE:
                status = btc_parser_attach_comments(parser, item->template_item->arguments, item);
                break;
            case BTC_TEMPLATE_DECLARATION:
                status = btc_parser_attach_comments(parser, item->template_declaration->arguments, item);
                break;
            case BTC_ALIAS:
            case BTC_IDENTIFIER:
            case BTC_MEMBER_EXPRESSION:
            case BTC_CONTAINER_PARAM:
                break;
            default:
                fprintf(stderr, "received unhandled ast item: %d\n", item->type);
                return BTC_UNEXPECTED_TOKEN;
        }
        BTC_PASS_OR_BREAK(status);
    }
    return status;
}

/**
 * consume tokens and fulfill ast
 */
int btc_parse(btc_parser* parser) {
    int status = BTC_OK;
    btc_ast_item* result;

    while(!btc_parser_eof(parser)) {
        btc_initialize_ast_item(&result);

        status = btc_parser_scan(parser, result);

        if(status != BTC_OK) {
            btc_destroy_ast_item(result);
            break;
        }

        btc_ast_list_add(parser->result, result);
    }

    btc_parser_attach_comments(parser, parser->result, NULL);

    return status;
}