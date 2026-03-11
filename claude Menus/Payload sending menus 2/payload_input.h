/**
 * @file payload_input.h
 * @brief Generic field input system for payload configuration
 * 
 * This allows you to prompt the user to input values for specific
 * payload fields using a numpad interface (following your existing pattern)
 */

#ifndef _PAYLOAD_INPUT_H
#define _PAYLOAD_INPUT_H

#include "ili9488_base.h"
#include "xpt2046.h"

// -----------------------------------------------------------------------------
// Field Input Configuration
// -----------------------------------------------------------------------------

/**
 * Field types that can be input
 */
typedef enum {
    FIELD_TYPE_UINT8,       // 0-255
    FIELD_TYPE_UINT16,      // 0-65535
    FIELD_TYPE_UINT32,      // 0-4294967295
    FIELD_TYPE_HEX_BYTE,    // 0x00-0xFF (displayed as hex)
} FieldType;

/**
 * Field definition structure
 */
typedef struct {
    const char* label;      // Field name (e.g., "Device ID:", "Command:")
    FieldType type;         // Type of field
    uint32_t min_value;     // Minimum allowed value
    uint32_t max_value;     // Maximum allowed value
    uint32_t default_value; // Default value
} FieldDefinition;

/**
 * Field value storage
 */
typedef struct {
    uint32_t value;         // Current value
    uint8_t is_set;         // Whether user has entered a value
} FieldValue;

/**
 * Complete payload configuration
 * 
 * Example:
 * PayloadConfig my_payload = {
 *     .num_fields = 3,
 *     .fields = (FieldDefinition[]){
 *         {"Device ID:", FIELD_TYPE_UINT8, 1, 255, 1},
 *         {"Command:", FIELD_TYPE_HEX_BYTE, 0, 255, 0x01},
 *         {"Data Length:", FIELD_TYPE_UINT16, 1, 1024, 8},
 *     },
 * };
 */
typedef struct {
    uint8_t num_fields;             // Number of fields to input
    const FieldDefinition* fields;  // Array of field definitions
    FieldValue* values;             // Array to store values (caller provides)
} PayloadConfig;

// -----------------------------------------------------------------------------
// Input Functions
// -----------------------------------------------------------------------------

/**
 * @brief Prompt user to input all fields for a payload
 * 
 * This function displays a numpad and prompts the user to enter
 * values for each field defined in the configuration.
 * 
 * @param pscreen Pointer to screen structure
 * @param config Pointer to payload configuration
 * @return true if user completed all fields, false if cancelled
 * 
 * Example usage:
 * ```c
 * FieldDefinition fields[] = {
 *     {"Device ID:", FIELD_TYPE_UINT8, 1, 255, 1},
 *     {"Command:", FIELD_TYPE_HEX_BYTE, 0, 255, 0x01},
 * };
 * FieldValue values[2];
 * 
 * PayloadConfig config = {
 *     .num_fields = 2,
 *     .fields = fields,
 *     .values = values,
 * };
 * 
 * if(get_payload_fields(&screen, &config)) {
 *     // User completed input
 *     uint8_t device_id = values[0].value;
 *     uint8_t command = values[1].value;
 *     // Send payload...
 * }
 * ```
 */
bool get_payload_fields(Ili9488Defines* pscreen, PayloadConfig* config);

/**
 * @brief Prompt user to input a single field value
 * 
 * @param pscreen Pointer to screen structure
 * @param field_def Field definition
 * @param current_value Current value (will be updated)
 * @return true if user entered a value, false if cancelled
 */
bool get_single_field(Ili9488Defines* pscreen, 
                      const FieldDefinition* field_def,
                      uint32_t* current_value);

/**
 * @brief Draw the numpad input screen
 * 
 * @param pscreen Pointer to screen structure
 * @param field_def Current field being edited
 * @param current_value Current value
 */
void draw_field_input_screen(Ili9488Defines* pscreen,
                              const FieldDefinition* field_def,
                              uint32_t current_value);

