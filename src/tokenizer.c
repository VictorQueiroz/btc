#include "btc.h"
#include "tokenizer.h"
#include "character.h"
#include "tokens_list.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void btc_tokenizer_init(btc_tokenizer** tokenizer_ptr){
    *tokenizer_ptr = calloc(1, sizeof(btc_tokenizer));

    if(!btc_tokenizer_check_option((*tokenizer_ptr), BTC_TOKENIZER_CONFIG_IGNORE_COMMENTS))
        btc_initialize_tokens_list(&(*tokenizer_ptr)->comments_list);

    btc_initialize_tokens_list(&(*tokenizer_ptr)->tokens_list);
}

void btc_tokenizer_slice_string(btc_tokenizer* tokenizer, char** output_ptr, size_t start_offset, size_t end_offset) {
    const size_t string_length = (end_offset - start_offset);
    *output_ptr = malloc((string_length * sizeof(char)) + 1);
    memcpy(*output_ptr, &tokenizer->string[start_offset], string_length);
    (*output_ptr)[string_length] = '\0';
} 

void btc_token_init(btc_token** token_ptr, int type){
    *token_ptr = calloc(1, sizeof(btc_token));

    btc_token* token = *token_ptr;
    token->type = type;
}

int btc_tokenizer_is_identifier_start(btc_tokenizer* tokenizer) {
    return ch_is_identifier_start(tokenizer->buffer[tokenizer->offset]);
}

/**
 * Compare if next characters are equivalent to `token`. 
 * Returns 0 if found any match, 1 if none are found
 */
int btc_tokenizer_compare(btc_tokenizer* tokenizer, const char* token) {
    if(strncmp(&tokenizer->string[tokenizer->offset], token, strlen(token)) == 0)
        return 1;

    return 0;
}

void btc_tokenizer_push_token(btc_tokenizer* tokenizer, btc_token* token) {
    btc_add_token(tokenizer->tokens_list, token);
}

void btc_tokenizer_push_keyword(btc_tokenizer* tokenizer, const char* value) {
    btc_token* token;
    btc_token_init(&token, BTC_TOKEN_KEYWORD);
    token->value = value;

    btc_tokenizer_push_token(tokenizer, token);
}

int btc_tokenizer_eof(btc_tokenizer* tokenizer){
    if(tokenizer->offset < tokenizer->string_length)
        return 0;

    return 1;
}

/**
 * Check if current character is a punctuator. 
 * Returns 1 if matches, 0 if otherwise
 */
int btc_tokenizer_is_punctuator(btc_tokenizer* tokenizer) {
    return ch_is_punctuator(tokenizer->buffer[tokenizer->offset]);
}

/**
 * Check if current character is a numeric value
 */
int btc_tokenizer_is_number(btc_tokenizer* tokenizer) {
    return ch_is_number(tokenizer->buffer[tokenizer->offset]);
}

int btc_tokenizer_is_identifier_part(btc_tokenizer* tokenizer) {
    if(btc_tokenizer_is_number(tokenizer))
        return 1;

    return btc_tokenizer_is_identifier_start(tokenizer);
}

void btc_tokenizer_scan_identifier(btc_tokenizer* tokenizer) {
    size_t start_offset = tokenizer->offset;

    do {
        ++tokenizer->offset;
    } while(btc_tokenizer_is_identifier_part(tokenizer) == 1 && btc_tokenizer_eof(tokenizer) != 1);

    size_t end_offset = tokenizer->offset;

    if(end_offset == start_offset) {
        fprintf(stderr, "unexpected token \"%c\"\n", tokenizer->string[start_offset]);
        return;
    }

    char* buffer;
    btc_tokenizer_slice_string(tokenizer, &buffer, start_offset, end_offset);

    btc_token* token;
    btc_token_init(&token, BTC_TOKEN_IDENTIFIER);
    token->value = buffer;
    token->allocated = buffer;

    btc_tokenizer_push_token(tokenizer, token);
}

int btc_tokenizer_scan_punctuator(btc_tokenizer* tokenizer){
    btc_token* token;
    btc_token_init(&token, BTC_TOKEN_PUNCTUATOR);

    if(btc_tokenizer_compare(tokenizer, "->")) {
        token->value = "->";
        tokenizer->offset += 2;
    } else {
        char* buffer;
        btc_tokenizer_slice_string(tokenizer, &buffer, tokenizer->offset, tokenizer->offset + 1);

        token->value = buffer;
        token->allocated = buffer;
        ++tokenizer->offset;
    }

    btc_tokenizer_push_token(tokenizer, token);
    return 0;
}

