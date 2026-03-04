/**
 * @file touch_application_menus.c
 * @brief Touch-based menu implementation for Whitelist, Send Payload, Exit
 * 
 * This file implements the complete touch menu system with a main loop
 * that runs until the user exits.
 */

#include "touch_menu_system.h"
#include "application_menus.h"
#include "xpt2046.h"
#include "ili9488_shapes.h"
#include <string.h>

// -----------------------------------------------------------------------------
// Global State
// -----------------------------------------------------------------------------

static Ili9488Defines* g_screen = NULL;
static TouchMenu g_main_menu;
static TouchMenu g_send_payload_menu;
static TouchMenu* g_active_menu = NULL;
static MenuState g_menu_state = MENU_STATE_MAIN;
static volatile bool g_menu_running = true;  // Set to false to exit menu system
static volatile bool g_touch_flag = false;   // Set by interrupt or polling

// -----------------------------------------------------------------------------
// Forward Declarations
// -----------------------------------------------------------------------------

void touch_main_menu_loop(void);
void touch_send_payload_loop(void);
void touch_action_open_whitelist(void);
void touch_action_open_send_payload(void);
void touch_action_exit_application(void);
void touch_action_send_payload_1(void);
void touch_action_send_payload_2(void);
void touch_action_send_payload_3(void);
void touch_action_send_payload_4(void);
void touch_action_back_to_main(void);

// -----------------------------------------------------------------------------
// Menu Definitions (in ROM)
// -----------------------------------------------------------------------------

const TouchMenuItem main_menu_touch_items[] = {
    {
        .text = "Whitelist",
        .action = touch_action_open_whitelist,
        .flags = 0
    },
    {
        .text = "Send Payload",
        .action = touch_action_open_send_payload,
        .flags = 0
    },
    {
        .text = "Exit",
        .action = touch_action_exit_application,
        .flags = 0
    },
};

const TouchMenuItem send_payload_touch_items[] = {
    {
        .text = "Payload 1",
        .action = touch_action_send_payload_1,
        .flags = 0
    },
    {
        .text = "Payload 2",
        .action = touch_action_send_payload_2,
        .flags = 0
    },
    {
        .text = "Payload 3",
        .action = touch_action_send_payload_3,
        .flags = 0
    },
    {
        .text = "Payload 4",
        .action = touch_action_send_payload_4,
        .flags = 0
    },
    {
        .text = "Back",
        .action = touch_action_back_to_main,
        .flags = 0
    },
};

// -----------------------------------------------------------------------------
// Menu Initialization
// -----------------------------------------------------------------------------

void touch_app_menus_init(Ili9488Defines* screen) {
    g_screen = screen;
    
    // Initialize main menu
    touch_menu_init(&g_main_menu,
                    "MAIN MENU",
                    main_menu_touch_items,
                    sizeof(main_menu_touch_items) / sizeof(TouchMenuItem));
    
    g_main_menu.bg_color = BLACK;
    g_main_menu.text_color = WHITE;
    g_main_menu.highlight_color = CYAN;
    g_main_menu.title_color = YELLOW;
    
    // Initialize send payload menu
    touch_menu_init(&g_send_payload_menu,
                    "SEND PAYLOAD",
                    send_payload_touch_items,
                    sizeof(send_payload_touch_items) / sizeof(TouchMenuItem));
    
    g_send_payload_menu.bg_color = BLACK;
    g_send_payload_menu.text_color = WHITE;
    g_send_payload_menu.highlight_color = GREEN;
    g_send_payload_menu.title_color = CYAN;
    
    // Set active menu
    g_active_menu = &g_main_menu;
    g_menu_state = MENU_STATE_MAIN;
    g_menu_running = true;
}

// -----------------------------------------------------------------------------
// Main Menu Loop - Runs until exit flag is set
// -----------------------------------------------------------------------------

/**
 * @brief Main menu loop - runs continuously until user exits
 * 
 * This function blocks and handles all menu interactions via touch.
 * It only returns when the user selects "Exit" from the main menu.
 */
