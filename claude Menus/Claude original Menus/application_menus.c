/**
 * @file application_menus.c
 * @brief Implementation of application-specific menus
 */

#include "application_menus.h"
#include "ili9488_shapes.h"
#include <string.h>

// -----------------------------------------------------------------------------
// Menu State Variables
// -----------------------------------------------------------------------------

MenuState g_current_menu_state = MENU_STATE_MAIN;
Ili9488Defines* g_screen = NULL;
Menu* g_active_menu = NULL;

// -----------------------------------------------------------------------------
// Main Menu Definition (stored in ROM)
// -----------------------------------------------------------------------------

const MenuItem main_menu_items[] = {
    {"Whitelist",     action_open_whitelist,      0},
    {"Send Payload",  action_open_send_payload,   0},
    {"Exit",          action_exit_application,    0},
};

Menu g_main_menu;

// -----------------------------------------------------------------------------
// Send Payload Submenu Definition (stored in ROM)
// -----------------------------------------------------------------------------

const MenuItem send_payload_items[] = {
    {"Payload 1",     action_send_payload_1,      0},
    {"Payload 2",     action_send_payload_2,      0},
    {"Payload 3",     action_send_payload_3,      0},
    {"Payload 4",     action_send_payload_4,      0},
    {"Back",          action_back_to_main,        0},
};

Menu g_send_payload_menu;

// -----------------------------------------------------------------------------
// Menu Initialization
// -----------------------------------------------------------------------------

void app_menus_init(Ili9488Defines* screen) {
    // Store screen pointer
    g_screen = screen;
    
    // Initialize main menu
    menu_init(&g_main_menu, 
              "MAIN MENU",
              main_menu_items,
              sizeof(main_menu_items) / sizeof(MenuItem));
    
    // Customize main menu colors
    g_main_menu.bg_color = BLACK;
    g_main_menu.text_color = WHITE;
    g_main_menu.selected_color = CYAN;
    g_main_menu.title_color = YELLOW;
    
    // Initialize send payload menu
    menu_init(&g_send_payload_menu,
              "SEND PAYLOAD",
              send_payload_items,
              sizeof(send_payload_items) / sizeof(MenuItem));
    
    // Customize send payload menu colors
    g_send_payload_menu.bg_color = BLACK;
    g_send_payload_menu.text_color = WHITE;
    g_send_payload_menu.selected_color = GREEN;
    g_send_payload_menu.title_color = CYAN;
    
    // Set main menu as active
    g_active_menu = &g_main_menu;
    g_current_menu_state = MENU_STATE_MAIN;
}

// -----------------------------------------------------------------------------
// Menu Rendering
// -----------------------------------------------------------------------------

void app_menu_render_current(void) {
    if (g_active_menu != NULL && g_screen != NULL) {
        menu_render(g_screen, g_active_menu);
    }
}

// -----------------------------------------------------------------------------
// Menu Navigation Functions
// -----------------------------------------------------------------------------

void app_menu_button_up(void) {
    if (g_active_menu == NULL) return;
    
    uint8_t old_index = g_active_menu->selected_index;
    if (menu_move_up(g_active_menu)) {
        menu_update_selection(g_screen, g_active_menu, old_index);
    }
}

void app_menu_button_down(void) {
    if (g_active_menu == NULL) return;
    
    uint8_t old_index = g_active_menu->selected_index;
    if (menu_move_down(g_active_menu)) {
        menu_update_selection(g_screen, g_active_menu, old_index);
    }
}

void app_menu_button_select(void) {
    if (g_active_menu == NULL) return;
    
    // Execute the selected action
    menu_execute_selected(g_active_menu);
}

void app_menu_button_back(void) {
    // Handle back button based on current state
    switch (g_current_menu_state) {
        case MENU_STATE_MAIN:
            // Already at main menu, maybe exit or do nothing
            action_exit_application();
            break;
            
        case MENU_STATE_SEND_PAYLOAD:
            // Go back to main menu
            action_back_to_main();
            break;
            
        case MENU_STATE_WHITELIST:
            // Go back to main menu
            action_back_to_main();
            break;
            
        default:
            break;
    }
}

