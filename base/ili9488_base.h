#ifndef _ILI9488_BASE
#define _ILI9488_BASE

#define _ILI9488

#include <stdint.h>

#include "system.h" 

typedef unsigned char color_t;

#define SOFTWARE_SPI
// #define HARDWARE_SPI
/**
 * Interface Structure
 * (This is an SPI Screen)
 * 
 */
typedef struct ILI9488_INTERFACE {
    uint8_t * lcd_rst_port;
    uint8_t   lcd_rst_pin;

} ili9488_interface_t;

/* 
 * Command Structures
 *
 * Ili9488CommandLengths: Another not especially helpful structure... 
 * 
 * Ili9488RamPointer
 */
typedef struct unusedCommandLengths{
    uint8_t set_ram_pointer;
    uint8_t send_command;
    uint8_t startup;
    uint8_t init;

} Ili9488CommandLengths;

typedef struct CursorPointer{
    uint16_t start_y;
    uint16_t start_x;
    uint16_t end_y;
    uint16_t end_x;

} Ili9488RamPointer;



/* Font Structures 
 * 
 * FontOffset: 
 * CharAttributes: 
 */
typedef struct Font_ByteOffset{
    const uint8_t * pfont;
    uint8_t ascii;

    uint8_t height;
    uint8_t width;
    uint8_t width_pad;

    uint8_t bytes_per_char;
    // uint8_t control;
} FontOffset;

typedef struct CharacterAttributes{
    uint8_t width;
    uint8_t height;
    uint8_t pad;
    uint8_t width_pad;
} CharAttributes;


/* 
 * API Structures
 *
 * Ili9488WriteNumber
 * 
 * Ili9488Print
 * 
 * Ili9488Prnt
 * 
 * Ili9488CommandLengths
 * 
 * Ili9488Cursor
 * 
 * Ili9488WriteBitmap
 * 
 * Ili9488WriteNumber
 * 
 * Ili9488VLine
 * 
 * Ili9488HLine
 * 
 * Ili9488Rect
 */

typedef struct WriteNumber{
    int32_t data;
    unsigned int right_aligned:1; // make a maximum number of characters that can be printed. (neede to right align the characters.)
    // uint8_t right_align;
    unsigned int scale:7;
    color_t fg;

    FontOffset font;
    Ili9488RamPointer ram_ptr;
} Ili9488WriteNumber;

typedef struct Print{
    char *       text;
    // uint8_t      length;
    // uint8_t      delay;
    // uint8_t      scale;
    unsigned int line_spacing:7;
    unsigned int clear_before:1;
    color_t      fg;
    color_t      bg;

    FontOffset font;
    Ili9488RamPointer ram_ptr;
} Ili9488Print;

/* A combination of the Print and Write number structures */
typedef struct Prnt{
    const char * text;
    int          data;
    uint8_t      msg_length;
    uint8_t      constrained_length; // make a maximum number of characters that can be printed. (neede to right align the characters.)
    uint8_t      right_align;
    uint8_t      delay;
    uint8_t      scale;

    Ili9488RamPointer ram_ptr;
} Ili9488Prnt;

typedef struct WriteBitmap{
    uint8_t * pbitmap;
    uint8_t   length;

    Ili9488RamPointer ram_ptr;
} Ili9488WriteBitmap;

typedef struct Cursor{
    uint8_t repeats;

    Ili9488RamPointer ram_ptr;
} Ili9488Cursor;

typedef struct RamWrite{
    uint8_t* buf;
    uint8_t buf_len;

    Ili9488RamPointer ram_ptr;
} Ili9488RamWrite;

typedef struct FillBlock{
    color_t color;

    Ili9488RamPointer ram_ptr;
} Ili9488FillBlock;

typedef struct WaitAnimate{
    Ili9488RamPointer ram_ptr;
    uint8_t three_ctr;
} Ili9488Wait;


/** 
 * Ili9488 Shapes
 *  */

typedef struct HVLine{
    uint16_t xstart;
    uint16_t ystart;
    uint16_t length;
    color_t color;
    uint16_t weight;
} Ili9488HVLine;

typedef struct Rect{
    uint16_t xstart;
    uint16_t ystart;
    uint16_t xend;
    uint16_t yend;
    uint16_t weight;
    color_t color;
} Ili9488Rect;

typedef struct Circle {
    uint16_t x;
    uint16_t y;

    uint8_t r;
    uint8_t weight;
    color_t color;
} Ili9488Circle;


/* 
 * Ili9488 Screen Defines:
 * 
 * This is the main structure used in data transmission to the screen.
 * */
typedef struct ScreenMetaData{
    uint16_t  ScreenHeight;
    uint16_t  ScreenWidth;

    uint8_t* pbuffer;
    size_t   buffer_size;

    // uint8_t* startup_buffer;
    // uint8_t  startup_size;

    // uint8_t  i2c_address;

    // uint8_t* rst_lat_port;
    // uint8_t  rst_pin;

    Ili9488RamPointer zeroed_ram_ptr;
    // Ili9488Wait* pwait;

    FontOffset offset_1x;
    FontOffset offset_2x;
    FontOffset offset_10x7;

    CharAttributes character;
} ScreenDefines;


/**
 * Top Level Structure
 */
typedef struct GeneralScreenStruct{
    ScreenDefines Screen;
    ili9488_interface_t interface;
    // Ili9488WriteNumber write_number;
    // Ili9488Print print;
    // Ili9488Prnt PRINT;
    // Ili9488WriteBitmap write_bitmap;
    // Ili9488Cursor cursor;
    // Ili9488RamWrite ram_write;
    // Ili9488RamPointer ram_ptr;
    // Ili9488Clear clr_line;
    // Ili9488Wait wait;
    // ScreenStringPerLine screen_strings;
} Ili9488Defines;


