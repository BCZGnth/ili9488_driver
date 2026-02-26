/**
 * @file touch_menu_system.c
 * @brief Implementation of touch-based menu system
 */

#include "touch_menu_system.h"
#include "ili9488_shapes.h"
#include <string.h>

// -----------------------------------------------------------------------------
// Touch State Variables
// -----------------------------------------------------------------------------

static TouchMenuState g_touch_state = TOUCH_MENU_IDLE;
static uint32_t g_last_touch_time = 0;
static uint32_t g_touch_timer = 0;  // Simple millisecond counter
static bool g_previous_touch = false;

// -----------------------------------------------------------------------------
// Touch Menu Initialization
// -----------------------------------------------------------------------------

void touch_menu_init(TouchMenu* menu, const char* title, 
                     const TouchMenuItem* items, uint8_t count) {
    menu->title = title;
    menu->items = items;
    menu->item_count = count;
    menu->current_highlight = 255;  // No highlight
    
    // Default colors
    menu->bg_color = BLACK;
    menu->text_color = WHITE;
    menu->highlight_color = CYAN;
    menu->title_color = YELLOW;
}

// -----------------------------------------------------------------------------
// Touch Box Building
// -----------------------------------------------------------------------------

TouchBox touch_menu_calculate_box(uint8_t item_index, uint16_t screen_width) {
    uint16_t item_y = touch_menu_get_item_y(item_index);
    
    TouchBox box = {
        .xs = item_y - 2,                           // Start X (top edge)
        .xe = item_y + MENU_ITEM_HEIGHT - 8,        // End X (bottom edge)
        .ys = MENU_MARGIN_LEFT - 2,                 // Start Y (left edge)
        .ye = screen_width - MENU_MARGIN_LEFT + 2,  // End Y (right edge)
        .key = item_index                           // Item index as key
    };
    
    return box;
}

void touch_menu_build_boxes(TouchMenu* menu) {
    // Note: The touch boxes are embedded in the TouchMenuItem structure
    // This function updates them based on current screen dimensions
    // In a real implementation, you might store these separately
    
    // For now, this is handled when we process touches
    // The boxes are calculated on-the-fly in touch_menu_process_touch()
}

// -----------------------------------------------------------------------------
// Touch Menu Rendering
// -----------------------------------------------------------------------------

void touch_menu_render(Ili9488Defines* screen, TouchMenu* menu) {
    // Clear screen
    ili9488_cls(*screen);
    
    // Draw title
    Ili9488Print title_print = {
        .text = (char*)menu->title,
        .fg = menu->title_color,
        .bg = menu->bg_color,
        .font = screen->Screen.offset_2x,
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
        uint16_t item_y = touch_menu_get_item_y(i);
        uint8_t is_highlighted = (menu->current_highlight == i) ? 1 : 0;
        
        // Background for highlighted item
        if (is_highlighted) {
            Ili9488FillBlock highlight_bg = {
                .color = menu->highlight_color,
                .ram_ptr = {
                    .start_x = item_y - 2,
                    .start_y = MENU_MARGIN_LEFT - 2,
                    .end_x = item_y + MENU_ITEM_HEIGHT - 8,
                    .end_y = screen->Screen.ScreenWidth - MENU_MARGIN_LEFT + 2
                }
            };
            ili9488_fill_color(*screen, highlight_bg);
        }
        
        // Draw item text
        Ili9488Print item_print = {
            .text = (char*)menu->items[i].text,
            .fg = is_highlighted ? BLACK : menu->text_color,
            .bg = is_highlighted ? menu->highlight_color : menu->bg_color,
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
// Touch Processing
// -----------------------------------------------------------------------------

uint8_t touch_menu_process_touch(TouchMenu* menu, Touch touch) {
    // Build touch box array on the fly
    TouchBox boxes[TOUCH_MENU_MAX_BOXES];
    uint8_t box_count = menu->item_count;
    
    if (box_count > TOUCH_MENU_MAX_BOXES) {
        box_count = TOUCH_MENU_MAX_BOXES;
    }
    
    // Calculate touch boxes for each menu item
    for (uint8_t i = 0; i < box_count; i++) {
        boxes[i] = touch_menu_calculate_box(i, 320);  // Assuming 320 width
    }
    
    // Check which box was touched
    uint8_t touched_key = xpt_is_touch_in_box_list(touch, boxes, box_count);
    
    return touched_key;
}

void touch_menu_highlight_item(Ili9488Defines* screen, TouchMenu* menu, 
                                 uint8_t item_index) {
    uint8_t old_highlight = menu->current_highlight;
    menu->current_highlight = item_index;
    
    // If nothing changed, return
    if (old_highlight == item_index) return;
    
    // Redraw old item (if valid)
    if (old_highlight < menu->item_count) {
        uint16_t old_y = touch_menu_get_item_y(old_highlight);
        
        // Clear highlight background
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
        
        // Redraw text (unhighlighted)
        Ili9488Print old_print = {
            .text = (char*)menu->items[old_highlight].text,
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
    }
    
    // Draw new highlight (if valid)
    if (item_index < menu->item_count) {
        uint16_t new_y = touch_menu_get_item_y(item_index);
        
        // Highlight background
        Ili9488FillBlock new_bg = {
            .color = menu->highlight_color,
            .ram_ptr = {
                .start_x = new_y - 2,
                .start_y = MENU_MARGIN_LEFT - 2,
                .end_x = new_y + MENU_ITEM_HEIGHT - 8,
                .end_y = screen->Screen.ScreenWidth - MENU_MARGIN_LEFT + 2
            }
        };
        ili9488_fill_color(*screen, new_bg);
        
        // Highlighted text
        Ili9488Print new_print = {
            .text = (char*)menu->items[item_index].text,
            .fg = BLACK,
            .bg = menu->highlight_color,
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
}

void touch_menu_execute_item(const TouchMenu* menu, uint8_t item_index) {
    if (item_index < menu->item_count && menu->items[item_index].action != NULL) {
        menu->items[item_index].action();
    }
}

// -----------------------------------------------------------------------------
// Touch State Management
// -----------------------------------------------------------------------------

TouchMenuState touch_menu_update_state(bool current_touch_state) {
    // Simple state machine for touch handling
    
    switch (g_touch_state) {
        case TOUCH_MENU_IDLE:
            if (current_touch_state && !g_previous_touch) {
                // Touch just detected
                g_touch_state = TOUCH_MENU_PRESSED;
            }
            break;
            
        case TOUCH_MENU_PRESSED:
            if (!current_touch_state && g_previous_touch) {
                // Touch released
                g_touch_state = TOUCH_MENU_RELEASED;
                g_last_touch_time = g_touch_timer;
            }
            break;
            
        case TOUCH_MENU_RELEASED:
            // Transition to debouncing after one cycle
            g_touch_state = TOUCH_MENU_DEBOUNCING;
            break;
            
        case TOUCH_MENU_DEBOUNCING:
            if (touch_menu_debounce_ready()) {
                g_touch_state = TOUCH_MENU_IDLE;
            }
            break;
    }
    
    g_previous_touch = current_touch_state;
    return g_touch_state;
}

void touch_menu_reset_debounce(void) {
    g_last_touch_time = g_touch_timer;
}

bool touch_menu_debounce_ready(void) {
    return ((g_touch_timer - g_last_touch_time) >= TOUCH_DEBOUNCE_MS);
}

/**
 * @brief Increment touch timer (call this every millisecond)
 * 
 * This should be called from a timer interrupt or in your main loop
 * with known timing
 */
void touch_menu_tick_ms(void) {
    g_touch_timer++;
}
