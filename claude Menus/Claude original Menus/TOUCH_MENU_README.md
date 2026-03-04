# Touch Menu System for ILI9488 + XPT2046

## Overview

This is a complete touch-based menu system for the PIC18F47K42 with ILI9488 display and XPT2046 touchscreen controller. The system uses a blocking loop architecture where the menu runs continuously until the user exits.

## Features

✅ **Touch-based navigation** - No buttons required  
✅ **Visual feedback** - Items highlight on touch  
✅ **Blocking menu loop** - Runs until user exits  
✅ **Debounced touch** - Prevents multiple activations  
✅ **Space efficient** - Menu data in ROM, minimal RAM  
✅ **Easy to customize** - Colors, layout, items  
✅ **Complete examples** - Ready to integrate  

## Menu Structure

```
MAIN MENU (runs until exit)
├── Whitelist         → Whitelist Screen (touch to return)
├── Send Payload      → Send Payload Submenu
└── Exit              → Exits menu loop

SEND PAYLOAD MENU
├── Payload 1         → Sends Payload 1 (returns to submenu)
├── Payload 2         → Sends Payload 2 (returns to submenu)
├── Payload 3         → Sends Payload 3 (returns to submenu)
├── Payload 4         → Sends Payload 4 (returns to submenu)
└── Back              → Returns to Main Menu
```

## Files Included

### Core Touch Menu System
1. `touch_menu_system.h` - Touch menu interface
2. `touch_menu_system.c` - Touch menu implementation

### Your Application
3. `touch_application_menus.c` - Complete menu implementation with loops
4. `main_touch_menu.c` - Main entry point example

### Dependencies
- Your existing `xpt2046.h/c` - Touch controller driver
- Your existing `ili9488*.h/c` - Display driver

## Quick Start

### Step 1: Add Files to Project

Add these files to your MPLAB X project:
- `touch_menu_system.h`
- `touch_menu_system.c`
- `touch_application_menus.c`
- `main_touch_menu.c` (or integrate into your existing main)

### Step 2: Basic Integration

In your `main.c`:

```c
#include "touch_application_menus.h"
#include "ili9488.h"

void main(void) {
    // Initialize hardware
    application_init();
    
    // Initialize display
    uint8_t spi_buffer[256];
    ili9488_interface_t display_interface = {
        .lcd_rst_port = &PORTA,
        .lcd_rst_pin = 0
    };
    
    Ili9488Defines screen = ili9488_initialize(
        display_interface,
        spi_buffer,
        sizeof(spi_buffer)
    );
    
    // Run menu system (blocks until user exits)
    touch_menu_run(&screen);
    
    // Menu exited, do cleanup
    while(1) {
        // Idle or low power mode
    }
}
```

### Step 3: Implement Your Payload Functions

Edit `touch_application_menus.c` and add your transmission code:

```c
void touch_action_send_payload_1(void) {
    // Show status (already implemented)
    ili9488_cls(*g_screen);
    // ... display code ...
    
    // YOUR CODE: Send your actual payload
    uint8_t payload1[] = {0xAA, 0xBB, 0xCC, 0xDD};
    UART1_WriteBuffer(payload1, sizeof(payload1));
    
    // Show completion (already implemented)
    // ...
}
```

### Step 4: Add Whitelist Logic

Implement your whitelist display in `touch_action_open_whitelist()`:

```c
void touch_action_open_whitelist(void) {
    // Basic screen already implemented
    
    // ADD YOUR CODE HERE:
    // - Display whitelist entries
    // - Allow adding/removing entries
    // - etc.
    
    // Touch to return (already implemented)
}
```

## How It Works

### The Main Loop

The system uses a **blocking loop architecture**:

```c
void touch_main_menu_loop(void) {
    // Render menu
    touch_menu_render(g_screen, &g_main_menu);
    
    // Loop until user exits
    while (g_menu_running) {
        // Check for touch
        bool touch_detected = xpt_touch_getxy(&current_touch);
        
        if (touch_detected) {
            // Determine which item was touched
            uint8_t touched_item = touch_menu_process_touch(&g_main_menu, current_touch);
            
            // Highlight the item
            touch_menu_highlight_item(g_screen, &g_main_menu, touched_item);
            
            // Wait for touch release
            while (TOUCH_IRQ_GetValue() == 0) {
                __delay_ms(10);
            }
            
            // Execute the action
            touch_menu_execute_item(&g_main_menu, touched_item);
            
            // Debounce
            __delay_ms(TOUCH_DEBOUNCE_MS);
        }
        
        __delay_ms(10);  // Small delay to prevent CPU thrashing
    }
}
```

