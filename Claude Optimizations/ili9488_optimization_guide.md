# ILI9488 PIC18F47K42 Code Size Optimization Guide

## Executive Summary
Your current driver has several areas for optimization. Based on analysis of your code, you can save **significant flash space** (estimated 20-40%) by implementing these optimizations, which will give you plenty of room for menu systems.

---

## Critical Optimizations (Highest Impact)

### 1. **ELIMINATE snprintf - HUGE Savings (~8-12KB)**

**Current Code (ili9488_api.c lines 220-280):**
```c
snprintf(temp_text, sizeof(temp_text), "%ld", args.data);
```

**Problem:** `snprintf` pulls in the entire printf library, which is **MASSIVE** on PIC18 (~8-12KB of flash).

**Solution:** Write a custom integer-to-string function:

```c
// Ultra-compact int32 to string converter (saves 8-10KB!)
static uint8_t int32_to_str(int32_t num, char* buf, uint8_t bufsize) {
    uint8_t i = 0;
    uint8_t negative = 0;
    
    if (num < 0) {
        negative = 1;
        num = -num;
    }
    
    // Handle zero case
    if (num == 0) {
        buf[i++] = '0';
        buf[i] = '\0';
        return 1;
    }
    
    // Convert digits (in reverse)
    uint8_t start = i;
    while (num > 0 && i < bufsize - 1) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    if (negative && i < bufsize - 1) {
        buf[i++] = '-';
    }
    
    buf[i] = '\0';
    
    // Reverse the string
    uint8_t end = i - 1;
    while (start < end) {
        char temp = buf[start];
        buf[start] = buf[end];
        buf[end] = temp;
        start++;
        end--;
    }
    
    return i;
}
```

**Impact:** Saves 8-12KB of flash - this is your biggest win!

---

### 2. **Consolidate Duplicate Code in Shapes**

**Problem:** `ili9488_draw_vline` and `ili9488_draw_hline` have nearly identical logic.

**Current:** ~350 lines of code
**Optimized:** ~200 lines

```c
// Generic line drawing function
static void ili9488_draw_line_internal(
    Ili9488Defines screen,
    uint16_t start_x, uint16_t start_y,
    uint16_t end_x, uint16_t end_y,
    color_t color)
{
    uint8_t byte_of_color = ((color << 3) & 0x38) | (color & 0x7);
    
    Ili9488RamPointer line_ptr = {
        .start_x = start_x,
        .end_x = end_x,
        .start_y = start_y,
        .end_y = end_y
    };
    
    uint24_t del_x = (end_x - start_x) + 1;
    uint24_t del_y = (end_y - start_y) + 1;
    uint24_t no_of_pixels = del_x * del_y;
    no_of_pixels = no_of_pixels + (no_of_pixels % 2);
    uint24_t no_of_bytes = no_of_pixels / 2;
    
    uint24_t iterations = no_of_bytes / screen.Screen.buffer_size;
    uint24_t remainder = no_of_bytes % screen.Screen.buffer_size;
    
    ili9488_set_ram_pointer(line_ptr);
    
    if (!iterations) {
        memset(screen.Screen.pbuffer, byte_of_color, remainder);
        ili9488_gram_write(screen.Screen.pbuffer, remainder);
    } else {
        memset(screen.Screen.pbuffer, byte_of_color, screen.Screen.buffer_size);
        ili9488_gram_write(screen.Screen.pbuffer, screen.Screen.buffer_size);
        
        while (iterations > 0) {
            ili9488_gram_write_continue(screen.Screen.pbuffer, screen.Screen.buffer_size);
            iterations--;
        }
        
        if (remainder > 0) {
            ili9488_gram_write(screen.Screen.pbuffer, remainder);
        }
    }
}

// Wrapper functions (inline or macro)
void ili9488_draw_vline(Ili9488Defines screen, Ili9488HVLine Line) {
    // Add your weight/boundary checks here
    ili9488_draw_line_internal(screen, 
        Line.xstart, Line.ystart,
        Line.xstart + Line.weight - 1, Line.ystart + Line.length - 1,
        Line.color);
}

void ili9488_draw_hline(Ili9488Defines screen, Ili9488HVLine Line) {
    // Add your artifact workaround here
    ili9488_draw_line_internal(screen,
        Line.xstart, Line.ystart,
        Line.xstart + Line.length - 1, Line.ystart + Line.weight - 1,
        Line.color);
}
```

