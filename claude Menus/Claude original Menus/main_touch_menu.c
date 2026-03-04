/**
 * @file main_touch_menu.c
 * @brief Main application entry point for touch menu system
 * 
 * This is a complete example showing how to integrate the touch menu
 * system into your PIC18F47K42 application.
 */

#include "touch_application_menus.h"
#include "ili9488.h"
#include "xpt2046.h"
#include "system.h"

// -----------------------------------------------------------------------------
// Application Initialization
// -----------------------------------------------------------------------------

void application_init(void) {
    // TODO: Add your hardware initialization here
    
    // Example initialization steps:
    // 1. Configure oscillator
    // 2. Setup I/O pins
    // 3. Initialize SPI peripherals
    // 4. Configure interrupts (if using)
    // 5. Enable peripherals
    
    // Example:
    // OSCCON1 = 0x60;  // HFINTOSC with 3x PLL
    // OSCFRQ = 0x08;   // 64 MHz
    
    // Configure SPI pins as outputs/inputs
    // Configure touch interrupt pin as input
    // etc.
}

// -----------------------------------------------------------------------------
// Main Application
// -----------------------------------------------------------------------------

void main(void) {
    // Initialize hardware
    application_init();
    
    // Initialize display
    uint8_t spi_buffer[256];  // Buffer for SPI operations
    
    ili9488_interface_t display_interface = {
        .lcd_rst_port = &PORTA,  // Adjust for your hardware
        .lcd_rst_pin = 0         // Adjust for your hardware
    };
    
    Ili9488Defines screen = ili9488_initialize(
        display_interface,
        spi_buffer,
        sizeof(spi_buffer)
    );
    
    // Optional: Show splash screen
    ili9488_cls(screen);
    Ili9488Print splash = {
        .text = "INITIALIZING...",
        .fg = CYAN,
        .bg = BLACK,
        .font = screen.Screen.offset_2x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 220,
            .start_y = 80,
            .end_x = 260,
            .end_y = 240
        }
    };
    ili9488_print(screen, splash);
    __delay_ms(1000);
    
    // Start the touch menu system
    // This function will block until user selects "Exit"
    touch_menu_run(&screen);
    
    // User has exited the menu system
    // Do cleanup or enter idle mode
    ili9488_cls(screen);
    
    Ili9488Print goodbye = {
        .text = "GOODBYE",
        .fg = WHITE,
        .bg = BLACK,
        .font = screen.Screen.offset_2x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 220,
            .start_y = 100,
            .end_x = 260,
            .end_y = 200
        }
    };
    ili9488_print(screen, goodbye);
    
    // Enter low power mode or restart
    while (1) {
        // Idle loop
        __delay_ms(1000);
        
        // Optional: Check for wake-up condition
        // If user touches screen, restart menu
        Touch wake_touch;
        if (xpt_touch_getxy(&wake_touch)) {
            // Wait for release
            while (TOUCH_IRQ_GetValue() == 0) {
                __delay_ms(10);
            }
            __delay_ms(200);
            
            // Restart menu system
            touch_menu_run(&screen);
        }
    }
}

// -----------------------------------------------------------------------------
// Alternative: With Timer Interrupt for Better Touch Detection
// -----------------------------------------------------------------------------

#ifdef USE_TIMER_INTERRUPT

volatile bool g_touch_pending = false;

void __interrupt() isr(void) {
    // Check for timer interrupt (example using Timer0)
    if (PIR0bits.TMR0IF) {
        PIR0bits.TMR0IF = 0;  // Clear flag
        
        // Increment touch menu timer
        touch_menu_tick_ms();  // Defined in touch_menu_system.c
        
        // Reload timer for 1ms interrupt
        // Adjust these values for your oscillator frequency
        TMR0H = 0xF0;
        TMR0L = 0x00;
    }
    
    // Optional: Touch interrupt on pin change
    // if (IOCxF & (1 << TOUCH_IRQ_PIN)) {
    //     IOCxF &= ~(1 << TOUCH_IRQ_PIN);  // Clear flag
    //     g_touch_pending = true;
    // }
}

void setup_timer_interrupt(void) {
    // Example Timer0 setup for 1ms interrupt @ 64MHz
    // Adjust for your clock frequency
    
    T0CON0 = 0x90;  // Timer0 enabled, 16-bit mode
    T0CON1 = 0x47;  // Fosc/4, prescaler 1:128
    
    TMR0H = 0xF0;   // Preload for ~1ms
    TMR0L = 0x00;
    
    PIE0bits.TMR0IE = 1;  // Enable Timer0 interrupt
    INTCONbits.GIE = 1;   // Enable global interrupts
    INTCONbits.PEIE = 1;  // Enable peripheral interrupts
}

void main_with_interrupts(void) {
    application_init();
    setup_timer_interrupt();
    
    // Rest of initialization same as main()
    // ...
    
    touch_menu_run(&screen);
    
    while (1) {
        // Idle
    }
}

#endif // USE_TIMER_INTERRUPT

// -----------------------------------------------------------------------------
// Debugging Helpers
// -----------------------------------------------------------------------------

#ifdef DEBUG_TOUCH

/**
 * @brief Simple touch test function
 * 
 * This function displays touch coordinates on screen for calibration
 * and testing purposes.
 */
void test_touch_display(Ili9488Defines* screen) {
    ili9488_cls(*screen);
    
    Ili9488Print title = {
        .text = "TOUCH TEST",
        .fg = YELLOW,
        .bg = BLACK,
        .font = screen->Screen.offset_2x,
        .clear_before = 0,
        .line_spacing = 0,
        .ram_ptr = {
            .start_x = 10,
            .start_y = 100,
            .end_x = 30,
            .end_y = 220
        }
    };
    ili9488_print(*screen, title);
    
    char coord_buffer[20];
    Touch t;
    
    while (1) {
        if (xpt_touch_getxy(&t)) {
            // Clear previous coordinates
            Ili9488FillBlock clear = {
                .color = BLACK,
                .ram_ptr = {
                    .start_x = 100,
                    .start_y = 50,
                    .end_x = 150,
                    .end_y = 270
                }
            };
            ili9488_fill_color(*screen, clear);
            
            // Display X coordinate
            Ili9488WriteNumber write_x = {
                .data = t.x,
                .right_aligned = 0,
                .clear_before = 0,
                .scale = 1,
                .fg = GREEN,
                .font = screen->Screen.offset_1x,
                .ram_ptr = {
                    .start_x = 100,
                    .start_y = 50,
                    .end_x = 120,
                    .end_y = 150
                }
            };
            ili9488_write_number(*screen, write_x);
            
            // Display Y coordinate
            Ili9488WriteNumber write_y = {
                .data = t.y,
                .right_aligned = 0,
                .clear_before = 0,
                .scale = 1,
                .fg = GREEN,
                .font = screen->Screen.offset_1x,
                .ram_ptr = {
                    .start_x = 120,
                    .start_y = 50,
                    .end_x = 140,
                    .end_y = 150
                }
            };
            ili9488_write_number(*screen, write_y);
            
            // Draw a dot at touch location
            ili9488_draw_pixel(t.x, t.y, RED, 1);
        }
        
        __delay_ms(50);
    }
}

#endif // DEBUG_TOUCH
