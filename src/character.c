#include "character.h"

#include <stdint.h>

int ch_is_number(uint8_t ch) {
    // 0 to 9 utf8 characters
    if(ch >= 0x30 && ch <= 0x39)
        return 1;
    return 0;
}

/**
 * almost like ch_is_number() except that it 
 * check for 2 extra characters (+, -)
 */
int ch_is_number_start(uint8_t ch) {
    if(ch == 0x2D || ch == 0x2B)
        return 1;

    return ch_is_number(ch);
}
