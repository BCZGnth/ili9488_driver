// -----------------------------------------------------------------------------
// ILI9488 Command Definitions - Optimized for Flash Size
// -----------------------------------------------------------------------------
// Using macros instead of const variables saves ~200-400 bytes of flash
// The upper byte contains parameter count, lower byte is the command
// -----------------------------------------------------------------------------

#ifndef ILI9488_COMMANDS_H_OPTIMIZED
#define ILI9488_COMMANDS_H_OPTIMIZED

#include "ili9488_base.h"
#include "system.h"

// -----------------------------------------------------------------------------
// ILI9488 Standard DCS Command Set (0x00–0x5F)
// Format: 0xLLCC where LL = parameter length, CC = command byte
// -----------------------------------------------------------------------------

#define ILI9488_NOP                          0x0000
#define ILI9488_SOFTWARE_RESET               0x0001

#define ILI9488_READ_DISPLAY_ID              0x0404
#define ILI9488_READ_NUMBER_OF_ERRORS        0x0305
#define ILI9488_READ_DISPLAY_STATUS          0x0509
#define ILI9488_READ_DISPLAY_POWER_MODE      0x020A
#define ILI9488_READ_DISPLAY_MADCTL          0x020B
#define ILI9488_READ_DISPLAY_PIXEL_FORMAT    0x020C
#define ILI9488_READ_DISPLAY_IMAGE_MODE      0x020D
#define ILI9488_READ_DISPLAY_SIGNAL_MODE     0x020E
#define ILI9488_READ_SELF_DIAGNOSTIC         0x020F

#define ILI9488_SLEEP_IN                     0x0010
#define ILI9488_SLEEP_OUT                    0x0011
#define ILI9488_PARTIAL_MODE_ON              0x0012
#define ILI9488_NORMAL_DISPLAY_MODE_ON       0x0013

#define ILI9488_DISPLAY_INVERSION_OFF        0x0020
#define ILI9488_DISPLAY_INVERSION_ON         0x0021
#define ILI9488_ALL_PIXELS_OFF               0x0022
#define ILI9488_ALL_PIXELS_ON                0x0023

#define ILI9488_DISPLAY_OFF                  0x0028
#define ILI9488_DISPLAY_ON                   0x0029

#define ILI9488_COLUMN_ADDRESS_SET           0x042A
#define ILI9488_PAGE_ADDRESS_SET             0x042B
#define ILI9488_MEMORY_WRITE                 0x032C
#define ILI9488_MEMORY_READ                  0x042E

#define ILI9488_PARTIAL_AREA                 0x0430
#define ILI9488_VERTICAL_SCROLL_DEFINITION   0x0633

#define ILI9488_TEARING_EFFECT_LINE_OFF      0x0034
#define ILI9488_TEARING_EFFECT_LINE_ON       0x0135

#define ILI9488_MEMORY_ACCESS_CONTROL        0x0136
#define ILI9488_VERTICAL_SCROLL_START_ADDR   0x0237
#define ILI9488_IDLE_MODE_OFF                0x0038
#define ILI9488_IDLE_MODE_ON                 0x0039
#define ILI9488_INTERFACE_PIXEL_FORMAT       0x013A
#define ILI9488_MEMORY_WRITE_CONTINUE        0x033C
#define ILI9488_MEMORY_READ_CONTINUE         0x043E

#define ILI9488_WRITE_TEAR_SCANLINE          0x0244
#define ILI9488_READ_SCANLINE                0x0345

#define ILI9488_WRITE_DISPLAY_BRIGHTNESS     0x0151
#define ILI9488_READ_DISPLAY_BRIGHTNESS      0x0252
#define ILI9488_WRITE_CONTROL_DISPLAY        0x0153
#define ILI9488_READ_CONTROL_DISPLAY         0x0254
#define ILI9488_WRITE_CABC                   0x0155
#define ILI9488_READ_CABC                    0x0256

#define ILI9488_WRITE_CABC_MIN_BRIGHTNESS    0x015E
#define ILI9488_READ_CABC_MIN_BRIGHTNESS     0x025F

#define ILI9488_READ_AUTOMATIC_BRIGHTNESS    0x0268

#define ILI9488_READ_ID1                     0x02DA
#define ILI9488_READ_ID2                     0x02DB
#define ILI9488_READ_ID3                     0x02DC

// -----------------------------------------------------------------------------
// ILI9488 Extended Command Set (0xB0–0xFF)
// -----------------------------------------------------------------------------

