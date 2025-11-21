// PIC32基本ﾍｯﾀﾞ(ﾚｼﾞｽﾀ定義)
// #include <xc.h>

// ISR関連ﾏｸﾛ(__ISRなど)
// #include <sys/attribs.h>

// ｱﾄﾞﾚｽ変換ﾏｸﾛ(KVA_TO_PAなど)
// #include <sys/kmem.h>

// 周辺機能初期化/設定用ﾌﾟﾛﾄﾀｲﾌﾟ
// #include "System_Peripherals.h"

// xc8 system include
#include "system.h"

// Software SPI 
#include "SoftSPI.h"

// ILI9488ﾄﾞﾗｲﾊﾞ関数ﾌﾟﾛﾄﾀｲﾌﾟ
#include "GLCD_ILI9488_Driver.h"
#include "GLCD_ILI9488_Commands.h"

// static uint32_t i;

// 関数名   : ILI9488_Reset
// 機能     : ILI9488をﾊ-ﾄﾞｳｪｱﾘｾｯﾄする(LOW=ﾘｾｯﾄ/HIGH=ｽﾀ-ﾄ)
// 引数     : なし
// 戻り値   : なし
// 使用条件 : SPI1_RSTﾋﾟﾝが正しく接続/設定されていること
void ILI9488_Reset(ili9488_interface_t interface)
{
    // ﾊ-ﾄﾞｳｪｱﾘｾｯﾄ実行
    *(interface.lcd_rst_port) &= ~(1 << interface.lcd_rst_pin);
    __delay_ms(20);
    *(interface.lcd_rst_port) |= (1 << interface.lcd_rst_pin);
    __delay_ms(5);
}

// 関数名   : ILI9488_SendCommand
// 機能     : ILI9488へ1ﾊﾞｲﾄのｺﾏﾝﾄﾞを送信する
// 引数     : data -> 送信するｺﾏﾝﾄﾞ(1ﾊﾞｲﾄ)
// 戻り値   : なし
// 使用条件 : SPI1/MSSPおよびILI9488関連の初期化が完了していること
void ILI9488_SendCommand(ili9488_interface_t interface, unsigned char data)
{
    // ｺﾏﾝﾄﾞﾓ-ﾄﾞ設定
    *(interface.spi_dc_port) &= ~(1 << interface.spi_dc_pin);
    
    // ｺﾏﾝﾄﾞ送信処理
    //SendDataProc(data);
    
    // 受信は無視
    (void)SoftSPI_Transfer(data, SOFT_SPI_MSB_FIRST);
  
    // DMA送信処理(必要に応じて使用)
    //DMA_SPI1_Send_Byte_Proc(data);
}

// 関数名   : ILI9488_SendData
// 機能     : ILI9488へ1ﾊﾞｲﾄのﾃﾞ-ﾀを送信する
// 引数     : data -> 送信する1ﾊﾞｲﾄのﾃﾞ-ﾀ
// 戻り値   : なし
// 使用条件 : SPI1/MSSPおよびILI9488関連の初期化が完了していること
void ILI9488_SendData(ili9488_interface_t interface, uint8_t data)
{
    // ﾃﾞ-ﾀﾓ-ﾄﾞ設定
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);
    
    // ﾃﾞ-ﾀ送信処理
    //SendDataProc(data);
    
    // 受信は無視
    (void)SoftSPI_Transfer(data, SOFT_SPI_MSB_FIRST);
    
    // DMA送信処理(必要に応じて使用)
    //DMA_SPI1_Send_Byte_Proc(data);
}

