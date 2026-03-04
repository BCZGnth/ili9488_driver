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
    Ili9488Defines* screen,
    uint16_t start_x, uint16_t start_y,
    uint16_t end_x, uint16_t end_y,
    color_t color)
{
    uint8_t byte_of_color = ((color << 3) & 0x38) | (color & 0x7);

    // Note: The artifact compensation does not account for lines that have perfect multiple of 8 pixels.
    uint8_t artifact_compensation_bytes = 1;
    
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
    uint24_t no_of_bytes = no_of_pixels / 2 + artifact_compensation_bytes;
    
    uint24_t iterations = no_of_bytes / screen->Screen.buffer_size;
    uint24_t remainder = no_of_bytes % screen->Screen.buffer_size;
    
    ili9488_set_ram_pointer(line_ptr);
    
    if (!iterations) {
        memset(screen->Screen.pbuffer, byte_of_color, remainder);
        // Make sure the artifact on the screen is taken care of as much as possible:
        memset(screen->Screen.pbuffer + remainder - artifact_compensation_bytes, 0, artifact_compensation_bytes);

        ili9488_gram_write(screen->Screen.pbuffer, remainder);
    } else {
        memset(screen->Screen.pbuffer, byte_of_color, screen->Screen.buffer_size);
        ili9488_gram_write(screen->Screen.pbuffer, screen->Screen.buffer_size);
        
        while (iterations > 0) {
            ili9488_gram_write_continue(screen->Screen.pbuffer, screen->Screen.buffer_size);
            iterations--;
        }
        
        if (remainder > 0) {
            // Make sure the artifact on the screen is taken care of as much as possible:
            memset(screen->Screen.pbuffer + remainder - artifact_compensation_bytes, 0, artifact_compensation_bytes);
            ili9488_gram_write(screen->Screen.pbuffer, remainder);
        }
    }
}

// Wrapper functions (inline or macro)
void ili9488_draw_vline(Ili9488Defines * screen, Ili9488HVLine * Line) {
    // Add your weight/boundary checks here
    if(Line->xstart < 0) Line->xstart = 0;
    if(Line->xstart >= screen->Screen.ScreenWidth) Line->xstart = screen->Screen.ScreenWidth - 1;
    if(Line->xstart + Line->length >= screen->Screen.ScreenWidth) return;

    // Make sure we draw the "weight" on the correct side of the line
    if(Line->xstart > screen->Screen.ScreenWidth / 2) {
        // Decrement by the weight to make sure the full line is drawn on the edges of the screen
        Line->xstart -= Line->weight;
    }

    ili9488_draw_line_internal(screen, 
        Line->xstart, Line->ystart,
        Line->xstart + Line->weight - 1, Line->ystart + Line->length - 1,
        Line->color);
}

void ili9488_draw_hline(Ili9488Defines * screen, Ili9488HVLine * Line) {
    // Add your weight/boundary checks here
    if(Line->ystart < 0) Line->ystart = 0;
    if(Line->ystart >= screen->Screen.ScreenHeight) Line->ystart = screen->Screen.ScreenHeight - 1;
    if(Line->ystart + Line->length >= screen->Screen.ScreenHeight) return;

    // Make sure we draw the "weight" on the correct side of the line
    if(Line->ystart > screen->Screen.ScreenHeight / 2) {
        // Decrement by the weight to make sure the full line is drawn on the edges of the screen
        Line->ystart -= Line->weight;
    }

    ili9488_draw_line_internal(screen,
        Line->xstart, Line->ystart,
        Line->xstart + Line->length - 1, Line->ystart + Line->weight - 1,
        Line->color);
}

void ili9488_draw_rect(Ili9488Defines * screen, Ili9488Rect * Rect){
    // ili9488_send_command(screen, SET_DISPLAY_ON_OUTPUT_IGNORES_RAM_CONTENT);
    ADD_TO_STACK_DEPTH();
    // level_log(TRACE, "Ili9488 Draw Rectangle");

    uint16_t hlength = (Rect->xend - Rect->xstart + 1);
    // uint8_t hxstart = Rect->xstart + Rect->weight;
    uint16_t vlength = (Rect->yend - Rect->ystart + 1);

    Ili9488HVLine hline1 = {
        .xstart = Rect->xstart,
        .ystart = Rect->ystart,
        .length = hlength,
        .weight = Rect->weight,
        .color  = Rect->color
    };

    Ili9488HVLine vline1 = {
        .xstart = Rect->xstart,
        .ystart = Rect->ystart,
        .length = vlength,
        .weight = Rect->weight,
        .color  = Rect->color
    };

    Ili9488HVLine hline2 = {
        .xstart = Rect->xstart,
        .ystart = Rect->yend,
        .length = hlength,
        .weight = Rect->weight,
        .color  = Rect->color
    };

    Ili9488HVLine vline2 = {
        .xstart = Rect->xend,
        .ystart = Rect->ystart,
        .length = vlength,
        .weight = Rect->weight,
        .color  = Rect->color
    };

    ili9488_draw_hline(screen, &hline1);
    ili9488_draw_vline(screen, &vline1);
    ili9488_draw_hline(screen, &hline2);
    ili9488_draw_vline(screen, &vline2);

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
void ili9488_draw_circle(Ili9488Circle * circ) {

    int16_t f = 1 - circ->r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * circ->r;
    int16_t x = 0;
    int16_t y = circ->r;

    ili9488_draw_pixel(circ->x,          circ->y + circ->r, circ->color, 0);
    ili9488_draw_pixel(circ->x,          circ->y - circ->r, circ->color, 0);
    ili9488_draw_pixel(circ->x + circ->r, circ->y,          circ->color, 0);
    ili9488_draw_pixel(circ->x - circ->r, circ->y,          circ->color, 0);

    while (x < y) {
        if (f >= 0) {
          y--;
          ddF_y += 2;
          f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ili9488_draw_pixel(circ->x + x, circ->y + y, circ->color, 0);
        ili9488_draw_pixel(circ->x - x, circ->y + y, circ->color, 0);
        ili9488_draw_pixel(circ->x + x, circ->y - y, circ->color, 0);
        ili9488_draw_pixel(circ->x - x, circ->y - y, circ->color, 0);
        ili9488_draw_pixel(circ->x + y, circ->y + x, circ->color, 0);
        ili9488_draw_pixel(circ->x - y, circ->y + x, circ->color, 0);
        ili9488_draw_pixel(circ->x + y, circ->y - x, circ->color, 0);
        ili9488_draw_pixel(circ->x - y, circ->y - x, circ->color, 0);
    }
}


void ili9488_loading_bar(Ili9488Defines * screen) {
    
    static Ili9488HVLine loading_bar = {
        .color = CYAN,
        // .ystart = 316,
        .length = 4,
        .weight = 5,
    };

    loading_bar.ystart = screen->Screen.ScreenHeight - 5;

    /* Could implement some waiting visualization function here */
    // loading_bar.length += 2;
    loading_bar.xstart += 4;
    if(loading_bar.xstart + loading_bar.length > screen->Screen.ScreenWidth - 5) {
        // loading_bar.length = 5;
        loading_bar.xstart = 4;
        if(loading_bar.color) {
            loading_bar.color = BLACK;
        } else {
            loading_bar.color = CYAN;
        }
    }
    ili9488_draw_hline(screen, &loading_bar);
}
