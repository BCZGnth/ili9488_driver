#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ili9488_api.h"
#include "ili9488_commands.h"
#include "ili9488_font.h"
// #include "ili9488_driver.h"
#include "logger.h"


/**
 * @brief a function for writing a bitmap to the screen
 *
 * @param xlength is expected to be an integer value telling how many rows
 * @param ylength is expected to be length in bytes (how many bytes long in the y length)
 * @param bitmap expected to be a pointer to some value that is at leat a byte long so that
 */
// void ili9488_write_bitmap(ili9488_interface_t inter, Ili9488WriteBitmap args) {
//     /** Preliminary checks that may or may not be useful... if a number is bigger than it is supposed to be, the extra bits will probably just be ignored. */
//     // if( (args.xstart > args.xend) || (args.xstart > 128)) return;
//     // if( (args.ystart > args.yend) || (args.ystart > 128)) return;

//     /** Set RAM pointer constraints based on x and y values given */
//     ili9488_set_ram_pointer(inter, args.ram_ptr);

//     // NOTE: This is a horizontal bitmap write
//     ili9488_gram_write(inter, args.pbitmap, args.length);
// }

/**
 * Function loads a static buffer with the screen-ready pixel 
 */
size_t load_glyph_3bit(char c, uint8_t fg, uint8_t bg, FontOffset font_offset, uint8_t * pbuf, size_t len)
{
    // Calculate the "ord" index of the character
    uint8_t index = (c - font_offset.ascii);
    printf("loading character: \"%c\" with index %d\n", c, index);

    /* Number of pixels and bytes needed (2 pixels per byte) */
    uint16_t pixel_count = font_offset.height * font_offset.width;
    uint16_t byte_index = 0;

    // Create a static buffer that will hold the glyph while we write it to the screen outside of this function.

    /* Potential for memory corruption here... */
    // memset(c->pdata, 0, GLYPH_BUFFER_LENGTH);

    fg &= 0x07;  /* keep only 3 bits */
    bg &= 0x07;

    uint16_t out_pixel = 0;

    /**
     * font_data[index][column (inverted)] contains one column, MSB = left-bottom pixel
     * 
     */
    for (uint8_t glyph_byte = 0; glyph_byte < font_offset.bytes_per_char; glyph_byte++)
    {
        uint8_t glyph_bits = *(font_offset.pfont + (index * font_offset.bytes_per_char) + glyph_byte);
        printf("\n");
        for (uint8_t pixel = 7; pixel >= 0; pixel++)
        {
            uint8_t bit   = (glyph_bits >> (7 - pixel)) & 1;

            /* Debug */
            if(bit) { printf("1"); } else { printf("0"); }

            uint8_t color = bit ? fg : bg;

            byte_index = out_pixel / 2;

            if (byte_index >= len) break;

            if ((out_pixel & 1) == 0)
                *(pbuf + byte_index) |= (color & 0x07) << 3; // Pushing the n pixel to bits 5:3  
            else
                *(pbuf + byte_index) |= (color & 0x07);      // Keeping the n + 1 pixel in bits 2:0 
                                                             // See datasheet section 4.7.2.1 for more information
            out_pixel++;
        }

        if (byte_index >= len) break;
    }

    return byte_index;
}


/**
 * @brief A function to set the ram pointer to a position on the  (Dependent on the addressing mode)
 *
 * @param x_start defines the LEFT-most bit (pixel) or 8-bit page (if in page addressing mode)
 * @param x_end Not Implemented. For advanced box defining that will come in handy when writing text to the screen
 * @param y_start defines the TOP-most bit (pixel) or 8-bit page (if in page addressing mode)
 * @param y_end Not Implemented. For advanced box defining that will come in handy when writing text to the screen
 */
void ili9488_ram_write(ili9488_interface_t inter, Ili9488RamWrite args) {
    /** Set RAM pointer constraints based on x and y values given */
    ili9488_set_ram_pointer(inter, args.ram_ptr);

    // NOTE: This is a vertical Ram Write (due to madctl and madctr and some other registers.)
    ili9488_gram_write(inter, args.buf, args.buf_len);
}

