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
#include "ili9488_base.h"

//==============================================================================
// ILI9488 Basic Control Function Prototypes
//==============================================================================

// Hardware reset of ILI9488 (LOW = reset / HIGH = start)
void ili9488_reset(ili9488_interface_t * interface);

// Initialize ILI9488 using SPI1 (MSSP)
Ili9488Defines ili9488_initialize(ili9488_interface_t * interface, uint8_t * spibuffer, uint8_t buffer_size);

#endif
