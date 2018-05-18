#include "btc.h"
#include "tokenizer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void btc_tokenizer_init(btc_tokenizer** tokenizer_ptr){
    *tokenizer_ptr = calloc(1, sizeof(btc_tokenizer));
}

void btc_token_init(btc_token** token_ptr, int type){
    *token_ptr = calloc(1, sizeof(btc_token));

    btc_token* token = *token_ptr;
    token->type = type;
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
    if(strncmp(&tokenizer->string[tokenizer->offset], token, strlen(token)) == 0)
        return 1;

    return 0;
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

    // : or ; or , or .
    if(ch == 0x3a || ch == 0x3b || ch == 0x2c || ch == 0x2e)
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

    do {
        ++tokenizer->offset;
    } while(btc_tokenizer_is_identifier_part(tokenizer) == 1 && btc_tokenizer_eof(tokenizer) != 1);

    size_t end_offset = tokenizer->offset;

    if(end_offset == start_offset) {
        fprintf(stderr, "unexpected token \"%c\"\n", tokenizer->string[start_offset]);
        return;
    }

    size_t total_characters = (end_offset - start_offset);
    char* buffer = calloc(1, total_characters*sizeof(char)+1);
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
        char* buffer = calloc(1, 2*sizeof(char));
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

    size_t characters_length = (end_offset - start_offset)*sizeof(char);
    char* buffer = malloc(characters_length + 1);
    strncpy(buffer, &tokenizer->string[start_offset], characters_length);
    buffer[characters_length] = '\0';

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

    const size_t end_offset = tokenizer->offset;

    ++tokenizer->offset;

    const size_t string_length = end_offset - start_offset;
    char* output = calloc(1, (string_length*sizeof(char)));
    memcpy(output, &tokenizer->string[start_offset], string_length);

    btc_token* token;
    btc_token_init(&token, BTC_TOKEN_LITERAL_STRING);

    token->value = output;
    token->allocated = output;

    btc_tokenizer_push_token(tokenizer, token);

    free(mark_initializer);

    return BTC_OK;
}

int btc_tokenizer_scan_comment(btc_tokenizer* tokenizer, const char* initializer) {
    char* finisher;

    if(strncmp(initializer, "/*", 2) == 0)
        finisher = "*/";
    else
        return BTC_UNEXPECTED_TOKEN;

    tokenizer->offset += strlen(initializer);

    size_t start_offset = tokenizer->offset;

    while(!btc_tokenizer_compare(tokenizer, finisher)) {
        tokenizer->offset++;
    }

    size_t end_offset = tokenizer->offset;
    tokenizer->offset += strlen(finisher);

    size_t total_characters = end_offset - start_offset;
    char* comment = calloc(1, total_characters*sizeof(char));
    memcpy(comment, &tokenizer->string[start_offset], total_characters);

    btc_token* token;
    btc_token_init(&token, BTC_TOKEN_COMMENT);

    token->value = comment;
    token->allocated = comment;

    btc_tokenizer_push_token(tokenizer, token);

    return BTC_OK;
}

int btc_tokenizer_identify(btc_tokenizer* tokenizer) {
    uint8_t ch = tokenizer->buffer[tokenizer->offset];

    if(btc_tokenizer_compare(tokenizer, "/*")) {
        btc_tokenizer_scan_comment(tokenizer, "/*");
    } else if(btc_tokenizer_is_keyword(tokenizer)) {
        btc_tokenizer_scan_keyword(tokenizer);
    } else if(ch == 0x22) { // string start
        btc_tokenizer_scan_string(tokenizer);
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