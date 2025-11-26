//==============================================================================
// File Name            : GLCD_ILI9488_Driver.h
// Creator              : Kensuke Takami
// Created Date         : August 6, 2025
// CPU                  : PIC32MX370F512H
// Compiler             : MPLAB XC32 v4.45
// Overview             : ILI9488 GLCD Driver (Control/Draw function prototypes)
// Operating Frequency  : 80MHz
// History              :
//    [2025/07/18] Initial version created
//    [2025/07/20] Added DMA-supporting drawing functions
//==============================================================================

//==============================================================================
// Include Guard: Macro to prevent multiple inclusions
//==============================================================================
#ifndef ILI9488_DRIVER
#define ILI9488_DRIVER

#include "system.h"

typedef struct ILI9488_INTERFACE {
    uint8_t * spi_miso_port;
    uint8_t * spi_mosi_port;
    uint8_t * spi_dc_port;
    uint8_t * spi_cs_port;
    uint8_t * spi_clock_port;
    
    uint8_t   spi_miso_pin;
    uint8_t   spi_mosi_pin;
    uint8_t   spi_dc_pin;
    uint8_t   spi_cs_pin;
    uint8_t   spi_clock_pin;

    uint8_t * lcd_rst_port;
    uint8_t   lcd_rst_pin;

} ili9488_interface_t;

//==============================================================================
// ILI9488 Basic Control Function Prototypes
//==============================================================================

// Hardware reset of ILI9488 (LOW = reset / HIGH = start)
void ILI9488_Reset(ili9488_interface_t interface);

// Send command to ILI9488 (received value is ignored)
void ILI9488_SendCommand(ili9488_interface_t interface, unsigned char cmd, uint8_t* pdata, size_t data_length);

// Send data to ILI9488 (received value is ignored)
void ILI9488_SendData(ili9488_interface_t interface, uint8_t * data, size_t len);

// Send single byte to ILI9488 (received value is ignored)
void ILI9488_SendByte(ili9488_interface_t interface, uint8_t data);

// Send/receive data to/from ILI9488
void ILI9488_TransferData(ili9488_interface_t interface, uint8_t * data_to_screen, uint8_t * data_from_screen, size_t len);

void ILI9488_ReadData(ili9488_interface_t interface, uint8_t* data_from_screen, size_t len);

// Initialize ILI9488 using SPI1 (MSSP)
void ILI9488_Initialize(ili9488_interface_t interface);

// Set the drawing window (coordinates x, y, width w, height h)
void ILI9488_Set_Window(ili9488_interface_t interface, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

//==============================================================================
// ILI9488 DMA Drawing Function Prototypes
//==============================================================================

// Fill the background with a specified color
// void DMA_ILI9488_FillBackground(ili9488_interface_t interface, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t R, uint8_t G, uint8_t B);

// Draw a border with a specified color
// void DMA_ILI9488_DrawBorder(ili9488_interface_t interface, uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint8_t R, uint8_t G, uint8_t B);

#endif
