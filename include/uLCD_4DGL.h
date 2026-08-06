//
// uLCD_4DGL is a class to drive 4D Systems TFT touch screens
//
// Fork of 4DGL library for 4D Systems LCD displays
// Copyright (C) <2010> Stephane ROCHON <stephane.rochon at free.fr>
// Modifed for Goldelox processor  Copyright (C) <2013> Jim Hamblen
// Modified by Tom Collins  Copyright (C) <2026> for ESP32 esp-idf framework
//   including addition of comments and uLCD_printf function
// uLCD_4DGL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// uLCD_4DGL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with uLCD_4DGL.  If not, see <http://www.gnu.org/licenses/>.

// @author Stephane Rochon

/**************************************************************************
*  This used to be a C++ class. Changed to C style for ESP-IDF compatibility
* 
* Example:
*  @code
*  *  Display a white circle on the screen
*  #include <stdio.h>
*  #include "uLCD_4DGL.h"
*  #include "freertos/FreeRTOS.h"
*  #include "freertos/task.h"
*  #include "driver/uart.h"
*  #define BUF_SIZE (1024)
* 
*  uart_config_t uLCD = {
*         .baud_rate = 115200,
*         .data_bits = UART_DATA_8_BITS,
*         .parity    = UART_PARITY_DISABLE,
*         .stop_bits = UART_STOP_BITS_1,
*         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
*         .source_clk = UART_SCLK_DEFAULT,
*  };
* 
*  int main() {
*      uLCD_4GDL myLCD;
*      myLCD.uart_num = UART_NUM_1; // Use UART1
*      myLCD.rst_pin = GPIO_NUM_18; // Use GPIO18 for RESET
* 
*      uart_driver_install(myLCD.uart_num, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
*      uart_param_config(myLCD.uart_num, &uLCD));
*      //   uart_set_pin();   //  only if need to change from default pins
*      myLCD_circle(&uLCD, 120, 160, 80, WHITE);
*   }
*   @endcode
*/

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "driver/uart.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"

// Debug Verbose off - SGE commands echoed to USB serial for debugmode=1
#ifndef DEBUGMODE
#define DEBUGMODE 0
#endif

// Common WAIT value in milliseconds between commands
#define TEMPO 0

// 4DGL SGE Function values for Goldelox Processor
#define CLS          '\xD7'
#define BAUDRATE     '\x0B' //null prefix
#define VERSION      '\x08' //null prefix
#define BCKGDCOLOR   '\x6E'
#define TXTBCKGDCOLOR '\x7E'
#define TXTFGGDCOLOR '\x7F'
#define DISPCONTROL  '\x68'
#define BEEP         '\xDA'
#define CIRCLE       '\xCD'
#define FCIRCLE      '\xCC'
#define TRIANGLE     '\xC9'
#define LINE         '\xD2'
#define FRECTANGLE   '\xCE'
#define RECTANGLE    '\xCF'
#define ELLIPSE      '\x65' //na
#define PIXEL        '\xCB'
#define READPIXEL    '\xCA'
#define SCREENCOPY   '\x63'  //na?
#define PENSIZE      '\xD8'
#define SETFONT      '\x7D'
#define TEXTMODE     '\x77'
#define TEXTBOLD     '\x76'
#define TEXTITALIC   '\x75'
#define TEXTINVERSE  '\x74'
#define TEXTUNDERLINE '\x73'
#define TEXTWIDTH    '\x7C'
#define TEXTHEIGHT   '\x7B'
#define TEXTCHAR     '\xFE'
#define TEXTSTRING   '\x06'  //null prefix
#define MOVECURSOR   '\xE4'
#define BLITCOM      '\x0A'
#define PUTCHAR      '\xFE'
#define DISPPOWER    '\x66'

