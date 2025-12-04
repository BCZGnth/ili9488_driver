#ifndef _ili9488_API
#define _ili9488_API

#include <stdio.h>
#include <stdint.h>
#include "ili9488_base.h"

#ifndef __delay_ms
#error MCC does not define __delay_ms Macro. Is _XTAL_FREQ defined?
#endif

void   ili9488_write_bitmap(ili9488_interface_t inter, Ili9488WriteBitmap args);
// void   ili9488_ramWrite(       ScreenDefines Screen, ili9488RamWrite    args);
size_t ili9488_write_number(ili9488_interface_t inter, Ili9488WriteNumber args);
void   ili9488_cls(         ili9488_interface_t inter);
// void   ili9488_clear_line(     ScreenDefines Screen, Ili9488Clear       args);
// void   ili9488_clear_word(     ScreenDefines Screen, Ili9488Clear       args);
size_t ili9488_print(       ili9488_interface_t inter, Ili9488Print       args);
// void   ili9488_blinking_cursor(ScreenDefines Screen, Ili9488Cursor      args);
// void   ili9488_waiting(        ScreenDefines Screen);


#endif // _ili9488_API