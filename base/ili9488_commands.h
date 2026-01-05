// -----------------------------------------------------------------------------
// ILI9488 Standard DCS Command Set (0x00–0x5F)
// -----------------------------------------------------------------------------
#ifndef ILI9488_COMMANDS_H
#define ILI9488_COMMANDS_H

#include "ili9488_base.h"
#include "system.h"

const uint16_t ILI9488_NOP                          = 0x000;
const uint16_t ILI9488_SOFTWARE_RESET               = 0x001;

const uint16_t ILI9488_READ_DISPLAY_ID              = 0x404;
const uint16_t ILI9488_READ_NUMBER_OF_ERRORS        = 0x305;
const uint16_t ILI9488_READ_DISPLAY_STATUS          = 0x509;
const uint16_t ILI9488_READ_DISPLAY_POWER_MODE      = 0x20A;
const uint16_t ILI9488_READ_DISPLAY_MADCTL          = 0x20B;
const uint16_t ILI9488_READ_DISPLAY_PIXEL_FORMAT    = 0x20C;
const uint16_t ILI9488_READ_DISPLAY_IMAGE_MODE      = 0x20D;
const uint16_t ILI9488_READ_DISPLAY_SIGNAL_MODE     = 0x20E;
const uint16_t ILI9488_READ_SELF_DIAGNOSTIC         = 0x20F;

const uint16_t ILI9488_SLEEP_IN                     = 0x010;
const uint16_t ILI9488_SLEEP_OUT                    = 0x011;
const uint16_t ILI9488_PARTIAL_MODE_ON              = 0x012;
const uint16_t ILI9488_NORMAL_DISPLAY_MODE_ON       = 0x013;

const uint16_t ILI9488_DISPLAY_INVERSION_OFF        = 0x020;
const uint16_t ILI9488_DISPLAY_INVERSION_ON         = 0x021;

// Added from screenshot 3:
const uint16_t ILI9488_ALL_PIXELS_OFF               = 0x022;
const uint16_t ILI9488_ALL_PIXELS_ON                = 0x023;

const uint16_t ILI9488_DISPLAY_OFF                  = 0x028;
const uint16_t ILI9488_DISPLAY_ON                   = 0x029;

const uint16_t ILI9488_COLUMN_ADDRESS_SET           = 0x42A;
const uint16_t ILI9488_PAGE_ADDRESS_SET             = 0x42B;
const uint16_t ILI9488_MEMORY_WRITE                 = 0x32C;
// const uint16_t ILI9488_COLOR_SET                    = 0x2D
const uint16_t ILI9488_MEMORY_READ                  = 0x42E;

const uint16_t ILI9488_PARTIAL_AREA                 = 0x430;
const uint16_t ILI9488_VERTICAL_SCROLL_DEFINITION   = 0x633;

const uint16_t ILI9488_TEARING_EFFECT_LINE_OFF      = 0x034;
const uint16_t ILI9488_TEARING_EFFECT_LINE_ON       = 0x135;

const uint16_t ILI9488_MEMORY_ACCESS_CONTROL        = 0x136;
const uint16_t ILI9488_VERTICAL_SCROLL_START_ADDR   = 0x237;
const uint16_t ILI9488_IDLE_MODE_OFF                = 0x038;
const uint16_t ILI9488_IDLE_MODE_ON                 = 0x039;
const uint16_t ILI9488_INTERFACE_PIXEL_FORMAT       = 0x13A;
const uint16_t ILI9488_MEMORY_WRITE_CONTINUE        = 0x33C;
const uint16_t ILI9488_MEMORY_READ_CONTINUE         = 0x43E;

const uint16_t ILI9488_WRITE_TEAR_SCANLINE          = 0x244;
const uint16_t ILI9488_READ_SCANLINE                = 0x345;

const uint16_t ILI9488_WRITE_DISPLAY_BRIGHTNESS     = 0x151;
const uint16_t ILI9488_READ_DISPLAY_BRIGHTNESS      = 0x252;
const uint16_t ILI9488_WRITE_CONTROL_DISPLAY        = 0x153;
const uint16_t ILI9488_READ_CONTROL_DISPLAY         = 0x254;
const uint16_t ILI9488_WRITE_CABC                   = 0x155;
const uint16_t ILI9488_READ_CABC                    = 0x256;

const uint16_t ILI9488_WRITE_CABC_MIN_BRIGHTNESS    = 0x15E;
const uint16_t ILI9488_READ_CABC_MIN_BRIGHTNESS     = 0x25F;

// Screenshot 1 extra:
const uint16_t ILI9488_READ_AUTOMATIC_BRIGHTNESS    = 0x268;

// ID register reads
const uint16_t ILI9488_READ_ID1                     = 0x2DA;
const uint16_t ILI9488_READ_ID2                     = 0x2DB;
const uint16_t ILI9488_READ_ID3                     = 0x2DC;

// -----------------------------------------------------------------------------
// ILI9488 Extended Command Set (0xB0–0xFF)
// (From screenshots 1 & 2: sections 5.3.x, pages ~219–278)
// -----------------------------------------------------------------------------

