#include <linux/input.h>

#include "util.h"
#include "key_util.h"

#define UK "\0" //UK represents unknown keys in the key list

/* The order of the keys is defined in linux/input.h */
static const char *keymap[] = {
    "UK", "_ESC_", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=",
    "_BACKSPACE_", "_TAB_", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p",
    "[", "]", "_ENTER_", "_CTRL_", "a", "s", "d", "f", "g", "h", "j", "k", "l",
    ";", "'", "`", "_SHIFT_", "\\", "z", "x", "c", "v", "b", "n", "m", ",",
    ".", "/", "_SHIFT_", "UK", "UK", " ", "_CAPSLOCK_", "_F1_", "_F2_", "_F3_",
    "_F4_", "_F5_", "_F6_", "_F7_", "_F8_", "_F9_", "_F10_", "_NUMLOCK_",
    "_SCROLLLOCK_", "_HOME_", "_UP_", "_PGUP_", "-", "_LEFT_", "5",
    "_RTARROW_", "+", "_END_", "_DOWN_", "_PGDN_", "_INS_", "_DEL_", "UK",
    "UK", "UK", "_F11_", "_F12_", "UK", "UK", "UK", "UK", "UK", "UK", "UK",
    "_ENTER_", "CTRL_", "/", "_PRTSCR_", "ALT", "UK", "_HOME_", "_UP_",
    "_PGUP_", "_LEFT_", "_RIGHT_", "_END_", "_DOWN_", "_PGDN_", "_INSERT_",
    "_DEL_", "UK", "UK", "UK", "UK", "UK", "UK", "UK", "_PAUSE_"
};
 static const char *keymap_shift[] = {
 "UK", "_ESC_", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+",
 "_BACKSPACE_", "_TAB_", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
 "{", "}", "_ENTER_", "_CTRL_", "A", "S", "D", "F", "G", "H", "J", "K", "L",
 ":", "\"", "~", "_SHIFT_", "|", "Z", "X", "C", "V", "B", "N", "M", "<",
 ">", "?", "_SHIFT_", "UK", "UK", " ", "_CAPSLOCK_", "_F1_", "_F2_", "_F3_",
 "_F4_", "_F5_", "_F6_", "_F7_", "_F8_", "_F9_", "_F10_", "_NUMLOCK_",
 "_SCROLLLOCK_", "_HOME_", "_UP_", "_PGUP_", "-", "_LEFT_", "5",
 "_RTARROW_", "+", "_END_", "_DOWN_", "_PGDN_", "_INS_", "_DEL_", "UK",
 "UK", "UK", "_F11_", "_F12_", "UK", "UK", "UK", "UK", "UK", "UK", "UK",
 "_ENTER_", "CTRL_", "/", "_PRTSCR_", "ALT", "UK", "_HOME_", "_UP_",
 "_PGUP_", "_LEFT_", "_RIGHT_", "_END_", "_DOWN_", "_PGDN_", "_INSERT_",
 "_DEL_", "UK", "UK", "UK", "UK", "UK", "UK", "UK", "_PAUSE_"
 };


/*Determines whether the key code is a shift. Shift ENGAGED = True */
bool is_shift(uint16_t code) {
   return code == KEY_LEFTSHIFT || code == KEY_RIGHTSHIFT;
}

/*Receives 16 bit key code and determines if shift is pressed, returns corresponding ascii char*/
char *get_key_text(uint16_t code, int shift_pressed) {
   ASSERT_ON_COMPILE(ARRAY_SIZE(keymap) == ARRAY_SIZE(keymap_shift));

   char **arr;
   if (shift_pressed == 1) {
      arr = keymap_shift;
   } else {
      arr = keymap;
   }

   if (code < ARRAY_SIZE(keymap)) {
      return arr[code];
   } else {
      printf("Unknown key: %u", code);
      return UK;
   }
}
