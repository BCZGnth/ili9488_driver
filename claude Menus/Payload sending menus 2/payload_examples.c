/**
 * @file payload_examples.c
 * @brief Examples showing how to use the field input system for payloads
 * 
 * Replace the send_payload_X() functions in payload_menu.c with these examples
 */

#include "payload_input.h"
#include "payload_menu.h"
#include "ili9488_shapes.h"

// -----------------------------------------------------------------------------
// Example 1: Simple Payload with 3 Fields
// -----------------------------------------------------------------------------

void send_payload_1_with_input(Ili9488Defines* pscreen) {
    
    /* Define the fields you want the user to input */
    FieldDefinition fields[] = {
        {
            .label = "Device ID:",
            .type = FIELD_TYPE_UINT8,
            .min_value = 1,
            .max_value = 255,
            .default_value = 1,
        },
        {
            .label = "Command:",
            .type = FIELD_TYPE_HEX_BYTE,
            .min_value = 0,
            .max_value = 255,
            .default_value = 0x01,
        },
        {
            .label = "Data Length:",
            .type = FIELD_TYPE_UINT16,
            .min_value = 1,
            .max_value = 1024,
            .default_value = 8,
        },
    };
    
    /* Storage for the values */
    FieldValue values[3];
    
    /* Configuration */
    PayloadConfig config = {
        .num_fields = 3,
        .fields = fields,
        .values = values,
    };
    
    /* Get input from user */
    if(!get_payload_fields(pscreen, &config)) {
        /* User cancelled - return to menu */
        return;
    }
    
    /* User completed input - extract values */
    uint8_t device_id = (uint8_t)values[0].value;
    uint8_t command = (uint8_t)values[1].value;
    uint16_t data_length = (uint16_t)values[2].value;
    
    /* Show sending screen */
    ili9488_cls(pscreen);
    
    Ili9488Print title = {
        .text = "SENDING PAYLOAD 1",
        .fg = GREEN,
        .font = pscreen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 20,
            .end_x = 30,
            .end_y = 300,
        },
    };
    ili9488_print(pscreen, title);
    
    /* Show the values being sent */
    char info_line[30];
    sprintf(info_line, "DevID:%u Cmd:0x%02X Len:%u", device_id, command, data_length);
    
    Ili9488Print info = {
        .text = info_line,
        .fg = YELLOW,
        .font = pscreen->Screen.offset_1x,
        .ram_ptr = {
            .start_x = 50,
            .start_y = 20,
            .end_x = 70,
            .end_y = 300,
        },
    };
    ili9488_print(pscreen, info);
    
    /* TODO: Send your actual payload here */
    /* Example:
     * uint8_t payload[] = {
     *     0xAA, 0xBB,        // Header
     *     device_id,         // Device ID from user input
     *     command,           // Command from user input
     *     (data_length >> 8) & 0xFF,  // Length high byte
     *     data_length & 0xFF,         // Length low byte
     * };
     * UART1_WriteBuffer(payload, sizeof(payload));
     */
    
    /* Show progress */
    Ili9488Print sending = {
        .text = "Transmitting...",
        .fg = CYAN,
        .font = pscreen->Screen.offset_1x,
        .ram_ptr = {
            .start_x = 100,
            .start_y = 50,
            .end_x = 120,
            .end_y = 250,
        },
    };
    ili9488_print(pscreen, sending);
    
    ili9488_loading_bar(pscreen);
    __delay_ms(1000);
    
    /* Show completion */
    Ili9488Print complete = {
        .text = "COMPLETE!",
        .fg = GREEN,
        .font = pscreen->Screen.offset_1x,
        .ram_ptr = {
            .start_x = 150,
            .start_y = 100,
            .end_x = 170,
            .end_y = 200,
        },
    };
    ili9488_print(pscreen, complete);
    
    __delay_ms(1500);
}

// -----------------------------------------------------------------------------
// Example 2: Payload with More Fields
// -----------------------------------------------------------------------------

