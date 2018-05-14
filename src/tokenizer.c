#include "tokenizer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void btc_tokenizer_init(btc_tokenizer** tokenizer_ptr){
    *tokenizer_ptr = malloc(sizeof(btc_tokenizer));

    btc_tokenizer* tokenizer = *tokenizer_ptr;
    tokenizer->buffer = NULL;
    tokenizer->string = "";
    tokenizer->offset = 0;
    tokenizer->first_token = NULL;
    tokenizer->last_token = NULL;
    tokenizer->string_length = 0;
}

void btc_token_init(btc_token** token_ptr, int type){
    *token_ptr = malloc(sizeof(btc_token));
    btc_token* token = *token_ptr;

    token->type = type;
    token->next_token = NULL;
    token->allocated = NULL;
    token->value = "";
    token->previous_token = NULL;
}

int btc_tokenizer_is_identifier_start(btc_tokenizer* tokenizer){
    uint8_t ch = tokenizer->buffer[tokenizer->offset];

    if(ch >= 0x41 && ch <= 0x5a) // A-Z
        return 1;
    else if(ch >= 0x61 && ch <= 0x7a) // a-z
        return 1;

    return 0;
}

/**
 * Compare if next characters are equivalent to `token`. 
 * Returns 0 if found any match, 1 if none are found
 */
int btc_tokenizer_compare(btc_tokenizer* tokenizer, const char* token) {
    size_t token_length = strlen(token);
    size_t offset = tokenizer->offset;
    for(int i = 0; i < token_length; i++) {
        if(token[i] != tokenizer->string[i + offset])
            return 0;
    }
    return 1;
}

void btc_tokenizer_push_token(btc_tokenizer* tokenizer, btc_token* token) {
    btc_token* last_token = tokenizer->last_token;
    token->previous_token = last_token;

    if(last_token != NULL)
        last_token->next_token = token;

    if(tokenizer->first_token == NULL)
        tokenizer->first_token = token;

    tokenizer->last_token = token;
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
    uint8_t ch = tokenizer->buffer[tokenizer->offset];

    // { or }
    if(ch == 0x7b || ch == 0x7d)
        return 1;

    // : or ; or , 
    if(ch == 0x3a || ch == 0x3b || ch == 0x2c)
        return 1;

    // ? or -
    if(ch == 0x3f || ch == 0x2d)
        return 1;

    // < or >
    if(ch == 0x3c || ch == 0x3e)
        return 1;

    return 0;
}

/**
 * Check if current character is a numeric value
 */
int btc_tokenizer_is_number(btc_tokenizer* tokenizer) {
    uint8_t ch = tokenizer->buffer[tokenizer->offset];

    if(ch >= 0x30 && ch <= 0x39)
        return 1;
    return 0;
}

int btc_tokenizer_is_identifier_part(btc_tokenizer* tokenizer) {
    if(btc_tokenizer_is_number(tokenizer))
        return 1;

    return btc_tokenizer_is_identifier_start(tokenizer);
}

void btc_tokenizer_scan_identifier(btc_tokenizer* tokenizer) {
    size_t start_offset = tokenizer->offset;

    while(btc_tokenizer_is_identifier_part(tokenizer) == 1 && btc_tokenizer_eof(tokenizer) != 1) {
        ++tokenizer->offset;
    }

    size_t end_offset = tokenizer->offset;

    if(end_offset == start_offset) {
        fprintf(stderr, "unexpected token \"%c\"\n", tokenizer->string[start_offset]);
        return;
    }

    size_t total_characters = (end_offset - start_offset);
    char* buffer = malloc(total_characters*sizeof(char));
    memcpy(buffer, &tokenizer->string[start_offset], total_characters);
    buffer[total_characters] = '\0';

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
        char* buffer = malloc(2);
        buffer[0] = tokenizer->string[tokenizer->offset];
        buffer[1] = '\0';

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

    if(btc_tokenizer_compare(tokenizer, "type"))
        return 1;
    return 0;
}

int btc_tokenizer_identify(btc_tokenizer* tokenizer) {
    uint8_t ch = tokenizer->buffer[tokenizer->offset];

    if(btc_tokenizer_compare(tokenizer, "type")) {
        btc_tokenizer_push_keyword(tokenizer, "type");
        tokenizer->offset += strlen("type");
    } else if(btc_tokenizer_is_identifier_start(tokenizer)) {
        btc_tokenizer_scan_identifier(tokenizer);
    } else if(ch == 0x20) { // whitespace
        ++tokenizer->offset;
    } else if(btc_tokenizer_is_punctuator(tokenizer)) {
        return btc_tokenizer_scan_punctuator(tokenizer);
    } else {
        fprintf(stderr, "unexpected end of string with \"%c\"\n", tokenizer->string[tokenizer->offset]);
        return 1;
    }

    return 0;
}

void btc_tokenizer_scan(btc_tokenizer* tokenizer, const char* string) {
    size_t string_length = strlen(string);
    uint8_t* buffer = malloc(string_length*sizeof(char));
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
    btc_token* token = tokenizer->last_token;
    btc_token* previous_token;

    while(token != NULL){
        previous_token = token->previous_token;
        if(token->allocated != NULL) {
            free(token->allocated);
        }
        free(token);
        token = previous_token;
    }

    free(tokenizer->buffer);
    free(tokenizer);
}