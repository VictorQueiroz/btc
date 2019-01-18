#include "token.h"

#include <stdlib.h>
#include <stddef.h>

void btc_destroy_token(btc_token* token) {
	if(token->allocated != NULL)
		free(token->allocated);

	free(token);
}