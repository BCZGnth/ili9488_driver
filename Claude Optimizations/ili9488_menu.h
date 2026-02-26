/**
 * @file ili9488_menu.h
 * @brief Space-efficient menu system for ILI9488 display
 * 
 * This menu system uses:
 * - Data-driven design (menu definitions in ROM)
 * - Function pointers for actions
 * - Minimal RAM usage
 * - Generic rendering code
 */

#ifndef ILI9488_MENU_H
#define ILI9488_MENU_H

#include "ili9488_base.h"
#include "ili9488_api.h"

// -----------------------------------------------------------------------------
// Menu Configuration
// -----------------------------------------------------------------------------

#define MENU_MAX_ITEMS          8   // Maximum items per menu
#define MENU_ITEM_HEIGHT        30  // Pixels per menu item
#define MENU_ITEM_SPACING       5   // Pixels between items
#define MENU_MARGIN_LEFT        10  // Left margin
#define MENU_MARGIN_TOP         20  // Top margin

// -----------------------------------------------------------------------------
// Menu Item Structure
// -----------------------------------------------------------------------------

typedef void (*menu_action_t)(void);  // Function pointer for menu actions

typedef struct {
    const char* text;       // Menu item text (stored in ROM)
    menu_action_t action;   // Function to call when selected
    uint8_t flags;          // Optional flags (reserved for future use)
} MenuItem;

// -----------------------------------------------------------------------------
// Menu Structure
// -----------------------------------------------------------------------------

typedef struct {
    const char* title;              // Menu title (stored in ROM)
    const MenuItem* items;          // Array of menu items (in ROM)
    uint8_t item_count;             // Number of items
    uint8_t selected_index;         // Currently selected item
    color_t bg_color;               // Background color
    color_t text_color;             // Normal text color
    color_t selected_color;         // Selected item color
    color_t title_color;            // Title color
} Menu;

// -----------------------------------------------------------------------------
// Menu API Functions
// -----------------------------------------------------------------------------

/**
 * @brief Initialize a menu structure
 * 
 * @param menu Pointer to menu structure
 * @param title Menu title string (in ROM)
 * @param items Array of menu items (in ROM)
 * @param count Number of items
 */
void menu_init(Menu* menu, const char* title, const MenuItem* items, uint8_t count);

/**
 * @brief Render the entire menu on screen
 * 
 * @param screen Display structure
 * @param menu Menu to render
 */
void menu_render(Ili9488Defines* screen, const Menu* menu);

/**
 * @brief Move selection up
 * 
 * @param menu Menu to modify
 * @return 1 if selection changed, 0 if at top
 */
uint8_t menu_move_up(Menu* menu);

/**
 * @brief Move selection down
 * 
 * @param menu Menu to modify
 * @return 1 if selection changed, 0 if at bottom
 */
uint8_t menu_move_down(Menu* menu);

/**
 * @brief Execute the currently selected menu item
 * 
 * @param menu Menu containing the selection
 */
void menu_execute_selected(const Menu* menu);

/**
 * @brief Redraw only the selection (efficient update)
 * 
 * @param screen Display structure
 * @param menu Menu to update
 * @param old_index Previous selection index
 */
void menu_update_selection(Ili9488Defines* screen, const Menu* menu, uint8_t old_index);

// -----------------------------------------------------------------------------
// Helper Functions
// -----------------------------------------------------------------------------

/**
 * @brief Calculate Y position for menu item
 * 
 * @param item_index Index of the item (0-based)
 * @return Y coordinate for the item
 */
static inline uint16_t menu_get_item_y(uint8_t item_index) {
    return MENU_MARGIN_TOP + 20 + (item_index * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING));
}

/**
 * @brief Check if item is currently selected
 * 
 * @param menu Menu structure
 * @param item_index Index to check
 * @return 1 if selected, 0 otherwise
 */
static inline uint8_t menu_is_selected(const Menu* menu, uint8_t item_index) {
    return (menu->selected_index == item_index) ? 1 : 0;
}

#endif // ILI9488_MENU_H
