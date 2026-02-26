/**
 * @file application_menus.h
 * @brief Application-specific menu definitions
 * 
 * Main Menu Structure:
 *   - Whitelist
 *   - Send Payload
 *   - Exit
 * 
 * Send Payload Submenu:
 *   - Payload 1
 *   - Payload 2
 *   - Payload 3
 *   - Payload 4
 *   - Back to Main Menu
 */

#ifndef APPLICATION_MENUS_H
#define APPLICATION_MENUS_H

#include "ili9488_menu.h"
#include "ili9488.h"

// -----------------------------------------------------------------------------
// Menu State Management
// -----------------------------------------------------------------------------

typedef enum {
    MENU_STATE_MAIN,
    MENU_STATE_WHITELIST,
    MENU_STATE_SEND_PAYLOAD,
    MENU_STATE_EXITING
} MenuState;

// -----------------------------------------------------------------------------
// Global Menu Variables
// -----------------------------------------------------------------------------

extern Menu g_main_menu;
extern Menu g_send_payload_menu;
extern MenuState g_current_menu_state;
extern Ili9488Defines* g_screen;  // Pointer to screen structure

// -----------------------------------------------------------------------------
// Menu Initialization
// -----------------------------------------------------------------------------

/**
 * @brief Initialize all application menus
 * 
 * @param screen Pointer to the display structure
 */
void app_menus_init(Ili9488Defines* screen);

/**
 * @brief Render the currently active menu
 */
void app_menu_render_current(void);

// -----------------------------------------------------------------------------
// Menu Navigation Functions
// -----------------------------------------------------------------------------

/**
 * @brief Handle UP button press
 */
void app_menu_button_up(void);

/**
 * @brief Handle DOWN button press
 */
void app_menu_button_down(void);

/**
 * @brief Handle SELECT/ENTER button press
 */
void app_menu_button_select(void);

/**
 * @brief Handle BACK button press
 */
void app_menu_button_back(void);

// -----------------------------------------------------------------------------
// Menu Action Functions (implement these in your application)
// -----------------------------------------------------------------------------

/**
 * @brief Navigate to whitelist screen
 */
void action_open_whitelist(void);

/**
 * @brief Navigate to send payload menu
 */
void action_open_send_payload(void);

/**
 * @brief Exit the application or return to idle screen
 */
void action_exit_application(void);

/**
 * @brief Send Payload 1
 */
void action_send_payload_1(void);

/**
 * @brief Send Payload 2
 */
void action_send_payload_2(void);

/**
 * @brief Send Payload 3
 */
void action_send_payload_3(void);

/**
 * @brief Send Payload 4
 */
void action_send_payload_4(void);

/**
 * @brief Return to main menu from send payload menu
 */
void action_back_to_main(void);

#endif // APPLICATION_MENUS_H
