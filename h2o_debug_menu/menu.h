#ifndef _MENU_H
#define _MENU_H

#include "xpt2046.h"
#include "ili9488_base.h"

void debug_draw_add_menu(Ili9488Defines screen);
void debug_draw_main_menu(Ili9488Defines screen);

static const TouchBox add_touch_boxes[] = {
    /* #1 */
    {
        .xs = 4,
        .ys = 24,
        .xe = 106,
        .ye = 126,
    }

    /* 2 */
};

#endif // _MENU_H