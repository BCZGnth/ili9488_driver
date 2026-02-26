#include "ili9488_shapes.h"
#include "ili9488.h"
#include "system.h"
#include "logger.h"
#include <string.h>
/**
 * Takes a start x,y coordinate and a length of pixels to draw a line
 */
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

void ili9488_draw_rect(Ili9488Defines screen, Ili9488Rect Rect){
    // ili9488_send_command(screen, SET_DISPLAY_ON_OUTPUT_IGNORES_RAM_CONTENT);
    ADD_TO_STACK_DEPTH();
    // level_log(TRACE, "Ili9488 Draw Rectangle");

    uint16_t hlength = (Rect.xend - Rect.xstart + 1);
    // uint8_t hxstart = Rect.xstart + Rect.weight;
    uint16_t vlength = (Rect.yend - Rect.ystart + 1);

    Ili9488HVLine hline1 = {
        .xstart = Rect.xstart,
        .ystart = Rect.ystart,
        .length = hlength,
        .weight = Rect.weight,
        .color  = Rect.color
    };

    Ili9488HVLine vline1 = {
        .xstart = Rect.xstart,
        .ystart = Rect.ystart,
        .length = vlength,
        .weight = Rect.weight,
        .color  = Rect.color
    };

    Ili9488HVLine hline2 = {
        .xstart = Rect.xstart,
        .ystart = Rect.yend,
        .length = hlength,
        .weight = Rect.weight,
        .color  = Rect.color
    };

    Ili9488HVLine vline2 = {
        .xstart = Rect.xend,
        .ystart = Rect.ystart,
        .length = vlength,
        .weight = Rect.weight,
        .color  = Rect.color
    };

    ili9488_draw_hline(screen, hline1);
    ili9488_draw_vline(screen, vline1);
    ili9488_draw_hline(screen, hline2);
    ili9488_draw_vline(screen, vline2);

    // ili9488_send_command(screen, SET_DISPLAY_ON_RESUME_RAM_CONTENT_DISPLAY);
    // level_log(TRACE, "ILI9488 Drew Rectangle");
    REMOVE_FROM_STACK_DEPTH();
}

/* Circle Functions Taken from the Adafruit GFX library 
 * Link: https://github.com/adafruit/Adafruit-GFX-Library/blob/master/Adafruit_GFX.cpp */

