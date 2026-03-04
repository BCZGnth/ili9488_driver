/**
 * @file main_payload_example.c
 * @brief Example showing how to use the payload menu system
 * 
 * This follows your existing code pattern and shows how to integrate
 * the new menu system into your application.
 */

#include "payload_menu.h"
#include "ili9488.h"
#include "system.h"

// -----------------------------------------------------------------------------
// Global Variables (you likely already have these defined)
// -----------------------------------------------------------------------------

/* Touch flag structure - define this in your main code or header */
struct {
    unsigned screen_touched : 1;
    /* Add your other flags here */
} payload_flags;

/* Touch coordinates - used by menu system */
Touch user_touch;

/* Whitelist data */
#define WHITELIST_MAX_SIZE 10
uint32_t whitelist_array[WHITELIST_MAX_SIZE];

SNWhitelist my_whitelist = {
    .arr = whitelist_array,
    .current = 0,
    .length = WHITELIST_MAX_SIZE
};

// -----------------------------------------------------------------------------
// Interrupt Service Routine (ISR) - Touch Detection
// -----------------------------------------------------------------------------

/**
 * Your existing ISR that sets the screen_touched flag
 * 
 * This is called when the touch IRQ pin changes state
 */
void __interrupt() isr(void) {
    
    /* Check for touch interrupt */
    if(IOCBFbits.IOCBF0) {  // Example: Interrupt on RB0
        IOCBFbits.IOCBF0 = 0;  // Clear flag
        
        /* Set flag to indicate screen was touched */
        payload_flags.screen_touched = 1;
        
        /* Small debounce delay */
        __delay_ms(50);
    }
    
    /* Add other interrupt handlers here */
    // if(PIR0bits.TMR0IF) { ... }
    // etc.
}

// -----------------------------------------------------------------------------
// Main Application
// -----------------------------------------------------------------------------

void main(void) {
    
    /* Initialize hardware */
    // Your hardware initialization code here
    // - Oscillator configuration
    // - Pin configuration
    // - SPI initialization
    // - etc.
    
    /* Initialize display */
    uint8_t spi_buffer[256];
    
    ili9488_interface_t display_interface = {
        .lcd_rst_port = &PORTA,  // Adjust for your hardware
        .lcd_rst_pin = 0         // Adjust for your hardware
    };
    
    Ili9488Defines screen = ili9488_initialize(
        display_interface,
        spi_buffer,
        sizeof(spi_buffer)
    );
    
    /* Setup touch interrupt */
    // Configure RB0 (or your touch IRQ pin) as input with interrupt-on-change
    // TRISBbits.TRISB0 = 1;        // Input
    // ANSELBbits.ANSELB0 = 0;      // Digital
    // WPUBbits.WPUB0 = 1;          // Weak pull-up
    // IOCBPbits.IOCBP0 = 1;        // Positive edge
    // IOCBNbits.IOCBN0 = 1;        // Negative edge
    // PIE0bits.IOCIE = 1;          // Enable IOC interrupt
    // INTCONbits.GIE = 1;          // Enable global interrupts
    // INTCONbits.PEIE = 1;         // Enable peripheral interrupts
    
    /* Optional: Add some test data to whitelist */
    whitelist_array[0] = 12345678;
    whitelist_array[1] = 87654321;
    whitelist_array[2] = 11111111;
    my_whitelist.current = 3;
    
    /* Show splash screen */
    ili9488_cls(screen);
    Ili9488Print splash = {
        .text = "INITIALIZING...",
        .fg = CYAN,
        .bg = BLACK,
        .font = screen.Screen.offset_2x,
        .ram_ptr = {
            .start_x = 220,
            .start_y = 80,
            .end_x = 260,
            .end_y = 240,
        },
    };
    ili9488_print(screen, splash);
    __delay_ms(1000);
    
    /* Run the main menu - this blocks until user selects "Exit" */
    doMainMenu(&my_whitelist, &screen);
    
    /* User has exited the menu */
    ili9488_cls(screen);
    
    Ili9488Print goodbye = {
        .text = "GOODBYE",
        .fg = WHITE,
        .bg = BLACK,
        .font = screen.Screen.offset_2x,
        .ram_ptr = {
            .start_x = 220,
            .start_y = 100,
            .end_x = 260,
            .end_y = 200,
        },
    };
    ili9488_print(screen, goodbye);
    
    /* Idle loop or low power mode */
    while(1) {
        __delay_ms(1000);
        
        /* Optional: Wake on touch and restart menu */
        if(payload_flags.screen_touched) {
            payload_flags.screen_touched = 0;
            
            /* Wait for release */
            while(TOUCH_IRQ_GetValue() == 0) {
                __delay_ms(10);
            }
            
            __delay_ms(200);  // Debounce
            
            /* Restart menu */
            doMainMenu(&my_whitelist, &screen);
            
            /* Redraw goodbye screen */
            ili9488_cls(screen);
            ili9488_print(screen, goodbye);
        }
    }
}