void send_payload_2_with_input(Ili9488Defines* pscreen) {
    
    /* More complex payload with 5 fields */
    FieldDefinition fields[] = {
        {"Target Addr:", FIELD_TYPE_HEX_BYTE, 0x01, 0xFF, 0x10},
        {"Cmd Type:", FIELD_TYPE_HEX_BYTE, 0x00, 0xFF, 0x20},
        {"Param 1:", FIELD_TYPE_UINT8, 0, 255, 100},
        {"Param 2:", FIELD_TYPE_UINT8, 0, 255, 50},
        {"Timeout (ms):", FIELD_TYPE_UINT16, 10, 5000, 1000},
    };
    
    FieldValue values[5];
    
    PayloadConfig config = {
        .num_fields = 5,
        .fields = fields,
        .values = values,
    };
    
    if(!get_payload_fields(pscreen, &config)) {
        return;  /* Cancelled */
    }
    
    /* Extract all values */
    uint8_t target_addr = (uint8_t)values[0].value;
    uint8_t cmd_type = (uint8_t)values[1].value;
    uint8_t param1 = (uint8_t)values[2].value;
    uint8_t param2 = (uint8_t)values[3].value;
    uint16_t timeout = (uint16_t)values[4].value;
    
    /* Send payload */
    ili9488_cls(pscreen);
    
    Ili9488Print status = {
        .text = "SENDING PAYLOAD 2",
        .fg = GREEN,
        .font = pscreen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 20,
            .end_x = 30,
            .end_y = 300,
        },
    };
    ili9488_print(pscreen, status);
    
    /* TODO: Build and send your payload
     * typedef struct {
     *     uint8_t header[2];
     *     uint8_t target_addr;
     *     uint8_t cmd_type;
     *     uint8_t param1;
     *     uint8_t param2;
     *     uint16_t timeout;
     *     uint8_t checksum;
     * } __attribute__((packed)) Payload2;
     * 
     * Payload2 payload = {
     *     .header = {0xAA, 0xBB},
     *     .target_addr = target_addr,
     *     .cmd_type = cmd_type,
     *     .param1 = param1,
     *     .param2 = param2,
     *     .timeout = timeout,
     *     .checksum = 0,
     * };
     * // Calculate checksum
     * // Send via UART/SPI/etc
     */
    
    __delay_ms(1000);
}

// -----------------------------------------------------------------------------
// Example 3: Simple Payload with Just One Field
// -----------------------------------------------------------------------------

void send_payload_3_with_input(Ili9488Defines* pscreen) {
    
    /* Single field - maybe just a command byte */
    FieldDefinition fields[] = {
        {"Command:", FIELD_TYPE_HEX_BYTE, 0x00, 0xFF, 0x55},
    };
    
    FieldValue values[1];
    
    PayloadConfig config = {
        .num_fields = 1,
        .fields = fields,
        .values = values,
    };
    
    if(!get_payload_fields(pscreen, &config)) {
        return;
    }
    
    uint8_t command = (uint8_t)values[0].value;
    
    /* Send single-byte command */
    ili9488_cls(pscreen);
    
    Ili9488Print title = {
        .text = "SENDING COMMAND",
        .fg = GREEN,
        .font = pscreen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 50,
            .end_x = 30,
            .end_y = 250,
        },
    };
    ili9488_print(pscreen, title);
    
    /* Show command being sent */
    char cmd_str[20];
    sprintf(cmd_str, "CMD: 0x%02X", command);
    
    Ili9488Print cmd_display = {
        .text = cmd_str,
        .fg = YELLOW,
        .font = pscreen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = 100,
            .start_y = 80,
            .end_x = 120,
            .end_y = 240,
        },
    };
    ili9488_print(pscreen, cmd_display);
    
    /* TODO: Send command byte
     * UART1_Write(command);
     */
    
    __delay_ms(1000);
}

