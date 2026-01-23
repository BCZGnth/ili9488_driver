#ifndef _ili9488_API
#define _ili9488_API

#include <stdio.h>
#include <stdint.h>
#include "ili9488_base.h"

#ifndef __delay_ms
#error MCC does not define __delay_ms Macro. Is _XTAL_FREQ defined?
#endif

void   ili9488_ram_write(                                   Ili9488RamWrite args);
void   ili9488_fill_color(       Ili9488Defines screen,     Ili9488FillBlock args);

void ili9488_write_bitmap(Ili9488Defines screen, Ili9488WriteBitmap args);
// void   ili9488_ramWrite(          ScreenDefines Screen,     ili9488RamWrite    args);
size_t ili9488_write_number(     Ili9488Defines screen,     Ili9488WriteNumber args);
void   ili9488_cls(              Ili9488Defines screen);
void ili9488_clear_block(        Ili9488Defines screen,     Ili9488RamPointer args);
// void   ili9488_clear_line(        ScreenDefines Screen,     Ili9488Clear       args);
// void   ili9488_clear_word(        ScreenDefines Screen,     Ili9488Clear       args);
size_t ili9488_print(            Ili9488Defines screen,     Ili9488Print       args);
void ili9488_draw_pixel(uint16_t x, uint16_t y, color_t color, uint8_t debug_flag);
// void   ili9488_blinking_cursor(   ScreenDefines Screen,     Ili9488Cursor      args);
// void   ili9488_waiting(        ScreenDefines Screen);


#endif // _ili9488_API