// -----------------------------------------------------------------------------
// Menu Action Implementations
// -----------------------------------------------------------------------------

void action_open_whitelist(void) {
    // Navigate to whitelist screen
    g_current_menu_state = MENU_STATE_WHITELIST;
    
    // Clear screen and show whitelist interface
    ili9488_cls(*g_screen);
    
    // Draw title
    Ili9488Print title = {
        .text = "WHITELIST",
        .fg = CYAN,
        .bg = BLACK,
        .font = g_screen->Screen.offset_2x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 20,
            .end_x = 30,
            .end_y = 300
        }
    };
    ili9488_print(*g_screen, title);
    
    // Draw separator
    Ili9488HVLine separator = {
        .xstart = 35,
        .ystart = 20,
        .length = 280,
        .weight = 2,
        .color = CYAN
    };
    ili9488_draw_hline(*g_screen, separator);
    
    // Display whitelist content
    Ili9488Print content = {
        .text = "Whitelist Screen",
        .fg = WHITE,
        .bg = BLACK,
        .font = g_screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 50,
            .start_y = 20,
            .end_x = 70,
            .end_y = 300
        }
    };
    ili9488_print(*g_screen, content);
    
    Ili9488Print instruction = {
        .text = "Press BACK to return",
        .fg = YELLOW,
        .bg = BLACK,
        .font = g_screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 450,
            .start_y = 20,
            .end_x = 470,
            .end_y = 300
        }
    };
    ili9488_print(*g_screen, instruction);
    
    // TODO: Add your actual whitelist display logic here
    // For example:
    // - Show list of whitelisted devices
    // - Allow adding/removing devices
    // - etc.
}

void action_open_send_payload(void) {
    // Switch to send payload menu
    g_current_menu_state = MENU_STATE_SEND_PAYLOAD;
    g_active_menu = &g_send_payload_menu;
    g_send_payload_menu.selected_index = 0;  // Reset selection
    
    // Render the send payload menu
    menu_render(g_screen, &g_send_payload_menu);
}

void action_exit_application(void) {
    // Exit the application
    g_current_menu_state = MENU_STATE_EXITING;
    
    // Clear screen
    ili9488_cls(*g_screen);
    
    // Show exit message
    Ili9488Print exit_msg = {
        .text = "EXITING...",
        .fg = RED,
        .bg = BLACK,
        .font = g_screen->Screen.offset_2x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 220,
            .start_y = 100,
            .end_x = 260,
            .end_y = 220
        }
    };
    ili9488_print(*g_screen, exit_msg);
    
    // TODO: Add your exit logic here
    // - Return to idle screen
    // - Enter low power mode
    // - etc.
}

void action_send_payload_1(void) {
    // Clear screen and show sending status
    ili9488_cls(*g_screen);
    
    // Title
    Ili9488Print title = {
        .text = "SENDING PAYLOAD 1",
        .fg = GREEN,
        .bg = BLACK,
        .font = g_screen->Screen.offset_2x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 20,
            .end_x = 30,
            .end_y = 300
        }
    };
    ili9488_print(*g_screen, title);
    
    // Status message
    Ili9488Print status = {
        .text = "Transmitting...",
        .fg = YELLOW,
        .bg = BLACK,
        .font = g_screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 100,
            .start_y = 50,
            .end_x = 120,
            .end_y = 250
        }
    };
    ili9488_print(*g_screen, status);
    
    // TODO: Add your actual payload transmission code here
    // Example:
    // uint8_t payload1[] = {0xAA, 0xBB, 0xCC, 0xDD};
    // send_data_over_interface(payload1, sizeof(payload1));
    
    // Show progress bar
    ili9488_loading_bar(*g_screen);
    __delay_ms(500);
    
    // Show completion message
    Ili9488Print complete = {
        .text = "COMPLETE!",
        .fg = GREEN,
        .bg = BLACK,
        .font = g_screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 150,
            .start_y = 100,
            .end_x = 170,
            .end_y = 200
        }
    };
    ili9488_print(*g_screen, complete);
    
    // Wait before returning to menu
    __delay_ms(1500);
    
    // Return to send payload menu
    menu_render(g_screen, &g_send_payload_menu);
}

