#include "menu.h"
#include "system.h"
#include "ili9488_base.h"
#include "ili9488_shapes.h"

void debug_draw_add_menu(Ili9488Defines screen) {
    
    ili9488_cls(screen);
    
    /* Draw the Frame that will seperate the number pad from the text display */
    Ili9488Rect outline = {
        .xstart = 0,
        .ystart = 0,
        .xend = screen.Screen.ScreenWidth - 1,
        .yend = screen.Screen.ScreenHeight - 1,
        .weight = 4,
        .color = CYAN,
    };

    Ili9488HVLine splitter = {
        .xstart = 4,
        .ystart = 20,
        .weight = 4,
        .length = screen.Screen.ScreenWidth - 9, // Account for the weight of the previous border on either side with the addition of one to account for a zero-indexed numbering system for the screen
        .color = CYAN,
    };

    ili9488_draw_rect(screen, outline);
    ili9488_draw_hline(screen, splitter);


    /* The Circles will each take up 102 pixels in diameter. 
     * This means that in total, three circles across will take up 
     * 306 pixels total, leaving 14 pixels for the border on both sides */

     /* Use a loop to draw the circles that will make up the numberpad */
    uint16_t circ_x_start = 4 + 51; // Circle Radius plus the offset for the border
    uint16_t circ_y_start = 24 + 51; // Circle Radius plus the offset for the split between the numberpad and the display area
    uint16_t num_x_start = circ_x_start - (screen.Screen.offset_2x.width / 2);
    uint16_t num_y_start = circ_y_start - (screen.Screen.offset_2x.height / 2);
    uint16_t step = 102;
    const char* numbers = "123456789x0+";
    char num_to_print[2] = {*numbers, '\0'};

    Ili9488Circle circ = {
        .x = circ_x_start,
        .y = circ_y_start,
        .r = 51, /* 103 pixels / 2 */
        .color = MAGENTA,
    };

    Ili9488Print circ_txt = {
        .text = &num_to_print,
        .fg = GREEN,
        .font = screen.Screen.offset_2x,
        .ram_ptr = {
            .start_x = num_x_start,
            .start_y = num_y_start,
            .end_x = num_x_start + screen.Screen.offset_2x.width,
            .end_y = num_y_start + screen.Screen.offset_2x.height,
        },
    };

    // Draw three circles across with their respective text
    for(uint8_t column = 0; column < 3; column++) {

        // Draw four rows of circles with their respective text
        for(uint8_t row = 0; row < 4; row++) {

            circ.x = circ_x_start + (step * column);
            circ.y = circ_y_start + (step * row);
            ili9488_draw_circle(circ);
            
            num_to_print[0] = *numbers;
            circ_txt.ram_ptr.start_x = num_x_start + (step * column);
            circ_txt.ram_ptr.end_x = num_x_start + screen.Screen.offset_2x.width + (step * column);
            circ_txt.ram_ptr.start_y = num_y_start + (step * row);
            circ_txt.ram_ptr.end_y = num_y_start + screen.Screen.offset_2x.height + (step * row);
            ili9488_print(screen, circ_txt);

            numbers++;
        }
    }
}

void debug_draw_main_menu(Ili9488Defines screen) {
    pass;
}