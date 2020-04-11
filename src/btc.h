#ifndef INTERNAL_BTC_H_
#define INTERNAL_BTC_H_

#define BTC_PASS_OR_RETURN_ERROR(expr) \
    if(expr != BTC_OK) {\
        fprintf(stderr, "failure at %s in line %d\n", __FILE__, __LINE__);\
        return expr;\
    }

#define BTC_PASS_OR_BREAK(expr) \
    if(expr != BTC_OK) break;

#include "../include/btc.h"

#endif
