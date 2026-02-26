/**
 * @file menu_example.c
 * @brief Example usage of the ILI9488 menu system
 * 
 * This file demonstrates how to create and use menus efficiently.
 * All menu data is stored in ROM to minimize RAM usage.
 */

#include "ili9488_menu.h"
#include "ili9488.h"

// -----------------------------------------------------------------------------
// Menu Action Functions
// -----------------------------------------------------------------------------

void action_settings(void) {
    // Navigate to settings screen
    // Implementation depends on your app
}

void action_data_view(void) {
    // Show data screen
}

void action_diagnostics(void) {
    // Run diagnostics
}

void action_about(void) {
    // Show about screen
}

void action_exit(void) {
    // Exit menu or return to previous screen
}

// Settings submenu actions
void action_brightness(void) {
    // Adjust brightness
}

void action_contrast(void) {
    // Adjust contrast
}

void action_reset(void) {
    // Reset to defaults
}

void action_back(void) {
    // Return to main menu
}

// -----------------------------------------------------------------------------
// Menu Definitions (stored in ROM)
// -----------------------------------------------------------------------------

// Main Menu
const MenuItem main_menu_items[] = {
    {"Settings",     action_settings,    0},
    {"Data View",    action_data_view,   0},
    {"Diagnostics",  action_diagnostics, 0},
    {"About",        action_about,       0},
    {"Exit",         action_exit,        0},
};

// Settings Submenu
const MenuItem settings_menu_items[] = {
    {"Brightness",   action_brightness,  0},
    {"Contrast",     action_contrast,    0},
    {"Reset",        action_reset,       0},
    {"Back",         action_back,        0},
};

// -----------------------------------------------------------------------------
// Global Menu Objects (can be in RAM or static)
// -----------------------------------------------------------------------------

Menu g_main_menu;
Menu g_settings_menu;
Menu* g_current_menu = NULL;  // Pointer to current active menu

// -----------------------------------------------------------------------------
// Menu Initialization
// -----------------------------------------------------------------------------

void menus_initialize(void) {
    // Initialize main menu
    menu_init(&g_main_menu, 
              "Main Menu",
              main_menu_items,
              sizeof(main_menu_items) / sizeof(MenuItem));
    
    // Initialize settings menu
    menu_init(&g_settings_menu,
              "Settings",
              settings_menu_items,
              sizeof(settings_menu_items) / sizeof(MenuItem));
    
    // Set main menu as current
    g_current_menu = &g_main_menu;
}

// -----------------------------------------------------------------------------
// Menu Event Handling
// -----------------------------------------------------------------------------

void menu_handle_button_up(Ili9488Defines* screen) {
    if (g_current_menu == NULL) return;
    
    uint8_t old_index = g_current_menu->selected_index;
    if (menu_move_up(g_current_menu)) {
        // Selection changed - update display efficiently
        menu_update_selection(screen, g_current_menu, old_index);
    }
}

void menu_handle_button_down(Ili9488Defines* screen) {
    if (g_current_menu == NULL) return;
    
    uint8_t old_index = g_current_menu->selected_index;
    if (menu_move_down(g_current_menu)) {
        // Selection changed - update display efficiently
        menu_update_selection(screen, g_current_menu, old_index);
    }
}

void menu_handle_button_select(void) {
    if (g_current_menu == NULL) return;
    
    // Execute the selected action
    menu_execute_selected(g_current_menu);
}

// -----------------------------------------------------------------------------
// Example Main Loop Integration
// -----------------------------------------------------------------------------

void example_menu_main_loop(Ili9488Defines* screen) {
    // Initialize menus
    menus_initialize();
    
    // Render initial menu
    menu_render(screen, g_current_menu);
    
    while (1) {
        // Poll buttons (your button reading code here)
        // if (button_up_pressed()) {
        //     menu_handle_button_up(screen);
        // }
        // if (button_down_pressed()) {
        //     menu_handle_button_down(screen);
        // }
        // if (button_select_pressed()) {
        //     menu_handle_button_select();
        // }
        
        // Other application logic
        __delay_ms(50);  // Debounce delay
    }
}

// -----------------------------------------------------------------------------
// Advanced: Dynamic Value Display in Menus
// -----------------------------------------------------------------------------

/**
 * Example: Menu item that displays a sensor value
 * This shows how to create dynamic menu text
 */

static char temp_display_buffer[20];  // Static buffer for dynamic text

void update_temperature_display(int16_t temperature) {
    // Use our optimized int-to-string function instead of snprintf
    // (assuming you've included ili9488_utils.h)
    char temp_str[8];
    uint8_t len = int32_to_str(temperature, temp_str, sizeof(temp_str));
    
    // Build display string
    strcpy(temp_display_buffer, "Temp: ");
    strcat(temp_display_buffer, temp_str);
    strcat(temp_display_buffer, "C");
}

// Menu item that uses dynamic text
MenuItem dynamic_menu_example[] = {
    {temp_display_buffer, action_data_view, 0},  // Points to RAM buffer
    {"Back", action_back, 0},
};

// -----------------------------------------------------------------------------
// Memory Usage Comparison
// -----------------------------------------------------------------------------

/*
 * MEMORY USAGE ANALYSIS:
 * 
 * Traditional approach (each menu item has its own screen coordinates):
 * - MenuItem: 10 bytes (text ptr, x, y, action ptr, flags)
 * - 5 items × 10 bytes = 50 bytes per menu
 * - 3 menus = 150 bytes
 * 
 * This approach (data-driven, calculated coordinates):
 * - MenuItem: 6 bytes (text ptr, action ptr, flags)
 * - 5 items × 6 bytes = 30 bytes per menu
 * - 3 menus = 90 bytes
 * - Menu struct: 16 bytes × 3 = 48 bytes
 * - Total: 138 bytes (saves 12 bytes, ~8%)
 * 
 * But the REAL savings come from:
 * - Single generic rendering function instead of menu-specific code
 * - ~500-800 bytes saved in code size
 * - Easier to maintain and extend
 */