/**
Each page has a capacity of 21 full characters. There are 8 pages,
so that gives us 168 characters at the max that the display can
print at one time.
*/



/**
 * @brief This function prints characters right justified. That means that you can only print as many characters as
 * @brief are specified in the length paramater of the function.
 *
 * @param     Screen: the screenDefines structure that general screen definitions
 * @param     args: the Ili9488WriteNumber struct that holds the specific paramaters for writing a number to the screen 
 *
 * This function first converts the integer data coming in, into string data using the snprintf function
 *
 * The snprintf function returns the number of characters written to the character buffer
 *
 * Then, based on the number of characters written into the buffer, we calculate how many psetup_bytes to pad
 * the i2c buffer with in order to right align the data.
 * (we need the number of bytes because the i2c buffer is loaded with zeros initialy
 *  This means that we just need to Screen.offset the pointer to where the font data
 *  is loaded into the i2c buffer)
 *
 * The i2c buffer is then loaded with zeros
 * (because each letter in the font is 5 bytes long, filling the buffer with zeros
 *  makes it possible to increment the pointer )
 *
 * Then the SSD1309_RAM_WRITE_BYTE is written to the load_i2c_buffer
 *
 * Next we write the characters to the i2c buffer via a for loop and the memcpy function
 *      the memcpy function copys memory from the source, and copies it to the destination
 *      its syntax is this:
 *                           memcpy(*Dest, *Source, how many bytes to copy)
 *
 *      for each loop of the iteration, the "n" variable keeps track of what character to write into the load_i2c_buffer
 *      The pointer to the destination buffer is calculated by:
 *
 *          Adding 1 to the pointer to the i2c buffer (don't overwrite the SSD1309_RAM_WRITE_BYTE)
 *          Adding (n * 6) to the pointer to the i2c buffer. The multiplier 6 is used because the characters are 5 bytes long and a pad byte is added in between each character
 *          Adding the right_aligh_character_offset to align the characters on the right side
 *
 *      The source pointer is calculated by:
 *
 *          Subtracting 32 from the character pointer stored in data_to_write[n] (the local font does not include the first 32 ascii characters)
 *          Multiplying that number by 5 (because each character takes up 5 bytes)
 *          Adding that number to a pointer to the font (to point to the correct character in the font array)
 *
 *      The length of data to copy is 5 because each character is 5 bytes long.
 *      the pointer buffer is incremented by 6 in order to put a column of space in between each character
 *      This is only possible because the i2c buffer is loaded with zeros before any data gets written to it.
 *
 */
// size_t ili9488_write_number(Ili9488Defines screen, Ili9488WriteNumber args) {

//     ADD_TO_STACK_DEPTH(); // ili9488_write_number
//     level_log(TRACE, "Writing Number: %d", args.data);

//     /** 
//      * Don't be fooled by the term "constrained length" This is just a parameter that is used to right align the text since the buffer gets written left to right. 
//      */

//     uint8_t write_length = (args.constrained_length * screen.Screen.character.width_pad);
//     if(screen.Screen.buffer_size < write_length) {
//         level_log(ERROR, "Buffer Size Too Small");
//         return 0;
//     }

//     ili9488_set_ram_pointer(screen.interface, args.ram_ptr);

//     uint8_t n;
//     uint8_t number_of_chars_written; // the snprintf function actually returns an integer value, but I hope that the amount of characters will never exceed 256...
//     unsigned char data_to_write[36]; // Don't really need to display numbers that are more than 6 digits long. 6 * 6 = 36
    
//     #ifndef USE_STATIC_BUFFERS
//     if (data_to_write == NULL)
//     {
//         level_log(ERROR, "SSD1309: Memory allocation failed for data_to_write");
//         return 0;
//     }
//     #endif // USE_STATIC_BUFFERS