//media commands for uSD card
#define MINIT        '\xB1'
#define SBADDRESS    '\xB9'
#define SSADDRESS    '\xB8'
#define READBYTE     '\xB7'
#define READWORD     '\xB6'
#define WRITEBYTE    '\xB5'
#define WRITEWORD    '\xB4'
#define FLUSHMEDIA   '\xB2'
#define DISPLAYIMAGE '\xB3'
#define DISPLAYVIDEO '\xBB'
#define DISPLAYFRAME '\xBA'

// Screen answers
#define ACK          '\x06'
#define NAK          '\x15'

// Screen states
#define OFF          '\x00'
#define ON           '\x01'

// Graphics modes
#define SOLID        '\x00'
#define WIREFRAME    '\x01'

// Text modes
#define TRANSPARENT  '\x00'
#define OPAQUE       '\x01'

// Fonts Sizes
#define FONT_7X8     '\x00' //only builtin font 
#define FONT_5X7     '\x04'
#define FONT_8X8     '\x01'
#define FONT_8X12    '\x02'
#define FONT_12X16   '\x03'
#define MEDIAFONT    '\x07'

// Data speed
#define BAUD_110     27271
#define BAUD_300     9999
#define BAUD_600     4999
#define BAUD_1200    2499
#define BAUD_2400    1249
#define BAUD_4800    624
#define BAUD_9600    312
#define BAUD_14400   207
#define BAUD_19200   155
#define BAUD_31250   95
#define BAUD_38400   77
#define BAUD_56000   53
#define BAUD_57600   51
#define BAUD_115200  25
#define BAUD_128000  22
#define BAUD_256000  11
#define BAUD_300000  10
#define BAUD_375000  8
#define BAUD_500000  6
#define BAUD_600000  4
#define BAUD_750000  3
#define BAUD_1000000 2
#define BAUD_1500000 1
#define BAUD_3000000 0

// Defined Colors
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define RED   0xFF0000
#define GREEN 0x00FF00
#define BLUE  0x0000FF
#define YELLOW 0xFFFF00
#define CYAN  0x00FFFF
#define MAGENTA 0xFF00FF
#define ORANGE 0xFFA500
#define PINK 0xFF8080
#define PURPLE 0x800080
#define LBLUE 0x8080FF
#define DGREEN 0x008000
#define BROWN 0xA52A2A
#define LGREY 0xBFBFBF
#define DGREY 0x5F5F5F

// Mode data
#define BACKLIGHT    '\x00'
#define DISPLAY      '\x01'
#define CONTRAST     '\x02'
#define POWER        '\x03'
#define ORIENTATION  '\x04'
#define TOUCH_CTRL   '\x05'
#define IMAGE_FORMAT '\x06'
#define PROTECT_FAT  '\x08'

// change this to your specific screen (newer versions) if needed
// Startup orientation is PORTRAIT so SIZE_X must be lesser than SIZE_Y
//uLCD144-G2 is a 128 by 128 pixel display
#define SIZE_X       128
#define SIZE_Y       128

#define IS_LANDSCAPE 0
#define IS_PORTRAIT  1

// Screen orientation
#define LANDSCAPE    '\x00'
#define LANDSCAPE_R  '\x01'
#define PORTRAIT     '\x02'
#define PORTRAIT_R   '\x03'

// Parameters
#define ENABLE       '\x00'
#define DISABLE      '\x01'
#define RESET        '\x02'

#define NEW          '\x00'
#define OLD          '\x01'

#define DOWN         '\x00'
#define UP           '\x01'

#define PROTECT      '\x00'
#define UNPROTECT    '\x02'


typedef struct {
    uart_port_t uart_num;
    int rst_pin;
    int tx_pin;
    int rx_pin;
    // Text data
    char current_col;
    char current_row;
    int  current_color;
    int  current_bckgcolor;
    char current_font;
    char current_orientation;
    char max_col;
    char max_row;
    int current_w, current_h;
    int current_fx, current_fy;
    int current_wf, current_hf;
    // Screen Data
    int type;
    int revision;
    int firmware;
    int reserved1;
    int reserved2;
}  uLCD_4DGL;


