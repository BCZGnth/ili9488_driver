/**
 * @file payload_menu_draw.c
 * @brief Implementation of menu drawing functions
 * 
 * Follows the pattern from menu.c
 */

#include "payload_menu_draw.h"
#include "ili9488_shapes.h"
#include "../debug.h"
#include <string.h>

/**
 * @param screen, a pointer to the screen structure
 * @param buttons, a pointer to the touchbox array holding the box/key associations 
 * @param labels, a pointer to the string array that holds labels for each button
 */
void draw_menu_items(Ili9488Defines* screen, TouchBox * buttons, const char ** labels, uint8_t count) {
    /* Draw the Frame */
    Ili9488Rect outline = {
        .xstart = 0,
        .ystart = 0,
        .xend = screen->Screen.ScreenWidth - 1,
        .yend = screen->Screen.ScreenHeight - 1,
        .weight = 4,
        .color = CYAN,
    }; 
}

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

    // /* Horizontal line separating whitelist display from buttons */
    // Ili9488HVLine splitter = {
    //     .xstart = 350,
    //     .ystart = 4,
    //     .weight = 4,
    //     .length = screen->Screen.ScreenWidth - 9,
    //     .color = CYAN,
    // };

    ili9488_draw_rect(screen, &outline);

    
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

    for(uint8_t i = 0; i < MAIN_MENU_NUM_OF_KEYS; i++) {
        /* Draw button outline */
        button_rect.xstart = main_touch_boxes[i].xs;
        button_rect.xend   = main_touch_boxes[i].xe;
        button_rect.ystart = main_touch_boxes[i].ys;
        button_rect.yend   = main_touch_boxes[i].ye;
        ili9488_draw_rect(screen, &button_rect);

        /* Calculate text centering */
        uint8_t text_length = strlen(button_txt[i]);
        uint16_t text_width = text_length * screen->Screen.offset_2x.width_pad;
        uint16_t button_width = main_touch_boxes[i].xe - main_touch_boxes[i].xs;
        uint16_t button_height = main_touch_boxes[i].ye - main_touch_boxes[i].ys;

        uint16_t text_x_center, text_y_center;
        if(button_width > text_width) {
            text_x_center = main_touch_boxes[i].xs + (button_width / 2) - (text_width / 2);
        } else {
            text_x_center = main_touch_boxes[i].xs;
        }
        if(button_height > screen->Screen.offset_2x.height) {
            text_y_center = main_touch_boxes[i].ys + (button_height / 2) - (screen->Screen.offset_2x.height / 2);
        } else {
            text_y_center = main_touch_boxes[i].ys;
        }

        /* Draw button text */
        button_label.text = (char*)button_txt[i];
        button_label.ram_ptr.start_x = text_x_center;
        button_label.ram_ptr.end_x   = text_x_center + text_length;
        button_label.ram_ptr.start_y = text_y_center;
        button_label.ram_ptr.end_y   = text_y_center + screen->Screen.offset_2x.height;
        // button_label.ram_ptr.start_x = main_touch_boxes[i].xs;
        // button_label.ram_ptr.end_x   = main_touch_boxes[i].xe;
        // button_label.ram_ptr.start_y = main_touch_boxes[i].ys;
        // button_label.ram_ptr.end_y   = main_touch_boxes[i].ye;
        
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

    Ili9488Rect button_rect = {
        .color = MAGENTA,
        .weight = 3,
    };

    Ili9488Print button_label = {
        .fg = GREEN,
        .font = screen->Screen.offset_2x,
    };

    for(uint8_t i = 0; i < PAYLOAD_MENU_NUM_OF_KEYS; i++) {
        /* Draw button outline */
        button_rect.xstart = payload_touch_boxes[i].xs;
        button_rect.xend   = payload_touch_boxes[i].xe;
        button_rect.ystart = payload_touch_boxes[i].ys;
        button_rect.yend   = payload_touch_boxes[i].ye;
        ili9488_draw_rect(screen, &button_rect);

        /* Calculate text position (centered) */
        uint8_t text_length = strlen(payload_labels[i]);
        uint16_t text_width = text_length * screen->Screen.offset_2x.width_pad;
        
        uint16_t button_width  = payload_touch_boxes[i].xe - payload_touch_boxes[i].xs;
        uint16_t button_height = payload_touch_boxes[i].ye - payload_touch_boxes[i].ys;

        uint16_t text_x_center = payload_touch_boxes[i].xs + (button_width / 2) - (text_width / 2);
        uint16_t text_y_center = payload_touch_boxes[i].ys + (button_height / 2) - (screen->Screen.offset_2x.height / 2);

        /* Draw button text */
        button_label.text = (char*)payload_labels[i];
        button_label.ram_ptr.start_x = text_x_center;
        button_label.ram_ptr.end_x   = text_x_center + text_width;
        button_label.ram_ptr.start_y = text_y_center;
        button_label.ram_ptr.end_y   = text_y_center + screen->Screen.offset_2x.height;
        
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
    static const char* numbers[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "<", "0", "Add"};
    // const char* numbers = "123456789x0+";
    // char num_to_print[][2] = {*numbers, '\0'};

    // Ili9488Circle circ = {
    //     .x = circ_x_start,
    //     .y = circ_y_start,
    //     .r = 51, /* 103 pixels / 2 */
    //     .color = MAGENTA,
    // };

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

void draw_whitelist_menu(Ili9488Defines * screen) {
    ili9488_cls(screen);

    #define WHITELIST_MENU_FONT screen->Screen.offset_2x

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
        .ystart = screen->Screen.ScreenHeight - 126,
        .weight = 4,
        .length = screen->Screen.ScreenWidth - 9, // Account for the weight of the previous border on either side with the addition of one to account for a zero-indexed numbering system for the screen
        .color = CYAN,
    };

    ili9488_draw_rect(screen, &outline);
    ili9488_draw_hline(screen, &splitter);

    /* Use a loop to draw the circles that will make up the numberpad */
    uint16_t circ_x_start = 4 + 51; // Circle Radius plus the offset for the border
    uint16_t circ_y_start = screen->Screen.ScreenHeight - 51 - 4; // Circle Radius plus the offset for the border which is 4 pixels wide
    uint16_t num_x_start = circ_x_start - (screen->Screen.offset_2x.width / 2);
    uint16_t num_y_start = circ_y_start - (screen->Screen.offset_2x.height / 2);
    uint16_t step = 102;
    static const char* button_txt[] = {"EXIT", "ADD", "CLEAR"};


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
        // .text = button_txt[0],
        .fg = GREEN,
        .font = screen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = num_x_start,
            .start_y = num_y_start,
            .end_x = num_x_start + screen->Screen.offset_2x.width,
            .end_y = num_y_start + screen->Screen.offset_2x.height,
        },

    };

    /* Draw the three Circles */
    for(uint8_t column = 0; column < WHITELIST_MENU_NUM_OF_KEYS; column++) {
        uint8_t text_length = strlen(button_txt[column]);
        text_length *= screen->Screen.offset_10x7.width_pad; 

        circ_txt.text = button_txt[column];
        circ_txt.ram_ptr.start_x = whitelist_touch_boxes[column].xs - (text_length / 2) + (step * column);
        circ_txt.ram_ptr.end_x = whitelist_touch_boxes[column].xs + (text_length / 2) + (step * column);

        rect.xstart = whitelist_touch_boxes[column].xs;
        rect.xend =   whitelist_touch_boxes[column].xe;
        rect.ystart = whitelist_touch_boxes[column].ys;
        rect.yend =   whitelist_touch_boxes[column].ye;

        ili9488_draw_rect(screen, &rect);

        ili9488_print(screen, &circ_txt);
    }

}

