#ifndef _KEY_UTIL_H_
#define _KEY_UTIL_H_

#include <stdint.h>
#include <stdbool.h>

bool is_shift(uint16_t code);
char* get_key_text(uint16_t code, int shift_pressed);

#endif
