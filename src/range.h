#ifndef BTC_RANGE_H
#define BTC_RANGE_H

#include <stddef.h>

typedef struct {
    // Start line number
    size_t start_line_number;
    // End line number
    size_t end_line_number;
    // Start offset
    size_t start_offset;
    // End offset
    size_t end_offset;
} btc_range;

#endif //BTC_RANGE_H