//     number_of_chars_written = snprintf(&data_to_write[0], args.constrained_length, "%u", args.data); // putting zeros at the end of the string so that it is less noise to the viewer
//     if(number_of_chars_written <= 0) {
//         level_log(ERROR, "snprintf call did not write data to a buffer. Possibly you have a bad args.data");
//     }
//     uint8_t right_align_character_offset = (args.constrained_length - number_of_chars_written) * 6;
//     if(args.constrained_length < number_of_chars_written) {
//         level_log(WARNING, "constrained length smaller than chars written. Setting Right align character offset to ZERO to eliminate memory corruption possibilities");
//         right_align_character_offset = 0;
//     }
//     level_log(TRACE, "Right-align offset is %d", right_align_character_offset);

//     /* Loading all Zeros into the I2C buffer */
//     memset(Screen.pbuffer, 0, write_length);

//     memcpy(Screen.pbuffer, (&SSD1309_RAM_WRITE_BYTE), Screen.offset.control);

//     level_log(TRACE, "Loading the I2C buffer with %d numeric characters", number_of_chars_written);
//     for (n = 0; n < number_of_chars_written; n++)
//     { // Iterate through all of the characters in the string

//         /** Write 5 bytes from a (calculated pointer to a character in the font) to the i2c buffer */

//         /**      The pointer to the destination buffer is calculated by:
//          *
//          *          Adding 1 to the pointer to the i2c buffer (don't overwrite the SSD1309_RAM_WRITE_BYTE)
//          *          Adding (n * 6) to the pointer to the i2c buffer. The multiplier 6 is used because the characters are 5 bytes long and a pad byte is added in between each character
//          *          Adding the right_aligh_character_offset to align the characters on the right side
//          *
//          *      The source pointer is calculated by:
//          *
//          *          Subtracting 32 from the character pointer stored in data_to_write[n] (the local font does not include the first 32 ascii characters)
//          *          Multiplying that number by 5 (because each character takes up 5 bytes)
//          *          Adding that number to a pointer to the font (to point to the correct character in the font array)
//          *
//          *      The length of data to copy is 5 because each character is 5 bytes long.
//          *      the pointer buffer is incremented by 6 in order to put a column of space in between each character
//          *      This is only possible because the i2c buffer is loaded with zeros before any data gets written to it.
//          */
//         if(n * Screen.character.width_pad > Screen.buffer_size) {
//             level_log(WARNING, "Can not load any more characters into buffer on iteration %d", n);
//             break;
//         }
//         memcpy((Screen.pbuffer + (n * Screen.character.width_pad) + Screen.offset.control + right_align_character_offset), (((data_to_write[n] - Screen.offset.ascii) * Screen.character.width) + Screen.offset.pfont), Screen.character.width);
//     }

//     ssd_write(Screen, write_length); // The number of bytes to write to the i2c buffer is the number of characters multiplied by the width of each character (5) plus the padding (1 byte) between each character

//     // Memory addressing mode back to page addressing
//     ILI9488_SendCommand(Screen, SET_MEMORY_ADDRESSING_MODE, PAGE_ADDRESSING);

//     // Set the column range back to its reset value
//     ILI9488_SendCommand(Screen, SET_COLUMN_ADDRESS, 0x0, 0xFF);

//     // Set the page range back to its reset value
//     ILI9488_SendCommand(Screen, SET_PAGE_ADDRESS, 0x0, 0x7);

//     level_log(TRACE, "SSD1309: Done Writing Number");
//     REMOVE_FROM_STACK_DEPTH(); // ili9488_write_number

//     return args.constrained_length * 6;
// }


/**
 * @param     Screen: the screenDefines structure that general screen definitions
 * @param     args: the Ili9488WriteNumber struct that holds the specific paramaters for writing a number to the screen 
 *
 * @brief A function to print a string on the screen
 */