### Touch Detection Flow

1. **Touch occurs** → XPT2046 IRQ pin goes LOW
2. **Read coordinates** → `xpt_touch_getxy()` returns X,Y
3. **Calculate touch boxes** → Menu items have defined touch areas
4. **Check which box** → `xpt_is_touch_in_box_list()` finds match
5. **Highlight item** → Visual feedback to user
6. **Wait for release** → Poll IRQ until HIGH
7. **Execute action** → Call the menu item's function
8. **Debounce** → Wait 200ms before accepting next touch

### Touch Box Calculation

Touch boxes are automatically calculated based on menu item positions:

```c
TouchBox touch_menu_calculate_box(uint8_t item_index, uint16_t screen_width) {
    uint16_t item_y = touch_menu_get_item_y(item_index);
    
    TouchBox box = {
        .xs = item_y - 2,                           // Top edge
        .xe = item_y + MENU_ITEM_HEIGHT - 8,        // Bottom edge
        .ys = MENU_MARGIN_LEFT - 2,                 // Left edge
        .ye = screen_width - MENU_MARGIN_LEFT + 2,  // Right edge
        .key = item_index
    };
    
    return box;
}
```

## Customization

### Change Menu Colors

Edit `touch_app_menus_init()` in `touch_application_menus.c`:

```c
g_main_menu.bg_color = BLACK;
g_main_menu.text_color = WHITE;
g_main_menu.highlight_color = CYAN;    // ← Change highlight color
g_main_menu.title_color = YELLOW;      // ← Change title color
```

### Add Menu Items

Edit the menu definition arrays:

```c
const TouchMenuItem main_menu_touch_items[] = {
    {"Whitelist",     touch_action_open_whitelist,   0},
    {"Send Payload",  touch_action_open_send_payload, 0},
    {"Settings",      touch_action_open_settings,    0},  // NEW
    {"Exit",          touch_action_exit_application, 0},
};
```

Then implement the action:

```c
void touch_action_open_settings(void) {
    // Your settings screen code
}
```

### Adjust Touch Sensitivity

Edit `touch_menu_system.h`:

```c
#define TOUCH_DEBOUNCE_MS   200  // Increase to reduce sensitivity
```

### Change Menu Layout

Edit spacing in `ili9488_menu.h`:

```c
#define MENU_ITEM_HEIGHT    30   // Height of each item
#define MENU_ITEM_SPACING   5    // Space between items
#define MENU_MARGIN_LEFT    10   // Left margin
#define MENU_MARGIN_TOP     20   // Top margin
```

## Touch Calibration

Your XPT2046 driver already has calibration values:

```c
#define _X_INITIAL 136
#define _X_FINAL 1919
#define _Y_INITIAL 112
#define _Y_FINAL 1959
```

If your touches are not accurate:

1. Run the debug touch test (see below)
2. Touch the four corners of your screen
3. Note the raw X/Y values
4. Update these constants in `xpt2046.c`

## Debugging

### Touch Coordinate Test

Enable debug mode in `main_touch_menu.c`:

```c
#define DEBUG_TOUCH

void main(void) {
    // ... initialization ...
    
    // Run touch test instead of menu
    test_touch_display(&screen);
}
```

This will display touch coordinates and draw dots where you touch.

### Check Touch Box Boundaries

Add debug output in `touch_menu_process_touch()`:

```c
uint8_t touch_menu_process_touch(TouchMenu* menu, Touch touch) {
    // ... existing code ...
    
    // Debug: Print touch coordinates
    // printf("Touch at X=%u, Y=%u\n", touch.x, touch.y);
    
    // Debug: Print box boundaries
    for (uint8_t i = 0; i < box_count; i++) {
        // printf("Box %u: xs=%u xe=%u ys=%u ye=%u\n", 
               i, boxes[i].xs, boxes[i].xe, boxes[i].ys, boxes[i].ye);
    }
    
    // ... rest of function ...
}
```

## Memory Usage

### RAM Usage
- Touch menu structures: ~200 bytes
- Touch state: ~10 bytes
- Touch coordinate buffer: 4 bytes
- **Total: ~220 bytes**