const uint16_t ILI9488_INTERFACE_MODE_CONTROL       = 0x1B0;
const uint16_t ILI9488_FRAME_RATE_CONTROL_NORMAL    = 0x2B1;
const uint16_t ILI9488_FRAME_RATE_CONTROL_IDLE      = 0x2B2;
const uint16_t ILI9488_FRAME_RATE_CONTROL_PARTIAL   = 0x2B3;
const uint16_t ILI9488_DISPLAY_INVERSION_CONTROL    = 0x1B4;
const uint16_t ILI9488_BLANKING_PORCH_CONTROL       = 0x4B5;
const uint16_t ILI9488_DISPLAY_FUNCTION_CONTROL     = 0x3B6;
const uint16_t ILI9488_ENTRY_MODE_SET               = 0x1B7;
const uint16_t ILI9488_COLOR_ENHANCEMENT_1          = 0xcB9;
const uint16_t ILI9488_COLOR_ENHANCEMENT_2          = 0xcBA;
const uint16_t ILI9488_HS_LANES_CONTROL             = 0x2BE;

// Power control (screenshot 2)= 
const uint16_t ILI9488_POWER_CONTROL_1              = 0x2C0;
const uint16_t ILI9488_POWER_CONTROL_2              = 0x1C1;
const uint16_t ILI9488_POWER_CONTROL_3              = 0x1C2;
const uint16_t ILI9488_POWER_CONTROL_4              = 0x1C3;
const uint16_t ILI9488_POWER_CONTROL_5              = 0x1C4;
const uint16_t ILI9488_VCOM_CONTROL                 = 0x4C5;

// CABC control blocks= 
const uint16_t ILI9488_CABC_CONTROL_1               = 0x2C6;
const uint16_t ILI9488_CABC_CONTROL_2               = 0x1C8;
const uint16_t ILI9488_CABC_CONTROL_3               = 0x1C9;
const uint16_t ILI9488_CABC_CONTROL_4               = 0x1CA;
const uint16_t ILI9488_CABC_CONTROL_5               = 0x1CB;
const uint16_t ILI9488_CABC_CONTROL_6               = 0x1CC;
const uint16_t ILI9488_CABC_CONTROL_7               = 0x1CD;
const uint16_t ILI9488_CABC_CONTROL_8               = 0x1CE;
const uint16_t ILI9488_CABC_CONTROL_9               = 0x1CF;

// NV Memory & ID read= 
const uint16_t ILI9488_NV_MEMORY_WRITE              = 0x2D0;
const uint16_t ILI9488_NV_MEMORY_KEY                = 0x3D1;
const uint16_t ILI9488_NV_MEMORY_STATUS_READ        = 0x3D2;
const uint16_t ILI9488_READ_ID4                     = 0x4D3;

// Adjustment controls= 
const uint16_t ILI9488_ADJUST_CONTROL_1             = 0x1D7;
const uint16_t ILI9488_READ_ID_VERSION              = 0x2D8;

// Gamma controls
const uint16_t ILI9488_POSITIVE_GAMMA_CONTROL       = 0xeE0;
const uint16_t ILI9488_NEGATIVE_GAMMA_CONTROL       = 0xeE1;
const uint16_t ILI9488_DIGITAL_GAMMA_CONTROL_1      = 0x3E2;
const uint16_t ILI9488_DIGITAL_GAMMA_CONTROL_2      = 0x3E3;

const uint16_t ILI9488_SET_IMAGE_FUNCTION           = 0x1E9;

// Additional manufacturer commands (seen in datasheet)
const uint16_t ILI9488_ADJUST_CONTROL_2             = 0xcF2;
const uint16_t ILI9488_ADJUST_CONTROL_3             = 0x4F7;
const uint16_t ILI9488_ADJUST_CONTROL_4             = 0x2F8;
const uint16_t ILI9488_ADJUST_CONTROL_5             = 0x1F9;
const uint16_t ILI9488_ADJUST_CONTROL_6             = 0x2FC;
const uint16_t ILI9488_ADJUST_CONTROL_7             = 0x2FF;

// Aliases (common names)
#define ILI9488_RAMWR                        ILI9488_MEMORY_WRITE
#define ILI9488_RAMRD                        ILI9488_MEMORY_READ
#define ILI9488_RAMWR_CONTINUE               ILI9488_MEMORY_WRITE_CONTINUE
#define ILI9488_RAMRD_CONTINUE               ILI9488_MEMORY_READ_CONTINUE


// Send command to ILI9488 (received value is ignored)
void ili9488_send_command(ili9488_interface_t interface, uint16_t cmd, ...);

// Send data to ILI9488 (received value is ignored)
void ili9488_send_data(ili9488_interface_t interface, uint8_t* pdata, size_t data_length);

// Send single byte to ILI9488 (received value is ignored)
void ili9488_send_byte(ili9488_interface_t interface, uint8_t data);

// Send/receive data to/from ILI9488
void ili9488_transfer_data(ili9488_interface_t interface, uint8_t* tx_data, uint8_t* rx_data, size_t len);

void ili9488_read_data(ili9488_interface_t interface, uint8_t* data_from_screen, uint24_t len);

void ili9488_set_ram_pointer(ili9488_interface_t inter, Ili9488RamPointer args);

void ili9488_gram_write(ili9488_interface_t inter, uint8_t * pbuf, uint24_t len);

void ili9488_gram_write_continue(ili9488_interface_t inter, uint8_t * pbuf, uint24_t len);


#endif