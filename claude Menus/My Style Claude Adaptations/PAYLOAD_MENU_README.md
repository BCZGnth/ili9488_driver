# Payload Menu System - Following Your Existing Pattern

## Overview

This menu system **follows your exact code pattern** from `debug_menu.c` and `menu.c`. It provides:

- **Main Menu** with: Whitelist, Send Payload, Exit buttons
- **Send Payload Submenu** with: Payload 1-4, Back button  
- **Whitelist Screen** to display serial numbers
- **Touch-based navigation** using your XPT2046 driver
- **Blocking loop architecture** (same as your `doMainMenu()` pattern)

## Files Provided

### Core Menu Files (Add to Your Project)
1. **payload_menu.h** - Menu function declarations
2. **payload_menu.c** - Menu logic implementation  
3. **payload_menu_draw.h** - Touch boxes and drawing functions
4. **payload_menu_draw.c** - Menu rendering implementation
5. **main_payload_example.c** - Complete usage example

### Your Existing Files (Referenced)
- `xpt2046.h/c` - Your touch controller driver ✅
- `ili9488*.h/c` - Your display driver ✅
- `debug.h` - Your SNWhitelist definition ✅

## Menu Structure

```
MAIN MENU (blocks until Exit)
├── Whitelist Button     → Shows whitelist screen
├── Send Payload Button  → Opens Send Payload submenu
└── Exit Button          → Exits menu loop

SEND PAYLOAD MENU (blocks until Back)
├── Payload 1 Button     → Sends Payload 1
├── Payload 2 Button     → Sends Payload 2
├── Payload 3 Button     → Sends Payload 3
├── Payload 4 Button     → Sends Payload 4
└── Back Button          → Returns to Main Menu

WHITELIST SCREEN
└── (Touch anywhere to return to Main Menu)
```

## Quick Integration

### Step 1: Add Global Variables

In your main file or header:

```c
/* Touch flag (set by your ISR or polling) */
struct {
    unsigned screen_touched : 1;
    // Your other flags
} payload_flags;

/* Touch coordinates (used by menu system) */
Touch user_touch;

/* Whitelist data */
#define WHITELIST_MAX_SIZE 10
uint32_t whitelist_array[WHITELIST_MAX_SIZE];

SNWhitelist my_whitelist = {
    .arr = whitelist_array,
    .current = 0,
    .length = WHITELIST_MAX_SIZE
};
```

### Step 2: Setup Touch Interrupt (or Polling)

**Option A: Interrupt-Based (Recommended)**

```c
void __interrupt() isr(void) {
    /* Touch interrupt on change */
    if(IOCBFbits.IOCBF0) {  // Your touch IRQ pin
        IOCBFbits.IOCBF0 = 0;
        payload_flags.screen_touched = 1;
        __delay_ms(50);  // Debounce
    }
}
```

**Option B: Polling-Based**

```c
while(1) {
    if(TOUCH_IRQ_GetValue() == 0) {
        payload_flags.screen_touched = 1;
        __delay_ms(50);
        while(TOUCH_IRQ_GetValue() == 0) {
            __delay_ms(10);
        }
    }
}
```

### Step 3: Run the Menu

```c
void main(void) {
    /* Initialize hardware and display */
    Ili9488Defines screen = ili9488_initialize(...);
    
    /* Run main menu (blocks until Exit) */
    doMainMenu(&my_whitelist, &screen);
    
    /* User exited */
    while(1) { /* idle */ }
}
```

### Step 4: Implement Your Payloads

Edit `payload_menu.c` and add your transmission code in the `send_payload_X()` functions:

```c
void send_payload_1(Ili9488Defines* pscreen) {
    // Status screen already drawn
    
    // YOUR CODE HERE:
    uint8_t payload1[] = {0xAA, 0xBB, 0xCC, 0xDD};
    UART1_WriteBuffer(payload1, sizeof(payload1));
    
    // Completion screen already handled
}
```

## How It Works (Same as Your Pattern)

### Main Menu Loop

