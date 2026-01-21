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
        .xe = 105,
        .ye = 125,
    },

    /* 2 */
    {
        .xs = 106,
        .ys = 24,
        .xe = 207,
        .ye = 125,
    },

    /* 3 */
    {
        .xs = 208,
        .ys = 24,
        .xe = 309,
        .ye = 125,
    },

    /* #4 */
    {
        .xs = 4,
        .ys = 126,
        .xe = 105,
        .ye = 227,
    },

    /* 5 */
    {
        .xs = 106,
        .ys = 126,
        .xe = 207,
        .ye = 227,
    },

    /* 6 */
    {
        .xs = 208,
        .ys = 126,
        .xe = 309,
        .ye = 227,
    },

    /* #7 */
    {
        .xs = 4,
        .ys = 228,
        .xe = 105,
        .ye = 329,
    },

    /* 8 */
    {
        .xs = 106,
        .ys = 228,
        .xe = 207,
        .ye = 329,
    },

    /* 9 */
    {
        .xs = 208,
        .ys = 228,
        .xe = 309,
        .ye = 329,
    },

    /* Delete */
    {
        .xs = 4,
        .ys = 330,
        .xe = 105,
        .ye = 431,
    },

    /* 0 */
    {
        .xs = 106,
        .ys = 330,
        .xe = 207,
        .ye = 431,
    },

    /* Add */
    {
        .xs = 208,
        .ys = 330,
        .xe = 309,
        .ye = 431,
    },
};

#endif // _MENU_H