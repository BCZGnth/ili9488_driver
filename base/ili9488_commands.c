#include "ili9488_commands.h"
#include "ili9488_base.h"
#include "ili_SoftSPI.h"
#include <stdarg.h>
#include <stdio.h>

// Function name   : ILI9488_SendCommand
// Functionality  : Send 1 byte command to ILI9488
// Arguments      : data -> Command to send (1 byte)
// Return         : None
// Conditions     : SPI1/MSSP and ILI9488 initialization must be completed
void ili9488_send_command(ili9488_interface_t interface, uint16_t len_cmd, ...)
{
    uint8_t len = (len_cmd >> 8) & 0xff;
    uint8_t cmd = (len_cmd & 0xff);
    uint8_t data;
    
    // Create the variadic list and start the 
    va_list params;
    // Begin the reading of the variadic arguments
    va_start(params, uint16_t);

    printf("len is %d\n", len);
    
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetLow();
    // Set command mode (DC pin is command when LOW and data when HIGH)
    *(interface.spi_dc_port) &= ~(1 << interface.spi_dc_pin);
    
    // Ignore reception with void cast
    #ifdef HARDWARE_SPI
    SPI1_Open(HOST_CONFIG);
    SPI1TCNT = 1;
    SPI1_ByteWrite(cmd);
    while(!PIR3bits.SPI1RXIF); //__delay_us(16);
    // while(SPI1CON2bits.BUSY) {/* printf(".\n"): */ continue; }
    SPI1_Close();
    #else
    fast_spi_write_byte(cmd);
    #endif
    

    // Set the dc pin high again for the data portion of the transmit
    DC1_SetHigh();

    if(len) {
        // Iterate for as many bytes as the command expects
        for(uint8_t blah = 0; blah < len; blah++)
        {
            data = va_arg(params, uint8_t);
            // printf("Send data: %d at index %d\n", data[blah], blah);
            
            
            #ifdef HARDWARE_SPI
            SPI1_Open(HOST_CONFIG);
            printf("Busy Bit is: %d TCZIF Bit is: %d", SPI1CON2bits.BUSY, SPI1INTFbits.TCZIF);
            SPI1_BufferWrite(data, len);
            printf("Busy Bit is: %d TCZIF Bit is: %d", SPI1CON2bits.BUSY, SPI1INTFbits.TCZIF);
            SPI1_Close();
            #else
            fast_spi_write_byte(data);
            #endif
        }
        
    }
    
    va_end(params);
    // Deselect Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetHigh();
}

/* Function name   : ILI9488_SendData
 * Functionality  : Send 1 byte data to ILI9488
 * Arguments      : data -> 1 byte data to send
 * Return         : None
 * Conditions     : SPI1/MSSP and ILI9488 initialization must be completed
 */
void ili9488_send_data(ili9488_interface_t interface, uint8_t* pdata, size_t data_length)
{
    
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetLow();
    
    // Set data mode (DC pin is command when LOW and data when HIGH)
    DC1_SetHigh();
    
    // Data sending process
    // SendDataProc(data);
    
    // Ignore reception with void cast
    #ifdef HARDWARE_SPI
    SPI1_Open(HOST_CONFIG);
    SPI1_BufferWrite(pdata, data_length);
    SPI1_Close();
    #else
    fast_spi_write_buffer(pdata, data_length);
    #endif

    // Deselect Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) |= ~(1 << interface.spi_cs_pin);

}

/* Function name   : ILI9488_SendData
 * Functionality  : Send 1 byte data to ILI9488
 * Arguments      : data -> 1 byte data to send
 * Return         : None
 * Conditions     : SPI1/MSSP and ILI9488 initialization must be completed
 */