// General Commands *******************************************************************************

// ALL COMMANDS require a pointer to a uLCD_4DGL structure as first argument


    /** Initialize (replaces constructor of previous C++ version) 
     * Param uart_port_t uart_num : UART port to use (UART_NUM_0, UART_NUM_1, etc)
     * Param gpio_num_t rst_pin : GPIO pin number to use for RESET
     * Param gpio_num_t tx_pin : GPIO pin number to use for TX 
     * Param gpio_num_t rx_pin : GPIO pin number to use for RX 
    */
    void uLCD_init(uLCD_4DGL *lcd, uart_port_t uart_num, gpio_num_t rst_pin, 
                   gpio_num_t tx_pin, gpio_num_t rx_pin);
    
    /** Clear the entire screen using the current background colour */
    void uLCD_cls(uLCD_4DGL*);

    /** Reset screen -- must be done at start of application and takes 3 seconds*/
    void uLCD_reset(uLCD_4DGL*);

    /** Set serial Baud rate (both sides : screen and mbed)
    * @param speed Correct BAUD value (see constants defined above)
    */
    void uLCD_baudrate(uLCD_4DGL*, int speed);

    /** Set background colour to the specified value
    * @param color in HEX RGB like 0xFF00FF
    */
    void uLCD_background_color(uLCD_4DGL*, int color);

    /**< Set display mode (PORTRAIT or LANDSCAPE) */
    void uLCD_display_control(uLCD_4DGL*, char mode);

    /** Set display power (ON/OFF) */
    void uLCD_display_power(uLCD_4DGL*, char mode);

// Graphics Commands *******************************************************************************

    /** Draw a circle centered at x,y with a radius and a colour. It uses Pen Size stored value to draw a solid or wireframe circle
    * @param x Horizontal position of the circle centre
    * @param y Vertical position of the circle centre
    * @param radius Radius of the circle
    * @param color Circle color in HEX RGB like 0xFF00FF
    */
    void uLCD_circle(uLCD_4DGL*, int x , int y , int radius, int color);

    // Filled Circle same as above but filled
    void uLCD_filled_circle(uLCD_4DGL*, int x , int y , int radius, int color);

    // Remaining graphic shapes --- similar format
    void uLCD_triangle(uLCD_4DGL* lcd, int x1, int y1 , int x2, int y2, int x3, int y3, int color);
    void uLCD_line(uLCD_4DGL* lcd, int x1, int y1 , int x2, int y2, int color);
    void uLCD_rectangle(uLCD_4DGL* lcd, int x1, int y1 , int x2, int y2, int color);
    void uLCD_filled_rectangle(uLCD_4DGL* lcd, int x1, int y1 , int x2, int y2, int color);

    // Draw a single pixel at x,y with specified color
    void uLCD_pixel(uLCD_4DGL* lcd, int x, int y, int color);

    // Read the color of a single pixel at x,y
    int  uLCD_read_pixel(uLCD_4DGL* lcd, int x, int y);

    // Draw a block of pixels (blit) at x,y with width w and height h from colors array
    void uLCD_BLIT(uLCD_4DGL*, int x, int y, int w, int h, int *colors);

