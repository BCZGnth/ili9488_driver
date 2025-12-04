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

    // NOTE: This is a horizontal Ram Write
    ili9488_gram_write(inter, args.bitmap, args.bitmap_length);
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
size_t ili9488_write_number(ScreenDefines Screen, Ili9488WriteNumber args) {

    ADD_TO_STACK_DEPTH(); // ili9488_write_number
    level_log(TRACE, "Writing Number: %d", args.data);

    /** 
     * Don't be fooled by the term "constrained length" This is just a parameter that is used to right align the text since the buffer gets written left to right. 
     */

    uint8_t write_length = (args.constrained_length * Screen.character.width_pad) + Screen.offset.control;
    if(Screen.buffer_size < write_length) {
        level_log(ERROR, "Buffer Size Too Small");
        return 0;
    }

    ili9488_set_ram_pointer(Screen, args.ram_ptr);

    uint8_t n;
    uint8_t number_of_chars_written; // the snprintf function actually returns an integer value, but I hope that the amount of characters will never exceed 256...
    unsigned char data_to_write[36]; // Don't really need to display numbers that are more than 6 digits long. 6 * 6 = 36
    
    #ifndef USE_STATIC_BUFFERS
    if (data_to_write == NULL)
    {
        level_log(ERROR, "SSD1309: Memory allocation failed for data_to_write");
        return 0;
    }
    #endif // USE_STATIC_BUFFERS

    number_of_chars_written = snprintf(&data_to_write[0], args.constrained_length, "%u", args.data); // putting zeros at the end of the string so that it is less noise to the viewer
    if(number_of_chars_written <= 0) {
        level_log(ERROR, "snprintf call did not write data to a buffer. Possibly you have a bad args.data");
    }
    uint8_t right_align_character_offset = (args.constrained_length - number_of_chars_written) * 6;
    if(args.constrained_length < number_of_chars_written) {
        level_log(WARNING, "constrained length smaller than chars written. Setting Right align character offset to ZERO to eliminate memory corruption possibilities");
        right_align_character_offset = 0;
    }
    level_log(TRACE, "Right-align offset is %d", right_align_character_offset);

    /* Loading all Zeros into the I2C buffer */
    memset(Screen.pbuffer, 0, write_length);

    memcpy(Screen.pbuffer, (&SSD1309_RAM_WRITE_BYTE), Screen.offset.control);

    level_log(TRACE, "Loading the I2C buffer with %d numeric characters", number_of_chars_written);
    for (n = 0; n < number_of_chars_written; n++)
    { // Iterate through all of the characters in the string

        /** Write 5 bytes from a (calculated pointer to a character in the font) to the i2c buffer */

        /**      The pointer to the destination buffer is calculated by:
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
         */
        if(n * Screen.character.width_pad > Screen.buffer_size) {
            level_log(WARNING, "Can not load any more characters into buffer on iteration %d", n);
            break;
        }
        memcpy((Screen.pbuffer + (n * Screen.character.width_pad) + Screen.offset.control + right_align_character_offset), (((data_to_write[n] - Screen.offset.ascii) * Screen.character.width) + Screen.offset.pfont), Screen.character.width);
    }

    ssd_write(Screen, write_length); // The number of bytes to write to the i2c buffer is the number of characters multiplied by the width of each character (5) plus the padding (1 byte) between each character

    // Memory addressing mode back to page addressing
    ILI9488_SendCommand(Screen, SET_MEMORY_ADDRESSING_MODE, PAGE_ADDRESSING);

    // Set the column range back to its reset value
    ILI9488_SendCommand(Screen, SET_COLUMN_ADDRESS, 0x0, 0xFF);

    // Set the page range back to its reset value
    ILI9488_SendCommand(Screen, SET_PAGE_ADDRESS, 0x0, 0x7);

    level_log(TRACE, "SSD1309: Done Writing Number");
    REMOVE_FROM_STACK_DEPTH(); // ili9488_write_number

    return args.constrained_length * 6;
}


/**
 * @param     Screen: the screenDefines structure that general screen definitions
 * @param     args: the Ili9488WriteNumber struct that holds the specific paramaters for writing a number to the screen 
 *
 * @brief A function to print a string on the screen
 */