size_t ili9488_print(Ili9488Defines screen, Ili9488Print args) {

    ADD_TO_STACK_DEPTH(); // ili9488_print    
    level_log(TRACE, "Printing: \"%s\"", args.text);

    // uint16_t tmp16; // A variable to hold the 16 bit value of a scaled byte
    // uint8_t tmp[2]; // tmp16 will get split into these two bytes to get the right endianness when sending the data over the i2c bus.
    uint8_t preserve_frame = 0;
    uint16_t box_width = args.ram_ptr.end_column - args.ram_ptr.start_column;
    uint16_t box_height = args.ram_ptr.end_row - args.ram_ptr.start_row;
    uint16_t xoff;
    uint16_t yoff;
    uint8_t write_height = char_bit_height_1x;
    size_t write_len;
    uint8_t char_pad = 2;
    size_t chars_written = 0;
    uint16_t row_increment = 0;

    Ili9488RamPointer char_placement;
    // Glyph char_attrs = {
    //     .character = '\0',
    //     .height = char_bit_height_1x,
    //     .width = char_bit_width_1x,
    // };

    /* Error Checks */
    // Length is zero
    // if(!args.length) {
    //     level_log(ERROR, "type <Ili9488Print> args.length not defined");
    //     return 0;
    // }
    // // If the bytes length will be larger than the buffer size
    // if(write_size >  Screen.buffer_size) {
    //     level_log(ERROR, "Buffer Size Too Small");
    //     return 0;
    // }

    /* Constrain the columns so that we skip the first and last. This way frames don't get overwritten */
    // if(preserve_frame){
    //     ILI9488_SendCommand(screen.interface, SET_COLUMN_ADDRESS, 1, 126);
    // }

    // ili9488_set_ram_pointer(screen.interface, args.ram_ptr); // Put the cursor where specified in the args structure

    /** Loading the I2C buffer */
    /** No scaling applied, and the message is copied into the i2c buffer */
    level_log(TRACE, "Print: No scale applied");
    // Iterate through the entire string until a null terminator
    const char* msg_letter = args.text;
    unsigned char msg_char = *msg_letter;
    /**
     * For each character:
     * 1) set ram pointer based on offsets calculated from box_width, box_height, x_start, y_start, char_width, char_height, chars_written
     *      If char height is greater than box height, constrain to box_height
     *      offset is:
     *          x)
     *              xoff = (char_width * chars_written);
     *              if(xoff + char_width > box_width) {
     *                  // Reset xoff and intcrement row counter
     *                  xoff = 0;
     *                  row_increment += 1
     *              }
     *          y)
     *              yoff = (char_height * row_increment);
     *              if(yoff + char_height > box_height) {
     *                  if ((yoff + char_height) - box_height) {
     *                      continue
     *                  }
     *                  // Constrain char_height to only go to the bottom of the box
     *                  write_height = (yoff + char_height) - box_height;
     *              }
     * 
     * NEEDS)
     *      row_increment
     *      yoff
     *      xoff
     *      write_height
     *      chars_written
     *      box_height
     *      box_width
     */
    while( msg_char != '\0')
    { // Iterate through all of the characters in the string
        
        xoff = (char_bit_width_1x + char_pad) * chars_written;
        if(xoff + char_bit_width_1x > box_width) {
            // Reset xoff and intcrement row counter
            xoff = 0;
            row_increment += 1;
        }
        yoff = (char_bit_height_1x * row_increment);
        if(yoff + char_bit_height_1x > box_height) {
            if ((yoff + char_bit_height_1x) - box_height) {
                continue;
            }
            // Constrain char_height to only go to the bottom of the box
            write_height = (uint8_t)((yoff + (uint16_t)char_bit_height_1x) - box_height);
        }

        // Calculate the ram_pointer based on offsets:
        char_placement.start_column = args.ram_ptr.start_column + xoff;
        char_placement.start_row    = args.ram_ptr.start_row + yoff;
        char_placement.end_column   = char_placement.start_column + char_bit_width_1x  /*- 1 To maintain Zero Indexing */;
        char_placement.end_row      = char_placement.start_row + write_height  /*- 1 To Maintain Zero Indexing */;
        printf("Char x = %d, Char y = %d, xoff = %d, yoff = %d", char_placement.start_column, char_placement.start_row, xoff, yoff);
        ili9488_set_ram_pointer(screen.interface, char_placement);

        // char_attrs.character = msg_char;

        /* The write_len variable will always be smaller or equal to the screen.Screen.buffer_size */
        write_len = load_glyph_3bit(msg_char, WHITE, BLACK, screen.Screen.offset, screen.Screen.pbuffer, screen.Screen.buffer_size);
        /* but, just in case */
        if (write_len > screen.Screen.buffer_size) {
            write_len = screen.Screen.buffer_size;
        }

        ili9488_gram_write(screen.interface, screen.Screen.pbuffer, write_len);
        
        /* We may potentially want to make word boundary aware printing...but not today.*/
        // for (unsigned char * word_letter = msg_letter; word_letter != '\0'; word_letter++) 
        // {
        //     continue;
        // }

        msg_letter++;
        msg_char = *(msg_letter);
        chars_written++;

        printf("MSG Pointer: %p - MSG Char: %c - Chars Written: %d\n", msg_letter, msg_char, chars_written);
    }
    
    // Memory addressing mode back to page addressing

    level_log(TRACE, "Print: Done Printing.");
    REMOVE_FROM_STACK_DEPTH(); // ili9488_print
    return (args.length * screen.Screen.character.width_pad);
}


