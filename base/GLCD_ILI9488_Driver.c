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
#include "GLCD_ILI9488_Driver.h"
#include "GLCD_ILI9488_Commands.h"

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

// Function name   : ILI9488_SendCommand
// Functionality  : Send 1 byte command to ILI9488
// Arguments      : data -> Command to send (1 byte)
// Return         : None
// Conditions     : SPI1/MSSP and ILI9488 initialization must be completed
void ILI9488_SendCommand(ili9488_interface_t interface, unsigned char data)
{
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) &= ~(1 << interface.spi_cs_pin);

    // Set command mode (DC pin is command when LOW and data when HIGH)
    *(interface.spi_dc_port) &= ~(1 << interface.spi_dc_pin);
    
    // Command sending process
    // SendDataProc(data);
    
    // Ignore reception with void cast
    (void)SoftSPI_Transfer(data, SOFT_SPI_MSB_FIRST);
  
    // DMA sending process (use if necessary)
    //DMA_SPI1_Send_Byte_Proc(data);

    // Deselect Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) |= ~(1 << interface.spi_cs_pin);
}

/* Function name   : ILI9488_SendData
 * Functionality  : Send 1 byte data to ILI9488
 * Arguments      : data -> 1 byte data to send
 * Return         : None
 * Conditions     : SPI1/MSSP and ILI9488 initialization must be completed
 */
void ILI9488_SendData(ili9488_interface_t interface, uint8_t* data, size_t len)
{
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) &= ~(1 << interface.spi_cs_pin);

    // Set data mode (DC pin is command when LOW and data when HIGH)
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);
    
    // Data sending process
    // SendDataProc(data);
    
    // Ignore reception with void cast
    for(int i = 0; i < len; i++){
        (void)SoftSPI_Transfer(*(data + i), SOFT_SPI_MSB_FIRST);
    }

    // DMA sending process (use if necessary)
    //DMA_SPI1_Send_Byte_Proc(data);

    // Deselect Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) |= ~(1 << interface.spi_cs_pin);
}

/* Function name   : ILI9488_SendData
 * Functionality  : Send 1 byte data to ILI9488
 * Arguments      : data -> 1 byte data to send
 * Return         : None
 * Conditions     : SPI1/MSSP and ILI9488 initialization must be completed
 */
void ILI9488_SendByte(ili9488_interface_t interface, uint8_t data)
{
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) &= ~(1 << interface.spi_cs_pin);

    // Set data mode (DC pin is command when LOW and data when HIGH)
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);

    // Ignore reception with void cast
    (void)SoftSPI_Transfer(data, SOFT_SPI_MSB_FIRST);

    // Deselect Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) |= ~(1 << interface.spi_cs_pin);
}
// Function name   : ILI9488_TransferData
// Functionality  : Send 1 byte data to ILI9488 (can also receive value if necessary)
// Arguments      : data        -> Data to send
//                  Send_Or_Recv-> Transfer mode
//                                SPI_MODE_WRITE : Send only (receive is ignored)
//                                SPI_MODE_XFER  : Send/receive (return received data)
//
// Return         : None (internal process handles reception if needed)
// Conditions     : SPI1/MSSP and ILI9488 initialization must be completed
void ILI9488_TransferData(ili9488_interface_t interface, uint8_t* data_to_screen, uint8_t* data_from_screen, size_t len)
{
    uint8_t u8_dummy_data;
    
    // Set data mode
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);
    
    // Data sending process (send/receive based on mode)
    for(int i = 0; i < len; i++){
        *(data_from_screen + i) = SoftSPI_Transfer(*(data_to_screen + i), SOFT_SPI_MSB_FIRST);
    }
}

