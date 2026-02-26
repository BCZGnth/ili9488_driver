# ILI9488 Menu System - Complete Implementation Guide

## Overview

This menu system provides a complete, space-efficient implementation for your PIC18F47K42 + ILI9488 display application with:

- **Main Menu**: Whitelist, Send Payload, Exit
- **Send Payload Submenu**: 4 different payload options + Back
- **Whitelist Screen**: Placeholder for whitelist functionality

## Files Included

### Core Menu System
1. `ili9488_menu.h` - Generic menu system interface
2. `ili9488_menu.c` - Generic menu rendering and navigation

### Your Application-Specific Menus
3. `application_menus.h` - Your menu definitions and actions
4. `application_menus.c` - Implementation of your menus
5. `main_menu_integration.c` - Complete integration example with button handling

### Optimization Helpers
6. `ili9488_utils.h` - Optimized utility functions (snprintf replacement, etc.)
7. `ili9488_commands_optimized.h` - Optimized command definitions

## Menu Structure

```
MAIN MENU
├── Whitelist         → Whitelist Screen
├── Send Payload      → Send Payload Menu
└── Exit              → Exit Application

SEND PAYLOAD MENU
├── Payload 1         → Sends Payload 1
├── Payload 2         → Sends Payload 2
├── Payload 3         → Sends Payload 3
├── Payload 4         → Sends Payload 4
└── Back              → Return to Main Menu
```

## Quick Start Integration

### Step 1: Add Files to Your Project

Copy these files to your MPLAB X project:
- `ili9488_menu.h/c`
- `application_menus.h/c`
- `main_menu_integration.c`

### Step 2: Configure Button Pins

Edit `main_menu_integration.c` and implement the button reading functions for your hardware:

```c
uint8_t read_button_up_hw(void) {
    return !PORTBbits.RB0;  // Example: Active-low button on RB0
}

uint8_t read_button_down_hw(void) {
    return !PORTBbits.RB1;  // Example: Active-low button on RB1
}

uint8_t read_button_select_hw(void) {
    return !PORTBbits.RB2;  // Example: Active-low button on RB2
}

uint8_t read_button_back_hw(void) {
    return !PORTBbits.RB3;  // Example: Active-low button on RB3
}
```

### Step 3: Implement Your Payload Functions

In `application_menus.c`, add your actual payload transmission code:

```c
void action_send_payload_1(void) {
    // Show status on screen (already implemented)
    ili9488_cls(*g_screen);
    // ... display code ...
    
    // YOUR CODE HERE: Send your actual payload
    uint8_t payload1[] = {0xAA, 0xBB, 0xCC, 0xDD};
    // send_uart_data(payload1, sizeof(payload1));
    // or
    // send_spi_data(payload1, sizeof(payload1));
    // or
    // send_i2c_data(payload1, sizeof(payload1));
    
    // Show completion (already implemented)
    // ...
}
```

### Step 4: Implement Whitelist Functionality

In `application_menus.c`, add your whitelist display and management:

```c
void action_open_whitelist(void) {
    // Basic screen is already implemented
    // Add your whitelist-specific code here
    
    // Example: Display list of whitelisted MAC addresses
    // for (int i = 0; i < whitelist_count; i++) {
    //     display_whitelist_entry(whitelist[i], i);
    // }
}
```

### Step 5: Build and Test

1. Build your project in MPLAB X
2. Program your PIC18F47K42
3. Test navigation:
   - UP/DOWN buttons navigate menu items
   - SELECT executes the current item
   - BACK returns to previous menu

## Customization Options

### Change Menu Colors

Edit `application_menus.c` in the `app_menus_init()` function:

```c
// Main menu colors
g_main_menu.bg_color = BLACK;
g_main_menu.text_color = WHITE;
g_main_menu.selected_color = CYAN;      // ← Change this
g_main_menu.title_color = YELLOW;       // ← Change this

// Send payload menu colors
g_send_payload_menu.bg_color = BLACK;
g_send_payload_menu.text_color = WHITE;
g_send_payload_menu.selected_color = GREEN;   // ← Different color
g_send_payload_menu.title_color = CYAN;
```

Available colors (3-bit):
- `BLACK`, `RED`, `GREEN`, `YELLOW`, `BLUE`, `MAGENTA`, `CYAN`, `WHITE`

### Add More Menu Items

To add items to the main menu, edit `application_menus.c`:

```c
const MenuItem main_menu_items[] = {
    {"Whitelist",     action_open_whitelist,      0},
    {"Send Payload",  action_open_send_payload,   0},
    {"Settings",      action_open_settings,       0},  // NEW ITEM
    {"Exit",          action_exit_application,    0},
};
```

Then implement the action function:

```c
void action_open_settings(void) {
    // Your settings screen code
}
```

### Add More Payloads

In `application_menus.c`, add to the payload menu:

```c
const MenuItem send_payload_items[] = {
    {"Payload 1",     action_send_payload_1,      0},
    {"Payload 2",     action_send_payload_2,      0},
    {"Payload 3",     action_send_payload_3,      0},
    {"Payload 4",     action_send_payload_4,      0},
    {"Payload 5",     action_send_payload_5,      0},  // NEW
    {"Payload 6",     action_send_payload_6,      0},  // NEW
    {"Back",          action_back_to_main,        0},
};
```

### Change Menu Layout

Edit `ili9488_menu.h` to adjust spacing:

```c
#define MENU_ITEM_HEIGHT        30   // Pixels per item (make smaller for more items)
#define MENU_ITEM_SPACING       5    // Space between items
#define MENU_MARGIN_LEFT        10   // Left margin
#define MENU_MARGIN_TOP         20   // Top margin
```

## Memory Usage

### RAM Usage (approximate):
- Menu structures: ~150 bytes
- Button state: ~20 bytes
- Active menu pointer: 2 bytes
- **Total: ~180 bytes**

### Flash Usage (approximate):
- Core menu code: ~2KB
- Application menus: ~3KB
- **Total: ~5KB**

### ROM Usage (menu text):
- All menu text strings stored in program memory (ROM)
- Approximately 100-200 bytes

## Button Debouncing

The system includes built-in debouncing:
- Default debounce time: 50ms
- Configurable in `main_menu_integration.c`:

```c
#define DEBOUNCE_TIME_MS 50     // Change this value
```

## Troubleshooting

### Buttons Not Responding
1. Check button reading functions return correct values
2. Verify debounce time isn't too long
3. Check button polling frequency (should be ~10ms)

### Display Issues
1. Verify screen initialization completed successfully
2. Check SPI buffer size (should be at least 256 bytes)
3. Ensure colors are defined correctly (3-bit values 0-7)

### Menu Not Displaying
1. Verify `app_menus_init()` is called with valid screen pointer
2. Check that `app_menu_render_current()` is called after init
3. Verify font data is accessible (check font offset initialization)

### Text Positioning Wrong
Remember: ILI9488 coordinates are swapped due to MADCTL settings:
- `start_x` is the VERTICAL position (row)
- `start_y` is the HORIZONTAL position (column)
- This is handled automatically by the driver

## Example Payload Implementation

Here's a complete example of implementing a UART payload sender:

```c
void action_send_payload_1(void) {
    // Show status screen
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
    
    // Define your payload
    uint8_t payload[] = {
        0xAA, 0xBB, 0xCC, 0xDD,  // Header
        0x01, 0x02, 0x03, 0x04,  // Data
        0xFF                      // Checksum
    };
    
    // Send via UART (example)
    for (uint8_t i = 0; i < sizeof(payload); i++) {
        UART1_Write(payload[i]);
        __delay_ms(10);  // Small delay between bytes
        
        // Update progress bar
        if (i % 2 == 0) {
            ili9488_loading_bar(*g_screen);
        }
    }
    
    // Show completion
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
```

## Advanced: Dynamic Menu Content

If you need to display changing values (sensor data, status, etc.), you can use RAM buffers for menu text:

```c
// Global buffer for dynamic text
static char temp_menu_text[20];

void update_temperature_menu_item(int16_t temp) {
    // Use your custom int-to-string function (no snprintf!)
    char temp_str[8];
    uint8_t len = int32_to_str(temp, temp_str, sizeof(temp_str));
    
    strcpy(temp_menu_text, "Temp: ");
    strcat(temp_menu_text, temp_str);
    strcat(temp_menu_text, "C");
}

// Menu item points to RAM buffer
MenuItem dynamic_menu[] = {
    {temp_menu_text, action_view_temp, 0},  // Uses RAM buffer
    {"Back", action_back, 0},               // Uses ROM string
};
```

## Next Steps

1. Implement your button reading hardware functions
2. Add your payload transmission code
3. Implement whitelist display and management
4. Customize colors and layout to your preference
5. Add any additional menu screens you need

## Support

If you need help:
1. Check the troubleshooting section above
2. Review the example code in `main_menu_integration.c`
3. Verify your hardware connections
4. Check that all required files are included in your project
