#ifndef _ILI9488_BASE
#define _ILI9488_BASE

#include <stdint.h>

#include "system.h" 


/**
 * Interface Structure
 * (This is an SPI Screen)
 * 
 */
typedef struct ILI9488_INTERFACE {
    uint8_t * spi_miso_port;
    uint8_t * spi_mosi_port;
    uint8_t * spi_dc_port;
    uint8_t * spi_cs_port;
    uint8_t * spi_clock_port;
    
    uint8_t   spi_miso_pin;
    uint8_t   spi_mosi_pin;
    uint8_t   spi_dc_pin;
    uint8_t   spi_cs_pin;
    uint8_t   spi_clock_pin;

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
    uint16_t row;
    uint16_t column;
} Ili9488RamPointer;

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
    uint32_t data;
    uint8_t constrained_length; // make a maximum number of characters that can be printed. (neede to right align the characters.)
    // uint8_t right_align;
    uint8_t scale;

    Ili9488RamPointer ram_ptr;
} Ili9488WriteNumber;

typedef struct Print{
    const char * text;
    uint8_t      length;
    uint8_t      delay;
    uint8_t      scale;

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
    uint8_t   xstart;
    uint8_t   ystart;
    uint8_t   xend;
    uint8_t   yend;

    Ili9488RamPointer ram_ptr;
} Ili9488WriteBitmap;

typedef struct Cursor{
    uint8_t repeats;

    Ili9488RamPointer ram_ptr;
} Ili9488Cursor;

typedef struct RamWrite{
    uint8_t* bitmap;
    uint8_t bitmap_length;

    Ili9488RamPointer ram_ptr;
} Ili9488RamWrite;

typedef struct ClearLine{
    Ili9488RamPointer ram_ptr;
    uint8_t char_length;
    uint8_t start_page;
    uint8_t end_page; 
} Ili9488Clear;

typedef struct WaitAnimate{
    Ili9488RamPointer ram_ptr;
    uint8_t three_ctr;
} Ili9488Wait;

typedef struct HVLine{
    uint8_t xstart;
    uint8_t ystart;
    uint8_t length;
} Ili9488HVLine;

typedef struct Rect{
    uint8_t xstart;
    uint8_t ystart;
    uint8_t xend;
    uint8_t yend;
} Ili9488Rect;


/* Font Structures 
 * 
 * FontOffset: 
 * CharAttributes: 
 */
typedef struct Font_ByteOffset{
    uint8_t* pfont;
    uint8_t ascii;
    // uint8_t control;
} FontOffset;

typedef struct CharacterAttributes{
    uint8_t width;
    uint8_t height;
    uint8_t pad;
    uint8_t width_pad;
} CharAttributes;


/* 
 * Ili9488 Screen Defines:
 * 
 * This is the main structure used in data transmission to the screen.
 * */
typedef struct ScreenMetaData{
    uint8_t  ScreenHeight;
    uint8_t  ScreenWidth;

    uint8_t* pbuffer;
    size_t   buffer_size;

    // uint8_t* startup_buffer;
    // uint8_t  startup_size;

    // uint8_t  i2c_address;

    // uint8_t* rst_lat_port;
    // uint8_t  rst_pin;

    Ili9488RamPointer zeroed_ram_ptr;
    // Ili9488Wait* pwait;

    FontOffset offset;
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


#endif // _Ili9488_BASE