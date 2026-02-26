/**
 * @file touch_menu_system.h
 * @brief Touch-based menu system for ILI9488 + XPT2046
 * 
 * This system uses touch input instead of buttons to navigate menus.
 * Touch boxes are created for each menu item, and touches are detected
 * and mapped to menu actions.
 */

#ifndef TOUCH_MENU_SYSTEM_H
#define TOUCH_MENU_SYSTEM_H

#include "ili9488_menu.h"
#include "xpt2046.h"

// -----------------------------------------------------------------------------
// Touch Menu Configuration
// -----------------------------------------------------------------------------

#define TOUCH_MENU_MAX_BOXES    8   // Maximum touch boxes per screen
#define TOUCH_DEBOUNCE_MS       200 // Minimum time between touch events

// -----------------------------------------------------------------------------
// Touch Menu Item Structure
// -----------------------------------------------------------------------------

typedef struct {
    const char* text;       // Menu item text (stored in ROM)
    menu_action_t action;   // Function to call when touched
    TouchBox touch_box;     // Touch area for this item
    uint8_t flags;          // Optional flags
} TouchMenuItem;

// -----------------------------------------------------------------------------
// Touch Menu Structure
// -----------------------------------------------------------------------------

typedef struct {
    const char* title;              // Menu title (stored in ROM)
    const TouchMenuItem* items;     // Array of touch menu items (in ROM)
    TouchBox* touch_boxes;          // Array of touch boxes for current menu
    uint8_t item_count;             // Number of items
    uint8_t current_highlight;      // Currently highlighted item (255 = none)
    color_t bg_color;               // Background color
    color_t text_color;             // Normal text color
    color_t highlight_color;        // Highlighted item color
    color_t title_color;            // Title color
} TouchMenu;

// -----------------------------------------------------------------------------
// Menu System State
// -----------------------------------------------------------------------------

typedef enum {
    TOUCH_MENU_IDLE,        // No touch detected
    TOUCH_MENU_PRESSED,     // Touch detected and held
    TOUCH_MENU_RELEASED,    // Touch just released
    TOUCH_MENU_DEBOUNCING   // In debounce period
} TouchMenuState;

// -----------------------------------------------------------------------------
// Touch Menu API Functions
// -----------------------------------------------------------------------------

/**
 * @brief Initialize a touch menu
 * 
 * @param menu Pointer to menu structure
 * @param title Menu title string (in ROM)
 * @param items Array of touch menu items (in ROM)
 * @param count Number of items
 */
void touch_menu_init(TouchMenu* menu, const char* title, 
                     const TouchMenuItem* items, uint8_t count);

/**
 * @brief Build touch boxes for the current menu
 * 
 * This must be called after init to set up the touch areas.
 * Touch boxes are automatically positioned based on menu layout.
 * 
 * @param menu Menu to build touch boxes for
 */
void touch_menu_build_boxes(TouchMenu* menu);

/**
 * @brief Render the touch menu on screen
 * 
 * @param screen Display structure
 * @param menu Menu to render
 */
void touch_menu_render(Ili9488Defines* screen, TouchMenu* menu);

/**
 * @brief Process a touch event
 * 
 * @param menu Current menu
 * @param touch Touch coordinates
 * @return Index of touched item (255 if no item touched)
 */
uint8_t touch_menu_process_touch(TouchMenu* menu, Touch touch);

/**
 * @brief Highlight a menu item (visual feedback on touch)
 * 
 * @param screen Display structure
 * @param menu Menu containing the item
 * @param item_index Index of item to highlight (255 to clear)
 */
void touch_menu_highlight_item(Ili9488Defines* screen, TouchMenu* menu, 
                                uint8_t item_index);

/**
 * @brief Execute the action for a menu item
 * 
 * @param menu Menu containing the item
 * @param item_index Index of item to execute
 */
void touch_menu_execute_item(const TouchMenu* menu, uint8_t item_index);

// -----------------------------------------------------------------------------
// Touch State Management
// -----------------------------------------------------------------------------

/**
 * @brief Update touch state machine
 * 
 * Call this in your main loop to handle touch debouncing and state transitions
 * 
 * @param current_touch_state Current touch reading (true if touched)
 * @return Current state of the touch menu system
 */
TouchMenuState touch_menu_update_state(bool current_touch_state);

/**
 * @brief Reset touch debounce timer
 * 
 * Call this after processing a touch event
 */
void touch_menu_reset_debounce(void);

/**
 * @brief Check if enough time has passed since last touch
 * 
 * @return true if debounce period is over
 */
bool touch_menu_debounce_ready(void);

// -----------------------------------------------------------------------------
// Helper Functions
// -----------------------------------------------------------------------------

/**
 * @brief Calculate Y position for menu item
 * 
 * @param item_index Index of the item (0-based)
 * @return Y coordinate for the item
 */
static inline uint16_t touch_menu_get_item_y(uint8_t item_index) {
    return MENU_MARGIN_TOP + 20 + (item_index * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING));
}

/**
 * @brief Calculate touch box for a menu item
 * 
 * @param item_index Index of the item
 * @param screen_width Width of the screen
 * @return TouchBox structure for this item
 */
TouchBox touch_menu_calculate_box(uint8_t item_index, uint16_t screen_width);

#endif // TOUCH_MENU_SYSTEM_H