void ili9488_cls(Ili9488Defines screen)
{
    ADD_TO_STACK_DEPTH();
    level_log(TRACE, "Clearing the screen");

    /* Calculate the most efficient way to clear the screen with the given buffer */
    // Calculate total screen size in bytes (assuming 1 bit per pixel, 8 pixels per byte vertically)
    uint32_t total_screen_bytes = ((uint32_t)screen.Screen.ScreenWidth) * ((uint32_t)screen.Screen.ScreenHeight) / (uint32_t)2;

    // Use the full buffer size for clearing (assuming clear_length is the usable buffer size)
    /* We will also assume that the Screen.buffer_size is greater than 1 */
    // unsigned int clear_length = Screen.buffer_size;

    /* No Error check needed since clear_length will always be less than Screen.buffer_size */
    // if(clear_length > Screen.buffer_size) {
    //     level_log(ERROR, "Buffer Size Too Small");
    //     return;
    // }

    // Calculate the number of full iterations needed
    uint32_t iterations = total_screen_bytes / (uint32_t)screen.Screen.buffer_size;
    uint32_t remainder = total_screen_bytes % (uint32_t)screen.Screen.buffer_size;

    // Log debug information
    level_log(TRACE, "Clear Length is: %d", screen.Screen.buffer_size);
    level_log(TRACE, "Screen Width: %d", screen.Screen.ScreenWidth);
    level_log(TRACE, "Screen Height: %d", screen.Screen.ScreenHeight);
    level_log(TRACE, "Total Screen Bytes: %d", total_screen_bytes);
    level_log(TRACE, "Number of Iterations: %d", iterations);
    level_log(TRACE, "Remainder Bytes: %d", remainder);
    
    // Initialize the buffer with zeros (clear screen data)
    memset(screen.Screen.pbuffer, 0, screen.Screen.buffer_size);

    
    Ili9488RamPointer full_screen = {
        .start_row = 0,
        .end_row = screen.Screen.ScreenHeight - 1,
        .start_column = 0,
        .end_column = screen.Screen.ScreenWidth - 1
    };

    ili9488_set_ram_pointer(screen.interface, full_screen);

    // Perform full buffer writes
    ili9488_gram_write(screen.interface, screen.Screen.pbuffer, screen.Screen.buffer_size);
    for (uint32_t i = 1; i < iterations; i++) {
        ili9488_gram_write_continue(screen.interface, screen.Screen.pbuffer, screen.Screen.buffer_size);
    }
    
    level_log(TRACE, "Wrote iterations, Now writing remainder");

    // Handle any remaining bytes
    if (remainder > 0) {
        ili9488_gram_write_continue(screen.interface, screen.Screen.pbuffer, (size_t)remainder);
     }

    level_log(TRACE, "SSD1309: Screen Cleared");
    REMOVE_FROM_STACK_DEPTH();
}


