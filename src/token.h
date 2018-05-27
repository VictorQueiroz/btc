#ifndef BTC_TOKEN_H_
#define BTC_TOKEN_H_

typedef struct _btc_token btc_token;
typedef struct _btc_token_range btc_token_range;

struct _btc_token_range {
	// Start line number
	int start_line_number;
	// End line number
	int end_line_number;
	// Start offset
	int start_offset;
	// End offset
	int end_offset;
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