void draw_whitelist(SNWhitelist * pwlist, Ili9488Defines * pscreen, uint8_t clear_before) {

    #define WHITELIST_TEXT_Y_START 55

    FontOffset whitelist_font = pscreen->Screen.offset_2x;
    uint8_t whitelist_line_spacing = 10;

    if(clear_before) {
        Ili9488RamPointer black_line = {
            .start_x = 20,
            .start_y = WHITELIST_TEXT_Y_START,
            .end_x = pscreen->Screen.ScreenWidth - 20,
            .end_y = 20 + whitelist_font.height,
        };

        for(int line = 0; line < pwlist->length; line++) {
            ili9488_clear_block(pscreen, &black_line);
            black_line.start_y += (whitelist_font.height + whitelist_line_spacing);
            black_line.end_y += (whitelist_font.height + whitelist_line_spacing);
        }
    }

    Ili9488WriteNumber sernum = {
        .clear_before = false,
        .fg = RED,
        .font = whitelist_font,
        .right_aligned = true,
        .ram_ptr = {
            .start_x = pscreen->Screen.ScreenWidth / 2,
            .start_y = WHITELIST_TEXT_Y_START,
            .end_x   = pscreen->Screen.ScreenWidth - 20,
            .end_y   = WHITELIST_TEXT_Y_START + whitelist_font.height
        }
    };

    Ili9488Print prnt = {
        .clear_before = false,
        .fg = YELLOW,
        .font = whitelist_font,
        .line_spacing = 0,
        .text = "Whitelist: ",
        .ram_ptr = {
            .start_x = 20,
            .start_y = WHITELIST_TEXT_Y_START,
            .end_x   = pscreen->Screen.ScreenWidth / 2,
            .end_y   = WHITELIST_TEXT_Y_START + whitelist_font.height
        }
    };
    
    #define DRAW_WHITELIST_TEXT_SIZE 10
    char text[DRAW_WHITELIST_TEXT_SIZE];

    /* If there are no serial numbers in the list, inform and break */
    if(pwlist->current == 0) {
        prnt.font = pscreen->Screen.offset_10x7;
        prnt.line_spacing = 3;
        prnt.text = "No Serial Numbers\nin Whitelist.";
        prnt.ram_ptr.end_x = pscreen->Screen.ScreenWidth - 25;
        prnt.ram_ptr.end_y = pscreen->Screen.ScreenHeight / 2;
        
        ili9488_print(pscreen, &prnt);
        return;
    }

    /* Print the heading title */
    ili9488_print(pscreen, &prnt);
    prnt.ram_ptr.start_y += (whitelist_font.height + whitelist_line_spacing);
    prnt.ram_ptr.end_y += (whitelist_font.height + whitelist_line_spacing);
    
    sernum.ram_ptr.start_y += (whitelist_font.height + whitelist_line_spacing);
    sernum.ram_ptr.end_y += (whitelist_font.height + whitelist_line_spacing);

    /* Display each serial number */
    for(uint8_t i = 0; i < pwlist->current; i++) {
        if(pwlist->arr[i] == 0) break;

        /* Create label "SN #X:" */
        strcpy(text, "SN #");
        char num_str[4];
        
        /* Convert index to string (simple implementation) */
        if(i < 10) {
            num_str[0] = '0' + i;
            num_str[1] = ':';
            num_str[2] = '\0';
        } else {
            num_str[0] = '0' + (i / 10);
            num_str[1] = '0' + (i % 10);
            num_str[2] = ':';
            num_str[3] = '\0';
        }
        strcat(text, num_str);
        
        prnt.text = text;
        prnt.ram_ptr.start_y += (whitelist_font.height + whitelist_line_spacing);
        prnt.ram_ptr.end_y += (whitelist_font.height + whitelist_line_spacing);

        sernum.data = pwlist->arr[i];
        sernum.ram_ptr.start_y += (whitelist_font.height + whitelist_line_spacing);
        sernum.ram_ptr.end_y += (whitelist_font.height + whitelist_line_spacing);

        ili9488_print(pscreen, &prnt);
        ili9488_write_number(pscreen, &sernum);
    }
}