### Flash Usage
- Core touch menu code: ~2.5KB
- Application menus: ~4KB
- **Total: ~6.5KB**

### ROM Usage (menu text)
- All menu strings in program memory
- Approximately 150-250 bytes

## Troubleshooting

### Touch Not Detected
1. Check `TOUCH_IRQ_GetValue()` returns correct state
2. Verify SPI communication with XPT2046
3. Check calibration values
4. Test with debug touch display

### Wrong Item Activated
1. Verify touch box calculations
2. Check coordinate scaling in `scale_touch_to_screen()`
3. Adjust `_REVERSE_X` and `_REVERSE_Y` if needed
4. Verify `MENU_ITEM_HEIGHT` matches visual layout

### Multiple Activations
1. Increase `TOUCH_DEBOUNCE_MS` (default 200ms)
2. Ensure release detection works (`TOUCH_IRQ_GetValue() == 0`)
3. Check for floating IRQ pin (add pull-up if needed)

### Screen Coordinates Wrong
Remember: Due to MADCTL settings:
- `start_x` = vertical position (row)
- `start_y` = horizontal position (column)

This is handled automatically by the driver.

## Advanced Features

### Adding Scrolling Menus

If you have more than 8 items, implement scrolling:

```c
// TODO: Add scroll indicator
// TODO: Detect swipe gestures
// TODO: Offset menu rendering
```

### Multi-level Menus

You can nest menus by creating additional menu structures:

```c
TouchMenu g_settings_menu;
TouchMenu g_network_settings_menu;
TouchMenu g_display_settings_menu;
```

### Dynamic Menu Content

Use RAM buffers for changing text:

```c
static char status_text[30];

void update_status_menu(void) {
    s// printf(status_text, "Battery: %d%%", get_battery_level());
    // Menu item points to status_text
}
```

## Integration with Other Features

### With Low Power Mode

```c
void main(void) {
    // Run menu
    touch_menu_run(&screen);
    
    // User exited, enter low power
    SLEEP();
    
    // Wake on touch interrupt
    // Restart menu
}
```

### With Periodic Tasks

```c
void touch_main_menu_loop(void) {
    // ... existing loop ...
    
    while (g_menu_running) {
        // Check touch
        // ...
        
        // Add periodic task
        if (task_timer_expired()) {
            update_sensor_data();
            refresh_display();
        }
        
        __delay_ms(10);
    }
}
```

## Example Payload Implementation

Complete example for UART payload:

```c
void touch_action_send_payload_1(void) {
    // Show status
    ili9488_cls(*g_screen);
    // ... display code (already implemented) ...
    
    // Define payload
    typedef struct {
        uint8_t header[2];
        uint8_t command;
        uint8_t data[4];
        uint8_t checksum;
    } Payload1;
    
    Payload1 payload = {
        .header = {0xAA, 0xBB},
        .command = 0x01,
        .data = {0x11, 0x22, 0x33, 0x44},
        .checksum = 0x00
    };
    
    // Calculate checksum
    uint8_t* bytes = (uint8_t*)&payload;
    for (uint8_t i = 0; i < sizeof(Payload1) - 1; i++) {
        payload.checksum ^= bytes[i];
    }
    
    // Send via UART
    UART1_WriteBuffer((uint8_t*)&payload, sizeof(Payload1));
    
    // Wait for completion
    while (!UART1_IsTxDone()) {
        ili9488_loading_bar(*g_screen);
        __delay_ms(100);
    }
    
    // Show completion
    // ... (already implemented) ...
}
```

## Tips and Best Practices

1. **Always debounce** - 200ms is a good starting point
2. **Wait for release** - Don't activate on touch down, wait for release
3. **Visual feedback** - Always highlight touched items
4. **Clear menus** - Don't overcrowd, max 6-7 items per screen
5. **Test thoroughly** - Touch is less precise than buttons
6. **Calibrate properly** - Spend time getting calibration right

## Next Steps

1. Integrate files into your project
2. Test touch detection with debug mode
3. Calibrate touch if needed
4. Implement your payload transmission code
5. Add whitelist functionality
6. Customize colors and layout
7. Test all menu paths thoroughly

## Support

If you encounter issues:
1. Enable debug mode and check touch coordinates
2. Verify SPI communication with both display and touch
3. Check all pin assignments
4. Review calibration values
5. Test with minimal code (single menu with one item)
