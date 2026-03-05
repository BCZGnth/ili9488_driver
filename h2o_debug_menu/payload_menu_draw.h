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

/**
 * @brief Draw the Whitelist options screen
 * 
 * Layout: Exit, Clear whitelist, and add number buttons
 * 
 * @param screen Pointer to screen structure
 */
void draw_whitelist_menu(Ili9488Defines * screen);

// -----------------------------------------------------------------------------
// Menu Button Keys
// -----------------------------------------------------------------------------

enum MainMenuKeys {
    MAIN_BUTTON_EXIT,
    MAIN_BUTTON_SEND_PAYLOAD,
    MAIN_BUTTON_WHITELIST_MENU,
    MAIN_BUTTON_SETTINGS,
    MAIN_MENU_NUM_OF_KEYS
};

enum WhitelistMenuKeys {
    WHITELIST_BUTTON_EXIT,
    WHITELIST_BUTTON_CLEAR,
    WHITELIST_BUTTON_ADD,
    WHITELIST_MENU_NUM_OF_KEYS
};

enum SettingsMenuKeys {
    SETTINGS_BUTTON_EXIT,
    SETTINGS_BUTTON_FREQUENCY,
    SETTINGS_BUTTON_SCAN_INTERVAL,
    SETTINGS_MENU_NUM_OF_KEYS
};

enum PayloadMenuKeys {
    PAYLOAD_BUTTON_1,
    PAYLOAD_BUTTON_2,
    PAYLOAD_BUTTON_3,
    PAYLOAD_BUTTON_4,
    PAYLOAD_BUTTON_EXIT,
    PAYLOAD_MENU_NUM_OF_KEYS
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
    ADD_MENU_NUM_OF_KEYS
};

//  Base offsets for:
//
//      x start and end values compensated to include margins
//      y start and end values compensated for headers at the top
// -----------------------------------------------------------------------------

#define X_START_MARGIN 9
#define X_END_MARGIN 309

#define Y_START_HEADER 30
#define Y_END 474

//  Y values defined With Margins for a single column
//      
// ------------------------------------------------------------------------------
#define OPTIONS_PER_PAGE 5
#define OPTION_RECT_HEIGHT 75
#define OPTION_BOTTOM_PAD 5
#define OPTION_TOTAL_HEIGHT OPTION_RECT_HEIGHT+OPTION_BOTTOM_PAD

#define OPTION1_Y_START Y_START_HEADER
#define OPTION2_Y_START Y_START_HEADER+OPTION_TOTAL_HEIGHT
#define OPTION3_Y_START Y_START_HEADER+(2*OPTION_TOTAL_HEIGHT)
#define OPTION4_Y_START Y_START_HEADER+(3*OPTION_TOTAL_HEIGHT)
#define OPTION5_Y_START Y_START_HEADER+(4*OPTION_TOTAL_HEIGHT)

#define OPTION1_Y_END OPTION1_Y_START+OPTION_TOTAL_HEIGHT-1
#define OPTION2_Y_END OPTION2_Y_START+OPTION_TOTAL_HEIGHT-1
#define OPTION3_Y_END OPTION3_Y_START+OPTION_TOTAL_HEIGHT-1
#define OPTION4_Y_END OPTION4_Y_START+OPTION_TOTAL_HEIGHT-1
#define OPTION5_Y_END OPTION5_Y_START+OPTION_TOTAL_HEIGHT-1


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
    /* Goto Payload Menu */
    {
        .key = MAIN_BUTTON_SEND_PAYLOAD,
        .xs = X_START_MARGIN,
        .ys = OPTION1_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION1_Y_END
    },
    /* Goto WHITELIST menu */
    {
        .key = MAIN_BUTTON_WHITELIST_MENU,
        .xs = X_START_MARGIN,
        .ys = OPTION2_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION2_Y_END,
    },
    /* Goto WHITELIST menu */
    {
        .key = MAIN_BUTTON_SETTINGS,
        .xs = X_START_MARGIN,
        .ys = OPTION3_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION3_Y_END,
    },
    /* Exit */
    {
        .key = MAIN_BUTTON_EXIT,
        .xs = X_START_MARGIN,
        .ys = OPTION5_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION5_Y_END,
    },
};


/**
 * Send Payload Menu Touch Boxes
 * 
 * Layout (480x320 screen):
 * +----------------------------------+
 * |  Payload 1                       |
 * |----------------------------------|
 * |  Payload 2                       |
 * |----------------------------------|
 * |  Payload 3                       |
 * |----------------------------------|
 * |  Payload 4                       |
 * |----------------------------------|
 * |       Exit Button                |
 * +----------------------------------+
 */
static const TouchBox payload_touch_boxes[] = {
    /* Payload 1 Button */
    {
        .key = PAYLOAD_BUTTON_1,
        .xs = X_START_MARGIN,
        .ys = OPTION1_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION1_Y_END
    },
    /* Payload 2 Button */
    {
        .key = PAYLOAD_BUTTON_2,
        .xs = X_START_MARGIN,
        .ys = OPTION2_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION2_Y_END
    },
    /* Payload 3 Button */
    {
        .key = PAYLOAD_BUTTON_3,
        .xs = X_START_MARGIN,
        .ys = OPTION3_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION3_Y_END
    },
    /* Payload 4 Button */
    {
        .key = PAYLOAD_BUTTON_4,
        .xs = X_START_MARGIN,
        .ys = OPTION4_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION4_Y_END
    },
    /* Back Button */
    {
        .key = PAYLOAD_BUTTON_EXIT,
        .xs = X_START_MARGIN,
        .ys = OPTION5_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION5_Y_END
    },
};

/**
 * Settings menu Touch Boxes
 * 
 * Layout (480x320 screen):
 * +----------------------------------+
 * |  Frequency                       |
 * |----------------------------------|
 * |  Scan Interval                   |
 * |----------------------------------|
 * |                                  |
 * |----------------------------------|
 * |                                  |
 * |----------------------------------|
 * |       Exit Button                |
 * +----------------------------------+
 */
static const TouchBox settings_touch_boxes[] = {
    /* Goto Payload Menu */
    {
        .key = SETTINGS_BUTTON_FREQUENCY,
        .xs = X_START_MARGIN,
        .ys = OPTION1_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION1_Y_END
    },
    /* Goto WHITELIST menu */
    {
        .key = SETTINGS_BUTTON_SCAN_INTERVAL,
        .xs = X_START_MARGIN,
        .ys = OPTION2_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION2_Y_END,
    },
    /* Exit */
    {
        .key = SETTINGS_BUTTON_SCAN_INTERVAL,
        .xs = X_START_MARGIN,
        .ys = OPTION5_Y_START,
        .xe = X_END_MARGIN,
        .ye = OPTION5_Y_END,
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

static const TouchBox whitelist_touch_boxes[] = {
    /* Exit */
    {
        .key = WHITELIST_BUTTON_EXIT,
        .xs = 7,
        .ys = 367,
        .xe = 108,
        .ye = 469,
    },
    /* Clear SN List */
    {
        .key = WHITELIST_BUTTON_CLEAR,
        .xs = 211,
        .ys = 367,
        .xe = 312,
        .ye = 469,
    },
    /* Add Serial Number */
    {
        .key = WHITELIST_BUTTON_ADD,
        .xs = 109,
        .ys = 367,
        .xe = 210,
        .ye = 469,
    },
};


#endif // _PAYLOAD_MENU_DRAW_H