```c
void doMainMenu(SNWhitelist* pwlist, Ili9488Defines* pscreen) {
    draw_main_menu(pscreen);        // Draw the menu
    draw_whitelist(pwlist, pscreen, 0);  // Show whitelist
    
    while(1) {
        if(payload_flags.screen_touched) {  // Wait for touch
            payload_flags.screen_touched = 0;
            
            if(main_get_process_touch(pwlist, pscreen)) {
                break;  // Exit selected
            }
        }
    }
}
```

This is **identical** to your `doMainMenu()` and `doAddMenu()` pattern!

### Touch Processing

```c
bool main_get_process_touch(SNWhitelist* pwlist, Ili9488Defines* pscreen) {
    
    if(xpt_touch_getxy(&user_touch)) {
        uint8_t touch_key = xpt_is_touch_in_box_list(user_touch, 
                                                      main_touch_boxes, 
                                                      num_boxes);
        
        if(touch_key == MAIN_BUTTON_EXIT) {
            return true;  // Exit
        }
        else if(touch_key == MAIN_BUTTON_WHITELIST) {
            doWhitelistScreen(pwlist, pscreen);  // Blocking call
            // Redraw when returning
            draw_main_menu(pscreen);
            draw_whitelist(pwlist, pscreen, 0);
        }
        // ... etc
    }
    
    return false;
}
```

Same pattern as your `main_get_process_touch()` and `add_get_process_touch()`!

## Touch Box Layout

### Main Menu (480x320 screen)

```
Screen Layout:
+----------------------------------+
|                                  |
|   Whitelist Display Area         |
|   (Serial numbers shown here)    |
|                                  |
|----------------------------------|
| Whitelist | Send Payload | Exit  |
+----------------------------------+
     ↑            ↑            ↑
  Button 1     Button 2     Button 3
```

Touch boxes defined in `payload_menu_draw.h`:

```c
static const TouchBox main_touch_boxes[] = {
    /* Whitelist Button */
    { .key = 0, .xs = 357, .ys = 7, .xe = 469, .ye = 108 },
    /* Send Payload Button */
    { .key = 1, .xs = 357, .ys = 109, .xe = 469, .ye = 210 },
    /* Exit Button */
    { .key = 2, .xs = 357, .ys = 211, .xe = 469, .ye = 312 },
};
```

### Send Payload Menu

```
+----------------------------------+
|  Payload 1  |  Payload 2  | Back |
|-------------|-------------|      |
|  Payload 3  |  Payload 4  |      |
+----------------------------------+
```

## Customization

### Change Button Colors

Edit `payload_menu_draw.c`:

```c
Ili9488Rect button_rect = {
    .color = MAGENTA,  // ← Change button outline color
    .weight = 2,
};

Ili9488Print button_label = {
    .fg = GREEN,  // ← Change text color
    .font = screen->Screen.offset_2x,
};
```

### Adjust Touch Box Positions

Edit `payload_menu_draw.h` - adjust the `xs, xe, ys, ye` values:

```c
static const TouchBox main_touch_boxes[] = {
    { 
        .key = MAIN_BUTTON_WHITELIST,
        .xs = 357,  // ← Top edge
        .ys = 7,    // ← Left edge
        .xe = 469,  // ← Bottom edge
        .ye = 108,  // ← Right edge
    },
    // ...
};
```

### Add More Payloads

1. Add to enum in `payload_menu_draw.h`:

```c
enum PayloadMenuKeys {
    PAYLOAD_BUTTON_1,
    PAYLOAD_BUTTON_2,
    PAYLOAD_BUTTON_3,
    PAYLOAD_BUTTON_4,
    PAYLOAD_BUTTON_5,  // NEW
    PAYLOAD_BUTTON_BACK,
};
```

2. Add touch box:

```c
static const TouchBox payload_touch_boxes[] = {
    // ... existing boxes ...
    { .key = PAYLOAD_BUTTON_5, .xs = ..., .ys = ..., .xe = ..., .ye = ... },
};
```

3. Add handler in `payload_menu.c`:

```c
bool payload_get_process_touch(Ili9488Defines* pscreen) {
    // ... existing code ...
    else if(touch_key == PAYLOAD_BUTTON_5) {
        send_payload_5(pscreen);
        draw_send_payload_menu(pscreen);
    }
}
```

4. Implement the function:

```c
void send_payload_5(Ili9488Defines* pscreen) {
    // Your code here
}
```

## Whitelist Functionality

