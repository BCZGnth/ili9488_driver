#ifndef GLCD_ILI9488_COMMANDS
#define GLCD_ILI9488_COMMANDS

#define ILI9488_NOP                          0x00
#define ILI9488_SOFTWARE_RESET               0x01

#define ILI9488_READ_DISPLAY_ID              0x04
#define ILI9488_READ_NUMBER_OF_ERRORS        0x05
#define ILI9488_READ_DISPLAY_STATUS          0x09
#define ILI9488_READ_DISPLAY_POWER_MODE      0x0A
#define ILI9488_READ_MADCTL                  0x0B
#define ILI9488_READ_PIXEL_FORMAT            0x0C
#define ILI9488_READ_IMAGE_MODE              0x0D
#define ILI9488_READ_SIGNAL_MODE             0x0E
#define ILI9488_READ_SELF_DIAGNOSTIC         0x0F

#define ILI9488_SLEEP_IN                     0x10
#define ILI9488_SLEEP_OUT                    0x11
#define ILI9488_PARTIAL_MODE_ON              0x12
#define ILI9488_NORMAL_DISPLAY_MODE_ON       0x13

#define ILI9488_DISPLAY_INVERSION_OFF        0x20
#define ILI9488_DISPLAY_INVERSION_ON         0x21

#define ILI9488_DISPLAY_OFF                  0x28
#define ILI9488_DISPLAY_ON                   0x29

#define ILI9488_COLUMN_ADDRESS_SET           0x2A
#define ILI9488_PAGE_ADDRESS_SET             0x2B
#define ILI9488_MEMORY_WRITE                 0x2C
#define ILI9488_COLOR_SET                    0x2D
#define ILI9488_MEMORY_READ                  0x2E

#define ILI9488_PARTIAL_AREA                 0x30
#define ILI9488_VERTICAL_SCROLLING_DEF       0x33

#define ILI9488_TEARING_EFFECT_LINE_OFF      0x34
#define ILI9488_TEARING_EFFECT_LINE_ON       0x35

#define ILI9488_MEMORY_ACCESS_CONTROL        0x36
#define ILI9488_VERTICAL_SCROLL_START_ADDR   0x37
#define ILI9488_IDLE_MODE_OFF                0x38
#define ILI9488_IDLE_MODE_ON                 0x39

#define ILI9488_PIXEL_FORMAT_SET             0x3A
#define ILI9488_WRITE_MEMORY_CONTINUE        0x3C
#define ILI9488_READ_MEMORY_CONTINUE         0x3E

#define ILI9488_SET_TEAR_SCANLINE            0x44
#define ILI9488_GET_SCANLINE                 0x45

#define ILI9488_WRITE_DISPLAY_BRIGHTNESS     0x51
#define ILI9488_READ_DISPLAY_BRIGHTNESS      0x52
#define ILI9488_WRITE_CTRL_DISPLAY           0x53
#define ILI9488_READ_CTRL_DISPLAY            0x54
#define ILI9488_WRITE_CABC                   0x55
#define ILI9488_READ_CABC                    0x56

#define ILI9488_WRITE_CABC_MIN_BRIGHTNESS    0x5E
#define ILI9488_READ_CABC_MIN_BRIGHTNESS     0x5F

// --- Manufacturer / Extended Commands ---
#define ILI9488_RGB_INTERFACE_CONTROL        0xB0
#define ILI9488_FRAME_RATE_CONTROL           0xB1
#define ILI9488_DISPLAY_INVERSION_CONTROL    0xB4
#define ILI9488_DISPLAY_FUNCTION_CONTROL     0xB6
#define ILI9488_ENTRY_MODE_SET               0xB7

#define ILI9488_BACKLIGHT_CONTROL_1          0xB8
#define ILI9488_BACKLIGHT_CONTROL_2          0xB9
#define ILI9488_BACKLIGHT_CONTROL_3          0xBA
#define ILI9488_BACKLIGHT_CONTROL_4          0xBB
#define ILI9488_BACKLIGHT_CONTROL_5          0xBC
#define ILI9488_BACKLIGHT_CONTROL_7          0xBE

#define ILI9488_POWER_CONTROL_1              0xC0
#define ILI9488_POWER_CONTROL_2              0xC1
#define ILI9488_POWER_CONTROL_3              0xC2
#define ILI9488_POWER_CONTROL_4              0xC3
#define ILI9488_POWER_CONTROL_5              0xC4
#define ILI9488_VCOM_CONTROL                 0xC5

#define ILI9488_NV_MEMORY_WRITE              0xD0
#define ILI9488_NV_MEMORY_KEY                0xD1
#define ILI9488_READ_ID4                     0xD3

#define ILI9488_POSITIVE_GAMMA_CONTROL       0xE0
#define ILI9488_NEGATIVE_GAMMA_CONTROL       0xE1

// Manufacturer reserved commands (present on page 144)
#define ILI9488_MANUFACTURER_CMD_1           0xE2
#define ILI9488_MANUFACTURER_CMD_2           0xE3
#define ILI9488_MANUFACTURER_CMD_3           0xE4
#define ILI9488_MANUFACTURER_CMD_4           0xE5
#define ILI9488_MANUFACTURER_CMD_5           0xE6
#define ILI9488_MANUFACTURER_CMD_6           0xE7
#define ILI9488_MANUFACTURER_CMD_7           0xE8
#define ILI9488_MANUFACTURER_CMD_8           0xE9


#endif