/**
 * @brief Process touch events for numpad input
 * 
 * @param pscreen Pointer to screen structure
 * @param field_def Field definition (for validation)
 * @param current_value Pointer to current value (updated on input)
 * @param cancelled Set to true if user cancels
 * @return true to break input loop (either completed or cancelled)
 */
bool process_field_input_touch(Ili9488Defines* pscreen,
                                const FieldDefinition* field_def,
                                uint32_t* current_value,
                                bool* cancelled);

// -----------------------------------------------------------------------------
// Numpad Touch Boxes (same as your add menu pattern)
// -----------------------------------------------------------------------------

enum FieldInputKeys {
    INPUT_BUTTON_ZERO,
    INPUT_BUTTON_ONE,
    INPUT_BUTTON_TWO,
    INPUT_BUTTON_THREE,
    INPUT_BUTTON_FOUR,
    INPUT_BUTTON_FIVE,
    INPUT_BUTTON_SIX,
    INPUT_BUTTON_SEVEN,
    INPUT_BUTTON_EIGHT,
    INPUT_BUTTON_NINE,
    INPUT_BUTTON_DELETE,
    INPUT_BUTTON_OK,
    INPUT_BUTTON_CANCEL,
};

/**
 * Touch boxes for numpad input
 * Layout identical to your add menu numpad
 */
static const TouchBox input_touch_boxes[] = {
    /* #1 */
    { .key = INPUT_BUTTON_ONE,    .xs = 9,   .ys = 69,  .xe = 110, .ye = 170 },
    /* 2 */
    { .key = INPUT_BUTTON_TWO,    .xs = 111, .ys = 69,  .xe = 212, .ye = 170 },
    /* 3 */
    { .key = INPUT_BUTTON_THREE,  .xs = 213, .ys = 69,  .xe = 314, .ye = 170 },
    /* #4 */
    { .key = INPUT_BUTTON_FOUR,   .xs = 9,   .ys = 171, .xe = 110, .ye = 272 },
    /* 5 */
    { .key = INPUT_BUTTON_FIVE,   .xs = 111, .ys = 171, .xe = 212, .ye = 272 },
    /* 6 */
    { .key = INPUT_BUTTON_SIX,    .xs = 213, .ys = 171, .xe = 314, .ye = 272 },
    /* #7 */
    { .key = INPUT_BUTTON_SEVEN,  .xs = 9,   .ys = 273, .xe = 110, .ye = 374 },
    /* 8 */
    { .key = INPUT_BUTTON_EIGHT,  .xs = 111, .ys = 273, .xe = 212, .ye = 374 },
    /* 9 */
    { .key = INPUT_BUTTON_NINE,   .xs = 213, .ys = 273, .xe = 314, .ye = 374 },
    /* Delete */
    { .key = INPUT_BUTTON_DELETE, .xs = 9,   .ys = 375, .xe = 110, .ye = 476 },
    /* 0 */
    { .key = INPUT_BUTTON_ZERO,   .xs = 111, .ys = 375, .xe = 212, .ye = 476 },
    /* OK */
    { .key = INPUT_BUTTON_OK,     .xs = 213, .ys = 375, .xe = 314, .ye = 476 },
};

// Add cancel button at top
static const TouchBox cancel_button_box = {
    .key = INPUT_BUTTON_CANCEL,
    .xs = 10,
    .ys = 10,
    .xe = 50,
    .ye = 50,
};

// -----------------------------------------------------------------------------
// Helper Functions
// -----------------------------------------------------------------------------

/**
 * @brief Validate if a value is within field constraints
 * 
 * @param value Value to check
 * @param field_def Field definition with min/max
 * @return true if valid, false if out of range
 */
bool validate_field_value(uint32_t value, const FieldDefinition* field_def);

/**
 * @brief Format a field value for display
 * 
 * Handles hex vs decimal formatting based on field type
 * 
 * @param value Value to format
 * @param field_def Field definition
 * @param buffer Output buffer
 * @param buffer_size Size of buffer
 */
void format_field_value(uint32_t value, 
                        const FieldDefinition* field_def,
                        char* buffer, 
                        uint8_t buffer_size);

#endif // _PAYLOAD_INPUT_H
