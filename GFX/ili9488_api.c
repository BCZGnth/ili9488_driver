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
 * 
 * The most significant bit of the font gets written 
 */
size_t load_glyph_3bit(char c, color_t fg, color_t bg, FontOffset font_offset, uint8_t * pbuf, size_t len)
{
    #define GLYPH_MSB 1
    #define GLYPH_LSB 0
    uint8_t bit_pack_order = GLYPH_MSB;
    /* Specify whether the bytes of the font are packed so 
    that the MSB is placed at the top or if the LSB is placed at the top */

    // Calculate the "ord" index of the character
    uint8_t index = (c - font_offset.ascii);

    /* Number of pixels and bytes needed (2 pixels per byte) */
    uint16_t pixel_count = font_offset.height * font_offset.width;
    uint16_t byte_index = 0;

    // Create a static buffer that will hold the glyph while we write it to the screen outside of this function.

    fg &= 0x07;  /* keep only 3 bits */
    bg &= 0x07;

    uint16_t out_pixel = 0;

    /**
     * font_data[index][column (inverted)] contains one column, MSB = left-bottom pixel
     * 
     * Iterate through the bytes of the character bitmap (column-major) and load each bit into the character buffer provided.
     */
    for (uint8_t glyph_byte = 0; glyph_byte < font_offset.bytes_per_char; glyph_byte++)
    {
        uint8_t glyph_bits = *( font_offset.pfont + (index * font_offset.bytes_per_char + glyph_byte) );

        for (uint8_t pixel = 0; pixel < 8; pixel++)
        {
            uint8_t bit   = (glyph_bits >> (7 - pixel)) & 1;

            uint8_t color = bit ? fg : bg;

            if ((out_pixel & 1) == 0)
                pbuf[byte_index] = (color & 0x07) << 3;     // Pushing the n pixel to bits 5:3  
                                                                // Set equal to clear the previous data from this byte address.
            else
                pbuf[byte_index] |= (color & 0x07);         // Keeping the n + 1 pixel in bits 2:0 
                                                                // See datasheet section 4.7.2.1 for more information

            out_pixel++;
            byte_index = out_pixel / 2;
            if (byte_index >= len) break;
        }
        
        if (byte_index >= len) break;
        if (out_pixel > pixel_count) break;
    }

    return pixel_count / 2;
}


/**
 * @brief A function to set the ram pointer to a position on the  (Dependent on the addressing mode)
 *
 * @param x_start defines the LEFT-most bit (pixel) or 8-bit page (if in page addressing mode)
 * @param x_end Not Implemented. For advanced box defining that will come in handy when writing text to the screen
 * @param y_start defines the TOP-most bit (pixel) or 8-bit page (if in page addressing mode)
 * @param y_end Not Implemented. For advanced box defining that will come in handy when writing text to the screen
 */
void ili9488_ram_write(Ili9488RamWrite args) {
    /** Set RAM pointer constraints based on x and y values given */
    ili9488_set_ram_pointer(args.ram_ptr);

    // NOTE: This is a vertical Ram Write (due to madctl and madctr and some other registers.)
    ili9488_gram_write(args.buf, args.buf_len);
}