// -----------------------------------------------------------------------------
// Alternative: Polling-Based Touch Detection (No Interrupts)
// -----------------------------------------------------------------------------

/**
 * If you don't want to use interrupts, you can poll for touches
 */
void main_with_polling(void) {
    
    /* Same initialization as above */
    // ...
    
    /* Main loop with polling */
    while(1) {
        /* Poll touch status */
        if(TOUCH_IRQ_GetValue() == 0) {  // Touch detected (active low)
            payload_flags.screen_touched = 1;
            
            /* Small delay for debounce */
            __delay_ms(50);
            
            /* Wait for release */
            while(TOUCH_IRQ_GetValue() == 0) {
                __delay_ms(10);
            }
        }
        
        /* Check if we're in menu mode */
        // Your application logic here
        
        __delay_ms(10);  // Small delay to prevent CPU thrashing
    }
}

// -----------------------------------------------------------------------------
// Example: How to Implement Your Payload Functions
// -----------------------------------------------------------------------------

/**
 * In payload_menu.c, you have TODO markers where you need to add
 * your actual payload transmission code.
 * 
 * Here's an example of what that might look like:
 */

/*
void send_payload_1(Ili9488Defines* pscreen) {
    
    // Show status screen (already implemented in payload_menu.c)
    // ...
    
    // YOUR PAYLOAD CODE HERE:
    
    // Example: UART transmission
    uint8_t payload1[] = {
        0xAA, 0xBB,        // Header
        0x01,              // Payload ID
        0x11, 0x22, 0x33,  // Data
        0xFF               // Checksum
    };
    
    // Send via UART
    for(uint8_t i = 0; i < sizeof(payload1); i++) {
        UART1_Write(payload1[i]);
        __delay_ms(10);
        
        // Update progress indicator
        if(i % 2 == 0) {
            ili9488_loading_bar(pscreen);
        }
    }
    
    // Wait for transmission complete
    while(!UART1_IsTxDone()) {
        __delay_ms(10);
    }
    
    // Show completion (already implemented)
    // ...
}
*/

// -----------------------------------------------------------------------------
// Tips and Notes
// -----------------------------------------------------------------------------

/**
 * IMPORTANT NOTES:
 * 
 * 1. Make sure payload_flags.screen_touched is set by your ISR or polling loop
 * 
 * 2. The user_touch variable is populated by xpt_touch_getxy() when
 *    a touch is detected
 * 
 * 3. The menu system uses blocking loops - doMainMenu() won't return
 *    until the user selects "Exit"
 * 
 * 4. Each submenu (Send Payload, Whitelist) also uses blocking loops
 *    and returns to the main menu when complete
 * 
 * 5. Add your actual payload transmission code in the send_payload_X()
 *    functions in payload_menu.c (marked with TODO comments)
 * 
 * 6. The whitelist functionality is already implemented - you can
 *    add/clear serial numbers through the menu or programmatically
 * 
 * 7. Touch boxes are defined in payload_menu_draw.h - adjust the
 *    coordinates if needed for your screen calibration
 */
