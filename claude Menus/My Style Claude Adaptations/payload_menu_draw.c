/**
 * @file payload_menu_draw.c
 * @brief Implementation of menu drawing functions
 * 
 * Follows the pattern from menu.c
 */

#include "payload_menu_draw.h"
#include "ili9488_shapes.h"
#include <string.h>

void draw_main_menu(Ili9488Defines* screen) {
    ili9488_cls(screen);
    
    /* Draw the Frame */
    Ili9488Rect outline = {
        .xstart = 0,
        .ystart = 0,
        .xend = screen->Screen.ScreenWidth - 1,
        .yend = screen->Screen.ScreenHeight - 1,
        .weight = 4,
        .color = CYAN,
    };

    /* Horizontal line separating whitelist display from buttons */
    Ili9488HVLine splitter = {
        .xstart = 350,
        .ystart = 4,
        .weight = 4,
        .length = screen->Screen.ScreenWidth - 9,
        .color = CYAN,
    };

    ili9488_draw_rect(screen, &outline);
    ili9488_draw_hline(screen, &splitter);

    /* Draw the three main menu buttons */
    static const char* button_txt[] = {"Add to Whitelist", "Send Payload", "Exit"};
    
    Ili9488Rect button_rect = {
        .color = MAGENTA,
        .weight = 2,
    };

    Ili9488Print button_label = {
        .fg = GREEN,
        .font = screen->Screen.offset_2x,
        .ram_ptr = {0},
    };

    #define TEXT_PAD 2

    for(uint8_t i = 0; i < 3; i++) {
        /* Draw button outline */
        button_rect.xstart = main_touch_boxes[i].xs;
        button_rect.xend   = main_touch_boxes[i].xe;
        button_rect.ystart = main_touch_boxes[i].ys;
        button_rect.yend   = main_touch_boxes[i].ye;
        ili9488_draw_rect(screen, &button_rect);

        /* Calculate text centering */
        uint8_t text_length = strlen(button_txt[i]);
        uint16_t text_width = text_length * screen->Screen.offset_2x.width_pad;
        uint16_t button_width = main_touch_boxes[i].ye - main_touch_boxes[i].ys;
        uint16_t button_height = main_touch_boxes[i].xe - main_touch_boxes[i].xs;

        uint16_t text_y_center = main_touch_boxes[i].ys + (button_width / 2) - (text_width / 2);
        uint16_t text_x_center = main_touch_boxes[i].xs + (button_height / 2) - (screen->Screen.offset_2x.height / 2);

        /* Draw button text */
        button_label.text = (char*)button_txt[i];
        // button_label.ram_ptr.start_x = text_x_center;
        // button_label.ram_ptr.end_x   = text_x_center + screen->Screen.offset_2x.height;
        // button_label.ram_ptr.start_y = text_y_center;
        // button_label.ram_ptr.end_y   = text_y_center + text_width;
        button_label.ram_ptr.start_x = main_touch_boxes[i].xs + TEXT_PAD;
        button_label.ram_ptr.end_x   = main_touch_boxes[i].xe - TEXT_PAD;
        button_label.ram_ptr.start_y = main_touch_boxes[i].ys + TEXT_PAD;
        button_label.ram_ptr.end_y   = main_touch_boxes[i].ye - TEXT_PAD;
        
        ili9488_print(screen, &button_label);
    }

    /* Draw title in whitelist area */
    Ili9488Print title = {
        .text = "MAIN MENU",
        .fg = YELLOW,
        .font = screen->Screen.offset_2x,
        .clear_before = 0,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 10,
            .end_x = 150,
            .end_y = 200,
        },
    };
    ili9488_print(screen, &title);
}

void draw_send_payload_menu(Ili9488Defines* screen) {
    ili9488_cls(screen);
    
    /* Draw the Frame */
    Ili9488Rect outline = {
        .xstart = 0,
        .ystart = 0,
        .xend = screen->Screen.ScreenWidth - 1,
        .yend = screen->Screen.ScreenHeight - 1,
        .weight = 4,
        .color = CYAN,
    };
    ili9488_draw_rect(screen, &outline);

    /* Draw title */
    Ili9488Print title = {
        .text = "SEND PAYLOAD",
        .fg = YELLOW,
        .font = screen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 10,
            .end_x = 150,
            .end_y = 200,
        },
    };
    ili9488_print(screen, &title);

    /* Draw payload buttons */
    static const char* payload_labels[] = {
        "Payload 1",
        "Payload 2", 
        "Payload 3",
        "Payload 4",
        "Back"
    };

    Ili9488Rect button_rect = {
        .color = MAGENTA,
        .weight = 3,
    };

    Ili9488Print button_label = {
        .fg = GREEN,
        .font = screen->Screen.offset_2x,
    };

    for(uint8_t i = 0; i < 5; i++) {
        /* Draw button outline */
        button_rect.xstart = payload_touch_boxes[i].xs;
        button_rect.xend   = payload_touch_boxes[i].xe;
        button_rect.ystart = payload_touch_boxes[i].ys;
        button_rect.yend   = payload_touch_boxes[i].ye;
        ili9488_draw_rect(screen, &button_rect);

        /* Calculate text position (centered) */
        uint8_t text_length = strlen(payload_labels[i]);
        uint16_t text_width = text_length * screen->Screen.offset_2x.width_pad;
        
        uint16_t button_width = payload_touch_boxes[i].ye - payload_touch_boxes[i].ys;
        uint16_t button_height = payload_touch_boxes[i].xe - payload_touch_boxes[i].xs;
        
        uint16_t text_y_center = payload_touch_boxes[i].ys + (button_width / 2) - (text_width / 2);
        uint16_t text_x_center = payload_touch_boxes[i].xs + (button_height / 2) - (screen->Screen.offset_2x.height / 2);

        /* Draw button text */
        button_label.text = (char*)payload_labels[i];
        button_label.ram_ptr.start_x = text_x_center;
        button_label.ram_ptr.end_x   = text_x_center + screen->Screen.offset_2x.height;
        button_label.ram_ptr.start_y = text_y_center;
        button_label.ram_ptr.end_y   = text_y_center + text_width;
        
        ili9488_print(screen, &button_label);
    }
}

