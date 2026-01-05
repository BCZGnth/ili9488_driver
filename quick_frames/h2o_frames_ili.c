#include "h2o_frames_ili.h"
#include "ili9488.h"

// void h2o_rectangular_frames(Ili9488Defines screen) {

//     Ili9488Rect half_screen_outline = {
//         .xstart = 0,
//         .xend = 59,
//         .ystart = 0,
//         .yend = 63
//     };

//     Ili9488Rect other_half_screen_outline = {
//         .xstart = 60,
//         .xend = 127,
//         .ystart = 0,
//         .yend = 31
//     };

//     Ili9488Rect other_other_half_screen_outline = {
//         .xstart = 60,
//         .xend = 127,
//         .ystart = 32,
//         .yend = 63
//     };

//     Ili9488Print h2o = {
//         .text = "H2O",
//         .length = 3,
//         .ram_ptr = {
//             .position = 3,
//             .page = 1
//         },
//         .scale = 1,
//         .delay = 1
//     };

//     Ili9488Print connected = {
//         .text = "Connected",
//         .length = 9,
//         .ram_ptr = {
//             .position = 3,
//             .page = 2
//         },
//         .scale = 1,
//         .delay = 1
//     };

//     Ili9488HVLine underscore = {
//         .xstart = 3,
//         .ystart = 25,
//         .length = 54
//     };

//     ili9488_cls(screen.Screen);
//     ili9488_draw_rect(screen.Screen, half_screen_outline);
//     ili9488_draw_rect(screen.Screen, other_half_screen_outline);
//     ili9488_draw_rect(screen.Screen, other_other_half_screen_outline);
//     ili9488_print(screen.Screen, h2o);
//     ili9488_print(screen.Screen, connected);
//     ili9488_draw_hline(screen.Screen, underscore);
// }

void h2o_outline(Ili9488Defines screen) {

    Ili9488Print h2o_connected = {
        .text = "H2O Disconnected",
        .length = 16,
        .ram_ptr = {
            .start_x = 138,
            .end_x = 342,
            .start_y = 100,
            .end_y = 116
        },
        .scale = 1,
        .delay = 1,
        .fg = YELLOW
    };

    Ili9488Rect full_outline = {
        .xstart = 0,
        .xend = screen.Screen.ScreenWidth - 1,
        .ystart = 0,
        .yend = screen.Screen.ScreenHeight - 1,
        .weight = 3,
        .color = MAGENTA,
    };

    ili9488_draw_rect(screen, full_outline);
    ili9488_print(screen, h2o_connected);
}

void generic_payload_frame(Ili9488Defines screen, uint32_t serial_number)
{
    Ili9488Print ser_num = {
        .text = "SER_NUM = ",
        .length = 6,
        .ram_ptr = {
            .start_x = (screen.Screen.ScreenWidth - 240) / 2,
            .end_x = ((screen.Screen.ScreenWidth - 240) / 2) + 120,
            .start_y = 6,
            .end_y = 6 + screen.Screen.character.height
        },
        // .scale = 1,
        // .delay = 0
        .fg = GREEN 
    };

    /* Print Serial number */
    Ili9488WriteNumber ser_val = {
        .constrained_length = 6,
        .data = serial_number,
        .ram_ptr = {
            .start_x = ((screen.Screen.ScreenWidth - 240) / 2) + 121,
            .end_x = ((screen.Screen.ScreenWidth - 240) / 2) + 241,
            .start_y = 6,
            .end_y = 6 + screen.Screen.character.height
        },
        // .scale = 1,
        .fg = CYAN
    };

    ili9488_print(       screen, ser_num);
    ili9488_write_number(screen, ser_val);
}

void pretty_payload_frame(Ili9488Defines screen, uint32_t serial_number)
{


    Ili9488Rect full_outline = {
        .xstart = 0,
        .xend = 127,
        .ystart = 0,
        .yend = 63
    };

    Ili9488Print SerNUM = {
        .text = "Ser #:",
        .length = 6,
        .ram_ptr = {
            .start_x = 0,
            .start_y = 0,
            .end_x = 0,
            .end_y = 0
        },
        .scale = 1,
        .delay = 0
    };

    Ili9488HVLine ser_num_underline = {
        .ystart = 18,
        .xstart = 41,
        .length = 50
    };

    /* Print Serial number */
    Ili9488WriteNumber ser_val = {
        .constrained_length = 6,
        .data = serial_number,
        .ram_ptr = {
            .start_x = 0,
            .start_y = 0,
            .end_x = 0,
            .end_y = 0
        },
        .scale = 1,
    };

    ili9488_write_number(screen, ser_val);
    ili9488_draw_rect(   screen, full_outline);
    ili9488_print(       screen, SerNUM);
    ili9488_draw_hline(  screen, ser_num_underline);
}