/**************************************************************************/
/*!
   @brief    Draw a circle outline
    @param    x0   Center-point x coordinate
    @param    circ.ystart   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 3-bit 1-1-1 Color to draw with
*/
/**************************************************************************/
void ili9488_draw_circle(Ili9488Circle circ) {

    int16_t f = 1 - circ.r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * circ.r;
    int16_t x = 0;
    int16_t y = circ.r;
  
    ili9488_draw_pixel(circ.x,          circ.y + circ.r, circ.color, 0);
    ili9488_draw_pixel(circ.x,          circ.y - circ.r, circ.color, 0);
    ili9488_draw_pixel(circ.x + circ.r, circ.y,          circ.color, 0);
    ili9488_draw_pixel(circ.x - circ.r, circ.y,          circ.color, 0);

    while (x < y) {
        if (f >= 0) {
          y--;
          ddF_y += 2;
          f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ili9488_draw_pixel(circ.x + x, circ.y + y, circ.color, 0);
        ili9488_draw_pixel(circ.x - x, circ.y + y, circ.color, 0);
        ili9488_draw_pixel(circ.x + x, circ.y - y, circ.color, 0);
        ili9488_draw_pixel(circ.x - x, circ.y - y, circ.color, 0);
        ili9488_draw_pixel(circ.x + y, circ.y + x, circ.color, 0);
        ili9488_draw_pixel(circ.x - y, circ.y + x, circ.color, 0);
        ili9488_draw_pixel(circ.x + y, circ.y - x, circ.color, 0);
        ili9488_draw_pixel(circ.x - y, circ.y - x, circ.color, 0);
    }
}

// /**************************************************************************/
// /*!
//     @brief    Quarter-circle drawer, used to do circles and roundrects
//     @param    x0   Center-point x coordinate
//     @param    y0   Center-point y coordinate
//     @param    r   Radius of circle
//     @param    cornername  Mask bit #1, #2, #4, and #8 to indicate which quarters
//               of the circle we're doing
//     @param    color 16-bit 5-6-5 Color to draw with
// */
// /**************************************************************************/
// void Adafruit_GFX::drawCircleHelper(int16_t x0, int16_t y0, int16_t r,
//                                     uint8_t cornername, uint16_t color) {
//   int16_t f = 1 - r;
//   int16_t ddF_x = 1;
//   int16_t ddF_y = -2 * r;
//   int16_t x = 0;
//   int16_t y = r;

//   while (x < y) {
//     if (f >= 0) {
//       y--;
//       ddF_y += 2;
//       f += ddF_y;
//     }
//     x++;
//     ddF_x += 2;
//     f += ddF_x;
//     if (cornername & 0x4) {
//       ili9488_draw_pixel(x0 + x, y0 + y, color);
//       ili9488_draw_pixel(x0 + y, y0 + x, color);
//     }
//     if (cornername & 0x2) {
//       ili9488_draw_pixel(x0 + x, y0 - y, color);
//       ili9488_draw_pixel(x0 + y, y0 - x, color);
//     }
//     if (cornername & 0x8) {
//       ili9488_draw_pixel(x0 - y, y0 + x, color);
//       ili9488_draw_pixel(x0 - x, y0 + y, color);
//     }
//     if (cornername & 0x1) {
//       ili9488_draw_pixel(x0 - y, y0 - x, color);
//       ili9488_draw_pixel(x0 - x, y0 - y, color);
//     }
//   }
// }

// /**************************************************************************/
// /*!
//    @brief    Draw a circle with filled color
//     @param    x0   Center-point x coordinate
//     @param    y0   Center-point y coordinate
//     @param    r   Radius of circle
//     @param    color 16-bit 5-6-5 Color to fill with
// */
// /**************************************************************************/
// void il9488_fill_circle(int16_t x0, int16_t y0, int16_t r, color_t color) {
//     ili9488_draw_vline(x0, y0 - r, 2 * r + 1, color);
//     fillCircleHelper(x0, y0, r, 3, 0, color);
// }

// /**************************************************************************/
// /*!
//     @brief  Half-circle drawer with fill, used for circles and roundrects
//     @param  x0       Center-point x coordinate
//     @param  y0       Center-point y coordinate
//     @param  r        Radius of circle
//     @param  corners  Mask bits indicating which sides of the circle we are
//                      doing, left (1) and/or right (2)
//     @param  delta    Offset from center-point, used for round-rects
//     @param  color    16-bit 5-6-5 Color to fill with
// */
// /**************************************************************************/
// void Adafruit_GFX::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
//                                     uint8_t corners, int16_t delta,
//                                     uint16_t color) {

//   int16_t f = 1 - r;
//   int16_t ddF_x = 1;
//   int16_t ddF_y = -2 * r;
//   int16_t x = 0;
//   int16_t y = r;
//   int16_t px = x;
//   int16_t py = y;

//   delta++; // Avoid some +1's in the loop

//   while (x < y) {
//     if (f >= 0) {
//       y--;
//       ddF_y += 2;
//       f += ddF_y;
//     }
//     x++;
//     ddF_x += 2;
//     f += ddF_x;
//     // These checks avoid double-drawing certain lines, important
//     // for the SSD1306 library which has an INVERT drawing mode.
//     if (x < (y + 1)) {
//       if (corners & 1)
//         writeFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
//       if (corners & 2)
//         writeFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
//     }
//     if (y != py) {
//       if (corners & 1)
//         writeFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
//       if (corners & 2)
//         writeFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
//       py = y;
//     }
//     px = x;
//   }
// }


void ili9488_loading_bar(Ili9488Defines screen) {
    
    static Ili9488HVLine loading_bar = {
        .color = CYAN,
        // .ystart = 316,
        .length = 4,
        .weight = 5,
    };

    loading_bar.ystart = screen.Screen.ScreenHeight - 5;

    /* Could implement some waiting visualization function here */
    // loading_bar.length += 2;
    loading_bar.xstart += 4;
    if(loading_bar.xstart + loading_bar.length > screen.Screen.ScreenWidth - 5) {
        // loading_bar.length = 5;
        loading_bar.xstart = 4;
        if(loading_bar.color) {
            loading_bar.color = BLACK;
        } else {
            loading_bar.color = CYAN;
        }
    }
    ili9488_draw_hline(screen, loading_bar);
}