#ifndef BTC_CHARACTER_H_
#define BTC_CHARACTER_H_

#include <stdint.h>

int ch_is_number(uint8_t ch);
int ch_is_number_start(uint8_t ch);
int ch_is_punctuator(uint8_t ch);
int ch_is_identifier_start(uint8_t ch);

#endif