void ili9488_fill_color(Ili9488Defines screen, Ili9488FillBlock args)
{
    ADD_TO_STACK_DEPTH();
    level_log(TRACE, "Filling a block");

    color_t color = 0;
    color |= (args.color & 0x7);
    color = color << 3;
    color |= (args.color & 0x7);

    uint24_t block_height = args.ram_ptr.end_y - args.ram_ptr.start_y + 1;
    uint24_t block_width = args.ram_ptr.end_x - args.ram_ptr.start_x + 1; 
    uint24_t total_block_bytes = block_height * block_width;

    // Calculate the number of full iterations needed
    uint24_t iterations = total_block_bytes / (uint24_t)screen.Screen.buffer_size;
    uint24_t remainder = total_block_bytes % (uint24_t)screen.Screen.buffer_size;

    // Log debug information
    level_log(TRACE, "Fill length is:");
    printf("%u", screen.Screen.buffer_size);
    level_log(TRACE, "Block Width:");
    printf("%lu", (uint32_t)block_width);
    level_log(TRACE, "Block Height:");
    printf("%lu", (uint32_t)block_height);
    level_log(TRACE, "Total Block Bytes:");
    printf("%lu", (uint32_t)total_block_bytes);
    level_log(TRACE, "Number of Iterations:");
    printf("%lu", (uint32_t)iterations);
    level_log(TRACE, "Remainder Bytes:");
    printf("%lu", (uint32_t)remainder);
    
    // Initialize the buffer with zeros (clear screen data)
    memset(screen.Screen.pbuffer, color, screen.Screen.buffer_size);

    ili9488_set_ram_pointer(args.ram_ptr);

    // Perform full buffer writes
    ili9488_gram_write(screen.Screen.pbuffer, screen.Screen.buffer_size);
    for (uint24_t i = 1; i < iterations; i++) {
        ili9488_gram_write_continue(screen.Screen.pbuffer, screen.Screen.buffer_size);
    }
    
    level_log(TRACE, "Wrote iterations, Now writing remainder");

    // Handle any remaining bytes
    if (remainder > 0) {
        ili9488_gram_write_continue(screen.Screen.pbuffer, (size_t)remainder);
     }

    level_log(TRACE, "ILI9488: Block Filled");
    REMOVE_FROM_STACK_DEPTH();
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
size_t ili9488_write_number(Ili9488Defines screen, Ili9488WriteNumber args) {

    ADD_TO_STACK_DEPTH(); // ili9488_write_number
    level_log(TRACE, "Writing Number:");
    printf("%ld or %lu", args.data, args.data);
    /** 
     * Don't be fooled by the term "constrained length" This is just a parameter that is used to right align the text since the buffer gets written left to right. 
     */

    #define MAX_NUMBER_OF_CHARS 10
    uint8_t n, number_of_chars_written; // the snprintf function actually returns an integer value, but I hope that the amount of characters will never exceed 255...
    unsigned char data_to_write[MAX_NUMBER_OF_CHARS]; // Don't really need to display numbers that are longer than 24 characters
    FontOffset write_num_off = args.font;
    // ili9488_set_ram_pointer(screen.interface, args.ram_ptr);
    
    // Hopefully won't ever need this...
    #ifndef USE_STATIC_BUFFERS
    if (data_to_write == NULL)
    {
        level_log(ERROR, "SSD1309: Memory allocation failed for data_to_write");
        return 0;
    }
    #endif // USE_STATIC_BUFFERS

    /**
     * Load the data_to_write buffer with the character from our variable
     * Use snprintf because it allows overwrite protection that always ends in a null terminator
     */
    number_of_chars_written = snprintf(&data_to_write[0], MAX_NUMBER_OF_CHARS, "%lu", args.data); // putting zeros at the end of the string so that it is less noise to the viewer
    if(number_of_chars_written <= 0) {
        level_log(ERROR, "snprintf call did not write data to a buffer. Possibly you have a bad args.data");
    }

    uint8_t right_align_pixel_offset = (args.constrained_length - number_of_chars_written) * write_num_off.width_pad;
    if(number_of_chars_written > args.constrained_length) {
        level_log(WARNING, "Constrained length smaller than chars written." /*Setting Right align character offset to ZERO to eliminate memory corruption possibilities"*/ );
        right_align_pixel_offset = 0;
    }
    level_log(TRACE, "Right-align offset is:");
    printf("%d", right_align_pixel_offset);

    /**
     * The constraint should just be 10 chars since that is the max. This makes the code easier for the user, but potentiall harder here
     * 1) the code needs to determine if the ram pointer passed is acceptable or not. (does the ram pointer enable the printing of the correct amount of chars?)
     *          This will be handled by only printing a hyphen if the pointer can not be reconciled. 
     * 2) The code needs to determine what the appropriate start index will be if the ram pointer is deemed acceptable, and set the start_x argument of the args.ram_pointer
     * 3) send the string and its ram_pointer to the print function.
     */
    Ili9488Print num_to_print = {
        .text         = &data_to_write[0],
        .fg           = args.fg,
        .ram_ptr      = args.ram_ptr,
        .line_spacing = 0,
        .font         = args.font,
    }; 

    num_to_print.ram_ptr.start_x = args.ram_ptr.start_x + right_align_pixel_offset;

    // if( num_to_print.ram_ptr.start_x + right_align_pixel_offset > write_num_off.width_pad) {
    //     num_to_print.ram_ptr.start_x += right_align_pixel_offset;
    // }
    // if(num_to_print.ram_ptr.start_x + right_align_pixel_offset - 1 < num_to_print.ram_ptr.end_x && num_to_print.ram_ptr.end_x - num_to_print.ram_ptr.start_x - right_align_pixel_offset >= number_of_chars_written * write_num_off.width_pad ) {
    //     num_to_print.ram_ptr.start_x += right_align_pixel_offset;
    // }

    ili9488_print(screen, num_to_print);


    level_log(TRACE, "ILI9488: Done Writing Number");
    REMOVE_FROM_STACK_DEPTH(); // ili9488_write_number

    return args.constrained_length * 6;
}


/**
 * @param     Screen: the screenDefines structure that general screen definitions
 * @param     args: the Ili9488WriteNumber struct that holds the specific paramaters for writing a number to the screen 
 *
 * @brief A function to print a string on the screen
 */
size_t ili9488_print(Ili9488Defines screen, Ili9488Print args) {

    ADD_TO_STACK_DEPTH(); // ili9488_print    
    level_log(TRACE, "Printing:");
    printf("\"%s\"", args.text);
    level_log(TRACE, "Foreground Color:");
    printf("0x%x", args.fg);

    /* Lock the background black */
    args.bg = BLACK;

    Ili9488FillBlock background = {
        .color = args.bg,
        .ram_ptr = args.ram_ptr
    };

    ili9488_fill_color(screen, background);
    // uint16_t tmp16; // A variable to hold the 16 bit value of a scaled byte
    // uint8_t tmp[2]; // tmp16 will get split into these two bytes to get the right endianness when sending the data over the i2c bus.
    uint8_t preserve_frame = 0;
    uint16_t box_width = args.ram_ptr.end_x - args.ram_ptr.start_x + 1;
    uint16_t box_height = args.ram_ptr.end_y - args.ram_ptr.start_y + 1;
    uint16_t xoff = 0;
    uint16_t yoff = 0;
    uint8_t write_width = args.font.width + 1; // adding one to artificially make the box bigger by one more column so that the mystery 8 bits before the ram pointer are dissolved
    uint8_t write_height = args.font.height;
    size_t write_len = 0;
    bool last_line_flag = false;
    uint8_t char_pad = 1;
    uint8_t char_height = args.font.height + args.line_spacing;
    size_t chars_written = 0;
    uint16_t row_increment = 0;

    char * word_search_str;
    uint8_t word_length;

    Ili9488RamPointer char_placement;
    // Glyph char_attrs = {
    //     .character = '\0',
    //     .height = char_bit_height_2x,
    //     .width = char_bit_width_2x,
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

        /**
         * Create a word-aware wrapping feature:
         *  1) Make sure we are not at the end of a line already
         *  2) at every space character, do a word-search (where is the next space relative from here)
         *  3) convert the word length to pixel length and compare to screen width left in row
         *  4) reset xoff and increment row_increment
         */
        if(xoff + write_width + 1 > box_width) {
            // Add one to account for a zero indexed column/row arrangement. If a box is 8 rows tall in the top left corner, rows 0 - 7 will be taken up. Thus row 8 will be the ninth row
            // Reset xoff and intcrement row counter
            xoff = 0;
            row_increment += 1;

            /* Break prematurely if the ram pointer box has been filled. */
            if(last_line_flag) break;

        } else if(msg_char == ' ') {
            word_length = 0;
            word_search_str = msg_letter;
            /* Initially increment the word_search_string so that it does not start on a space thus voiding the entire while loop */
            word_search_str++;
            while(*word_search_str != ' ' && *word_search_str != '\0' && *word_search_str != '\n') {
                word_length++;
                word_search_str++;
            }

            /* Convert to pixel length. Add 1 to the word length to account for the space in front of the word. */
            word_length = (word_length + 1) * (write_width + char_pad);

            /* Compare word pixel length to remaining pixel length of row */
            if( word_length > box_width - xoff ) {
                xoff = 0;
                row_increment += 1;
                msg_letter++;
                msg_char = *(msg_letter);
                continue;
            }

        } else if(msg_char == '\n') {
            row_increment += 1;
            xoff = 0;

            /* Increment past the newline character, so that we write the next character after it. */
            msg_letter++;
            msg_char = *(msg_letter);
        }

        yoff = (char_height * row_increment);
        if(yoff + char_height >= box_height) {
            // if ((yoff + (write_height * 2)) > box_height) {
            //     break;
            // }
            // else if (write_height == box_height - yoff - 1) {
            //     break;
            // }
            // Constrain char_height to only go to the bottom of the box
            // The unfortunate problem with this solution is that it does not allow for the correct pixels to be written at the correct places in the smaller box
            
            /* We are just going to break here instead of trying to display gibborish on the screen it makes more sense this way */
            break;
            
            write_height = (uint8_t)(box_height - yoff);
            last_line_flag = true;
        }

        // Calculate the ram_pointer based on offsets:
        char_placement.start_x = args.ram_ptr.start_x + xoff;
        char_placement.start_y    = args.ram_ptr.start_y + yoff;
        char_placement.end_x   = char_placement.start_x + write_width - 1  /*- 1 To maintain Zero Indexing */;
        char_placement.end_y      = char_placement.start_y + write_height - 1 /*- 1 To Maintain Zero Indexing */;
        printf("Char x = %u, Char y = %u, xoff = %u, yoff = %u", char_placement.start_x, char_placement.start_y, xoff, yoff);
        ili9488_set_ram_pointer(char_placement);

        // char_attrs.character = msg_char;

        /* The write_len variable will always be smaller or equal to the screen.Screen.buffer_size */
        write_len = load_glyph_3bit(msg_char, args.fg, args.bg, args.font, screen.Screen.pbuffer, screen.Screen.buffer_size);
        /* but, just in case */
        if (write_len > screen.Screen.buffer_size) {
            write_len = screen.Screen.buffer_size;
        }
        // level_log(TRACE, "Write Length is: %d", write_len);
        ili9488_gram_write(screen.Screen.pbuffer, write_len);
        
        /* We may potentially want to make word boundary aware printing...but not today.*/
        // for (unsigned char * word_letter = msg_letter; word_letter != '\0'; word_letter++) 
        // {
        //     continue;
        // }

        msg_letter++;
        msg_char = *(msg_letter);
        chars_written++;
        xoff += (write_width + char_pad);

        // printf("MSG Pointer: %p - MSG Char: %c - Chars Written: %d\n", msg_letter, msg_char, chars_written);
    }
    
    // Memory addressing mode back to page addressing

    level_log(TRACE, "Print: Done Printing.");
    REMOVE_FROM_STACK_DEPTH(); // ili9488_print
    return (chars_written * screen.Screen.character.width_pad);
}