int btc_tokenizer_is_keyword(btc_tokenizer* tokenizer) {
    if(!btc_tokenizer_is_identifier_start(tokenizer))
        return 0;

    if(btc_tokenizer_compare(tokenizer, "alias"))
        return 1;
    else if(btc_tokenizer_compare(tokenizer, "type"))
        return 1;
    else if(btc_tokenizer_compare(tokenizer, "namespace"))
        return 1;
    else if(btc_tokenizer_compare(tokenizer, "import"))
        return 1;
    return 0;
}

void btc_tokenizer_scan_keyword(btc_tokenizer* tokenizer) {
    size_t start_offset = tokenizer->offset;

    do {
        tokenizer->offset++;
    } while(!btc_tokenizer_eof(tokenizer) && btc_tokenizer_is_identifier_part(tokenizer));

    size_t end_offset = tokenizer->offset;

    if(end_offset == start_offset)
        fprintf(stderr, "unexpected token \"%c\"\n", tokenizer->string[start_offset]);

    char* buffer;
    btc_tokenizer_slice_string(tokenizer, &buffer, start_offset, end_offset);

    btc_token* token;
    btc_token_init(&token, BTC_TOKEN_KEYWORD);

    token->value = buffer;
    token->allocated = buffer;

    btc_tokenizer_push_token(tokenizer, token);
}

int btc_tokenizer_scan_string(btc_tokenizer* tokenizer) {
    char* mark_initializer = calloc(1, 2*sizeof(char));
    mark_initializer[0] = tokenizer->string[tokenizer->offset++];
    mark_initializer[1] = '\0';

    size_t start_offset = tokenizer->offset;

    while(!btc_tokenizer_eof(tokenizer) && !btc_tokenizer_compare(tokenizer, mark_initializer)) {
        ++tokenizer->offset;
    }

    if(!btc_tokenizer_compare(tokenizer, mark_initializer)) {
        fprintf(stderr, "unexpected end of string with \"%c\"\n", tokenizer->string[tokenizer->offset]);
        return BTC_UNEXPECTED_END;
    }

    ++tokenizer->offset;

    const size_t end_offset = tokenizer->offset;
    char* output;
    btc_tokenizer_slice_string(tokenizer, &output, start_offset + 1, end_offset - 1);

    btc_token* token;
    btc_token_init(&token, BTC_TOKEN_LITERAL_STRING);

    token->value = output;
    token->allocated = output;

    btc_tokenizer_push_token(tokenizer, token);

    free(mark_initializer);

    return BTC_OK;
}

void btc_tokenizer_set_option(btc_tokenizer* tokenizer, int flag, int on) {
    if(on == 1)
        tokenizer->flags |= flag;
    else
        tokenizer->flags &= ~flag;
}

int btc_tokenizer_check_option(btc_tokenizer* tokenizer, int flag) {
    if(tokenizer->flags & flag)
        return 1;

    return 0;
}

int btc_tokenizer_scan_comment_block(btc_tokenizer* tokenizer, const char* open_comment_token, const char* close_comment_token) {
    size_t start_offset = tokenizer->offset;
    tokenizer->offset += strlen(open_comment_token);


    while(!btc_tokenizer_compare(tokenizer, close_comment_token)) {
        tokenizer->offset++;
    }

    tokenizer->offset += strlen(close_comment_token);

    if(!btc_tokenizer_check_option(tokenizer, BTC_TOKENIZER_CONFIG_IGNORE_COMMENTS)) {
        const size_t end_offset = tokenizer->offset;

        char* comment;
        btc_tokenizer_slice_string(tokenizer, &comment, start_offset + strlen(open_comment_token), end_offset - strlen(close_comment_token));

        btc_token* token;
        btc_token_init(&token, BTC_TOKEN_COMMENT);

        token->value = comment;
        token->allocated = comment;

        btc_tokenizer_push_token(tokenizer, token);
    }

    return BTC_OK;
}

int btc_tokenizer_is_number_part(btc_tokenizer* tokenizer) {
    uint8_t ch = tokenizer->buffer[tokenizer->offset];

    // .
    if(ch == 0x2E)
        return 1;

    return btc_tokenizer_is_number(tokenizer);
}

