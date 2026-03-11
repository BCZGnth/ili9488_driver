# Field Input System for Payload Configuration

## Overview

This system allows you to **prompt the user to input specific values** for your payloads using a numpad interface (same style as your `debug_draw_add_menu()`).

Instead of hardcoding payload values, you define which fields you want the user to configure, and the system automatically:
- ✅ Shows a numpad for each field
- ✅ Validates input against min/max values
- ✅ Displays field labels and current values
- ✅ Handles hex or decimal display
- ✅ Allows user to cancel

## Files Added

1. **payload_input.h** - Field input system API
2. **payload_input.c** - Implementation
3. **payload_examples.c** - Complete usage examples

## Quick Start

### Define Your Fields

```c
FieldDefinition fields[] = {
    {
        .label = "Device ID:",           // What to show user
        .type = FIELD_TYPE_UINT8,        // Type of field
        .min_value = 1,                  // Minimum allowed
        .max_value = 255,                // Maximum allowed
        .default_value = 1,              // Starting value
    },
    {
        .label = "Command:",
        .type = FIELD_TYPE_HEX_BYTE,     // Shows as 0xXX
        .min_value = 0,
        .max_value = 255,
        .default_value = 0x01,
    },
    {
        .label = "Data Length:",
        .type = FIELD_TYPE_UINT16,       // Up to 65535
        .min_value = 1,
        .max_value = 1024,
        .default_value = 8,
    },
};
```

### Get User Input

```c
/* Storage for values */
FieldValue values[3];

/* Configuration */
PayloadConfig config = {
    .num_fields = 3,
    .fields = fields,
    .values = values,
};

/* Prompt user for input */
if(get_payload_fields(&screen, &config)) {
    /* User completed - extract values */
    uint8_t device_id = (uint8_t)values[0].value;
    uint8_t command = (uint8_t)values[1].value;
    uint16_t data_length = (uint16_t)values[2].value;
    
    /* Send your payload with these values */
} else {
    /* User cancelled - return to menu */
}
```

## Field Types

### `FIELD_TYPE_UINT8`
- Range: 0-255
- Display: Decimal (e.g., "123")
- Use for: Device IDs, counts, small numbers

### `FIELD_TYPE_UINT16`
- Range: 0-65535
- Display: Decimal (e.g., "1024")
- Use for: Buffer sizes, timeouts, lengths

### `FIELD_TYPE_UINT32`
- Range: 0-4294967295
- Display: Decimal (e.g., "1234567")
- Use for: Serial numbers, timestamps, large values

### `FIELD_TYPE_HEX_BYTE`
- Range: 0x00-0xFF
- Display: Hexadecimal (e.g., "0xA5")
- Use for: Addresses, command bytes, registers

## User Interface

### Input Screen Layout

```
+----------------------------------+
| Device ID:                    [X]| <- Cancel button (red X)
| Current Value: 123               | <- Field label and value
|----------------------------------|
| 1 | 2 | 3 |                      |
|---|---|---|                      |
| 4 | 5 | 6 |    Numpad            |
|---|---|---|    (same as your     |
| 7 | 8 | 9 |     add menu)        |
|---|---|---|                      |
| x | 0 | OK|                      |
+----------------------------------+
```

### Button Functions

- **0-9**: Enter digits
- **x (Delete)**: Delete last digit
- **OK**: Confirm and move to next field
- **X (Cancel)**: Cancel entire input sequence

### User Flow

1. User touches "Send Payload 1"
2. System shows input screen for first field ("Device ID:")
3. User enters value using numpad
4. User presses OK
5. System validates (checks min/max)
6. If valid, shows next field
7. Repeat for all fields
8. When all fields complete, send payload
9. If user presses Cancel, return to menu

## Integration Examples

### Example 1: Simple 3-Field Payload

```c
void send_payload_1(Ili9488Defines* pscreen) {
    
    /* Define fields */
    FieldDefinition fields[] = {
        {"Device ID:", FIELD_TYPE_UINT8, 1, 255, 1},
        {"Command:", FIELD_TYPE_HEX_BYTE, 0, 255, 0x01},
        {"Length:", FIELD_TYPE_UINT16, 1, 1024, 8},
    };
    
    FieldValue values[3];
    
    PayloadConfig config = {
        .num_fields = 3,
        .fields = fields,
        .values = values,
    };
    
    /* Get input */
    if(!get_payload_fields(pscreen, &config)) {
        return;  /* Cancelled */
    }
    
    /* Extract values */
    uint8_t device_id = values[0].value;
    uint8_t command = values[1].value;
    uint16_t length = values[2].value;
    
    /* Build payload */
    uint8_t payload[] = {
        0xAA, 0xBB,              // Header
        device_id,               // From user input
        command,                 // From user input
        (length >> 8) & 0xFF,    // Length high byte
        length & 0xFF,           // Length low byte
    };
    
    /* Send it */
    UART1_WriteBuffer(payload, sizeof(payload));
    
    /* Show completion screen */
    // ...
}
```

