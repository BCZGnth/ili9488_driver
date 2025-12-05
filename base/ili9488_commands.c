#include "ili9488_commands.h"
#include "ili9488_base.h"
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
    
    SPI1_Open(HOST_CONFIG);
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) &= ~(1 << interface.spi_cs_pin);
    // Set command mode (DC pin is command when LOW and data when HIGH)
    *(interface.spi_dc_port) &= ~(1 << interface.spi_dc_pin);
    
    // Ignore reception with void cast
    SPI1_ByteWrite(cmd);
    while(SPI1CON2bits.BUSY) {/* printf(".\n"): */ continue; }
    
    // Set the dc pin high again for the data portion of the transmit
    __delay_us(1);
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);
    __delay_us(1);
    
    // Begin the reading of the variadic arguments
    va_start(params, uint16_t);
    // Iterate for as many bytes as the command expects
    for(uint8_t i = 0; i < len; i++)
    {
        data = va_arg(params, uint8_t);
        SPI1_ByteWrite(data);
        while(SPI1CON2bits.BUSY) continue;
    }
    va_end(params);

    // Deselect Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) |= (1 << interface.spi_cs_pin);

    SPI1_Close();
}

/* Function name   : ILI9488_SendData
 * Functionality  : Send 1 byte data to ILI9488
 * Arguments      : data -> 1 byte data to send
 * Return         : None
 * Conditions     : SPI1/MSSP and ILI9488 initialization must be completed
 */
void ili9488_send_data(ili9488_interface_t interface, uint8_t* pdata, size_t data_length)
{
    SPI1_Open(HOST_CONFIG);

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) &= ~(1 << interface.spi_cs_pin);

    // Set data mode (DC pin is command when LOW and data when HIGH)
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);
    
    // Data sending process
    // SendDataProc(data);
    
    // Ignore reception with void cast
    SPI1_BufferWrite(pdata, data_length - 1);

    // DMA sending process (use if necessary)
    //DMA_SPI1_Send_Byte_Proc(data);

    // Deselect Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) |= ~(1 << interface.spi_cs_pin);

    SPI1_Close();
}

/* Function name   : ILI9488_SendData
 * Functionality  : Send 1 byte data to ILI9488
 * Arguments      : data -> 1 byte data to send
 * Return         : None
 * Conditions     : SPI1/MSSP and ILI9488 initialization must be completed
 */
void ili9488_send_byte(ili9488_interface_t interface, uint8_t data)
{
    SPI1_Open(HOST_CONFIG);

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) &= ~(1 << interface.spi_cs_pin);
    // Set data mode (DC pin is command when LOW and data when HIGH)
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);

    // Ignore reception with void cast
    SPI1_ByteWrite(data);

    // Deselect Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) |= (1 << interface.spi_cs_pin);

    SPI1_Close();
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
void ili9488_transfer_data(ili9488_interface_t interface, uint8_t* exchange_data, size_t len)
{
    SPI1_Open(HOST_CONFIG);
    uint8_t u8_dummy_data;

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) &= ~(1 << interface.spi_cs_pin);
    // Set data mode
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);
    
    // Data sending process (send from buffer and write back to the same buffer)
    SPI1_BufferExchange(exchange_data, len);
    while(!SPI1_Host.IsTxReady()) continue;

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) |= (1 << interface.spi_cs_pin);

    SPI1_Close();
}

void ili9488_read_data(ili9488_interface_t interface, uint8_t* data_from_screen, size_t len)
{
    SPI1_Open(HOST_CONFIG);
    uint8_t u8_dummy_data;

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) &= ~(1 << interface.spi_cs_pin);
    // Set data mode
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);
    
    // Data sending process (send/receive based on mode)
    SPI1_BufferRead(data_from_screen, len);
    while(!SPI1_Host.IsRxReady()) continue;

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) |= (1 << interface.spi_cs_pin);

    SPI1_Close();
}
/**
 * 
 */
void ili9488_set_ram_pointer(ili9488_interface_t inter, Ili9488RamPointer args) 
{
    uint8_t start_msb = (args.start_row >> 8) & 0xff;
    uint8_t start_lsb = (args.start_row & 0xff);
    uint8_t end_msb   = (args.end_row >> 8) & 0xff;
    uint8_t end_lsb   = (args.end_row & 0xff);
    ili9488_send_command(inter, ILI9488_PAGE_ADDRESS_SET, start_msb, start_lsb, end_msb, end_lsb);

    start_msb = (args.start_column >> 8) & 0xff;
    start_lsb = (args.start_column & 0xff);
    end_msb   = (args.end_column >> 8) & 0xff;
    end_lsb   = (args.end_column & 0xff);
    ili9488_send_command(inter, ILI9488_COLUMN_ADDRESS_SET, start_msb, start_lsb, end_msb, end_lsb);
}

void ili9488_gram_write(ili9488_interface_t inter, uint8_t * pbuf, size_t len)
{
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(inter.spi_cs_port) &= ~(1 << inter.spi_cs_pin);
    // Set data mode to Command
    *(inter.spi_dc_port) |= (1 << inter.spi_dc_pin);
    SPI1_ByteWrite((ILI9488_RAMWR & 0xff));
    // Set data mode to data
    *(inter.spi_dc_port) |= (1 << inter.spi_dc_pin);
    SPI1_BufferWrite(pbuf, len - 1);

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(inter.spi_cs_port) |= (1 << inter.spi_cs_pin);

}