// Text Commands
    // Set the current font (only only the builtin font is available)
    void uLCD_set_font(uLCD_4DGL* lcd, char mode);

    // This does not actually change the font, but figures out how many columns and rows fit on the screen
    // based on the current font and the requested size multipliersvoid 
    void uLCD_set_font_size(uLCD_4DGL*, char width, char height);  

    // Set text mode (TRANSPARENT or OPAQUE)
    void uLCD_text_mode(uLCD_4DGL* lcd, char mode);

    // Set text attributes (0=disable, 1=enable)
    void uLCD_text_bold(uLCD_4DGL* lcd, char enable);
    void uLCD_text_italic(uLCD_4DGL* lcd, char enable);
    void uLCD_text_inverse(uLCD_4DGL* lcd, char enable);
    void uLCD_text_underline(uLCD_4DGL* lcd, char enable);

    // Set the current font size (width and height are INTEGER MULTIPLIERS of the base font size)
    void uLCD_text_width(uLCD_4DGL* lcd, char width);
    void uLCD_text_height(uLCD_4DGL* lcd, char height);

    // Print a single character at col,row position
    void uLCD_text_char(uLCD_4DGL* lcd, char c, char col, char row, int color);

    // Print a string starting at col,row position -- WILL NOT WRAP AROUND THE SCREEN or go to next line
    void uLCD_text_string(uLCD_4DGL* lcd, char *s, char col, char row, char font, int color);

    // Position the text cursor at col,row
    void uLCD_locate(uLCD_4DGL* lcd, char col, char row);

    // Set text foreground color
    void uLCD_text_color(uLCD_4DGL* lcd, int color);

    // Set text background color
    void uLCD_textbackground_color(uLCD_4DGL* lcd, int color);

    // Put a single character at current cursor position
    // Advances cursor position automatically, with wraparound, processes special characters \n, \r, \f
    void uLCD_putc(uLCD_4DGL* lcd, char c);

    // Put a string at current cursor position, with fast option FOR SHORT STRINGS ONLY
    // If fast=1, no check for end of line or screen wraparound is done
    void uLCD_puts(uLCD_4DGL* lcd, char *s, int fast);

    // Formatted print at current cursor position -- WILL NOT WRAP AROUND THE SCREEN or go to next line
    void uLCD_printf(uLCD_4DGL* lcd, char *s, ...);

//Media Commands
    // Initialize SD card
    int uLCD_media_init(uLCD_4DGL*);

    // Set byte address for next read/write
    void uLCD_set_byte_address(uLCD_4DGL* lcd, int address);

    // Set sector address for next read/write
    void uLCD_set_sector_address(uLCD_4DGL* lcd, int sector);

    // Read a byte from uSD card
    char uLCD_read_byte(uLCD_4DGL* lcd);

    // Read a word (2 bytes) from uSD card
    int  uLCD_read_word(uLCD_4DGL* lcd);

    // Write a byte to uSD card
    void uLCD_write_byte(uLCD_4DGL* lcd, int byte);

    // Write a word (2 bytes) to uSD card
    void uLCD_write_word(uLCD_4DGL* lcd, int word);

    // Flush any buffered data to uSD card
    void uLCD_flush_media(uLCD_4DGL* lcd);

    // Display an image from uSD card (at address set by uLCD_set_byte_address)
    //  Placed with upper left corner at x,y
    void uLCD_display_image(uLCD_4DGL* lcd, int x, int y);

    // Display a video clip from uSD card (at address set by uLCD_set_byte_address)
    //  Placed with upper left corner at x,y
    void uLCD_display_video(uLCD_4DGL* lcd, int x, int y);

    // Display a video frame from uSD card (at address set by uLCD_set_byte_address)
    //  Placed with upper left corner at x,y
    void uLCD_display_frame(uLCD_4DGL* lcd, int x, int y, int framenum);

// Low Level Commands - generally not needed directly by user application
    void uLCD_freeBUFFER  (uLCD_4DGL*);
    void uLCD_writeBYTE   (uLCD_4DGL* lcd, char c);
    void uLCD_writeBYTEfast   (uLCD_4DGL* lcd, char c);
    int  uLCD_writeCOMMAND(uLCD_4DGL* lcd, char *command, int len);
    int  uLCD_writeCOMMANDnull(uLCD_4DGL* lcd, char *command, int len);
    int  uLCD_readVERSION (uLCD_4DGL* lcd, char *buffer, int number);
    int  uLCD_getSTATUS   (uLCD_4DGL* lcd, char *buffer,int len);
    int  uLCD_version     (uLCD_4DGL* lcd);
;

typedef unsigned char BYTE;
