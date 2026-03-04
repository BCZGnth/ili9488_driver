/**
 * @file ili9488_utils.h
 * @brief Optimized utility functions for ILI9488 driver
 * 
 * This file contains replacement functions for heavy standard library
 * functions like sn// printf to save flash space on PIC18.
 */

#ifndef ILI9488_UTILS_H
#define ILI9488_UTILS_H

#include <stdint.h>

/**
 * @brief Convert int32_t to string without using sn// printf
 * 
 * This function replaces sn// printf for integer conversion, saving
 * approximately 8-12KB of flash memory by avoiding the // printf library.
 * 
 * @param num The 32-bit integer to convert
 * @param buf Buffer to store the string (must be at least 12 bytes for int32_t)
 * @param bufsize Size of the buffer
 * @return Number of characters written (excluding null terminator)
 * 
 * @note Maximum output for int32_t: "-2147483648" = 11 chars + null = 12 bytes
 */
static inline uint8_t int32_to_str(int32_t num, char* buf, uint8_t bufsize) {
    uint8_t i = 0;
    uint8_t negative = 0;
    int32_t temp = num;
    
    // Handle negative numbers
    if (num < 0) {
        negative = 1;
        num = -num;
    }
    
    // Handle zero special case
    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return 1;
    }
    
    // Convert digits in reverse order
    while (num > 0 && i < bufsize - 1) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // Add negative sign if needed
    if (negative && i < bufsize - 1) {
        buf[i++] = '-';
    }
    
    buf[i] = '\0';
    
    // Reverse the string
    uint8_t start = 0;
    uint8_t end = i - 1;
    while (start < end) {
        char tmp = buf[start];
        buf[start] = buf[end];
        buf[end] = tmp;
        start++;
        end--;
    }
    
    return i;
}

/**
 * @brief Convert uint32_t to string without using sn// printf
 * 
 * Optimized version for unsigned integers (slightly faster, no sign handling)
 * 
 * @param num The 32-bit unsigned integer to convert
 * @param buf Buffer to store the string (must be at least 11 bytes for uint32_t)
 * @param bufsize Size of the buffer
 * @return Number of characters written (excluding null terminator)
 * 
 * @note Maximum output for uint32_t: "4294967295" = 10 chars + null = 11 bytes
 */
static inline uint8_t uint32_to_str(uint32_t num, char* buf, uint8_t bufsize) {
    uint8_t i = 0;
    
    // Handle zero special case
    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return 1;
    }
    
    // Convert digits in reverse order
    while (num > 0 && i < bufsize - 1) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    buf[i] = '\0';
    
    // Reverse the string
    uint8_t start = 0;
    uint8_t end = i - 1;
    while (start < end) {
        char tmp = buf[start];
        buf[start] = buf[end];
        buf[end] = tmp;
        start++;
        end--;
    }
    
    return i;
}

/**
 * @brief Pack 3-bit color into byte format for ILI9488
 * 
 * The ILI9488 in 3-bit mode packs 2 pixels per byte.
 * This helper ensures consistent color packing across the driver.
 * 
 * @param color 3-bit color value (0-7)
 * @return Packed byte with color in both nibbles
 */
static inline uint8_t pack_3bit_color(uint8_t color) {
    color &= 0x07;  // Ensure only 3 bits
    return (color << 3) | color;
}

/**
 * @brief Calculate pixel count and ensure even number
 * 
 * ILI9488 in 3-bit mode needs even pixel counts (2 pixels per byte).
 * This helper rounds up to nearest even number.
 * 
 * @param pixels Raw pixel count
 * @return Even pixel count (rounded up if necessary)
 */
static inline uint24_t make_even_pixels(uint24_t pixels) {
    return pixels + (pixels & 1);
}

/**
 * @brief Convert even pixel count to byte count
 * 
 * For 3-bit mode: 2 pixels = 1 byte
 * 
 * @param pixels Even pixel count
 * @return Number of bytes needed
 */
static inline uint24_t pixels_to_bytes(uint24_t pixels) {
    return pixels >> 1;  // Divide by 2 (faster than division)
}

#endif // ILI9488_UTILS_H