// 関数名   : ILI9488_TransferData
// 機能     : ILI9488へ1ﾊﾞｲﾄのﾃﾞ-ﾀを送信する(必要に応じて受信値も取得)
// 引数     : data        -> 送信するﾃﾞ-ﾀ
//            Send_Or_Recv-> 転送ﾓ-ﾄﾞ指定
//                          SPI_MODE_WRITE : 送信のみ(受信値は無視)
//                          SPI_MODE_XFER  : 送受信(受信値を返す)
//
// 戻り値   : なし(受信値が必要な場合は内部処理で扱う)
// 使用条件 : SPI1/MSSPおよびILI9488関連の初期化が完了していること
void ILI9488_TransferData(ili9488_interface_t interface, uint8_t data)
{
    uint8_t u8_dummy_data;
    
    // ﾃﾞ-ﾀﾓ-ﾄﾞ設定
    *(interface.spi_dc_port) |= (1 << interface.spi_dc_pin);
    
    // ﾃﾞ-ﾀ送信処理(ﾓ-ﾄﾞに応じて送受信)
    u8_dummy_data = SoftSPI_Transfer(data, SOFT_SPI_MSB_FIRST);
}

// 関数名   : ILI9488_Initialize
// 機能     : ILI9488(3.2inch):X480xY320
// 引数     : 
// 戻り値   : 
// 使用条件 : SPIﾓ-ﾄﾞ3で動作(必須条件) CKE=0 CKP=1の設定でないと動作しない(ﾃﾞ-ﾀｼ-ﾄ記載無し)(CPOL=1/CPHA=1)
// CKE=0 : ﾃﾞ-ﾀ転送はｸﾛｯｸの立ち上がり(LOW⇒HIGH)で行う。
// CKP=1 : ｸﾛｯｸｱｲﾄﾞﾙ状態はHIGH
ili9488_interface_t ILI9488_Initialize(ili9488_interface_t interface)
{
    SoftSPI_InitDataInPin(interface.spi_miso_port, interface.spi_miso_pin);
    SoftSPI_InitDataOutPin(interface.spi_mosi_port, interface.spi_mosi_pin);
    SoftSPI_InitClockPin(interface.spi_clock_port, interface.spi_clock_pin);
    SoftSPI_InitSelectPin(interface.spi_cs_port, interface.spi_cs_pin);


    ILI9488_Reset(interface);

    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    // reset chip select pin
    *(interface.spi_cs_port) &= ~(1 << interface.spi_cs_pin);
    // SPI1_ILI9488_CS     = 0x00;
    
    // Normal display mode ON (13h)
    ILI9488_SendCommand(interface, ILI9488_NORMAL_DISPLAY_MODE_ON);

    // IDLE Mode OFF (39h)
    ILI9488_SendCommand(interface, ILI9488_IDLE_MODE_ON);
    
    // Interface Pixel Format (3Ah) = 0x01 (8bit)
    ILI9488_SendCommand(interface, ILI9488_PIXEL_FORMAT_SET);
    ILI9488_SendData(interface, 0x66);
    //ILI9488_WriteData_Proc(0x55);   // 16bit (default=18bit 0x66)
    
    // Memory Access Control (36h) : 0x48
    // 0x88 (MY=1, MX=0, MV=0) → 左下原点、横向き
    ILI9488_SendCommand(interface, ILI9488_MEMORY_ACCESS_CONTROL);
    //ILI9488_WriteData_Proc(0x48);
    ILI9488_SendData(interface, 0x88);
    
    // Frame Rate Control (B2h)
    ILI9488_SendCommand(interface, ILI9488_FRAME_RATE_CONTROL);
    ILI9488_SendData(interface, 0x00);  // 推奨値（データシート依存）
    ILI9488_SendData(interface, 0x80);

    // Display Function Control (B6h)
    ILI9488_SendCommand(interface, ILI9488_DISPLAY_FUNCTION_CONTROL);
    ILI9488_SendData(interface, 0x0A);
    ILI9488_SendData(interface, 0x82);

    ILI9488_SendCommand(interface, ILI9488_WRITE_DISPLAY_BRIGHTNESS);
    ILI9488_SendData(interface, 0xFF); // 最大輝度
        
    // Display ON (29h)
    ILI9488_SendCommand(interface, ILI9488_DISPLAY_ON);

    // Sleep OUT (11h)
    ILI9488_SendCommand(interface, ILI9488_SLEEP_OUT);

    __delay_ms(5);
    
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    // SPI1_ILI9488_CS     = 0x01;
    *(interface.spi_cs_port) |= (1 << interface.spi_cs_pin);
}

