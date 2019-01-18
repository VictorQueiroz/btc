#ifndef BTC_TOKEN_H_
#define BTC_TOKEN_H_

#include <stddef.h>

typedef struct _btc_token btc_token;
typedef struct _btc_token_range btc_token_range;

struct _btc_token_range {
	// Start line number
	size_t start_line_number;
	// End line number
	size_t end_line_number;
	// Start offset
	size_t start_offset;
	// End offset
	size_t end_offset;
};

struct _btc_token {
    int type;

    const char* value;
    char* allocated;
    double number;

    struct _btc_token_range range;

};

void btc_destroy_token(btc_token* token);

#endif