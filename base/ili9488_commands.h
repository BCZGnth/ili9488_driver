// -----------------------------------------------------------------------------
// ILI9488 Standard DCS Command Set (0x00–0x5F)
// -----------------------------------------------------------------------------
#ifndef ILI9488_COMMANDS_H
#define ILI9488_COMMANDS_H

#define ILI9488_NOP                          0x00
#define ILI9488_SOFTWARE_RESET               0x01

#define ILI9488_READ_DISPLAY_ID              0x04
#define ILI9488_READ_NUMBER_OF_ERRORS        0x05
#define ILI9488_READ_DISPLAY_STATUS          0x09
#define ILI9488_READ_DISPLAY_POWER_MODE      0x0A
#define ILI9488_READ_DISPLAY_MADCTL          0x0B
#define ILI9488_READ_DISPLAY_PIXEL_FORMAT    0x0C
#define ILI9488_READ_DISPLAY_IMAGE_MODE      0x0D
#define ILI9488_READ_DISPLAY_SIGNAL_MODE     0x0E
#define ILI9488_READ_SELF_DIAGNOSTIC         0x0F

#define ILI9488_SLEEP_IN                     0x10
#define ILI9488_SLEEP_OUT                    0x11
#define ILI9488_PARTIAL_MODE_ON              0x12
#define ILI9488_NORMAL_DISPLAY_MODE_ON       0x13

#define ILI9488_DISPLAY_INVERSION_OFF        0x20
#define ILI9488_DISPLAY_INVERSION_ON         0x21

// Added from screenshot 3:
#define ILI9488_ALL_PIXELS_OFF               0x22
#define ILI9488_ALL_PIXELS_ON                0x23

#define ILI9488_DISPLAY_OFF                  0x28
#define ILI9488_DISPLAY_ON                   0x29

#define ILI9488_COLUMN_ADDRESS_SET           0x2A
#define ILI9488_PAGE_ADDRESS_SET             0x2B
#define ILI9488_MEMORY_WRITE                 0x2C
#define ILI9488_COLOR_SET                    0x2D
#define ILI9488_MEMORY_READ                  0x2E

#define ILI9488_PARTIAL_AREA                 0x30
#define ILI9488_VERTICAL_SCROLL_DEFINITION   0x33

#define ILI9488_TEARING_EFFECT_LINE_OFF      0x34
#define ILI9488_TEARING_EFFECT_LINE_ON       0x35

#define ILI9488_MEMORY_ACCESS_CONTROL        0x36
#define ILI9488_VERTICAL_SCROLL_START_ADDR   0x37
#define ILI9488_IDLE_MODE_OFF                0x38
#define ILI9488_IDLE_MODE_ON                 0x39
#define ILI9488_INTERFACE_PIXEL_FORMAT       0x3A
#define ILI9488_MEMORY_WRITE_CONTINUE        0x3C
#define ILI9488_MEMORY_READ_CONTINUE         0x3E

#define ILI9488_WRITE_TEAR_SCANLINE          0x44
#define ILI9488_READ_SCANLINE                0x45

#define ILI9488_WRITE_DISPLAY_BRIGHTNESS     0x51
#define ILI9488_READ_DISPLAY_BRIGHTNESS      0x52
#define ILI9488_WRITE_CONTROL_DISPLAY        0x53
#define ILI9488_READ_CONTROL_DISPLAY         0x54
#define ILI9488_WRITE_CABC                   0x55
#define ILI9488_READ_CABC                    0x56

#define ILI9488_WRITE_CABC_MIN_BRIGHTNESS    0x5E
#define ILI9488_READ_CABC_MIN_BRIGHTNESS     0x5F

// Screenshot 1 extra:
#define ILI9488_READ_AUTOMATIC_BRIGHTNESS    0x68

// ID register reads
#define ILI9488_READ_ID1                     0xDA
#define ILI9488_READ_ID2                     0xDB
#define ILI9488_READ_ID3                     0xDC

// -----------------------------------------------------------------------------
// ILI9488 Extended Command Set (0xB0–0xFF)
// (From screenshots 1 & 2: sections 5.3.x, pages ~219–278)
// -----------------------------------------------------------------------------