// -----------------------------------------------------------------------------
// Example 4: Payload with Pre-set Values (No User Input)
// -----------------------------------------------------------------------------

void send_payload_4_no_input(Ili9488Defines* pscreen) {
    
    /* This one doesn't ask for input - just sends a fixed payload */
    
    ili9488_cls(pscreen);
    
    Ili9488Print title = {
        .text = "SENDING PAYLOAD 4",
        .fg = GREEN,
        .font = pscreen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 20,
            .end_x = 30,
            .end_y = 300,
        },
    };
    ili9488_print(pscreen, title);
    
    Ili9488Print info = {
        .text = "Fixed Payload",
        .fg = YELLOW,
        .font = pscreen->Screen.offset_1x,
        .ram_ptr = {
            .start_x = 50,
            .start_y = 80,
            .end_x = 70,
            .end_y = 220,
        },
    };
    ili9488_print(pscreen, info);
    
    /* Send fixed payload */
    uint8_t fixed_payload[] = {
        0xAA, 0xBB,  // Header
        0x04,        // Payload ID
        0x00, 0x00,  // Fixed data
        0xFF         // Footer
    };
    
    /* TODO: Send via your interface
     * UART1_WriteBuffer(fixed_payload, sizeof(fixed_payload));
     */
    
    ili9488_loading_bar(pscreen);
    __delay_ms(500);
    
    Ili9488Print complete = {
        .text = "SENT!",
        .fg = GREEN,
        .font = pscreen->Screen.offset_2x,
        .ram_ptr = {
            .start_x = 150,
            .start_y = 120,
            .end_x = 170,
            .end_y = 180,
        },
    };
    ili9488_print(pscreen, complete);
    
    __delay_ms(1500);
}

// -----------------------------------------------------------------------------
// Example 5: Advanced - Using Different Field Types
// -----------------------------------------------------------------------------

void send_payload_advanced_example(Ili9488Defines* pscreen) {
    
    /* Demonstrate all field types */
    FieldDefinition fields[] = {
        /* Hex byte - good for addresses, commands */
        {"Start Addr:", FIELD_TYPE_HEX_BYTE, 0x00, 0xFF, 0x80},
        
        /* uint8 - good for counts, IDs */
        {"Retry Count:", FIELD_TYPE_UINT8, 1, 10, 3},
        
        /* uint16 - good for lengths, timeouts */
        {"Buffer Size:", FIELD_TYPE_UINT16, 16, 2048, 256},
        
        /* uint32 - good for serial numbers, large values */
        {"Serial Num:", FIELD_TYPE_UINT32, 1000000, 9999999, 1234567},
    };
    
    FieldValue values[4];
    
    PayloadConfig config = {
        .num_fields = 4,
        .fields = fields,
        .values = values,
    };
    
    if(!get_payload_fields(pscreen, &config)) {
        return;
    }
    
    /* Extract values with appropriate types */
    uint8_t start_addr = (uint8_t)values[0].value;
    uint8_t retry_count = (uint8_t)values[1].value;
    uint16_t buffer_size = (uint16_t)values[2].value;
    uint32_t serial_num = values[3].value;
    
    /* Build and send your payload... */
}

// -----------------------------------------------------------------------------
// How to Integrate Into payload_menu.c
// -----------------------------------------------------------------------------

/*
 * In payload_menu.c, replace your send_payload_X() functions with these:
 * 
 * void send_payload_1(Ili9488Defines* pscreen) {
 *     send_payload_1_with_input(pscreen);
 * }
 * 
 * void send_payload_2(Ili9488Defines* pscreen) {
 *     send_payload_2_with_input(pscreen);
 * }
 * 
 * void send_payload_3(Ili9488Defines* pscreen) {
 *     send_payload_3_with_input(pscreen);
 * }
 * 
 * void send_payload_4(Ili9488Defines* pscreen) {
 *     send_payload_4_no_input(pscreen);  // This one doesn't ask for input
 * }
 */