void ili9488_send_byte(ili9488_interface_t interface, uint8_t data)
{
    
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetLow();
    // Set data mode (DC pin is command when LOW and data when HIGH)
    DC1_SetHigh();
    
    // Ignore reception with void cast
    #ifdef HARDWARE_SPI
    SPI1_Open(HOST_CONFIG);
    SPI1_ByteWrite(data);;
    while(!PIR3bits.SPI1TXIF);
    SPI1_Close();
    #else
    fast_spi_write_byte(data);
    #endif

    // Deselect Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetHigh();

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
void ili9488_transfer_data(ili9488_interface_t interface, uint8_t* tx_data, uint8_t* rx_data, size_t len)
{
    
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetLow();
    // Set data mode
    DC1_SetHigh();
    
    // Data sending process (send from buffer and write back to the same buffer)
    #ifdef HARDWARE_SPI
    SPI1_Open(HOST_CONFIG);
    SPI1_Transfer(tx_data, rx_data, len);
    // __delay_us(16);
    SPI1_Close();
    // while(!PIR3bits.SPI1TXIF);
    // while(!SPI1_Host.IsTxReady()) continue;
    #else
    fast_spi_transfer_buffer(tx_data, rx_data, len);
    #endif

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetHigh();

}

void ili9488_read_data(ili9488_interface_t interface, uint8_t* data_from_screen, uint24_t len)
{
    
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetLow();
    // Set data mode
    DC1_SetHigh();
    
    // Data sending process (send/receive based on mode)
    #ifdef HARDWARE_SPI
    SPI1_Open(HOST_CONFIG);
    SPI1_BufferRead(data_from_screen, len);
    SPI1_Close();
    // __delay_us(16);
    // while(SPI1CON2bits.BUSY) continue;
    #else
    fast_spi_read_buffer(data_from_screen, len);
    #endif

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetHigh();

}
/**
 * 
 */
void ili9488_set_ram_pointer(ili9488_interface_t inter, Ili9488RamPointer args) 
{
    uint8_t start_msb = (args.start_y >> 8) & 0xff;
    uint8_t start_lsb = (args.start_y & 0xff);
    uint8_t end_msb   = (args.end_y >> 8) & 0xff;
    uint8_t end_lsb   = (args.end_y & 0xff);
    ili9488_send_command(inter, ILI9488_COLUMN_ADDRESS_SET, start_msb, start_lsb, end_msb, end_lsb);

    start_msb = (args.start_x >> 8) & 0xff;
    start_lsb = (args.start_x & 0xff);
    end_msb   = (args.end_x >> 8) & 0xff;
    end_lsb   = (args.end_x & 0xff);
    ili9488_send_command(inter, ILI9488_PAGE_ADDRESS_SET, start_msb, start_lsb, end_msb, end_lsb);
}

void ili9488_gram_write(ili9488_interface_t inter, uint8_t * pbuf, uint24_t len)
{
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetLow();
    // Set data mode to Command
    DC1_SetLow();

    #ifdef HARDWARE_SPI
    SPI1_Open(HOST_CONFIG);
    SPI1_ByteWrite((ILI9488_RAMWR & 0xff));
    while(!PIR3bits.SPI1TXIF);
    SPI1_Close();
    // Set data mode to data
    DC1_SetHigh();
    SPI1_BufferWrite(pbuf, len);
    #else
    fast_spi_write_byte((ILI9488_RAMWR & 0xff));
    DC1_SetHigh();
    fast_spi_write_buffer(pbuf, len);
    #endif

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetHigh();

}

void ili9488_gram_write_continue(ili9488_interface_t inter, uint8_t * pbuf, uint24_t len)
{
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetLow();
    // Set data mode to Command
    DC1_SetLow();

    #ifdef HARDWARE_SPI
    SPI1_Open(HOST_CONFIG);
    SPI1_ByteWrite((ILI9488_RAMWR_CONTINUE & 0xff));
    while(!PIR3bits.SPI1TXIF);
    SPI1_Close();
    // Set data mode to data
    DC1_SetHigh();
    SPI1_BufferWrite(pbuf, len);
    #else
    fast_spi_write_byte((ILI9488_RAMWR_CONTINUE & 0xff));
    DC1_SetHigh();
    fast_spi_write_buffer(pbuf, len);
    #endif

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    CS1_SetHigh();

}