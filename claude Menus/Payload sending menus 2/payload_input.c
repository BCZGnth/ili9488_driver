/**
 * @file payload_input.c
 * @brief Implementation of generic field input system
 */

#include "payload_input.h"
#include "payload_menu.h"
#include "ili9488_shapes.h"
#include <string.h>

// External flag and touch from your main code
extern struct {
    unsigned screen_touched : 1;
} payload_flags;

extern Touch user_touch;

// -----------------------------------------------------------------------------
// Main Field Input Function
// -----------------------------------------------------------------------------

bool get_payload_fields(Ili9488Defines* pscreen, PayloadConfig* config) {
    
    /* Initialize all values to defaults */
    for(uint8_t i = 0; i < config->num_fields; i++) {
        config->values[i].value = config->fields[i].default_value;
        config->values[i].is_set = 0;
    }
    
    /* Prompt for each field */
    for(uint8_t i = 0; i < config->num_fields; i++) {
        if(!get_single_field(pscreen, &config->fields[i], &config->values[i].value)) {
            /* User cancelled */
            return false;
        }
        config->values[i].is_set = 1;
    }
    
    /* All fields completed */
    return true;
}

// -----------------------------------------------------------------------------
// Single Field Input
// -----------------------------------------------------------------------------

bool get_single_field(Ili9488Defines* pscreen, 
                      const FieldDefinition* field_def,
                      uint32_t* current_value) {
    
    /* Start with default value */
    *current_value = field_def->default_value;
    
    /* Draw the input screen */
    draw_field_input_screen(pscreen, field_def, *current_value);
    
    bool cancelled = false;
    
    /* Input loop */
    while(1) {
        if(payload_flags.screen_touched) {
            payload_flags.screen_touched = 0;
            
            if(process_field_input_touch(pscreen, field_def, current_value, &cancelled)) {
                /* Input complete or cancelled */
                break;
            }
        }
    }
    
    return !cancelled;
}

// -----------------------------------------------------------------------------
// Screen Drawing
// -----------------------------------------------------------------------------

void draw_field_input_screen(Ili9488Defines* pscreen,
                              const FieldDefinition* field_def,
                              uint32_t current_value) {
    
    ili9488_cls(pscreen);
    
    /* Draw frame */
    Ili9488Rect outline = {
        .xstart = 0,
        .ystart = 0,
        .xend = pscreen->Screen.ScreenWidth - 1,
        .yend = pscreen->Screen.ScreenHeight - 1,
        .weight = 4,
        .color = CYAN,
    };
    ili9488_draw_rect(pscreen, &outline);
    
    /* Horizontal splitter */
    Ili9488HVLine splitter = {
        .xstart = 4,
        .ystart = 64,
        .weight = 4,
        .length = pscreen->Screen.ScreenWidth - 9,
        .color = CYAN,
    };
    ili9488_draw_hline(pscreen, &splitter);
    
    /* Draw field label */
    Ili9488Print label = {
        .text = (char*)field_def->label,
        .fg = YELLOW,
        .font = pscreen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 10,
            .end_x = 30,
            .end_y = 200,
        },
    };
    ili9488_print(pscreen, &label);
    
    /* Draw current value */
    char value_str[12];
    format_field_value(current_value, field_def, value_str, sizeof(value_str));
    
    Ili9488Print value_display = {
        .text = value_str,
        .fg = GREEN,
        .font = pscreen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = 35,
            .start_y = 50,
            .end_x = 55,
            .end_y = 250,
        },
    };
    ili9488_print(pscreen, &value_display);
    
    /* Draw numpad (identical to your add menu) */
    uint16_t step = 102;
    static const char* numbers[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "x", "0", "OK"};
    
    Ili9488Rect rect = {
        .color = MAGENTA,
        .weight = 2,
    };
    
    Ili9488Print button_txt = {
        .fg = GREEN,
        .font = pscreen->Screen.offset_2x,
    };
    
    /* Draw 3x4 grid of buttons */
    for(uint8_t column = 0; column < 3; column++) {
        for(uint8_t row = 0; row < 4; row++) {
            uint8_t idx = column + (3 * row);
            
            /* Draw button outline */
            rect.xstart = input_touch_boxes[idx].xs;
            rect.xend   = input_touch_boxes[idx].xe;
            rect.ystart = input_touch_boxes[idx].ys;
            rect.yend   = input_touch_boxes[idx].ye;
            ili9488_draw_rect(pscreen, &rect);
            
            /* Draw button text */
            button_txt.text = (char*)numbers[idx];
            button_txt.ram_ptr.start_x = input_touch_boxes[idx].xs + step / 2;
            button_txt.ram_ptr.end_x   = input_touch_boxes[idx].xe;
            button_txt.ram_ptr.start_y = input_touch_boxes[idx].ys + step / 2;
            button_txt.ram_ptr.end_y   = input_touch_boxes[idx].ye;
            ili9488_print(pscreen, &button_txt);
        }
    }
    
    /* Draw cancel button */
    Ili9488Rect cancel_rect = {
        .xstart = cancel_button_box.xs,
        .xend = cancel_button_box.xe,
        .ystart = cancel_button_box.ys,
        .yend = cancel_button_box.ye,
        .weight = 2,
        .color = RED,
    };
    ili9488_draw_rect(pscreen, &cancel_rect);
    
    Ili9488Print cancel_txt = {
        .text = "X",
        .fg = RED,
        .font = pscreen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = 20,
            .start_y = 20,
            .end_x = 40,
            .end_y = 40,
        },
    };
    ili9488_print(pscreen, &cancel_txt);
}