void ili9488_draw_pixel(uint16_t x, uint16_t y, color_t c) {
    ADD_TO_STACK_DEPTH();
    printf("Drawing a pixel at (%u, %u) with color %b", x, y, c);

    Ili9488RamPointer ptr = {
        .start_x = x,
        .end_x = x,
        .start_y = y,
        .end_y = y,
    };

    ili9488_set_ram_pointer(ptr);

    ili9488_gram_write(&c, 1);

    level_log(TRACE, "Drew Pixel");
    REMOVE_FROM_STACK_DEPTH();
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
    level_log(TRACE, "Clear Length is:");
    printf("%u", screen.Screen.buffer_size);
    level_log(TRACE, "Screen Width:");
    printf("%u", screen.Screen.ScreenWidth);
    level_log(TRACE, "Screen Height:");
    printf("%u", screen.Screen.ScreenHeight);
    level_log(TRACE, "Total Screen Bytes:");
    printf("%lu", total_screen_bytes);
    level_log(TRACE, "Number of Iterations:");
    printf("%lu", iterations);
    level_log(TRACE, "Remainder Bytes:");
    printf("%lu", remainder);
    
    // Initialize the buffer with zeros (clear screen data)
    memset(screen.Screen.pbuffer, 0, screen.Screen.buffer_size);

    
    Ili9488RamPointer full_screen = {
        .start_y = 0,
        .end_y = screen.Screen.ScreenHeight - 1,
        .start_x = 0,
        .end_x = screen.Screen.ScreenWidth - 1
    };

    ili9488_set_ram_pointer(full_screen);

    // Perform full buffer writes
    ili9488_gram_write(screen.Screen.pbuffer, screen.Screen.buffer_size);
    for (uint32_t i = 1; i < iterations; i++) {
        ili9488_gram_write_continue(screen.Screen.pbuffer, screen.Screen.buffer_size);
    }
    
    level_log(TRACE, "Wrote iterations, Now writing remainder");

    // Handle any remaining bytes
    if (remainder > 0) {
        ili9488_gram_write_continue(screen.Screen.pbuffer, (size_t)remainder);
     }

    level_log(TRACE, "Ili9488: Screen Cleared");
    REMOVE_FROM_STACK_DEPTH();
}


void ili9488_clear_block(Ili9488Defines screen, Ili9488RamPointer args)
{

    ADD_TO_STACK_DEPTH();

    level_log(TRACE, "Clearing a block");

    Ili9488FillBlock clear_block = {
        .color = BLACK,
        .ram_ptr = args
    };

    ili9488_fill_color(screen, clear_block);

    level_log(TRACE, "ILI9488: Screen Block Cleared");
    REMOVE_FROM_STACK_DEPTH();
}

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

