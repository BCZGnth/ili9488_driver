#ifndef _MENU_H
#define _MENU_H

#include "xpt2046.h"
#include "ili9488_base.h"

void debug_draw_add_menu(Ili9488Defines screen);
void debug_draw_main_menu(Ili9488Defines screen);

enum AddMenuKeys {
    ADD_BUTTON_ONE,
    ADD_BUTTON_TWO,
    ADD_BUTTON_THREE,
    ADD_BUTTON_FOUR,
    ADD_BUTTON_FIVE,
    ADD_BUTTON_SIX,
    ADD_BUTTON_SEVEN,
    ADD_BUTTON_EIGHT,
    ADD_BUTTON_NINE,
    ADD_BUTTON_ZERO,
    ADD_BUTTON_DELETE,
    ADD_BUTTON_ADD_SN,
};

enum MainMenuKeys {
    MAIN_BUTTON_EXIT,
    MAIN_BUTTON_CLEAR_SN_LIST,
    MAIN_BUTTON_ADD_SN,
};

static const TouchBox add_touch_boxes[] = {
    /* #1 */
    {
        .key = ADD_BUTTON_ONE,
        .xs = 9,
        .ys = 69,
        .xe = 110,
        .ye = 170,
    },
    /* 2 */
    {
        .key = ADD_BUTTON_TWO,
        .xs = 111,
        .ys = 69,
        .xe = 212,
        .ye = 170,
    },
    /* 3 */
    {
        .key = ADD_BUTTON_THREE,
        .xs = 213,
        .ys = 69,
        .xe = 314,
        .ye = 170,
    },
    /* #4 */
    {
        .key = ADD_BUTTON_FOUR,
        .xs = 9,
        .ys = 171,
        .xe = 110,
        .ye = 272,
    },
    /* 5 */
    {
        .key = ADD_BUTTON_FIVE,
        .xs = 111,
        .ys = 171,
        .xe = 212,
        .ye = 272,
    },
    /* 6 */
    {
        .key = ADD_BUTTON_SIX,
        .xs = 213,
        .ys = 171,
        .xe = 314,
        .ye = 272,
    },
    /* #7 */
    {
        .key = ADD_BUTTON_SEVEN,
        .xs = 9,
        .ys = 273,
        .xe = 110,
        .ye = 374,
    },
    /* 8 */
    {
        .key = ADD_BUTTON_EIGHT,
        .xs = 111,
        .ys = 273,
        .xe = 212,
        .ye = 374,
    },
    /* 9 */
    {
        .key = ADD_BUTTON_NINE,
        .xs = 213,
        .ys = 273,
        .xe = 314,
        .ye = 374,
    },
    /* Delete */
    {
        .key = ADD_BUTTON_DELETE,
        .xs = 9,
        .ys = 375,
        .xe = 110,
        .ye = 476,
    },
    /* 0 */
    {
        .key = ADD_BUTTON_ZERO,
        .xs = 111,
        .ys = 375,
        .xe = 212,
        .ye = 476,
    },
    /* Add */
    {
        .key = ADD_BUTTON_ADD_SN,
        .xs = 213,
        .ys = 375,
        .xe = 314,
        .ye = 476,
    },
};

#endif // _MENU_H