**Impact:** Saves ~1-2KB

---

### 3. **Optimize Command Constants to Macros**

**Current (ili9488_commands.h):** 
```c
const uint16_t ILI9488_NOP = 0x000;
const uint16_t ILI9488_SOFTWARE_RESET = 0x001;
// ... 50+ more
```

**Problem:** Each `const` takes up RAM **and** flash. They're not being optimized away.

**Solution:** Use macros or enum:

```c
// Option 1: Macros (zero overhead)
#define ILI9488_NOP                    0x000
#define ILI9488_SOFTWARE_RESET         0x001
#define ILI9488_DISPLAY_ON             0x029
// etc.

// Option 2: Enum (compiler can optimize better)
enum {
    ILI9488_NOP = 0x000,
    ILI9488_SOFTWARE_RESET = 0x001,
    ILI9488_DISPLAY_ON = 0x029,
    // etc.
};
```

**Impact:** Saves ~200-400 bytes

---

### 4. **Simplify Variadic Command Function**

**Current:** `ili9488_send_command(uint16_t len_cmd, ...)`

**Problem:** Variadic functions are expensive on PIC18.

**Better approach:**

```c
// For most common cases (0-4 params)
void ili9488_cmd_0(uint8_t cmd);
void ili9488_cmd_1(uint8_t cmd, uint8_t p1);
void ili9488_cmd_2(uint8_t cmd, uint8_t p1, uint8_t p2);
void ili9488_cmd_4(uint8_t cmd, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4);

// For rare cases with many params
void ili9488_cmd_n(uint8_t cmd, uint8_t* params, uint8_t len);
```

**Or use inline helper:**

```c
static inline void ili9488_cmd_start(uint8_t cmd) {
    CS1_SetLow();
    DC1_SetLow();
    fast_spi_write_byte(cmd);
    DC1_SetHigh();
}

static inline void ili9488_cmd_end(void) {
    CS1_SetHigh();
}

// Usage:
ili9488_cmd_start(ILI9488_POWER_CONTROL_1);
fast_spi_write_byte(0x17);
fast_spi_write_byte(0x15);
ili9488_cmd_end();
```

**Impact:** Saves ~500-800 bytes

---

## Medium Priority Optimizations

### 5. **Remove Debug/Logging Code**

**Lines to strip:**
- All `ADD_TO_STACK_DEPTH()` / `REMOVE_FROM_STACK_DEPTH()`
- All commented-out `printf()` statements
- All `level_log()` calls

**Impact:** Saves ~300-600 bytes

---

### 6. **Optimize Structures - Use Bitfields**

**Current structures waste space.** Example optimization:

```c
// BEFORE: 8 bytes
typedef struct {
    uint16_t xstart;
    uint16_t ystart;
    uint16_t length;
    color_t color;    // only 3 bits used!
    uint16_t weight;
} Ili9488HVLine;

// AFTER: 6 bytes (25% smaller)
typedef struct {
    uint16_t xstart;
    uint16_t ystart;
    uint16_t length;
    uint16_t weight;
    uint8_t color : 3;  // Only uses 3 bits
    uint8_t _pad : 5;
} Ili9488HVLine;
```

**Impact:** Saves ~100-200 bytes across all structures

---

### 7. **Use const for Font Data**

Your fonts are already const, which is good! But verify they're in program memory:

```c
// Make absolutely sure fonts are in flash, not RAM
static const __prog__ font_char_1x_t ascii_font_1x[] = {
    // ...
};
```