The whitelist display is **already fully implemented** using your existing `draw_whitelist()` function pattern.

### Add Serial Number Programmatically

```c
uint32_t new_sn = 12345678;
add_sn_to_whitelist(&my_whitelist, &new_sn);
```

### Clear Whitelist

```c
clear_sn_whitelist(&my_whitelist);
```

### Check if Empty

```c
if(my_whitelist.current == 0) {
    // Whitelist is empty
}
```

## Debugging

### Test Touch Coordinates

Add debug output in `main_get_process_touch()`:

```c
if(xpt_touch_getxy(&user_touch)) {
    printf("Touch at X=%u, Y=%u\n", user_touch.x, user_touch.y);
    // ... rest of code
}
```

### Check Touch Box Detection

```c
uint8_t touch_key = xpt_is_touch_in_box_list(...);
if(touch_key == 255) {
    printf("Touch not in any box!\n");
} else {
    printf("Touch in box with key: %u\n", touch_key);
}
```

### Visual Button Feedback

Add temporary highlighting when button is touched:

```c
else if(touch_key == MAIN_BUTTON_WHITELIST) {
    // Flash the button white
    Ili9488Rect highlight = main_touch_boxes[touch_key];
    highlight.color = WHITE;
    ili9488_draw_rect(pscreen, &highlight);
    __delay_ms(100);
    
    // Then proceed normally
    doWhitelistScreen(pwlist, pscreen);
    // ...
}
```

## Comparison to Your Existing Code

| Your Code | New Code | Same Pattern? |
|-----------|----------|---------------|
| `doMainMenu()` | `doMainMenu()` | ✅ Yes |
| `doAddMenu()` | `doSendPayloadMenu()` | ✅ Yes |
| `main_get_process_touch()` | `main_get_process_touch()` | ✅ Yes |
| `add_get_process_touch()` | `payload_get_process_touch()` | ✅ Yes |
| `debug_draw_main_menu()` | `draw_main_menu()` | ✅ Yes |
| `debug_draw_add_menu()` | `draw_send_payload_menu()` | ✅ Yes |
| `add_touch_boxes[]` | `payload_touch_boxes[]` | ✅ Yes |
| `main_touch_boxes[]` | `main_touch_boxes[]` | ✅ Yes |

**Everything follows your exact pattern!**

## Memory Usage

| Component | RAM | Flash |
|-----------|-----|-------|
| Menu structures | ~100 bytes | ~3KB |
| Touch boxes | ~60 bytes | ~200 bytes |
| Whitelist (10 entries) | 44 bytes | - |
| **Total** | **~200 bytes** | **~3.2KB** |

Very similar to your existing menu system size.

## Next Steps

1. ✅ Add the 4 files to your MPLAB X project
2. ✅ Define `payload_flags` and `user_touch` globals
3. ✅ Setup touch interrupt or polling to set `payload_flags.screen_touched`
4. ✅ Call `doMainMenu(&whitelist, &screen)` from your main()
5. ✅ Implement your payload transmission in `send_payload_X()` functions
6. ✅ Test and adjust touch box coordinates if needed

## Example Payload Implementation

Here's a complete example for UART:

```c
void send_payload_1(Ili9488Defines* pscreen) {
    // Clear and show status (already implemented)
    // ...
    
    // Define payload
    typedef struct {
        uint8_t header[2];
        uint8_t command;
        uint8_t data[4];
        uint8_t checksum;
    } __attribute__((packed)) Payload1;
    
    Payload1 payload = {
        .header = {0xAA, 0xBB},
        .command = 0x01,
        .data = {0x11, 0x22, 0x33, 0x44},
        .checksum = 0
    };
    
    // Calculate checksum
    uint8_t* bytes = (uint8_t*)&payload;
    for(uint8_t i = 0; i < sizeof(Payload1) - 1; i++) {
        payload.checksum ^= bytes[i];
    }
    
    // Send via UART
    UART1_WriteBuffer(bytes, sizeof(Payload1));
    
    // Show progress
    while(!UART1_IsTxDone()) {
        ili9488_loading_bar(pscreen);
        __delay_ms(100);
    }
    
    // Show completion (already implemented)
    // ...
}
```

That's it! Your menu system is ready to use with the exact same pattern you're already familiar with.