### Example 2: Single Field

```c
void send_payload_simple(Ili9488Defines* pscreen) {
    
    /* Just one field - a command byte */
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
    
    /* Send single byte */
    UART1_Write((uint8_t)values[0].value);
}
```

### Example 3: Many Fields

```c
void send_payload_complex(Ili9488Defines* pscreen) {
    
    FieldDefinition fields[] = {
        {"Target:", FIELD_TYPE_HEX_BYTE, 0x01, 0xFF, 0x10},
        {"Cmd:", FIELD_TYPE_HEX_BYTE, 0x00, 0xFF, 0x20},
        {"Param1:", FIELD_TYPE_UINT8, 0, 255, 100},
        {"Param2:", FIELD_TYPE_UINT8, 0, 255, 50},
        {"Timeout:", FIELD_TYPE_UINT16, 10, 5000, 1000},
        {"Serial:", FIELD_TYPE_UINT32, 1000000, 9999999, 1234567},
    };
    
    FieldValue values[6];
    
    PayloadConfig config = {
        .num_fields = 6,
        .fields = fields,
        .values = values,
    };
    
    if(!get_payload_fields(pscreen, &config)) {
        return;
    }
    
    /* Build complex payload from all 6 values */
    // ...
}
```

### Example 4: No Input (Fixed Payload)

```c
void send_payload_fixed(Ili9488Defines* pscreen) {
    
    /* This payload doesn't need user input */
    /* Just send a fixed command */
    
    uint8_t payload[] = {0xAA, 0xBB, 0x04, 0xFF};
    UART1_WriteBuffer(payload, sizeof(payload));
    
    /* Show "Sent!" message */
    // ...
}
```

## Validation

The system automatically validates:

### Range Checking
```c
FieldDefinition field = {
    .label = "Retry Count:",
    .type = FIELD_TYPE_UINT8,
    .min_value = 1,      // Can't be less than 1
    .max_value = 10,     // Can't be more than 10
    .default_value = 3,
};

/* If user tries to enter 0 or 11+, system shows error */
```

### Overflow Prevention
```c
/* User can't enter values that would overflow the field type */
/* For UINT8: max is 255 */
/* For UINT16: max is 65535 */
/* For UINT32: max is 4294967295 */
```

### Error Display
```c
/* If value is out of range: */
"Value out of range!"  // Shown in red for 1 second

/* If max value would be exceeded: */
"Max exceeded!"        // Shown in red for 0.5 seconds
```

## Customization

### Change Colors

Edit `payload_input.c`:

```c
/* Field label color */
Ili9488Print label = {
    .fg = YELLOW,  // ← Change this
    // ...
};

/* Value display color */
Ili9488Print value_display = {
    .fg = GREEN,   // ← Change this
    // ...
};

/* Button color */
Ili9488Rect rect = {
    .color = MAGENTA,  // ← Change this
    // ...
};
```

### Adjust Touch Boxes

Touch boxes are identical to your add menu. If you need to adjust:

Edit `payload_input.h`:
```c
static const TouchBox input_touch_boxes[] = {
    { .key = INPUT_BUTTON_ONE, .xs = 9, .ys = 69, .xe = 110, .ye = 170 },
    // Adjust xs, ys, xe, ye values
    // ...
};
```

### Add Hex Display for Other Types

Currently only `FIELD_TYPE_HEX_BYTE` shows hex. To add hex display for uint16:

Edit `format_field_value()` in `payload_input.c`:

```c
if(field_def->type == FIELD_TYPE_HEX_BYTE || 
   field_def->type == FIELD_TYPE_HEX_WORD) {  // New type
    // Format as hex
    // ...
}
```

## Complete Workflow Example

### Payload 1: Device Configuration