Use XC8's `__prog__` qualifier to force program memory storage.

---

## Low Priority but Easy Wins

### 8. **Remove Unused Code**

**You have TONS of commented-out code.** Delete it entirely:
- Lines 676-823 in ili9488_api.c
- Lines 283-396 in ili9488_shapes.c
- Commented functions in ili9488_driver.c

**Impact:** Cleaner code, ~500 bytes saved in some cases

---

### 9. **Optimize Color Byte Packing**

This pattern appears everywhere:
```c
uint8_t byte_of_color = ((color << 3) & 0x38) | (color & 0x7);
```

Make it a macro or inline function:
```c
#define PACK_3BIT_COLOR(c) (((c) << 3) | ((c) & 0x7))

// Or inline
static inline uint8_t pack_color(color_t c) {
    return (c << 3) | (c & 0x7);
}
```

**Impact:** Saves ~50-100 bytes

---

### 10. **Reduce Structure Passing**

You're passing large structures by value in many places. Pass pointers instead:

```c
// BEFORE
void ili9488_draw_vline(Ili9488Defines screen, Ili9488HVLine Line);

// AFTER (saves stack space and code)
void ili9488_draw_vline(Ili9488Defines* screen, const Ili9488HVLine* Line);
```

---

## Menu System Design Recommendations

Now that you've freed up space, here's how to build space-efficient menus:

### Data-Driven Menu Structure

```c
typedef struct {
    const char* text;      // Pointer to ROM string
    void (*action)(void);  // Function pointer
    uint8_t x;
    uint8_t y;
} MenuItem;

typedef struct {
    const MenuItem* items; // Array in ROM
    uint8_t count;
    uint8_t selected;
} Menu;

// Define menus in ROM
const MenuItem main_menu_items[] = {
    {"Settings", &settings_menu, 10, 20},
    {"Data", &data_screen, 10, 40},
    {"About", &about_screen, 10, 60},
};

const Menu main_menu = {
    .items = main_menu_items,
    .count = 3,
    .selected = 0
};

// Generic menu renderer
void render_menu(const Menu* menu) {
    for (uint8_t i = 0; i < menu->count; i++) {
        color_t color = (i == menu->selected) ? CYAN : WHITE;
        Ili9488Print print_item = {
            .text = menu->items[i].text,
            .fg = color,
            .bg = BLACK,
            .font = screen.Screen.offset_1x,
            .ram_ptr = {
                .start_x = menu->items[i].x,
                .start_y = menu->items[i].y,
                .end_x = screen.Screen.ScreenWidth - 1,
                .end_y = menu->items[i].y + 15
            }
        };
        ili9488_print(screen, print_item);
    }
}
```

---

## Compiler Flags for XC8

Add these to your project settings:

```
-Os                          # Optimize for size
--opt=all                    # Enable all optimizations
-D__OPTIMIZE_SIZE__          # Size-focused optimizations
--RUNTIME=+clear             # Reduce runtime overhead
```

---

## Expected Savings Summary

| Optimization | Estimated Savings |
|-------------|-------------------|
| Remove snprintf | **8,000-12,000 bytes** |
| Consolidate line drawing | 1,000-2,000 bytes |
| Command constants → macros | 200-400 bytes |
| Simplify variadic functions | 500-800 bytes |
| Remove debug code | 300-600 bytes |
| Delete commented code | 200-500 bytes |
| Structure optimizations | 100-200 bytes |
| Misc optimizations | 200-400 bytes |
| **TOTAL** | **~10-16 KB saved** |

This should give you **plenty** of space for sophisticated menu systems!

---

## Implementation Priority

1. **Start with snprintf removal** - biggest impact
2. **Delete all commented code** - easy cleanup
3. **Consolidate line drawing** - good practice
4. **Convert constants to macros** - simple search/replace
5. **Add menu system** - now you have room!

Would you like me to create optimized versions of any specific files?