// -----------------------------------------------------------------------------
// Touch Processing
// -----------------------------------------------------------------------------

bool process_field_input_touch(Ili9488Defines* pscreen,
                                const FieldDefinition* field_def,
                                uint32_t* current_value,
                                bool* cancelled) {
    
    if(!xpt_touch_getxy(&user_touch)) {
        return false;
    }
    
    /* Check cancel button first */
    if(xpt_is_touch_in_box(user_touch, cancel_button_box)) {
        *cancelled = true;
        return true;  /* Break loop */
    }
    
    /* Check numpad buttons */
    uint8_t touch_key = xpt_is_touch_in_box_list(user_touch, 
                                                  (TouchBox*)input_touch_boxes,
                                                  sizeof(input_touch_boxes)/sizeof(TouchBox));
    
    if(touch_key == 255) {
        return false;  /* No button touched */
    }
    
    /* Handle delete */
    if(touch_key == INPUT_BUTTON_DELETE) {
        if(*current_value == 0) {
            /* Nothing to delete, maybe cancel? */
            *cancelled = true;
            return true;
        }
        
        *current_value /= 10;
        
        /* Redraw screen with new value */
        draw_field_input_screen(pscreen, field_def, *current_value);
        return false;
    }
    
    /* Handle OK */
    if(touch_key == INPUT_BUTTON_OK) {
        /* Validate value */
        if(validate_field_value(*current_value, field_def)) {
            return true;  /* Value accepted */
        } else {
            /* Show error - value out of range */
            Ili9488Print error = {
                .text = "Value out of range!",
                .fg = RED,
                .font = pscreen->Screen.offset_1x,
                .ram_ptr = {
                    .start_x = 55,
                    .start_y = 50,
                    .end_x = 65,
                    .end_y = 250,
                },
            };
            ili9488_print(pscreen, &error);
            __delay_ms(1000);
            
            /* Redraw */
            draw_field_input_screen(pscreen, field_def, *current_value);
            return false;
        }
    }
    
    /* Handle number input (0-9) */
    if(touch_key >= INPUT_BUTTON_ZERO && touch_key <= INPUT_BUTTON_NINE) {
        uint8_t digit = touch_key;  /* Key value equals digit */
        
        /* Check for overflow */
        if(*current_value > 429496729) {
            /* Would overflow on multiply */
            return false;
        }
        
        uint32_t new_value = (*current_value * 10) + digit;
        
        /* Check against max value */
        if(new_value > field_def->max_value) {
            /* Flash error */
            Ili9488Print error = {
                .text = "Max exceeded!",
                .fg = RED,
                .font = pscreen->Screen.offset_1x,
                .ram_ptr = {
                    .start_x = 55,
                    .start_y = 50,
                    .end_x = 65,
                    .end_y = 200,
                },
            };
            ili9488_print(pscreen, &error);
            __delay_ms(500);
            
            /* Redraw */
            draw_field_input_screen(pscreen, field_def, *current_value);
            return false;
        }
        
        *current_value = new_value;
        
        /* Redraw with new value */
        draw_field_input_screen(pscreen, field_def, *current_value);
    }
    
    return false;  /* Continue input loop */
}

// -----------------------------------------------------------------------------
// Helper Functions
// -----------------------------------------------------------------------------

bool validate_field_value(uint32_t value, const FieldDefinition* field_def) {
    return (value >= field_def->min_value && value <= field_def->max_value);
}

void format_field_value(uint32_t value, 
                        const FieldDefinition* field_def,
                        char* buffer, 
                        uint8_t buffer_size) {
    
    if(field_def->type == FIELD_TYPE_HEX_BYTE) {
        /* Format as hex: 0xXX */
        buffer[0] = '0';
        buffer[1] = 'x';
        
        /* Convert to hex */
        uint8_t high = (value >> 4) & 0x0F;
        uint8_t low = value & 0x0F;
        
        buffer[2] = (high < 10) ? ('0' + high) : ('A' + high - 10);
        buffer[3] = (low < 10) ? ('0' + low) : ('A' + low - 10);
        buffer[4] = '\0';
    }
    else {
        /* Format as decimal */
        /* Use simple uint32 to string conversion */
        if(value == 0) {
            buffer[0] = '0';
            buffer[1] = '\0';
            return;
        }
        
        uint8_t i = 0;
        uint32_t temp = value;
        
        /* Convert in reverse */
        while(temp > 0 && i < buffer_size - 1) {
            buffer[i++] = '0' + (temp % 10);
            temp /= 10;
        }
        buffer[i] = '\0';
        
        /* Reverse the string */
        uint8_t start = 0;
        uint8_t end = i - 1;
        while(start < end) {
            char tmp = buffer[start];
            buffer[start] = buffer[end];
            buffer[end] = tmp;
            start++;
            end--;
        }
    }
}