#define ILI9488_INTERFACE_MODE_CONTROL       0x01B0
#define ILI9488_FRAME_RATE_CONTROL_NORMAL    0x02B1
#define ILI9488_FRAME_RATE_CONTROL_IDLE      0x02B2
#define ILI9488_FRAME_RATE_CONTROL_PARTIAL   0x02B3
#define ILI9488_DISPLAY_INVERSION_CONTROL    0x01B4
#define ILI9488_BLANKING_PORCH_CONTROL       0x04B5
#define ILI9488_DISPLAY_FUNCTION_CONTROL     0x03B6
#define ILI9488_ENTRY_MODE_SET               0x01B7
#define ILI9488_COLOR_ENHANCEMENT_1          0x0CB9
#define ILI9488_COLOR_ENHANCEMENT_2          0x0CBA
#define ILI9488_HS_LANES_CONTROL             0x02BE

// Power control
#define ILI9488_POWER_CONTROL_1              0x02C0
#define ILI9488_POWER_CONTROL_2              0x01C1
#define ILI9488_POWER_CONTROL_3              0x01C2
#define ILI9488_POWER_CONTROL_4              0x01C3
#define ILI9488_POWER_CONTROL_5              0x01C4
#define ILI9488_VCOM_CONTROL                 0x04C5

// CABC control blocks
#define ILI9488_CABC_CONTROL_1               0x02C6
#define ILI9488_CABC_CONTROL_2               0x01C8
#define ILI9488_CABC_CONTROL_3               0x01C9
#define ILI9488_CABC_CONTROL_4               0x01CA
#define ILI9488_CABC_CONTROL_5               0x01CB
#define ILI9488_CABC_CONTROL_6               0x01CC
#define ILI9488_CABC_CONTROL_7               0x01CD
#define ILI9488_CABC_CONTROL_8               0x01CE
#define ILI9488_CABC_CONTROL_9               0x01CF

// NV Memory & ID read
#define ILI9488_NV_MEMORY_WRITE              0x02D0
#define ILI9488_NV_MEMORY_KEY                0x03D1
#define ILI9488_NV_MEMORY_STATUS_READ        0x03D2
#define ILI9488_READ_ID4                     0x04D3

// Adjustment controls
#define ILI9488_ADJUST_CONTROL_1             0x01D7
#define ILI9488_READ_ID_VERSION              0x02D8

// Gamma controls
#define ILI9488_POSITIVE_GAMMA_CONTROL       0x0EE0
#define ILI9488_NEGATIVE_GAMMA_CONTROL       0x0EE1
#define ILI9488_DIGITAL_GAMMA_CONTROL_1      0x03E2
#define ILI9488_DIGITAL_GAMMA_CONTROL_2      0x03E3

#define ILI9488_SET_IMAGE_FUNCTION           0x01E9

// Additional manufacturer commands
#define ILI9488_ADJUST_CONTROL_2             0x0CF2
#define ILI9488_ADJUST_CONTROL_3             0x04F7
#define ILI9488_ADJUST_CONTROL_4             0x02F8
#define ILI9488_ADJUST_CONTROL_5             0x01F9
#define ILI9488_ADJUST_CONTROL_6             0x02FC
#define ILI9488_ADJUST_CONTROL_7             0x02FF

// Aliases (common names)
#define ILI9488_RAMWR                        ILI9488_MEMORY_WRITE
#define ILI9488_RAMRD                        ILI9488_MEMORY_READ
#define ILI9488_RAMWR_CONTINUE               ILI9488_MEMORY_WRITE_CONTINUE
#define ILI9488_RAMRD_CONTINUE               ILI9488_MEMORY_READ_CONTINUE


// Send command to ILI9488 (received value is ignored)
// void ili9488_send_command(uint16_t cmd, ...);

// Send data to ILI9488 (received value is ignored)
// void ili9488_send_data(uint8_t* pdata, size_t data_length);
void ili9488_0byte_command(uint16_t cmd);
void ili9488_1byte_command(uint16_t cmd, uint8_t b1);
void ili9488_2byte_command(uint16_t cmd, uint8_t b1, uint8_t b2);
void ili9488_3byte_command(uint16_t cmd, uint8_t b1, uint8_t b2, uint8_t b3);
void ili9488_4byte_command(uint16_t cmd, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);

// Send single byte to ILI9488 (received value is ignored)
void ili9488_send_byte(uint8_t data);

// Send/receive data to/from ILI9488
void ili9488_transfer_data(uint8_t* tx_data, uint8_t* rx_data, size_t len);

void ili9488_read_data(uint8_t* data_from_screen, uint24_t len);

void ili9488_set_ram_pointer(Ili9488RamPointer args);

void ili9488_gram_write(uint8_t * pbuf, uint24_t len);

void ili9488_gram_write_continue(uint8_t * pbuf, uint24_t len);


#endif