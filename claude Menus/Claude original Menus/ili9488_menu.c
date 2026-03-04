/**
 * @file ili9488_menu.c
 * @brief Menu system implementation
 */

#include "ili9488_menu.h"
#include "ili9488_shapes.h"
#include <string.h>

// -----------------------------------------------------------------------------
// Menu Initialization
// -----------------------------------------------------------------------------

void menu_init(Menu* menu, const char* title, const MenuItem* items, uint8_t count) {
    menu->title = title;
    menu->items = items;
    menu->item_count = count;
    menu->selected_index = 0;
    
    // Default colors
    menu->bg_color = BLACK;
    menu->text_color = WHITE;
    menu->selected_color = CYAN;
    menu->title_color = YELLOW;
}

// -----------------------------------------------------------------------------
// Menu Rendering
// -----------------------------------------------------------------------------

void menu_render(Ili9488Defines* screen, const Menu* menu) {
    // Clear screen
    ili9488_cls(*screen);
    
    // Draw title
    Ili9488Print title_print = {
        .text = (char*)menu->title,
        .fg = menu->title_color,
        .bg = menu->bg_color,
        .font = screen->Screen.offset_2x,  // Larger font for title
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = MENU_MARGIN_TOP - 10,
            .start_y = MENU_MARGIN_LEFT,
            .end_x = MENU_MARGIN_TOP + 20,
            .end_y = screen->Screen.ScreenWidth - MENU_MARGIN_LEFT
        }
    };
    ili9488_print(*screen, title_print);
    
    // Draw separator line under title
    Ili9488HVLine separator = {
        .xstart = MENU_MARGIN_TOP + 5,
        .ystart = MENU_MARGIN_LEFT,
        .length = screen->Screen.ScreenWidth - (2 * MENU_MARGIN_LEFT),
        .weight = 2,
        .color = menu->title_color
    };
    ili9488_draw_hline(*screen, separator);
    
    // Draw each menu item
    for (uint8_t i = 0; i < menu->item_count; i++) {
        uint16_t item_y = menu_get_item_y(i);
        uint8_t is_selected = menu_is_selected(menu, i);
        
        // Background for selected item
        if (is_selected) {
            Ili9488FillBlock selection_bg = {
                .color = menu->selected_color,
                .ram_ptr = {
                    .start_x = item_y - 2,
                    .start_y = MENU_MARGIN_LEFT - 2,
                    .end_x = item_y + MENU_ITEM_HEIGHT - 8,
                    .end_y = screen->Screen.ScreenWidth - MENU_MARGIN_LEFT + 2
                }
            };
            ili9488_fill_color(*screen, selection_bg);
        }
        
        // Draw item text
        Ili9488Print item_print = {
            .text = (char*)menu->items[i].text,
            .fg = is_selected ? BLACK : menu->text_color,
            .bg = is_selected ? menu->selected_color : menu->bg_color,
            .font = screen->Screen.offset_1x,
            .clear_before = 0,
            .line_spacing = 0,
            .ram_ptr = {
                .start_x = item_y,
                .start_y = MENU_MARGIN_LEFT + 5,
                .end_x = item_y + MENU_ITEM_HEIGHT - 10,
                .end_y = screen->Screen.ScreenWidth - MENU_MARGIN_LEFT - 5
            }
        };
        ili9488_print(*screen, item_print);
    }
}

// -----------------------------------------------------------------------------
// Menu Navigation
// -----------------------------------------------------------------------------

uint8_t menu_move_up(Menu* menu) {
    if (menu->selected_index > 0) {
        menu->selected_index--;
        return 1;
    }
    return 0;
}

uint8_t menu_move_down(Menu* menu) {
    if (menu->selected_index < menu->item_count - 1) {
        menu->selected_index++;
        return 1;
    }
    return 0;
}

void menu_execute_selected(const Menu* menu) {
    if (menu->items[menu->selected_index].action != NULL) {
        menu->items[menu->selected_index].action();
    }
}

// -----------------------------------------------------------------------------
// Efficient Menu Update
// -----------------------------------------------------------------------------

void menu_update_selection(Ili9488Defines* screen, const Menu* menu, uint8_t old_index) {
    // Redraw old selection (now unselected)
    uint16_t old_y = menu_get_item_y(old_index);
    
    // Clear old selection background
    Ili9488FillBlock old_bg = {
        .color = menu->bg_color,
        .ram_ptr = {
            .start_x = old_y - 2,
            .start_y = MENU_MARGIN_LEFT - 2,
            .end_x = old_y + MENU_ITEM_HEIGHT - 8,
            .end_y = screen->Screen.ScreenWidth - MENU_MARGIN_LEFT + 2
        }
    };
    ili9488_fill_color(*screen, old_bg);
    
    // Redraw old item text (unselected)
    Ili9488Print old_print = {
        .text = (char*)menu->items[old_index].text,
        .fg = menu->text_color,
        .bg = menu->bg_color,
        .font = screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = old_y,
            .start_y = MENU_MARGIN_LEFT + 5,
            .end_x = old_y + MENU_ITEM_HEIGHT - 10,
            .end_y = screen->Screen.ScreenWidth - MENU_MARGIN_LEFT - 5
        }
    };
    ili9488_print(*screen, old_print);
    
    // Draw new selection
    uint16_t new_y = menu_get_item_y(menu->selected_index);
    
    // Selection background
    Ili9488FillBlock new_bg = {
        .color = menu->selected_color,
        .ram_ptr = {
            .start_x = new_y - 2,
            .start_y = MENU_MARGIN_LEFT - 2,
            .end_x = new_y + MENU_ITEM_HEIGHT - 8,
            .end_y = screen->Screen.ScreenWidth - MENU_MARGIN_LEFT + 2
        }
    };
    ili9488_fill_color(*screen, new_bg);
    
    // Selection text
    Ili9488Print new_print = {
        .text = (char*)menu->items[menu->selected_index].text,
        .fg = BLACK,
        .bg = menu->selected_color,
        .font = screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = new_y,
            .start_y = MENU_MARGIN_LEFT + 5,
            .end_x = new_y + MENU_ITEM_HEIGHT - 10,
            .end_y = screen->Screen.ScreenWidth - MENU_MARGIN_LEFT - 5
        }
    };
    ili9488_print(*screen, new_print);
}