void action_send_payload_2(void) {
    // Similar to payload 1, but with different data
    ili9488_cls(*g_screen);
    
    Ili9488Print title = {
        .text = "SENDING PAYLOAD 2",
        .fg = GREEN,
        .bg = BLACK,
        .font = g_screen->Screen.offset_2x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 20,
            .end_x = 30,
            .end_y = 300
        }
    };
    ili9488_print(*g_screen, title);
    
    Ili9488Print status = {
        .text = "Transmitting...",
        .fg = YELLOW,
        .bg = BLACK,
        .font = g_screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 100,
            .start_y = 50,
            .end_x = 120,
            .end_y = 250
        }
    };
    ili9488_print(*g_screen, status);
    
    // TODO: Send your payload 2 data
    
    ili9488_loading_bar(*g_screen);
    __delay_ms(500);
    
    Ili9488Print complete = {
        .text = "COMPLETE!",
        .fg = GREEN,
        .bg = BLACK,
        .font = g_screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 150,
            .start_y = 100,
            .end_x = 170,
            .end_y = 200
        }
    };
    ili9488_print(*g_screen, complete);
    
    __delay_ms(1500);
    menu_render(g_screen, &g_send_payload_menu);
}

void action_send_payload_3(void) {
    // Similar implementation for payload 3
    ili9488_cls(*g_screen);
    
    Ili9488Print title = {
        .text = "SENDING PAYLOAD 3",
        .fg = GREEN,
        .bg = BLACK,
        .font = g_screen->Screen.offset_2x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 20,
            .end_x = 30,
            .end_y = 300
        }
    };
    ili9488_print(*g_screen, title);
    
    Ili9488Print status = {
        .text = "Transmitting...",
        .fg = YELLOW,
        .bg = BLACK,
        .font = g_screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 100,
            .start_y = 50,
            .end_x = 120,
            .end_y = 250
        }
    };
    ili9488_print(*g_screen, status);
    
    // TODO: Send your payload 3 data
    
    ili9488_loading_bar(*g_screen);
    __delay_ms(500);
    
    Ili9488Print complete = {
        .text = "COMPLETE!",
        .fg = GREEN,
        .bg = BLACK,
        .font = g_screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 150,
            .start_y = 100,
            .end_x = 170,
            .end_y = 200
        }
    };
    ili9488_print(*g_screen, complete);
    
    __delay_ms(1500);
    menu_render(g_screen, &g_send_payload_menu);
}

void action_send_payload_4(void) {
    // Similar implementation for payload 4
    ili9488_cls(*g_screen);
    
    Ili9488Print title = {
        .text = "SENDING PAYLOAD 4",
        .fg = GREEN,
        .bg = BLACK,
        .font = g_screen->Screen.offset_2x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 20,
            .end_x = 30,
            .end_y = 300
        }
    };
    ili9488_print(*g_screen, title);
    
    Ili9488Print status = {
        .text = "Transmitting...",
        .fg = YELLOW,
        .bg = BLACK,
        .font = g_screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 100,
            .start_y = 50,
            .end_x = 120,
            .end_y = 250
        }
    };
    ili9488_print(*g_screen, status);
    
    // TODO: Send your payload 4 data
    
    ili9488_loading_bar(*g_screen);
    __delay_ms(500);
    
    Ili9488Print complete = {
        .text = "COMPLETE!",
        .fg = GREEN,
        .bg = BLACK,
        .font = g_screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 150,
            .start_y = 100,
            .end_x = 170,
            .end_y = 200
        }
    };
    ili9488_print(*g_screen, complete);
    
    __delay_ms(1500);
    menu_render(g_screen, &g_send_payload_menu);
}

void action_back_to_main(void) {
    // Return to main menu
    g_current_menu_state = MENU_STATE_MAIN;
    g_active_menu = &g_main_menu;
    g_main_menu.selected_index = 0;  // Reset selection to top
    
    // Render main menu
    menu_render(g_screen, &g_main_menu);
}