size_t ili9488_print(ScreenDefines Screen, Ili9488Print args) {

    
    ADD_TO_STACK_DEPTH(); // ili9488_print    
    level_log(TRACE, "Printing: \"%s\"", args.text);
    
    
    uint16_t tmp16; // A variable to hold the 16 bit value of a scaled byte
    uint8_t tmp[2]; // tmp16 will get split into these two bytes to get the right endianness when sending the data over the i2c bus.
    size_t write_size;
    
    /* Calculate the total length of the i2c write */
    level_log(TRACE, "Truncating the write length for the scaled font. That way it doesn't overwrite itself");
    if((args.length * Screen.character.width_pad * (args.scale * args.scale) + Screen.offset.control) > Screen.ScreenWidth * args.scale) {
        write_size = Screen.ScreenWidth * args.scale;
    } else {
        write_size = (args.length * Screen.character.width_pad * (args.scale * args.scale) + Screen.offset.control);
    }
    
    /* Error Checks */
    // Length is zero
    if(!args.length) {
        level_log(ERROR, "type <Ili9488Print> args.length not defined");
        return 0;
    }
    // If the bytes length will be larger than the buffer size
    if(write_size >  Screen.buffer_size) {
        level_log(ERROR, "Buffer Size Too Small");
        return 0;
    }

    /* Constrain the columns so that we skip the first and last. This way frames don't get overwritten */
    ILI9488_SendCommand(Screen, SET_COLUMN_ADDRESS, 1, 126);

    ili9488_set_ram_pointer(Screen, args.ram_ptr); // Put the cursor where specified in the args structure

    // char message_chars[48];
    // uint8_t message_chars_length = sizeof(message_chars);

    // /* Memory Corruption Error Checking */
    // if(args.length)
    // memcpy(&message_chars, args.text, args.length);

    memset(Screen.pbuffer, 0, Screen.buffer_size);

    level_log(TRACE, "TESTING...the Screen.pbuffer is equal to: %x",  *(Screen.pbuffer));

    /** Loading the I2C buffer */
    switch (args.scale)
    {

#warning Scaling only enables a single row of scaled text to be printed to the screen from a single function call

        case 2:
        /** Scaling the text by 2. (Right now the code does not support anything else...)*/
            level_log(TRACE, "Print: Scaling the text by 2");

            ADD_TO_STACK_DEPTH(); // loading i2c buffer with scaled data

            /** 
             * Iteration for scaling the font. 
             *
             *    Iterate over the number of characters in the char* args.text (denoted by args.length)
             *          
             *          Iterate over each column of the letter to scale each column individually
             *           - use memset to set the corresponding block of memory in Screen.Screen.pbuffer
             *           - NOTE: we use memset because the vertical scaling is done with a lookup table, but the horizontal scaling is done by writing the same byte to adjacent memory addresses
             *           - NOTE: args.scale is used as the amount of bytes to write to Screen.Screen.pbuffer because the character will be args.scale times wider.
             */
            int letter_increment;
            int current_column;
            int column_offset = args.scale;

            memcpy(Screen.pbuffer, (&SSD1309_RAM_WRITE_BYTE), 1);

            level_log(TRACE, "TESTING...the Screen.pbuffer is equal to: %x",  *(Screen.pbuffer));

            for (int char_index = 0; char_index < args.length; char_index++)
            {
                /** Letter increment is the number by which to offset the buffer when writing the next letter 
                 * Since each letter will take up (width * height) bits, it will take up (width * height / 8) bytes 
                 * So a scaled letter will take up ( (scale * width) * (scale * height) / 8 ) bytes. 
                 * since "scale" will always be (scale * height / 8) since a character is 8 bits (one byte) tall 
                 * This means that we can make the translation a little more efficient by saying 
                 * each letter will take up ( (scale * width) * scale ) */

                letter_increment = char_index * Screen.character.width_pad * (args.scale * args.scale);
                for (int char_column = 0; char_column < 5; char_column++)
                {
                    /** Letter column is to calculate the Screen.offset for one column of scaled letter. */
                    /** Scaling the letter by x will give the scaled letter x^2 times as many columns. */
                    current_column = char_column * args.scale * args.scale;
                    // tmp = *( real.pfont + ((message_chars[n] - Screen.offset.ascii) * Screen.character.width) + i); // To get the individual bytes of the character
                    tmp16 = scale_lut[*(Screen.offset.pfont + ((args.text[char_index] - Screen.offset.ascii) * Screen.character.width) + char_column)];
                    tmp[1] = (tmp16 >> 8);
                    tmp[0] = (tmp16 & 0xff);

                    /* Memory Corruption Error Checking */
                    if(Screen.offset.control + letter_increment + current_column + column_offset > Screen.buffer_size) {
                        level_log(ERROR, "Buffer Size Too Small");
                        break;
                    }

                    memcpy(( Screen.pbuffer + Screen.offset.control + letter_increment + current_column ),                &(tmp[0]), args.scale);
                    memcpy(( Screen.pbuffer + Screen.offset.control + letter_increment + current_column + column_offset), &(tmp[0]), args.scale);

                    // memset(( Screen.pbuffer + Screen.offset.control + letter_increment + current_column ), tmp[0], 1); // Write the scaled byte to the i2c buffer
                    // memset(( Screen.pbuffer + Screen.offset.control + letter_increment + current_column + 1), tmp[1], 1);
                    level_log(TRACE, "Blah Blah Blah: %X", scale_lut[tmp[0]]);
                }
            }

            level_log(TRACE, "TESTING...the Screen.pbuffer is equal to: %x",  *(Screen.pbuffer));
            REMOVE_FROM_STACK_DEPTH(); // loading i2c buffer with scaled data
            break;

        default:
        /** No scaling applied, and the message is copied into the i2c buffer */
            level_log(TRACE, "Print: No scale applied");

            memcpy(Screen.pbuffer, (&SSD1309_RAM_WRITE_BYTE), 1);

            for (int letter = 0; letter < args.length; letter++)
            { // Iterate through all of the characters in the string

                /** Write 5 bytes from a (calculated pointer to a character in the font) to the i2c buffer */

                /**      The pointer to the destination buffer is calculated by:
                 *
                 *          Adding 1 to the pointer to the i2c buffer (don't overwrite the SSD1309_RAM_WRITE_BYTE)
                 *          Adding (n * 6) to the pointer to the i2c buffer. The multiplier 6 is used because the characters are 5 bytes long and a pad byte is added in between each character
                 *
                 *
                 *      The source pointer is calculated by:
                 *
                 *          Subtracting 32 from the character pointer stored in message_chars[n] (the local font does not include the first 32 ascii characters)
                 *          Multiplying that number by 5 (because each character takes up 5 bytes)
                 *          Adding that number to a pointer to the font (to point to the correct character in the font array)
                 *
                 *      The length of data to copy is 5 because each character is 5 bytes long.
                 *      the pointer buffer is incremented by 6 in order to put a column of space in between each character
                 *      This is only possible because the i2c buffer is loaded with zeros before any data gets written to it.
                 */
                if( Screen.offset.control + ((Screen.character.width_pad) * letter) + Screen.character.width > Screen.buffer_size) {
                    level_log(ERROR, "Buffer Size Too Small");
                    break;
                }
                memcpy((Screen.pbuffer + Screen.offset.control + ((Screen.character.width_pad) * letter)), (((args.text[letter] - Screen.offset.ascii) * Screen.character.width) + Screen.offset.pfont), Screen.character.width);
            }

            write_size = (args.length * Screen.character.width_pad * (args.scale * args.scale) + Screen.offset.control);
            break;
    }

    /** Writing the contents of the I2C buffer over the bus */
    switch (args.delay)
    {

    case 0:

        level_log(TRACE, "Print: No delay");
        ssd_write(Screen, write_size);

        break;

    default:

        /** Memory Corruption Error Checking */
        if(Screen.buffer_size < Screen.character.width_pad * args.scale * args.scale + Screen.offset.control) {
            level_log(ERROR, "Buffer Size Too Small");
            return 0;
        }
        /** Allocate memory to write a single letter each time */
        uint8_t letter_buffer[33];
        memcpy(&letter_buffer, (&SSD1309_RAM_WRITE_BYTE), Screen.offset.control);

        level_log(TRACE, "Print: Delay of %d", args.delay);

        /** My own version of the ssd_write function that has a spefic delay for each character built in!! */
        for (int letter_index = 0; letter_index < args.length; letter_index++)
        {
            ADD_TO_STACK_DEPTH(); // Adding to the stack depth because this is a different I2C write than anything else in the program

            /** The width of a letter is calculated by the following logic: 
             * Since each letter will take up (width * height) bits, it will take up (width * height / 8) bytes 
             * So a scaled letter will take up ( (scale * width) * (scale * height) / 8 ) bytes. 
             * since "scale" will always be (scale * height / 8) since a character is 8 bits (one byte) tall 
             * This means that we can make the translation a little more efficient by saying 
             * each letter will take up ( (scale * width) * scale ) 
             * Since letter_index holds the index of the letter that we are currently writing to the screen,  */
            memcpy((letter_buffer + Screen.offset.control), (Screen.pbuffer + Screen.offset.control + (letter_index * Screen.character.width_pad * args.scale * args.scale)), (Screen.character.width_pad * (args.scale * args.scale)));

            /** The amount of bytes that will be written over the buffer is equal to the width of a letter (defined in the above comment) */

        break;

    } // Switch(args.delay)

    // Memory addressing mode back to page addressing

    level_log(TRACE, "Print: Done Printing. Setting Memory Addressing Mode to Page Addressing");

    REMOVE_FROM_STACK_DEPTH(); // ili9488_print

    return (args.length * Screen.character.width_pad);
}


void ili9488_cls(ili9488_interface_t inter);
{
    ADD_TO_STACK_DEPTH();
    level_log(TRACE, "Clearing the screen");

    Ili9488RamPointer full_screen = {
        .start_row = 0,
        .start_column = 0,
        .end_row = 319,
        .end_column = 479
    };

    ili9488_set_ram_pointer(inter, full_screen);

    /* Calculate the most efficient way to clear the screen with the given buffer */
    // Calculate total screen size in bytes (assuming 1 bit per pixel, 8 pixels per byte vertically)
    unsigned total_screen_bytes = Screen.ScreenWidth * Screen.ScreenHeight / 2;

    // Use the full buffer size for clearing (assuming clear_length is the usable buffer size)
    /* We will also assume that the Screen.buffer_size is greater than 1 */
    unsigned int clear_length = Screen.buffer_size - (unsigned int)Screen.offset.control;

    /* No Error check needed since clear_length will always be less than Screen.buffer_size */
    // if(clear_length > Screen.buffer_size) {
    //     level_log(ERROR, "Buffer Size Too Small");
    //     return;
    // }

    // Calculate the number of full iterations needed
    unsigned int iterations = total_screen_bytes / clear_length;
    unsigned int remainder = total_screen_bytes % clear_length;

    // Log debug information
    level_log(TRACE, "Clear Length is: %d", clear_length);
    level_log(TRACE, "Screen Width: %d", Screen.ScreenWidth);
    level_log(TRACE, "Screen Height: %d", Screen.ScreenHeight);
    level_log(TRACE, "Total Screen Bytes: %d", total_screen_bytes);
    level_log(TRACE, "Number of Iterations: %d", iterations);
    level_log(TRACE, "Remainder Bytes: %d", remainder);
    
    Screen.pbuffer[0] = SSD1309_RAM_WRITE_BYTE; // Assuming control byte is needed at the start

    // Initialize the buffer with zeros (clear screen data)
    memset(Screen.pbuffer + Screen.offset.control, 0, clear_length);

    // Perform full buffer writes
    for (int i = 0; i < iterations; i++) {
        ssd_write(Screen, Screen.buffer_size);
    }

    level_log(TRACE, "Wrote iterations, Now writing remainder");

    // Handle any remaining bytes
    if (remainder > 0) {
        ssd_write(Screen, remainder + Screen.offset.control);
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

/**
 * Function loads a static buffer with the screen-ready pixel 
 */
Glyph load_glyph_3bit(Glyph c, uint8_t fg, uint8_t bg, FontOffset font_offset)
{
    #define GLYPH_BUFFER_LENGTH 80
    static uint8_t GlyphBuffer[GLYPH_BUFFER_LENGTH];

    // Unnecessary Check for character ord. We will neglect this since reading from anywhere in memory will only mess up the display of that undefined character
    // if (c < FONT_FIRST_CHAR || c > FONT_LAST_CHAR)
    //     return g;

    // Calculate the "ord" index of the character
    uint8_t index = (c.character - font_offset.ascii);

    /* Number of pixels and bytes needed (2 pixels per byte) */
    uint16_t pixel_count = c.width * c.height;
    uint16_t  glyph_byte_count = (pixel_count + (pixel_count % 8) / 8);
    uint16_t byte_count  = (pixel_count + 1) / 2;

    if(byte_count > GLYPH_BUFFER_LENGTH) {
        byte_count = GLYPH_BUFFER_LENGTH;
    }

    // Create a static buffer that will hold the glyph while we write it to the screen outside of this function.

    c.data = &GlyphBuffer;

    /* Potential for memory corruption here... */
    memset(c.data, 0, GLYPH_BUFFER_LENGTH);

    fg &= 0x07;  /* keep only 3 bits */
    bg &= 0x07;

    uint16_t out_pixel = 0;

    /**
     * font_data[index][row] contains one row, MSB = left pixel
     *  Because the font will be stored horizontally for now, we can make some assumptions in the workings of this function
     */
    for (uint8_t glyph_byte = 0; glyph_byte < glyph_byte_count; glyph_byte++)
    {
        uint8_t glyph_bits = ascii_font[index][glyph_byte];

        for (uint8_t pixel = 0; pixel < 8; pixel++)
        {
            uint8_t bit   = (glyph_bits >> (7 - pixel)) & 1;
            uint8_t color = bit ? fg : bg;

            uint16_t byte_index = out_pixel / 2;

            if ((out_pixel & 1) == 0)
                c.data[byte_index] |= (color & 0x07);
            else
                c.data[byte_index] |= (color & 0x07) << 3;

            out_pixel++;
        }
    }

    return c;
}
