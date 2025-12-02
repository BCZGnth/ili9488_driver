// PIC32 basic header (register definitions)
// #include <xc.h>

// ISR-related macros (__ISR, etc.)
// #include <sys/attribs.h>

// Address conversion macros (KVA_TO_PA, etc.)
// #include <sys/kmem.h>

// Peripheral initialization/configuration prototypes
// #include "System_Peripherals.h"

// xc8 system include
#include "system.h"

// Software SPI
#include "SoftSPI.h"

// ILI9488 driver function prototypes
#include "ili9488_driver.h"
#include "ili9488_commands.h"
#include "ili9488_base.h"

// static uint32_t i;

// Function name   : ILI9488_Reset
// Functionality  : Hardware reset of ILI9488 (LOW=reset/HIGH=start)
// Arguments      : None
// Return         : None
// Conditions     : SPI1_RST pin must be correctly connected/configured
void ILI9488_Reset(ili9488_interface_t interface)
{
    // Execute hardware reset
    *(interface.lcd_rst_port) &= ~(1 << interface.lcd_rst_pin);
    __delay_ms(20);
    *(interface.lcd_rst_port) |= (1 << interface.lcd_rst_pin);
    __delay_ms(5);
}

// Function name   : ILI9488_Initialize
// Functionality  : Initialize ILI9488 (3.2inch): X480 x Y320
// Arguments      : None
// Return         : Initialized interface
// Conditions     : Must operate in SPI mode 3 (CKE=0, CKP=1, CPOL=1, CPHA=1)
Ili9488Defines ILI9488_Initialize(ili9488_interface_t interface)
{
    // SoftSPI_InitDataInPin(interface.spi_miso_port, interface.spi_miso_pin);
    // SoftSPI_InitDataOutPin(interface.spi_mosi_port, interface.spi_mosi_pin);
    // SoftSPI_InitClockPin(interface.spi_clock_port, interface.spi_clock_pin);
    // SoftSPI_InitSelectPin(interface.spi_cs_port, interface.spi_cs_pin);
    Ili9488Defines LCD = {
        .Screen = {
            .pbuffer = NULL,
            .buffer_size = 0,
            .character = {
                .height = 16,
                .width = 10,
                .pad = 2,
                .width_pad = 12
            },
            .offset = {
                .ascii = 32,
                .pfont = NULL
            },
            .ScreenHeight = 320,
            .ScreenWidth = 480,
            .zeroed_ram_ptr = {
                .column = 0,
                .row = 0,
            }
        },
        .interface = interface
    };

    ILI9488_Reset(LCD.interface);
    __delay_ms(150); // See reset page (306-309) of datasheet. This is the longest necessary time needed after any hardware reset
    
    // Positive and negative gamma control taken from: https://github.com/jaretburkett/ILI9488/blob/master/ILI9488.cpp
    // uint8_t pos_gamma_ctrl[] = {0x00,
    //                             0x03,
    //                             0x09,
    //                             0x08,
    //                             0x16,
    //                             0x0A,
    //                             0x3F,
    //                             0x78,
    //                             0x4C,
    //                             0x09,
    //                             0x0A,
    //                             0x08,
    //                             0x16,
    //                             0x1A,
    //                             0x0F}
    // ILI9488_SendCommand(interface, ILI9488_POSITIVE_GAMMA_CONTROL, pos_gamma_ctrl, sizeof(pos_gamma_ctrl));
    // uint8_t neg_gamma_ctrl[] = {0x00,
    //                             0x16,
    //                             0x19,
    //                             0x03,
    //                             0x0F,
    //                             0x05,
    //                             0x32,
    //                             0x45,
    //                             0x46,
    //                             0x04,
    //                             0x0E,
    //                             0x0D,
    //                             0x35,
    //                             0x37,
    //                             0x0F}
    // ILI9488_SendCommand(interface, ILI9488_NEGATIVE_GAMMA_CONTROL, neg_gamma_ctrl, sizeof(neg_gamma_ctrl));

    const uint8_t pwr_ctrl_1[]     = {0x17, 
                                      0x15};

    const uint8_t pwr_ctrl_2[]     = {0x15};

    const uint8_t pwr_ctrl_3[]     = {0x00, 
                                      0x12, 
                                      0x80};

    const uint8_t mad_ctrl[]       = {0x00};
    const uint8_t pxl_fmt[]        = {0x66};
    const uint8_t interface_ctrl[] = {0x80};
    const uint8_t frm_rt_ctrl[]    = {0x00};
    const uint8_t disp_inv_ctrl[]  = {0x02};

    const uint8_t disp_func_ctrl[] = {0x02,
                                      0x02};

    const uint8_t img_func[]       = {0x00};

    const uint8_t adj_ctrl_3[]     = {0xa9, 
                                      0x5c, 
                                      0x2c, 
                                      0xb2};

    ILI9488_SendCommand(interface, ILI9488_POWER_CONTROL_1, pwr_ctrl_1, 2);      //Power Control 1
    // ILI9488_SendByte(interface, 0x17);    //Vreg1out
    // ILI9488_SendByte(interface, 0x15);    //Verg2out
    
    ILI9488_SendCommand(interface, ILI9488_POWER_CONTROL_2, pwr_ctrl_2, 1);      //Power Control 2
    // ILI9488_SendByte(interface, 0x41);    //VGH,VGL
    
    ILI9488_SendCommand(interface, ILI9488_POWER_CONTROL_3, pwr_ctrl_3, 3);      //Power Control 3
    // ILI9488_SendByte(interface, 0x00);
    // ILI9488_SendByte(interface, 0x12);    //Vcom
    // ILI9488_SendByte(interface, 0x80);
    
    ILI9488_SendCommand(interface, ILI9488_MEMORY_ACCESS_CONTROL, mad_ctrl, 1);      //Memory Access
    // ILI9488_SendByte(interface, 0x00);  // No change to the r/w scanning of the screen
    
    ILI9488_SendCommand(interface, ILI9488_INTERFACE_PIXEL_FORMAT, pxl_fmt, 1);      // Interface Pixel Format
    // ILI9488_SendByte(interface, 0x66); 	  //18 bit
    
    ILI9488_SendCommand(interface, ILI9488_INTERFACE_MODE_CONTROL, interface_ctrl, 1);      // Interface Mode Control
    // ILI9488_SendByte(interface, 0x80);     			 //SDO NOT USE
    
    ILI9488_SendCommand(interface, ILI9488_FRAME_RATE_CONTROL_IDLE, frm_rt_ctrl, 1);      //Frame rate
    // ILI9488_SendByte(interface, 0x00);    //60Hz
    
    ILI9488_SendCommand(interface, ILI9488_DISPLAY_INVERSION_CONTROL, disp_inv_ctrl, 1);      //Display Inversion Control
    // ILI9488_SendByte(interface, 0x02);    //2-dot
    
    ILI9488_SendCommand(interface, ILI9488_DISPLAY_FUNCTION_CONTROL, disp_func_ctrl, 2);      //Display Function Control  RGB/MCU Interface Control
    // ILI9488_SendByte(interface, 0x02);    //MCU
    // ILI9488_SendByte(interface, 0x02);    //Source,Gate scan dieection
    
    ILI9488_SendCommand(interface, ILI9488_SET_IMAGE_FUNCTION, img_func, 1);      // Set Image Functio
    // ILI9488_SendByte(interface, 0x00);    // Disable 24 bit data
    
    ILI9488_SendCommand(interface, ILI9488_ADJUST_CONTROL_3, adj_ctrl_3, 4);      // Adjust Control
    // ILI9488_SendByte(interface, 0xA9);
    // ILI9488_SendByte(interface, 0x51);
    // ILI9488_SendByte(interface, 0x2C);
    // ILI9488_SendByte(interface, 0x82);    // D7 stream, loose
    
    // Sleep OUT (11h)
    ILI9488_SendCommand(interface, ILI9488_SLEEP_OUT, NULL, 0);
    __delay_ms(120);
    // Display ON (29h)
    ILI9488_SendCommand(interface, ILI9488_DISPLAY_ON, NULL, 0);
    __delay_ms(120);

    return LCD;
}

