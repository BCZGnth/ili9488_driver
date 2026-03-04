/**
 * @file payload_menu_draw.h
 * @brief Menu drawing functions for the payload menu system
 * 
 * Follows the pattern from menu.h/menu.c
 */

#ifndef _PAYLOAD_MENU_DRAW_H
#define _PAYLOAD_MENU_DRAW_H

#include "xpt2046.h"
#include "ili9488_base.h"

/**
 * @brief Draw the main menu screen
 * 
 * Layout: 3 buttons at bottom (Whitelist, Send Payload, Exit)
 *         Whitelist display area above
 * 
 * @param screen Pointer to screen structure
 */
void draw_main_menu(Ili9488Defines* screen);

/**
 * @brief Draw the number pad screen
 * 
 * Layout: 10 numeric buttons, with an enter and delete / exit key
 *         Whitelist display area above
 * 
 * @param screen Pointer to screen structure
 */
void debug_draw_add_menu(Ili9488Defines * screen);

/**
 * @brief Draw the send payload menu screen
 * 
 * Layout: 4 payload buttons + Back button
 * 
 * @param screen Pointer to screen structure
 */
void draw_send_payload_menu(Ili9488Defines* screen);

// -----------------------------------------------------------------------------
// Menu Button Keys
// -----------------------------------------------------------------------------

enum MainMenuKeys {
    MAIN_BUTTON_WHITELIST,
    MAIN_BUTTON_SEND_PAYLOAD,
    MAIN_BUTTON_EXIT,
};

enum PayloadMenuKeys {
    PAYLOAD_BUTTON_1,
    PAYLOAD_BUTTON_2,
    PAYLOAD_BUTTON_3,
    PAYLOAD_BUTTON_4,
    PAYLOAD_BUTTON_BACK,
};

enum AddMenuKeys {
    ADD_BUTTON_ZERO,
    ADD_BUTTON_ONE,
    ADD_BUTTON_TWO,
    ADD_BUTTON_THREE,
    ADD_BUTTON_FOUR,
    ADD_BUTTON_FIVE,
    ADD_BUTTON_SIX,
    ADD_BUTTON_SEVEN,
    ADD_BUTTON_EIGHT,
    ADD_BUTTON_NINE,
    ADD_BUTTON_DELETE,
    ADD_BUTTON_ADD_SN,
};

// -----------------------------------------------------------------------------
// Touch Box Definitions
// -----------------------------------------------------------------------------

/**
 * Main Menu Touch Boxes
 * 
 * Layout (480x320 screen):
 * +----------------------------------+
 * |                                  |
 * |     Whitelist Display Area       |
 * |                                  |
 * |----------------------------------|
 * | Whitelist | Send Payload | Exit  |
 * +----------------------------------+
 */
static const TouchBox main_touch_boxes[] = {
    /* Clear SN List */
    {
        .key = MAIN_BUTTON_WHITELIST,
        .xs = 211,
        .ys = 367,
        .xe = 312,
        .ye = 469,
    },
    /* Add Serial Number */
    {
        .key = MAIN_BUTTON_SEND_PAYLOAD,
        .xs = 109,
        .ys = 367,
        .xe = 210,
        .ye = 469,
    },
    /* Exit */
    {
        .key = MAIN_BUTTON_EXIT,
        .xs = 7,
        .ys = 367,
        .xe = 108,
        .ye = 469,
    },
};


/**
 * Send Payload Menu Touch Boxes
 * 
 * Layout (480x320 screen):
 * +----------------------------------+
 * |  Payload 1  |  Payload 2         |
 * |----------------------------------|
 * |  Payload 3  |  Payload 4         |
 * |----------------------------------|
 * |       Back Button                |
 * +----------------------------------+
 */
static const TouchBox payload_touch_boxes[] = {
    /* Payload 1 Button */
    {
        .key = PAYLOAD_BUTTON_1,
        .xs = 10,
        .ys = 10,
        .xe = 150,
        .ye = 150,
    },
    /* Payload 2 Button */
    {
        .key = PAYLOAD_BUTTON_2,
        .xs = 151,
        .ys = 10,
        .xe = 310,
        .ye = 150,
    },
    /* Payload 3 Button */
    {
        .key = PAYLOAD_BUTTON_3,
        .xs = 10,
        .ys = 160,
        .xe = 150,
        .ye = 310,
    },
    /* Payload 4 Button */
    {
        .key = PAYLOAD_BUTTON_4,
        .xs = 151,
        .ys = 160,
        .xe = 150,
        .ye = 310,
    },
    /* Back Button */
    {
        .key = PAYLOAD_BUTTON_BACK,
        .xs = 85,
        .ys = 320,
        .xe = 235,
        .ye = 470,
    },
};

/**
 * Send Payload Menu Touch Boxes
 * 
 * Layout (480x320 screen):
 * +----------------------------------+
 * |       Number entry line(s)       |
 * |----------------------------------|
 * |           <Numbers>              |
 * |----------------------------------|
 * |   Delete  |    0    |   Add sn   |
 * +----------------------------------+
 */
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

#endif // _PAYLOAD_MENU_DRAW_H
