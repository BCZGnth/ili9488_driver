#ifndef _ILI9488_SHAPES
#define _ILI9488_SHAPES

#include "ili9488_api.h"

void ili9488_draw_vline(Ili9488Defines screen, Ili9488HVLine Line);
void ili9488_draw_hline(Ili9488Defines screen, Ili9488HVLine Line);
void ili9488_draw_rect(Ili9488Defines screen, Ili9488Rect Rect);

void ili9488_draw_circle(Ili9488Circle circ);

void ili9488_loading_bar(Ili9488Defines screen);

#endif // _ILI9488_SHAPES