/**
 * Added 
 */
typedef struct {
    unsigned memory_access_busy : 1; // Bit 0
    unsigned tear_scanline_flag : 1; // Bit 1
    unsigned idle_mode : 1;          // Bit 2
    unsigned partial_mode : 1;       // Bit 3
    unsigned sleep_out : 1;          // Bit 4
    unsigned idlemode : 1;           // Bit 5
    unsigned tear_effect_flag : 1;   // Bit 6
    unsigned reserved : 1;           // Bit 7
} ili9488_status_byte0_t;

typedef struct {
    uint8_t param1;   // Don't care (all X's)
    
    // Second Parameter (D[31:24])
    union {
        uint8_t raw;
        struct {
            unsigned reserved_24              : 1;  // D24 (bit 0)
            unsigned horizontal_refresh_order : 1; // D25 (bit 1): 0=Left to Right, 1=Right to Left
            unsigned rgb_bgr_order            : 1;  // D26 (bit 2): 0=RGB, 1=BGR
            unsigned vertical_refresh         : 1;  // D27 (bit 3): 0=Top to Bottom, 1=Bottom to Top
            unsigned row_column_exchange      : 1;  // D28 (bit 4): 0=Normal, 1=Reverse
            unsigned column_address_order     : 1;  // D29 (bit 5): 0=Left to Right, 1=Right to Left
            unsigned row_address_order        : 1;  // D30 (bit 6): 0=Top to Bottom, 1=Bottom to Top
            unsigned booster_voltage_status   : 1;  // D31 (bit 7): 0=OFF, 1=ON
        } bits;
    } param2;
    
    // Third Parameter (D[23:16])
    union {
        uint8_t raw;
        struct {
            unsigned display_normal_mode    : 1;  // D16 (bit 0): 0=OFF, 1=ON
            unsigned sleep_mode             : 1;  // D17 (bit 1): 0=IN, 1=OUT
            unsigned partial_mode           : 1;  // D18 (bit 2): 0=OFF, 1=ON
            unsigned idle_mode              : 1;  // D19 (bit 3): 0=OFF, 1=ON
            unsigned interface_color_pixel  : 3;  // D[22:20] (bits 4-6): 101=16bit, 110=18bit, 111=24bit
            unsigned reserved_23            : 1;  // D23 (bit 7)
        } bits;
    } param3;
    
    // Fourth Parameter (D[15:8])
    union {
        uint8_t raw;
        struct {
            unsigned reserved_8             : 1;  // D8 (bit 0)
            unsigned tearing_effect_line_on : 1;  // D9 (bit 1): 0=OFF, 1=ON
            unsigned display_on             : 1;  // D10 (bit 2): 0=OFF, 1=ON
            unsigned reserved_11_12         : 2;  // D[12:11] (bits 3-4)
            unsigned inversion_status       : 1;  // D13 (bit 5): 0=OFF, 1=ON
            unsigned reserved_14            : 1;  // D14 (bit 6)
            unsigned vertical_scrolling     : 1;  // D15 (bit 7): 0=OFF, 1=ON
        } bits;
    } param4;
    
    // Fifth Parameter (D[7:0])
    union {
        uint8_t raw;
        struct {
            unsigned reserved_0_4           : 5;  // D[4:0] (bits 0-4)
            unsigned tearing_effect_mode    : 1;  // D5 (bit 5): 0=Mode 1 (V-Blanking), 1=Mode 2 (H&V-Blanking)
            unsigned reserved_6_7           : 2;  // D[7:6] (bits 6-7)
        } bits;
    } param5;
    
} ili9488_display_status_t;

typedef struct {
    unsigned booster_voltage_status     : 1; // Booster on and working (1) : Booster off or has fault (0)
    unsigned idle_mode_on_off           : 1; // On (1) : Off (0)
    unsigned partial_mode_on_off        : 1; // On (1) : Off (0)
    unsigned sleep_in_out               : 1; // Out (1) : In (0)
    unsigned display_normal_mode_on_off : 1; // On (1) : Off (0)
    unsigned display_on_off             : 1; // On (1) : Off (0)
    unsigned reserved                   : 2;
} ili9488_power_mode_t;

typedef struct {
    unsigned refresh_order : 1;   // MY
    unsigned row_col_exchange : 1; // MX
    unsigned row_col_swap : 1;     // MV
    unsigned rgb_bgr_order : 1;    // ML
    unsigned vertical_refresh : 1; // BGR
    unsigned horiz_refresh : 1;    // MH
    unsigned reserved : 2;         // Reserved bits
} ili9488_madctl_t;

typedef struct {
    unsigned reserved;

    struct {
        unsigned reserved1 : 1;
        unsigned dpi : 3;
        unsigned reserved2 : 1;
        unsigned dsi : 3;
    } color_depth;  // Typically 0x55 = 16-bit, 0x66 = 18-bit

} ili9488_pixel_format_t;

typedef struct {
    unsigned reserved : 4;
    unsigned invert_mode : 1;  // 0 = Normal, 1 = Inverted
    unsigned reserved2 : 3;
} ili9488_image_mode_t;

typedef struct
{
    unsigned char character;
    uint8_t * pdata;     /* Packed 3-bit pixels: 2 pixels per byte */
    uint16_t width;
    uint16_t height;
} Glyph;

#endif // _Ili9488_BASE