// Function name   : ILI9488_Set_Window
// Functionality  : Specify drawing area (starts drawing from specified X and Y coordinates, auto carriage return)
// Arguments      : 
// Return         : None
// Conditions     : None
void ILI9488_Set_Window(ili9488_interface_t interface, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    // Set X range
    ILI9488_SendCommand(interface, ILI9488_PAGE_ADDRESS_SET, NULL, 0);
    
    // Send X-axis start point to SPI buffer in 8-bit units
    ILI9488_SendByte(interface, x >> 8);
    ILI9488_SendByte(interface, x & 0xFF);
    
    // Send X-axis end point to SPI buffer in 8-bit units (0 + 480 - 1 = 479)
    ILI9488_SendByte(interface, (x + w - 1) >> 8);
    ILI9488_SendByte(interface, (x + w - 1) & 0xFF);
    
    // Set Y range
    ILI9488_SendCommand(interface, 0x2A, NULL, 0);
    
    // Send Y-axis start point to SPI buffer in 8-bit units
    ILI9488_SendByte(interface, y >> 8);
    ILI9488_SendByte(interface, y & 0xFF);
    
    // Send Y-axis end point to SPI buffer in 8-bit units (0 + 360 - 1 = 359)
    ILI9488_SendByte(interface, (y + h - 1) >> 8);
    ILI9488_SendByte(interface, (y + h - 1) & 0xFF);
    
    ILI9488_SendCommand(interface, 0x2C, NULL, 0);
}

// Function name   : DMA_ILI9488_FillBackground
// Functionality  : Fill the background color of ILI9488, sending data via DMA in 8-bit (1 byte) units
// Arguments      : 
// Return         : None
// Conditions     : None
// void DMA_ILI9488_FillBackground(ili9488_interface_t interface, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t R, uint8_t G, uint8_t B)
// {
//     // Refresh SPI1 receive buffer
//     SPI1_FlushRX();
    
//     // Issue chip select (target device)
//     SPI1_ILI9488_CS = 0x00;
    
