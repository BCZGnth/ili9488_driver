#include "ili9488_commands.h"
#include "ili9488_base.h"

// Function name   : ILI9488_SendCommand
// Functionality  : Send 1 byte command to ILI9488
// Arguments      : data -> Command to send (1 byte)
// Return         : None
// Conditions     : SPI1/MSSP and ILI9488 initialization must be completed
void ILI9488_SendCommand(ili9488_interface_t interface, unsigned char cmd, uint8_t* pdata, size_t data_length)
{
    // 
    SPI1_Open(HOST_CONFIG);

    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    *(interface.spi_cs_port) &= ~(1 << interface.spi_cs_pin);

    // Set command mode (DC pin is command when LOW and data when HIGH)
    *(interface.spi_dc_port) &= ~(1 << interface.spi_dc_pin);

    // Ignore reception with void cast
    SPI1_ByteWrite(cmd);
    while(SPI1CON2bits.BUSY) printf(".\n");
    // Set the dc pin high again for the data portion of the transmit
    __delay_us(1);
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);
    __delay_us(1);

    // Send the command data
    // printf("Command is: %x\n", cmd);
    // printf("Data length is: %d\n", data_length);

    if(data_length != 0)
    {
        SPI1_BufferWrite(pdata, data_length);
        while(SPI1CON2bits.BUSY) continue;
    }

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
void ILI9488_SendData(ili9488_interface_t interface, uint8_t* pdata, size_t data_length)
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
void ILI9488_SendByte(ili9488_interface_t interface, uint8_t data)
{
    SPI1_Open(HOST_CONFIG);
    // Select Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    // *(interface.spi_cs_port) &= ~(1 << interface.spi_cs_pin);

    // Set data mode (DC pin is command when LOW and data when HIGH)
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);

    // Ignore reception with void cast
    SPI1_ByteWrite(data);

    // Deselect Chip (Pin is active low) see page 39 of datasheet: https://www.hpinfotech.ro/ILI9488.pdf
    // *(interface.spi_cs_port) |= ~(1 << interface.spi_cs_pin);
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
void ILI9488_TransferData(ili9488_interface_t interface, uint8_t* exchange_data, size_t len)
{
    SPI1_Open(HOST_CONFIG);
    uint8_t u8_dummy_data;
    
    // Set data mode
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);
    
    // Data sending process (send from buffer and write back to the same buffer)
    SPI1_BufferExchange(exchange_data, len);
    while(!SPI1_Host.IsTxReady()) continue;
    SPI1_Close();
}

void ILI9488_ReadData(ili9488_interface_t interface, uint8_t* data_from_screen, size_t len)
{
    SPI1_Open(HOST_CONFIG);
    uint8_t u8_dummy_data;
    
    // Set data mode
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);
    
    // Data sending process (send/receive based on mode)
    SPI1_BufferRead(data_from_screen, len);
    while(!SPI1_Host.IsRxReady()) continue;
    SPI1_Close();
}
