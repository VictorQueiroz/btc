#ifndef PARSER_IDENTIFIER_H_
#define PARSER_IDENTIFIER_H_

#define BTC_IDENTIFIER 5

typedef struct _btc_ast_identifier btc_ast_identifier;

struct _btc_ast_identifier {
    const char* value;
};

#endif
