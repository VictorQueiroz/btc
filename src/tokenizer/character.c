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

int ch_is_punctuator(uint8_t ch) {
    // { or }
    if(ch == 0x7b || ch == 0x7d)
        return 1;

    // : or ; or , or .
    if(ch == 0x3a || ch == 0x3b || ch == 0x2c || ch == 0x2e)
        return 1;

    // ? or - or =
    if(ch == 0x3f || ch == 0x2d || ch == 0x3D)
        return 1;

    // < or >
    if(ch == 0x3c || ch == 0x3e)
        return 1;

    return 0;
}

int ch_is_identifier_start(uint8_t ch) {
    if(ch >= 0x41 && ch <= 0x5a) // A-Z
        return 1;
    else if(ch >= 0x61 && ch <= 0x7a) // a-z
        return 1;
    else if(ch == 0x5F) // _
        return 1;

    return 0;
}

int ch_is_line_terminator(uint8_t ch) {
    return ch == 0x0A;
}