// 関数名   : ILI9488_Set_Window
// 機能     : 描画範囲を指定(指定してX・Y始点を起点に描画を開始する、自動的にｷｬﾘｯｼﾞﾘﾀ-ﾝする)
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void ILI9488_Set_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    // X範囲設定
    ILI9488_SendCommand(0x2B);
    
    // X軸始点を8ﾋﾞｯﾄ単位でSPIﾊﾞｯﾌｧへ送信
    ILI9488_SendData(x >> 8);
    ILI9488_SendData(x & 0xFF);
    
    // X軸終点を8ﾋﾞｯﾄ単位でSPIﾊﾞｯﾌｧへ送信(0 + 480 - 1 = 479)
    ILI9488_SendData((x + w - 1) >> 8);
    ILI9488_SendData((x + w - 1) & 0xFF);
    
    // Y範囲設定
    ILI9488_SendCommand(0x2A);
    
    // Y軸始点を8ﾋﾞｯﾄ単位でSPIﾊﾞｯﾌｧへ送信
    ILI9488_SendData(y >> 8);
    ILI9488_SendData(y & 0xFF);
    
    // Y軸終点を8ﾋﾞｯﾄ単位でSPIﾊﾞｯﾌｧへ送信(0 + 360 - 1 = 359)
    ILI9488_SendData((y + h - 1) >> 8);
    ILI9488_SendData((y + h - 1) & 0xFF);
    
    ILI9488_SendCommand(0x2C);
}

// 関数名   : DMA_ILI9488_BackFround_Color
// 機能     : ILI948の背景色を塗り潰し、8ﾋﾞｯﾄ(1ﾊﾞｲﾄ)単位でDMA送信
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void DMA_ILI9488_FillBackground(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t R, uint8_t G, uint8_t B)
{
    // SPI1受信ﾊﾞｯﾌｧをﾘﾌﾚｯｼｭ
    SPI1_FlushRX();
    
    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    SPI1_ILI9488_CS     = 0x00;
    
    // 描画範囲を設定(X軸始点,Y軸始点,幅指定,高さ指定)
    ILI9488_Set_Window(x, y, w, h);
   
    // SPIｸﾛｯｸﾎﾞ-ﾚ-ﾄ設定(最大ｸﾛｯｸ周波数:20MHzで動作)
    SPI1BRG = 80000000 / (2 * (20000000)) - 1;
    
    // 1ﾗｲﾝ分の幅分までを配列へｶﾗ-ﾃﾞ-ﾀをｾｯﾄ
    for (i = 0; i < w; i++)
    {
        Color_Packet_One_Line[(i * 3) + 0] = (R & 0xFC);  // Red
        
        Color_Packet_One_Line[(i * 3) + 1] = (G & 0xFC);  // Green
        
        Color_Packet_One_Line[(i * 3) + 2] = (B & 0xFC);  // Blue
    }
        
    // 全配列の要素数を確認(指定した描画幅からﾊﾞｲﾄ数を得る)
    // ﾌﾙ画面ﾍﾞﾀ塗りの場合:1ﾗｲﾝ(480ﾋﾟｸｾﾙ)×24ﾋﾞｯﾄ=11520ﾋﾞｯﾄ(1440ﾊﾞｲﾄ)
    //uint16_t size = sizeof(Color_Packet_One_Line);
    uint16_t size = (w * 24) / 8;
    
    // 画面縦領域360行にｶﾗ-ｾｯﾄする
    for(int j = 0; j < h; j++)
    {
        DMA0_to_SPI1_Data_Transmission_Proc(Color_Packet_One_Line,size);
        
        // 最低限のRX掃除(ENHBUF=1だと特に効く)
        while (SPI1STATbits.SPIRBF)
        {
            (void)SPI1BUF;
        }
        
        if (SPI1STATbits.SPIROV) SPI1STATCLR = _SPI1STAT_SPIROV_MASK;        
    }
    
    while (SPI1STATbits.SPIBUSY){}

    // ﾁｯﾌﾟｾﾚｸﾄ発行(ﾀ-ｹﾞｯﾄﾃﾞﾊﾞｲｽ指定)
    SPI1_ILI9488_CS     = 0x01;
    
    // SPIｸﾛｯｸﾎﾞ-ﾚ-ﾄ設定(最大ｸﾛｯｸ周波数:10MHzで動作)
    SPI1BRG = 80000000 / (2 * (10000000)) - 1;
    
    // SPI1の受信ﾊﾞｯﾌｧおよびｴﾗ-ﾌﾗｸﾞをｸﾘｱする
    SPI1_FlushRX();
}