// void ili9488_clear_block(Ili9488Defines screen, Ili9488RamPointer args)
// {

//     ADD_TO_STACK_DEPTH();

//     level_log(TRACE, "Clearing the screen");

//     /** Memory Corruption Error Checking */
//     if(screen.Screen.buffer_size < screen.Screen.ScreenWidth) {
//         level_log(ERROR, "Buffer Size Too Small");
//         return;
//     }

//     ili9488_set_ram_pointer(screen.interface, args);

//     #ifndef USE_STATIC_BUFFERS
//     Screen.pbuffer = malloc(129);
//     if(!Screen.pbuffer){
//         level_log(ERROR, "Memory allocation failed for the I2C buffer");
//         REMOVE_FROM_STACK_DEPTH();
//         return;
//     }
//     #endif

//     for (uint8_t i = args.start_page; i < args.end_page; i++)
//     {
//         ssd_write(Screen, Screen.ScreenWidth + Screen.offset.control);
//     }
//     // ssd_write(9); // Compensate for the control byte at the start of the buffer. (The for loop really only writes 127 bytes to the screen)

//     level_log(TRACE, "SSD1309: Screen Cleared");
//     REMOVE_FROM_STACK_DEPTH();
// }

// void ili9488_clear_word(ScreenDefines Screen, Ili9488Clear args)
// {
//     ADD_TO_STACK_DEPTH();
//     level_log(TRACE, "Clearing a word of length %d", args.char_length);

//     ili9488_set_ram_pointer(Screen, args.ram_ptr);

//     #ifndef USE_STATIC_BUFFERS
//     Screen.pbuffer = malloc(129);
//     if(!Screen.pbuffer){
//         level_log(ERROR, "Memory allocation failed for the I2C buffer");
//         REMOVE_FROM_STACK_DEPTH();
//         return;
//     }
//     #endif
//     #ifdef USE_STATIC_BUFFERS
//     if( (Screen.character.width_pad * args.char_length + Screen.offset.control) > Screen.buffer_size) { 
//         level_log(ERROR, "Buffer Too Small");
//         return; 
//     }
//     #endif

//     // Set the bytes that we need to zero. Length of each character plus the length of the pad between characters plus the length of the control data
//     memset(Screen.pbuffer, 0, (Screen.character.width_pad * args.char_length) + Screen.offset.control);
//     memcpy(Screen.pbuffer, (&SSD1309_RAM_WRITE_BYTE), 1);

//     // Write all the bytes that we manipulated
//     ssd_write(Screen, (Screen.character.width_pad * args.char_length) + Screen.offset.control);

//     level_log(TRACE, "SSD1309: Word(s) Cleared");
//     REMOVE_FROM_STACK_DEPTH();
// }

// void ili9488_blinking_cursor(ScreenDefines Screen, Ili9488Cursor args)
// {
//     /** Start by keeping track of how many function calls deep we are */
//     ADD_TO_STACK_DEPTH();
//     level_log(TRACE, "Blinking Cursor: page %u, column %u, repeats %u", args.ram_ptr.page, args.ram_ptr.position, args.repeats);

//     size_t size;

//     if(Screen.offset.control + 5 > Screen.buffer_size) {
//         level_log(ERROR, "Buffer Size Too Small");
//     }

//     /** malloc so that the cursor doesn't take up extra space */
//     uint8_t cursor[5];

//     ili9488_set_ram_pointer(Screen, args.ram_ptr);

//     /** Modifying repeats to translate from number of repeats to iterations of the for loop that have to run */
//     args.repeats *= 2;
//     /** Using the pre-increment operator rather than adding one to repeats */
//     args.repeats += 1; // Add one to make sure we end on a blank cursor 

//     for (int i = 1; i < args.repeats; ++i)
//     {

//         /** Decide whether to set the cursor all zeros or all ones */
//         memset(&cursor, ((i % 2) * 0xFF), 5);

//         /** Loading and writing the i2c buffer */
//         size = load_i2c_buffer(Screen,(uint8_t*)(&SSD1309_RAM_WRITE_BYTE), 1, cursor, 5);
//         ssd_write(Screen, size);