void debug_draw_add_menu(Ili9488Defines * screen) {
    
    ili9488_cls(screen);
    
    /* Draw the Frame that will seperate the number pad from the text display */
    Ili9488Rect outline = {
        .xstart = 0,
        .ystart = 0,
        .xend = screen->Screen.ScreenWidth - 1,
        .yend = screen->Screen.ScreenHeight - 1,
        .weight = 4,
        .color = CYAN,
    };

    Ili9488HVLine splitter = {
        .xstart = 4,
        .ystart = 60,
        .weight = 4,
        .length = screen->Screen.ScreenWidth - 9, // Account for the weight of the previous border on either side with the addition of one to account for a zero-indexed numbering system for the screen
        .color = CYAN,
    };

    ili9488_draw_rect(screen, &outline);
    ili9488_draw_hline(screen, &splitter);


    /* The Circles will each take up 102 pixels in diameter. 
     * This means that in total, three circles across will take up 
     * 306 pixels total, leaving 14 pixels for the border on both sides */

     /* Use a loop to draw the circles that will make up the numberpad */
    uint16_t circ_x_start = 4 + 51; // Circle Radius plus the offset for the border
    uint16_t circ_y_start = 24 + 51; // Circle Radius plus the offset for the split between the numberpad and the display area
    uint16_t num_x_start = circ_x_start - (screen->Screen.offset_2x.width / 2);
    uint16_t num_y_start = circ_y_start - (screen->Screen.offset_2x.height / 2);
    uint16_t step = 102;
    static const char* numbers[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "x", "0", "+"};
    // const char* numbers = "123456789x0+";
    // char num_to_print[][2] = {*numbers, '\0'};

    Ili9488Circle circ = {
        .x = circ_x_start,
        .y = circ_y_start,
        .r = 51, /* 103 pixels / 2 */
        .color = MAGENTA,
    };

    Ili9488Rect rect = {
        .color = MAGENTA,
        .weight = 2,
        .xstart = circ_x_start - (step / 2),
        .xend =   circ_x_start + (step / 2),
        .ystart = circ_y_start - (step / 2),
        .yend =   circ_y_start + (step / 2),
    };

    Ili9488Print circ_txt = {
        // .text = &(num_to_print[0]),
        .fg = GREEN,
        .font = screen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = num_x_start,
            .start_y = num_y_start,
            .end_x = num_x_start + screen->Screen.offset_2x.width,
            .end_y = num_y_start + screen->Screen.offset_2x.height,
        },
    };

    // Draw three circles across with their respective text
    for(uint8_t column = 0; column < 3; column++) {

        // Draw four rows of circles with their respective text
        for(uint8_t row = 0; row < 4; row++) {

            // circ.x = circ_x_start + (step * column);
            // circ.y = circ_y_start + (step * row);
            // ili9488_draw_circle(circ);

            rect.xstart = add_touch_boxes[column + (3*row)].xs;
            rect.xend   = add_touch_boxes[column + (3*row)].xe;
            rect.ystart = add_touch_boxes[column + (3*row)].ys;
            rect.yend   = add_touch_boxes[column + (3*row)].ye;
            ili9488_draw_rect(screen, &rect);
            

            // num_to_print[0] = *numbers;
            circ_txt.text = numbers[(3*row) + column];
            circ_txt.ram_ptr.start_x = add_touch_boxes[column + (3*row)].xs + step / 2; // num_x_start + (step * column);
            circ_txt.ram_ptr.end_x   = add_touch_boxes[column + (3*row)].xe; // num_y_start + (step * row);
            circ_txt.ram_ptr.start_y = add_touch_boxes[column + (3*row)].ys + step / 2; // num_x_start + screen.Screen.offset_2x.width + (step * column);
            circ_txt.ram_ptr.end_y   = add_touch_boxes[column + (3*row)].ye; // num_y_start + screen.Screen.offset_2x.height + (step * row);
            ili9488_print(screen, &circ_txt);

            // numbers++;
        }
    }
}