//     // Set drawing area (start point X and Y, width, height)
//     ILI9488_Set_Window(x, y, w, h);
   
//     // Set SPI clock rate (maximum clock frequency: 20MHz)
//     SPI1BRG = 80000000 / (2 * (20000000)) - 1;
    
//     // Set color data for one line in the array
//     for (i = 0; i < w; i++)
//     {
//         Color_Packet_One_Line[(i * 3) + 0] = (R & 0xFC);  // Red
        
//         Color_Packet_One_Line[(i * 3) + 1] = (G & 0xFC);  // Green
        
//         Color_Packet_One_Line[(i * 3) + 2] = (B & 0xFC);  // Blue
//     }
        
//     // Check the number of elements in the full array (byte count based on the specified drawing width)
//     // Full screen fill: 1 line (480 pixels) × 24 bits = 11520 bits (1440 bytes)
//     uint16_t size = (w * 24) / 8;
    
//     // Set the color for each vertical line of 360 pixels
//     for(int j = 0; j < h; j++)
//     {
//         DMA0_to_SPI1_Data_Transmission_Proc(Color_Packet_One_Line, size);
        
//         // Clear RX buffer
//         while (SPI1STATbits.SPIRBF)
//         {
//             (void)SPI1BUF;
//         }
        
//         if (SPI1STATbits.SPIROV) SPI1STATCLR = _SPI1STAT_SPIROV_MASK;        
//     }
    
//     while (SPI1STATbits.SPIBUSY){}

//     // Issue chip select (target device)
//     SPI1_ILI9488_CS = 0x01;
    
//     // Set SPI clock rate (maximum clock frequency: 10MHz)
//     SPI1BRG = 80000000 / (2 * (10000000)) - 1;
    
//     // Clear SPI1 receive buffer and error flags
//     SPI1_FlushRX();
// }

// // Function name   : DMA_ILI9488_DrawLineFromBuffer
// // Functionality  : Transfer one line of data read from SD to ILI9488 via DMA
// // Arguments      : x, y   -> Starting coordinates for drawing
// //                  w     -> Width of the drawing (in pixels)
// //                  pLine -> One line of RGB data (8-bit × 3 bytes × w pixels)
// // Return         : None
// void DMA_ILI9488_DrawLineFromBuffer(ili9488_interface_t interface, uint16_t x, uint16_t y, uint16_t w, uint8_t *pLine)
// {
//     // Refresh SPI1 receive buffer
//     SPI1_FlushRX();
    
//     // Select chip (ILI9488)
//     SPI1_ILI9488_CS = 0x00;
    
//     // Set drawing area to one line
//     ILI9488_Set_Window(x, y, w, 1);

//     // Set SPI clock to 20MHz
//     SPI1BRG = 80000000 / (2 * (20000000)) - 1;

//     // Calculate the number of bytes in one line (RGB888 compatible: 3 bytes × w pixels)
//     uint16_t size = (w * 24) / 8;
    
//     // Debug output: check the first 16 bytes
//     /*printf("DMA send check (y=%d, size=%d)\r\n", y, size);
//     for (int i = 0; i < 16; i++)
//     {
//         printf("%02X ", pLine[i]);
//     }
//     printf("\r\n");*/
        
//     // Start transfer via DMA
//     DMA0_to_SPI1_Data_Transmission_Proc(pLine, size);

//     // Wait for DMA to complete
//     while (DCH0CONbits.CHEN);

//     // Wait for SPI to complete
//     while (SPI1STATbits.SPIBUSY);

//     // Clear RX buffer
//     while (SPI1STATbits.SPIRBF)
//     {
//         (void)SPI1BUF;
//     }
//     if (SPI1STATbits.SPIROV)
//     {
//         SPI1STATCLR = _SPI1STAT_SPIROV_MASK;
//     }

//     // Wait for transfer to complete
//     while (SPI1STATbits.SPIBUSY) {}

//     // Release chip select
//     SPI1_ILI9488_CS = 0x01;

//     // Return SPI clock to 10MHz
//     SPI1BRG = 80000000 / (2 * (10000000)) - 1;

//     // Clear SPI buffer
//     SPI1_FlushRX();
// }

// // Function name   : DMA_ILI9488_DrawBorder
// // Functionality  : Draw a border from the specified coordinates with the given width and height
// // Arguments      : 
// // Return         : None
// // Conditions     : None
// void DMA_ILI9488_DrawBorder(ili9488_interface_t interface, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t R, uint8_t G, uint8_t B)
// {
//     // Draw border (bottom line)
//     DMA_ILI9488_FillBackground(interface, x, y, w, 1 , R, G, B);
    
//     // Draw border (top line) (Y start = Y start + height)
//     DMA_ILI9488_FillBackground(interface, x, y + h, w, 1 , R, G, B);
    
//     // Draw border (left line)
//     DMA_ILI9488_FillBackground(interface, x, y, 1, h , R, G, B);
    
//     // Draw border (right line) (X start = X start + width)
//     DMA_ILI9488_FillBackground(interface, x + w, y, 1, h , R, G, B);
// }