//         ili9488_set_ram_pointer(Screen, args.ram_ptr);
//         __delay_ms(200);
//     }

//     level_log(TRACE, "Blinking Cursor: Done Blinking Cursor");
//     REMOVE_FROM_STACK_DEPTH();
// }

// void ili9488_progress_bar(ScreenDefines Screen )
// {
//     return;
// }

// void ili9488_waiting(ScreenDefines Screen)
// {
//     ADD_TO_STACK_DEPTH();
//     level_log(TRACE, "SSD1309 Waiting...");
    
//     uint8_t animation_length = 14;
//     Ili9488RamPointer wait_ram_ptr = {
//         .page = 7,
//         .position = 112
//     };

//     /* Memory Corruption Error Checking */
//     if(Screen.buffer_size < animation_length + Screen.offset.control) { 
//         level_log(ERROR, "Buffer Too Small"); 
//         return; 
//     }

//     ili9488_set_ram_pointer(Screen, wait_ram_ptr);

//     uint8_t dots[24] = {0, 0xc0, 0xc0, 0, 0,  0, 0xc0, 0xc0, 0, 0,  0, 0xc0, 0xc0, 0, 0,  0, 0, 0, 0, 0,   0, 0, 0, 0 };
    
//     /* Depending on where we are in the cycle, either add another dot, or clear the space. 
//        We are using arrays instead of the ili9488_print function because this is at least a little faster. */
//     switch (Screen.pwait->three_ctr) {
//         case 0:
//             memset(Screen.pbuffer, SSD1309_RAM_WRITE_BYTE, Screen.offset.control);
//             memset(Screen.pbuffer + Screen.offset.control, 0, animation_length + Screen.offset.control);
//             ssd_write(Screen, animation_length + Screen.offset.control);
//             Screen.pwait->three_ctr += 1;
//             break;
//         case 1:
//         case 5:
//             memset(Screen.pbuffer, SSD1309_RAM_WRITE_BYTE, Screen.offset.control);
//             memcpy(Screen.pbuffer + Screen.offset.control, &dots + 10, animation_length + Screen.offset.control);
//             ssd_write(Screen, animation_length + Screen.offset.control);
//             Screen.pwait->three_ctr += 1;
//             break;
//         case 2:
//         case 4:
//             memset(Screen.pbuffer, SSD1309_RAM_WRITE_BYTE, Screen.offset.control);
//             memcpy(Screen.pbuffer + Screen.offset.control, &dots + 5, animation_length + Screen.offset.control);
//             ssd_write(Screen, animation_length + Screen.offset.control);
//             Screen.pwait->three_ctr += 1;
//             break;
//         case 3:
//             memset(Screen.pbuffer, SSD1309_RAM_WRITE_BYTE, Screen.offset.control);
//             memcpy(Screen.pbuffer + Screen.offset.control, &dots, animation_length + Screen.offset.control);
//             ssd_write(Screen, animation_length + Screen.offset.control);
//             Screen.pwait->three_ctr += 1;
//             break;
//         case 6:
//         default:
//         /* Fast default case */
//             memset(Screen.pbuffer, SSD1309_RAM_WRITE_BYTE, Screen.offset.control);
//             memset(Screen.pbuffer + Screen.offset.control, 0, animation_length + Screen.offset.control);
//             ssd_write(Screen, animation_length + Screen.offset.control);
//             Screen.pwait->three_ctr = 0;
//             break;
//         // default:
//         //     memset(Screen.pbuffer, SSD1309_RAM_WRITE_BYTE, Screen.offset.control);
//         //     memset(Screen.pbuffer + Screen.offset.control, 0, animation_length + Screen.offset.control);
//         //     ssd_write(Screen, animation_length * Screen.character.width);
//         //     Screen.pwait->three_ctr = 0;
//         //     break;
//     }

//     level_log(TRACE, "SSD1309 Done Waiting");
//     REMOVE_FROM_STACK_DEPTH();
//     return;
// }