// Function name   : ILI9488_Initialize
// Functionality  : Initialize ILI9488 (3.2inch): X480 x Y320
// Arguments      : None
// Return         : Initialized interface
// Conditions     : Must operate in SPI mode 3 (CKE=0, CKP=1, CPOL=1, CPHA=1)
ili9488_interface_t ILI9488_Initialize(ili9488_interface_t interface)
{
    SoftSPI_InitDataInPin(interface.spi_miso_port, interface.spi_miso_pin);
    SoftSPI_InitDataOutPin(interface.spi_mosi_port, interface.spi_mosi_pin);
    SoftSPI_InitClockPin(interface.spi_clock_port, interface.spi_clock_pin);
    SoftSPI_InitSelectPin(interface.spi_cs_port, interface.spi_cs_pin);

    ILI9488_Reset(interface);

    // Issue chip select (target device)
    // reset chip select (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) |= (1 << interface.spi_cs_pin);
    
    // Normal display mode ON (13h)
    ILI9488_SendCommand(interface, ILI9488_NORMAL_DISPLAY_MODE_ON);

    // IDLE Mode OM (39h) (only allows 8-colors to be displayed instead of the 262,144 when not idle)
    ILI9488_SendCommand(interface, ILI9488_IDLE_MODE_ON);
    
    // Interface Pixel Format (3Ah) = 0x01 (8bit)
    ILI9488_SendCommand(interface, ILI9488_PIXEL_FORMAT_SET);
    ILI9488_SendByte(interface, 0x66);
    
    // Memory Access Control (36h) : 0x48
    // 0x88 (MY=1, MX=0, MV=0) → Bottom-left origin, landscape orientation
    ILI9488_SendCommand(interface, ILI9488_MEMORY_ACCESS_CONTROL);
    ILI9488_SendByte(interface, 0x88);
    
    // Frame Rate Control (B2h)
    ILI9488_SendCommand(interface, ILI9488_FRAME_RATE_CONTROL);
    ILI9488_SendByte(interface, 0x00);  // Recommended value (depends on datasheet)
    ILI9488_SendByte(interface, 0x80);

    // Display Function Control (B6h)
    ILI9488_SendCommand(interface, ILI9488_DISPLAY_FUNCTION_CONTROL);
    ILI9488_SendByte(interface, 0x0A);
    ILI9488_SendByte(interface, 0x82);

    ILI9488_SendCommand(interface, ILI9488_WRITE_DISPLAY_BRIGHTNESS);
    ILI9488_SendByte(interface, 0xFF); // Maximum brightness
        
    // Display ON (29h)
    ILI9488_SendCommand(interface, ILI9488_DISPLAY_ON);

    // Sleep OUT (11h)
    ILI9488_SendCommand(interface, ILI9488_SLEEP_OUT);

    __delay_ms(5);
    
    // Issue chip select (target device)
    *(interface.spi_cs_port) |= (1 << interface.spi_cs_pin);
}

// Function name   : ILI9488_Set_Window
// Functionality  : Specify drawing area (starts drawing from specified X and Y coordinates, auto carriage return)
// Arguments      : 
// Return         : None
// Conditions     : None
void ILI9488_Set_Window(ili9488_interface_t interface, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    // Set X range
    ILI9488_SendCommand(interface, ILI9488_PAGE_ADDRESS_SET);
    
    // Send X-axis start point to SPI buffer in 8-bit units
    ILI9488_SendByte(interface, x >> 8);
    ILI9488_SendByte(interface, x & 0xFF);
    
    // Send X-axis end point to SPI buffer in 8-bit units (0 + 480 - 1 = 479)
    ILI9488_SendByte(interface, (x + w - 1) >> 8);
    ILI9488_SendByte(interface, (x + w - 1) & 0xFF);
    
    // Set Y range
    ILI9488_SendCommand(interface, 0x2A);
    
    // Send Y-axis start point to SPI buffer in 8-bit units
    ILI9488_SendByte(interface, y >> 8);
    ILI9488_SendByte(interface, y & 0xFF);
    
    // Send Y-axis end point to SPI buffer in 8-bit units (0 + 360 - 1 = 359)
    ILI9488_SendByte(interface, (y + h - 1) >> 8);
    ILI9488_SendByte(interface, (y + h - 1) & 0xFF);
    
    ILI9488_SendCommand(interface, 0x2C);
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