#define ILI9488_INTERFACE_MODE_CONTROL       0xB0
#define ILI9488_FRAME_RATE_CONTROL_NORMAL    0xB1
#define ILI9488_FRAME_RATE_CONTROL_IDLE      0xB2
#define ILI9488_FRAME_RATE_CONTROL_PARTIAL   0xB3
#define ILI9488_DISPLAY_INVERSION_CONTROL    0xB4
#define ILI9488_BLANKING_PORCH_CONTROL       0xB5
#define ILI9488_DISPLAY_FUNCTION_CONTROL     0xB6
#define ILI9488_ENTRY_MODE_SET               0xB7
#define ILI9488_COLOR_ENHANCEMENT_1          0xB9
#define ILI9488_COLOR_ENHANCEMENT_2          0xBA
#define ILI9488_HS_LANES_CONTROL             0xBE

// Power control (screenshot 2)
#define ILI9488_POWER_CONTROL_1              0xC0
#define ILI9488_POWER_CONTROL_2              0xC1
#define ILI9488_POWER_CONTROL_3              0xC2
#define ILI9488_POWER_CONTROL_4              0xC3
#define ILI9488_POWER_CONTROL_5              0xC4
#define ILI9488_VCOM_CONTROL                 0xC5

// CABC control blocks
#define ILI9488_CABC_CONTROL_1               0xC6
#define ILI9488_CABC_CONTROL_2               0xC8
#define ILI9488_CABC_CONTROL_3               0xC9
#define ILI9488_CABC_CONTROL_4               0xCA
#define ILI9488_CABC_CONTROL_5               0xCB
#define ILI9488_CABC_CONTROL_6               0xCC
#define ILI9488_CABC_CONTROL_7               0xCD
#define ILI9488_CABC_CONTROL_8               0xCE
#define ILI9488_CABC_CONTROL_9               0xCF

// NV Memory & ID read
#define ILI9488_NV_MEMORY_WRITE              0xD0
#define ILI9488_NV_MEMORY_KEY                0xD1
#define ILI9488_NV_MEMORY_STATUS_READ        0xD2
#define ILI9488_READ_ID4                     0xD3

// Adjustment controls
#define ILI9488_ADJUST_CONTROL_1             0xD7
#define ILI9488_READ_ID_VERSION              0xD8

// Gamma controls
#define ILI9488_POSITIVE_GAMMA_CONTROL       0xE0
#define ILI9488_NEGATIVE_GAMMA_CONTROL       0xE1
#define ILI9488_DIGITAL_GAMMA_CONTROL_1      0xE2
#define ILI9488_DIGITAL_GAMMA_CONTROL_2      0xE3

#define ILI9488_SET_IMAGE_FUNCTION           0xE9

// Additional manufacturer commands (seen in datasheet)
#define ILI9488_ADJUST_CONTROL_2             0xF2
#define ILI9488_ADJUST_CONTROL_3             0xF7
#define ILI9488_ADJUST_CONTROL_4             0xF8
#define ILI9488_ADJUST_CONTROL_5             0xF9

// Aliases (common names)
#define ILI9488_RAMWR                        ILI9488_MEMORY_WRITE
#define ILI9488_RAMRD                        ILI9488_MEMORY_READ
#define ILI9488_RAMWR_CONTINUE               ILI9488_MEMORY_WRITE_CONTINUE
#define ILI9488_RAMRD_CONTINUE               ILI9488_MEMORY_READ_CONTINUE

typedef struct {
    unsigned memory_access_busy : 1; // Bit 0
    unsigned tear_scanline_flag : 1; // Bit 1
    unsigned idle_mode : 1;          // Bit 2
    unsigned partial_mode : 1;       // Bit 3
    unsigned sleep_out : 1;          // Bit 4
    unsigned idlemode : 1;           // Bit 5
    unsigned tear_effect_flag : 1;   // Bit 6
    unsigned reserved : 1;           // Bit 7
} ili9488_status_byte0_t;