// 関数名   : DMA_ILI9488_DrawLineFromBuffer
// 機能     : SDから読み込んだ1ﾗｲﾝ分のﾃﾞ-ﾀをILI9488へDMA転送
// 引数     : x,y   → 描画開始座標
//          : w     → 描画幅(ﾋﾟｸｾﾙ数)
//          : pLine → 1ﾗｲﾝ分のRGBﾃﾞ-ﾀ(8ﾋﾞｯﾄ×3ﾊﾞｲﾄ×w個)
// 戻り値   : なし
void DMA_ILI9488_DrawLineFromBuffer(uint16_t x, uint16_t y, uint16_t w, uint8_t *pLine)
{
    // SPI1受信ﾊﾞｯﾌｧをﾘﾌﾚｯｼｭ
    SPI1_FlushRX();
    
    // ﾁｯﾌﾟｾﾚｸﾄ(ILI9488を選択)
    SPI1_ILI9488_CS = 0x00;
    
    // 描画範囲を1ﾗｲﾝに設定
    ILI9488_Set_Window(x, y, w, 1);
    


    // SPIｸﾛｯｸを20MHzへ設定
    SPI1BRG = 80000000 / (2 * (20000000)) - 1;

    // 1ﾗｲﾝのﾊﾞｲﾄ数 (RGB888互換: 3ﾊﾞｲﾄ×wﾋﾟｸｾﾙ)
    uint16_t size = (w * 24) / 8;
    
    // デバッグ出力: 先頭16バイトを確認
    /*printf("DMA send check (y=%d, size=%d)\r\n", y, size);
    for (int i = 0; i < 16; i++)
    {
        printf("%02X ", pLine[i]);
    }
    printf("\r\n");*/
        
    // DMAで転送開始
    DMA0_to_SPI1_Data_Transmission_Proc(pLine, size);

    // DMA完了待ち
    while (DCH0CONbits.CHEN);

    // SPI完了待ち
    while (SPI1STATbits.SPIBUSY);




    // 受信掃除
    while (SPI1STATbits.SPIRBF)
    {
        (void)SPI1BUF;
    }
    if (SPI1STATbits.SPIROV)
    {
        SPI1STATCLR = _SPI1STAT_SPIROV_MASK;
    }

    // 転送完了待ち
    while (SPI1STATbits.SPIBUSY) {}

    // ﾁｯﾌﾟｾﾚｸﾄ解除
    SPI1_ILI9488_CS = 0x01;

    // SPIを10MHzへ戻す
    SPI1BRG = 80000000 / (2 * (10000000)) - 1;

    // SPIﾊﾞｯﾌｧ掃除
    SPI1_FlushRX();
}

// 関数名   : DMA_ILI9488_Border_Drow
// 機能     : 指定した座標を起点に幅・高さ分の罫線を書く
// 引数     : 
// 戻り値   : 
// 使用条件 : 
void DMA_ILI9488_DrawBorder(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t R, uint8_t G, uint8_t B)
{
    // 罫線描画(下ﾗｲﾝ)
    DMA_ILI9488_FillBackground(x, y, w, 1 , R, G, B);
    
    // 罫線描画(上ﾗｲﾝ)(Y始点=Y始点+高さ指定)
    DMA_ILI9488_FillBackground(x, y + h, w, 1 , R, G, B);
    
    // 罫線描画(左ﾗｲﾝ)
    DMA_ILI9488_FillBackground(x, y, 1, h , R, G, B);
    
    // 罫線描画(右ﾗｲﾝ)(X始点=X始点+幅指定)
    DMA_ILI9488_FillBackground(x + w, y, 1, h , R, G, B);
}