int btc_tokenizer_is_number_start(btc_tokenizer* tokenizer){
    size_t offset = tokenizer->offset;
    uint8_t ch = tokenizer->buffer[offset];

    // - or +
    if(ch_is_number_start(ch)) {
        int only_ch = offset == (tokenizer->string_length - 1) ? 1 : 0;

        if(only_ch || !ch_is_number(tokenizer->buffer[offset + 1]))
            return 0;

        return 1;
    }

    return btc_tokenizer_is_number(tokenizer);
}

void btc_tokenizer_scan_number(btc_tokenizer* tokenizer) {
    size_t start_offset = tokenizer->offset;

    if(btc_tokenizer_is_number_start(tokenizer))
        ++tokenizer->offset;

    while(!btc_tokenizer_eof(tokenizer) && btc_tokenizer_is_number_part(tokenizer)) {
        ++tokenizer->offset;
    }

    size_t end_offset = tokenizer->offset;

    if(start_offset == end_offset) {
        fprintf(stderr, "unexpected end of number\n");
        return;
    }

    char* number_string;
    btc_tokenizer_slice_string(tokenizer, &number_string, start_offset, end_offset);

    btc_token* token;
    btc_token_init(&token, BTC_TOKEN_LITERAL_NUMBER);

    token->number = atof(number_string);
    token->allocated = number_string;

    btc_tokenizer_push_token(tokenizer, token);
}

int btc_tokenizer_scan_sl_comment(btc_tokenizer* tokenizer, const char* open_comment_token) {
    const size_t start_offset = tokenizer->offset;

    tokenizer->offset += strlen(open_comment_token);

    while(!ch_is_line_terminator(tokenizer->buffer[tokenizer->offset]))
        ++tokenizer->offset;

    if(!btc_tokenizer_check_option(tokenizer, BTC_TOKENIZER_CONFIG_IGNORE_COMMENTS)) {
        const size_t end_offset = tokenizer->offset;

        char* buffer;
        btc_tokenizer_slice_string(tokenizer, &buffer, start_offset + strlen(open_comment_token), end_offset);

        btc_token* token;
        btc_token_init(&token, BTC_TOKEN_COMMENT);

        token->value = buffer;
        token->allocated = buffer;
        btc_tokenizer_push_comment(tokenizer, token);
    }

    return BTC_OK;
}

int btc_tokenizer_identify(btc_tokenizer* tokenizer) {
    int status = BTC_OK;
    uint8_t ch = tokenizer->buffer[tokenizer->offset];

    if(ch_is_line_terminator(ch)) {
        ++tokenizer->offset;
        ++tokenizer->line_number;
    } else if(btc_tokenizer_compare(tokenizer, "//")) {
        status = btc_tokenizer_scan_sl_comment(tokenizer, "//");
    } else if(btc_tokenizer_compare(tokenizer, "/*")) {
        status = btc_tokenizer_scan_comment_block(tokenizer, "/*", "*/");
    } else if(btc_tokenizer_is_number_start(tokenizer)) {
        btc_tokenizer_scan_number(tokenizer);
    } else if(btc_tokenizer_is_keyword(tokenizer)) {
        btc_tokenizer_scan_keyword(tokenizer);
    } else if(ch == 0x22) { // string start
        btc_tokenizer_scan_string(tokenizer);
    } else if(btc_tokenizer_is_identifier_start(tokenizer)) {
        btc_tokenizer_scan_identifier(tokenizer);
    } else if(ch == 0x20) { // whitespace
        ++tokenizer->offset;
    } else if(btc_tokenizer_is_punctuator(tokenizer)) {
        status = btc_tokenizer_scan_punctuator(tokenizer);
    } else {
        fprintf(stderr, "Unexpected character %c\n", ch);
        status = BTC_UNEXPECTED_TOKEN;
    }

    return status;
}

void btc_tokenizer_scan(btc_tokenizer* tokenizer, const char* string) {
    size_t string_length = strlen(string);
    uint8_t* buffer = calloc(1, string_length*sizeof(char)+1);
    memcpy(buffer, string, string_length);

    tokenizer->string = string;
    tokenizer->buffer = buffer;
    tokenizer->string_length = string_length;

    while(btc_tokenizer_eof(tokenizer) != 1){
        if(btc_tokenizer_identify(tokenizer) != 0)
            break;
    }
}

void btc_tokenizer_destroy(btc_tokenizer* tokenizer){
    btc_destroy_tokens_list(tokenizer->tokens_list);

    free(tokenizer->buffer);
    free(tokenizer);
}