typedef struct {
    uint8_t param1;   // Don't care (all X's)
    
    // Second Parameter (D[31:24])
    union {
        uint8_t raw;
        struct {
            unsigned reserved_24            : 1;  // D24 (bit 0)
            unsigned horizontal_refresh_order : 1; // D25 (bit 1): 0=Left to Right, 1=Right to Left
            unsigned rgb_bgr_order          : 1;  // D26 (bit 2): 0=RGB, 1=BGR
            unsigned vertical_refresh       : 1;  // D27 (bit 3): 0=Top to Bottom, 1=Bottom to Top
            unsigned row_column_exchange    : 1;  // D28 (bit 4): 0=Normal, 1=Reverse
            unsigned column_address_order   : 1;  // D29 (bit 5): 0=Left to Right, 1=Right to Left
            unsigned row_address_order      : 1;  // D30 (bit 6): 0=Top to Bottom, 1=Bottom to Top
            unsigned booster_voltage_status : 1;  // D31 (bit 7): 0=OFF, 1=ON
        } bits;
    } param2;
    
    // Third Parameter (D[23:16])
    union {
        uint8_t raw;
        struct {
            unsigned display_normal_mode    : 1;  // D16 (bit 0): 0=OFF, 1=ON
            unsigned sleep_mode             : 1;  // D17 (bit 1): 0=IN, 1=OUT
            unsigned partial_mode           : 1;  // D18 (bit 2): 0=OFF, 1=ON
            unsigned idle_mode              : 1;  // D19 (bit 3): 0=OFF, 1=ON
            unsigned interface_color_pixel  : 3;  // D[22:20] (bits 4-6): 101=16bit, 110=18bit, 111=24bit
            unsigned reserved_23            : 1;  // D23 (bit 7)
        } bits;
    } param3;
    
    // Fourth Parameter (D[15:8])
    union {
        uint8_t raw;
        struct {
            unsigned reserved_8             : 1;  // D8 (bit 0)
            unsigned tearing_effect_line_on : 1;  // D9 (bit 1): 0=OFF, 1=ON
            unsigned display_on             : 1;  // D10 (bit 2): 0=OFF, 1=ON
            unsigned reserved_11_12         : 2;  // D[12:11] (bits 3-4)
            unsigned inversion_status       : 1;  // D13 (bit 5): 0=OFF, 1=ON
            unsigned reserved_14            : 1;  // D14 (bit 6)
            unsigned vertical_scrolling     : 1;  // D15 (bit 7): 0=OFF, 1=ON
        } bits;
    } param4;
    
    // Fifth Parameter (D[7:0])
    union {
        uint8_t raw;
        struct {
            unsigned reserved_0_4           : 5;  // D[4:0] (bits 0-4)
            unsigned tearing_effect_mode    : 1;  // D5 (bit 5): 0=Mode 1 (V-Blanking), 1=Mode 2 (H&V-Blanking)
            unsigned reserved_6_7           : 2;  // D[7:6] (bits 6-7)
        } bits;
    } param5;
    
} ili9488_display_status_t;

typedef struct {
    unsigned booster_voltage_status : 1; // Booster on and working (1) : Booster off or has fault (0)
    unsigned idle_mode_on_off : 1; // On (1) : Off (0)
    unsigned partial_mode_on_off : 1; // On (1) : Off (0)
    unsigned sleep_in_out : 1; // Out (1) : In (0)
    unsigned display_normal_mode_on_off : 1; // On (1) : Off (0)
    unsigned display_on_off : 1; // On (1) : Off (0)
    unsigned reserved : 2;
} ili9488_power_mode_t;

typedef struct {
    unsigned refresh_order : 1;   // MY
    unsigned row_col_exchange : 1; // MX
    unsigned row_col_swap : 1;     // MV
    unsigned rgb_bgr_order : 1;    // ML
    unsigned vertical_refresh : 1; // BGR
    unsigned horiz_refresh : 1;    // MH
    unsigned reserved : 2;         // Reserved bits
} ili9488_madctl_t;

typedef struct {
    unsigned reserved;

    struct {
        unsigned reserved1 : 1;
        unsigned dpi : 3;
        unsigned reserved2 : 1;
        unsigned dsi : 3;
    } color_depth;  // Typically 0x55 = 16-bit, 0x66 = 18-bit

} ili9488_pixel_format_t;

typedef struct {
    unsigned reserved : 4;
    unsigned invert_mode : 1;  // 0 = Normal, 1 = Inverted
    unsigned reserved2 : 3;
} ili9488_image_mode_t;


// Send command to ILI9488 (received value is ignored)
void ILI9488_SendCommand(ili9488_interface_t interface, unsigned char cmd, uint8_t* pdata, size_t data_length);

// Send data to ILI9488 (received value is ignored)
void ILI9488_SendData(ili9488_interface_t interface, uint8_t * data, size_t len);

// Send single byte to ILI9488 (received value is ignored)
void ILI9488_SendByte(ili9488_interface_t interface, uint8_t data);

// Send/receive data to/from ILI9488
void ILI9488_TransferData(ili9488_interface_t interface, uint8_t* exchange_data, size_t len);

void ILI9488_ReadData(ili9488_interface_t interface, uint8_t* data_from_screen, size_t len);

#endif