void touch_main_menu_loop(void) {
    // Render the main menu
    touch_menu_render(g_screen, &g_main_menu);
    
    Touch current_touch;
    uint8_t last_highlighted = 255;
    
    // Main loop - runs until g_menu_running is set to false
    while (g_menu_running) {
        
        // Check if touch is available
        // Option 1: Flag-based (if you set g_touch_flag from interrupt)
        // Option 2: Direct polling (shown below)
        
        bool touch_detected = xpt_touch_getxy(&current_touch);
        
        if (touch_detected) {
            // Process the touch
            uint8_t touched_item = touch_menu_process_touch(&g_main_menu, current_touch);
            
            if (touched_item != 255 && touched_item < g_main_menu.item_count) {
                // Valid touch detected - highlight the item
                if (touched_item != last_highlighted) {
                    touch_menu_highlight_item(g_screen, &g_main_menu, touched_item);
                    last_highlighted = touched_item;
                }
            }
            
            // Wait for touch release
            while (TOUCH_IRQ_GetValue() == 0) {
                // Waiting for release
                __delay_ms(10);
            }
            
            // Execute action if valid
            if (touched_item != 255 && touched_item < g_main_menu.item_count) {
                touch_menu_execute_item(&g_main_menu, touched_item);
                last_highlighted = 255;  // Reset highlight
            }
            
            // Debounce delay
            __delay_ms(TOUCH_DEBOUNCE_MS);
        }
        
        // Small delay to prevent CPU thrashing
        __delay_ms(10);
        
        // Check if we should exit based on menu state
        if (g_menu_state == MENU_STATE_EXITING) {
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// Send Payload Menu Loop
// -----------------------------------------------------------------------------

void touch_send_payload_loop(void) {
    // Render the send payload menu
    touch_menu_render(g_screen, &g_send_payload_menu);
    
    Touch current_touch;
    uint8_t last_highlighted = 255;
    
    // Loop until user goes back
    while (g_menu_state == MENU_STATE_SEND_PAYLOAD) {
        
        bool touch_detected = xpt_touch_getxy(&current_touch);
        
        if (touch_detected) {
            uint8_t touched_item = touch_menu_process_touch(&g_send_payload_menu, current_touch);
            
            if (touched_item != 255 && touched_item < g_send_payload_menu.item_count) {
                // Highlight
                if (touched_item != last_highlighted) {
                    touch_menu_highlight_item(g_screen, &g_send_payload_menu, touched_item);
                    last_highlighted = touched_item;
                }
            }
            
            // Wait for release
            while (TOUCH_IRQ_GetValue() == 0) {
                __delay_ms(10);
            }
            
            // Execute
            if (touched_item != 255 && touched_item < g_send_payload_menu.item_count) {
                touch_menu_execute_item(&g_send_payload_menu, touched_item);
                last_highlighted = 255;
            }
            
            __delay_ms(TOUCH_DEBOUNCE_MS);
        }
        
        __delay_ms(10);
    }
    
    // When we exit this loop, return to main menu
    g_menu_state = MENU_STATE_MAIN;
    touch_main_menu_loop();
}

// -----------------------------------------------------------------------------
// Menu Action Implementations
// -----------------------------------------------------------------------------

void touch_action_open_whitelist(void) {
    g_menu_state = MENU_STATE_WHITELIST;
    
    // Clear screen and show whitelist
    ili9488_cls(*g_screen);
    
    // Title
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
    
    // Separator
    Ili9488HVLine separator = {
        .xstart = 35,
        .ystart = 20,
        .length = 280,
        .weight = 2,
        .color = CYAN
    };
    ili9488_draw_hline(*g_screen, separator);
    
    // Content
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
    
    // Instruction
    Ili9488Print instruction = {
        .text = "Touch to return",
        .fg = YELLOW,
        .bg = BLACK,
        .font = g_screen->Screen.offset_1x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 450,
            .start_y = 20,
            .end_x = 470,
            .end_y = 200
        }
    };
    ili9488_print(*g_screen, instruction);
    
    // TODO: Add your whitelist display logic here
    
    // Wait for touch to return
    Touch t;
    while (!xpt_touch_getxy(&t)) {
        __delay_ms(10);
    }
    
    // Wait for release
    while (TOUCH_IRQ_GetValue() == 0) {
        __delay_ms(10);
    }
    
    __delay_ms(TOUCH_DEBOUNCE_MS);
    
    // Return to main menu
    g_menu_state = MENU_STATE_MAIN;
    touch_menu_render(g_screen, &g_main_menu);
}

void touch_action_open_send_payload(void) {
    g_menu_state = MENU_STATE_SEND_PAYLOAD;
    g_active_menu = &g_send_payload_menu;
    
    // Enter send payload menu loop
    touch_send_payload_loop();
}

void touch_action_exit_application(void) {
    g_menu_state = MENU_STATE_EXITING;
    g_menu_running = false;  // This will cause touch_main_menu_loop to exit
    
    // Show exit message
    ili9488_cls(*g_screen);
    
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
    
    __delay_ms(1000);
}

void touch_action_send_payload_1(void) {
    // Show sending screen
    ili9488_cls(*g_screen);
    
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
    
    // TODO: Send your actual payload here
    // uint8_t payload1[] = {0xAA, 0xBB, 0xCC, 0xDD};
    // send_data(payload1, sizeof(payload1));
    
    // Show progress
    ili9488_loading_bar(*g_screen);
    __delay_ms(500);
    
    // Completion
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
    
    // Return to send payload menu
    touch_menu_render(g_screen, &g_send_payload_menu);
}

void touch_action_send_payload_2(void) {
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
    
    // TODO: Send payload 2
    
    __delay_ms(1000);
    touch_menu_render(g_screen, &g_send_payload_menu);
}

void touch_action_send_payload_3(void) {
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
    
    // TODO: Send payload 3
    
    __delay_ms(1000);
    touch_menu_render(g_screen, &g_send_payload_menu);
}

void touch_action_send_payload_4(void) {
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
    
    // TODO: Send payload 4
    
    __delay_ms(1000);
    touch_menu_render(g_screen, &g_send_payload_menu);
}

void touch_action_back_to_main(void) {
    // Exit send payload loop and return to main
    g_menu_state = MENU_STATE_MAIN;
    g_active_menu = &g_main_menu;
}

// -----------------------------------------------------------------------------
// Public Entry Point
// -----------------------------------------------------------------------------

/**
 * @brief Start the touch menu system
 * 
 * This function initializes the menu system and enters the main loop.
 * It will not return until the user selects "Exit" from the main menu.
 * 
 * @param screen Pointer to initialized display structure
 */
void touch_menu_run(Ili9488Defines* screen) {
    // Initialize menus
    touch_app_menus_init(screen);
    
    // Enter main menu loop (blocks until exit)
    touch_main_menu_loop();
    
    // When we get here, user has exited
    // You can add cleanup code here if needed
}
