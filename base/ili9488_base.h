#ifndef _ili9488_BASE
#define _ili9488_BASE

#include <stdint.h>

#include "system.h" 

/* 
 * Command Structures
 *
 * ili9488CommandLengths: Another not especially helpful structure... 
 * 
 * ili9488RamPointer
 */
typedef struct unusedCommandLengths{
    uint8_t set_ram_pointer;
    uint8_t send_command;
    uint8_t startup;
    uint8_t init;
} ili9488CommandLengths;

typedef struct CursorPointer{
    uint8_t page;
    uint8_t position;
} ili9488RamPointer;

/* 
 * API Structures
 *
 * ili9488WriteNumber
 * 
 * ili9488Print
 * 
 * ili9488Prnt
 * 
 * ili9488CommandLengths
 * 
 * ili9488Cursor
 * 
 * ili9488WriteBitmap
 * 
 * ili9488WriteNumber
 * 
 * ili9488VLine
 * 
 * ili9488HLine
 * 
 * ili9488Rect
 * 
 * ScreenStringPerLine
 */
typedef struct ScreenString{
    uint8_t line_length;
    char* line0;
    char* line1;
    char* line2;
    char* line3;
    char* line4;
    char* line5;
    char* line6;
    char* line7;
} ScreenStringPerLine;

typedef struct WriteNumber{
    uint32_t data;
    uint8_t constrained_length; // make a maximum number of characters that can be printed. (neede to right align the characters.)
    // uint8_t right_align;
    uint8_t scale;

    ili9488RamPointer ram_ptr;
} ili9488WriteNumber;

typedef struct Print{
    const char * text;
    uint8_t      length;
    uint8_t      delay;
    uint8_t      scale;

    ili9488RamPointer ram_ptr;
} ili9488Print;

/* A combination of the Print and Write number structures */
typedef struct Prnt{
    const char * text;
    int          data;
    uint8_t      msg_length;
    uint8_t      constrained_length; // make a maximum number of characters that can be printed. (neede to right align the characters.)
    uint8_t      right_align;
    uint8_t      delay;
    uint8_t      scale;

    ili9488RamPointer ram_ptr;
} ili9488Prnt;

typedef struct WriteBitmap{
    uint8_t * pbitmap;
    uint8_t   length;
    uint8_t   xstart;
    uint8_t   ystart;
    uint8_t   xend;
    uint8_t   yend;

    ili9488RamPointer ram_ptr;
} ili9488WriteBitmap;

typedef struct Cursor{
    uint8_t repeats;

    ili9488RamPointer ram_ptr;
} ili9488Cursor;

typedef struct RamWrite{
    uint8_t* bitmap;
    uint8_t bitmap_length;

    ili9488RamPointer ram_ptr;
} ili9488RamWrite;

typedef struct ClearLine{
    ili9488RamPointer ram_ptr;
    uint8_t char_length;
    uint8_t start_page;
    uint8_t end_page; 
} ili9488Clear;

typedef struct WaitAnimate{
    ili9488RamPointer ram_ptr;
    uint8_t three_ctr;
} ili9488Wait;

typedef struct HVLine{
    uint8_t xstart;
    uint8_t ystart;
    uint8_t length;
} ili9488HVLine;

typedef struct Rect{
    uint8_t xstart;
    uint8_t ystart;
    uint8_t xend;
    uint8_t yend;
} ili9488Rect;


/* Font Structures 
 * 
 * FontOffset: 
 * CharAttributes: 
 */
typedef struct Font_ByteOffset{
    uint8_t* pfont;
    uint8_t ascii;
    uint8_t control;
} FontOffset;

typedef struct CharacterAttributes{
    uint8_t width;
    uint8_t height;
    uint8_t pad;
    uint8_t width_pad;
} CharAttributes;


/* 
 * ili9488 Screen Defines:
 * 
 * This is the main structure used in data transmission to the screen.
 * */
typedef struct ScreenMetaData{
    uint8_t  ScreenHeight;
    uint8_t  ScreenWidth;

    uint8_t* pbuffer;
    size_t   buffer_size;

    uint8_t* startup_buffer;
    uint8_t  startup_size;

    uint8_t  i2c_address;

    uint8_t* rst_lat_port;
    uint8_t  rst_pin;

    ili9488RamPointer zeroed_ram_ptr;
    ili9488Wait* pwait;

    FontOffset offset;
    CharAttributes character;
} ScreenDefines;


/**
 * Top Level Structure
 */
typedef struct GeneralScreenStruct{
    ScreenDefines Screen;
    // ili9488WriteNumber write_number;
    // ili9488Print print;
    // ili9488Prnt PRINT;
    // ili9488WriteBitmap write_bitmap;
    // ili9488Cursor cursor;
    // ili9488RamWrite ram_write;
    // ili9488RamPointer ram_ptr;
    // ili9488Clear clr_line;
    ili9488Wait wait;
    // ScreenStringPerLine screen_strings;
} ili9488Defines;


#endif // _ili9488_BASE