```c
void send_payload_1(Ili9488Defines* pscreen) {
    
    /* Step 1: Define what you want to configure */
    FieldDefinition fields[] = {
        {"Device ID:", FIELD_TYPE_UINT8, 1, 32, 1},
        {"Mode:", FIELD_TYPE_HEX_BYTE, 0x00, 0x03, 0x01},
        {"Rate (Hz):", FIELD_TYPE_UINT16, 1, 10000, 100},
    };
    
    FieldValue values[3];
    PayloadConfig config = {
        .num_fields = 3,
        .fields = fields,
        .values = values,
    };
    
    /* Step 2: Get user input */
    if(!get_payload_fields(pscreen, &config)) {
        return;  /* User cancelled */
    }
    
    /* Step 3: Extract values */
    uint8_t device_id = (uint8_t)values[0].value;
    uint8_t mode = (uint8_t)values[1].value;
    uint16_t rate = (uint16_t)values[2].value;
    
    /* Step 4: Build payload packet */
    typedef struct {
        uint8_t header[2];
        uint8_t cmd;
        uint8_t device_id;
        uint8_t mode;
        uint16_t rate;
        uint8_t checksum;
    } __attribute__((packed)) ConfigPacket;
    
    ConfigPacket packet = {
        .header = {0xAA, 0xBB},
        .cmd = 0x10,  // Config command
        .device_id = device_id,
        .mode = mode,
        .rate = rate,
        .checksum = 0,
    };
    
    /* Step 5: Calculate checksum */
    uint8_t* bytes = (uint8_t*)&packet;
    for(uint8_t i = 0; i < sizeof(ConfigPacket) - 1; i++) {
        packet.checksum ^= bytes[i];
    }
    
    /* Step 6: Show sending status */
    ili9488_cls(pscreen);
    Ili9488Print status = {
        .text = "CONFIGURING DEVICE",
        .fg = GREEN,
        .font = pscreen->Screen.offset_2x,
        .ram_ptr = {.start_x = 10, .start_y = 20, .end_x = 30, .end_y = 300},
    };
    ili9488_print(pscreen, status);
    
    /* Step 7: Send via UART */
    UART1_WriteBuffer(bytes, sizeof(ConfigPacket));
    
    /* Step 8: Wait for completion */
    while(!UART1_IsTxDone()) {
        ili9488_loading_bar(pscreen);
        __delay_ms(100);
    }
    
    /* Step 9: Show completion */
    Ili9488Print done = {
        .text = "COMPLETE!",
        .fg = GREEN,
        .font = pscreen->Screen.offset_2x,
        .ram_ptr = {.start_x = 150, .start_y = 100, .end_x = 170, .end_y = 200},
    };
    ili9488_print(pscreen, done);
    __delay_ms(1500);
}
```

## Tips and Best Practices

### 1. Field Order
Put most important/commonly changed fields first:
```c
FieldDefinition fields[] = {
    {"Command:", ...},      // Most important - shown first
    {"Length:", ...},       // Less important
    {"Timeout:", ...},      // Least important - shown last
};
```

### 2. Good Default Values
Set defaults to most common values:
```c
{
    .label = "Retry Count:",
    .default_value = 3,  // Most common case
    // User can just press OK if 3 is what they want
}
```

### 3. Reasonable Ranges
Don't make ranges too wide:
```c
/* Bad - user has to enter 6 digits */
{"Count:", FIELD_TYPE_UINT32, 0, 999999, 100}

/* Good - practical range */
{"Count:", FIELD_TYPE_UINT8, 1, 20, 5}
```

### 4. Clear Labels
Keep labels short but descriptive:
```c
/* Good */
"Device ID:"
"Cmd Byte:"
"Rate (Hz):"

/* Too long */
"Please enter the device identification number:"
```

### 5. Use Hex for Commands/Addresses
```c
/* Commands and addresses are easier to read in hex */
{"Cmd:", FIELD_TYPE_HEX_BYTE, ...}    // Shows as 0xA5
{"Addr:", FIELD_TYPE_HEX_BYTE, ...}   // Shows as 0x80

/* Counts and sizes better in decimal */
{"Count:", FIELD_TYPE_UINT8, ...}     // Shows as 42
{"Size:", FIELD_TYPE_UINT16, ...}     // Shows as 1024
```

## Memory Usage

| Component | RAM | Flash |
|-----------|-----|-------|
| Field definitions (ROM) | 0 | ~12 bytes per field |
| Field values (RAM) | 5 bytes per field | 0 |
| Input code | ~50 bytes | ~2.5KB |
| **Total (3 fields)** | **~65 bytes** | **~2.5KB** |

Very lightweight!

## Next Steps

1. ✅ Add `payload_input.h` and `payload_input.c` to your project
2. ✅ Look at `payload_examples.c` for usage patterns
3. ✅ Replace your `send_payload_X()` functions with versions that use field input
4. ✅ Define your field configurations based on what values you need
5. ✅ Test and adjust min/max values as needed

That's it! Your users can now configure payloads interactively instead of hardcoded values.
