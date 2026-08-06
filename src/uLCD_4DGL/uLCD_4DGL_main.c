//
// uLCD_4DGL is a class to drive 4D Systems uLCD 144 G2
//
// Copyright (C) <2010> Stephane ROCHON <stephane.rochon at free.fr>
// Modifed for Goldelox processor  Copyright (C) <2013> Jim Hamblen
// Modified by Tom Collins  Copyright (C) <2026> for ESP32 esp-idf framework
//
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

#include "../include/uLCD_4DGL.h"

#define ARRAY_SIZE(X) sizeof(X)/sizeof(X[0])

//******************************************************************************************************
void uLCD_init(uLCD_4DGL *lcd, uart_port_t uart_num, gpio_num_t rst_pin, gpio_num_t tx_pin, gpio_num_t rx_pin){
    // Initialize (replaces constructor of previous C++ version)
    // Normal value for uart_num is UART_NUM_1 (UART 0 is used for console)
    // UART 1 is best for GPIO9 (RX) and GPIO10 (TX) because they are hardwired to it (avoids muxing)
    // Normal value for rst_pin is GPIO_NUM_18

    #define BUF_SIZE (1024)
 
    uart_config_t uLCD = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    lcd->uart_num = uart_num; // Use UART 1 for uLCD
    lcd->rst_pin = rst_pin; //  Assign pins according to input parameters
    lcd->tx_pin = tx_pin; 
    lcd->rx_pin = rx_pin; 

    gpio_reset_pin(lcd->rst_pin); // Configure RESET pin
    gpio_set_direction(lcd->rst_pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(lcd->rst_pin, 1);

    // Install UART driver for uLCD 
    ESP_ERROR_CHECK(uart_driver_install(lcd->uart_num, BUF_SIZE * 2, 256, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(lcd->uart_num, &uLCD));
    ESP_ERROR_CHECK(uart_set_pin(lcd->uart_num, lcd->tx_pin, lcd->rx_pin, -1, -1)); 
    // Reset pin setup
    gpio_set_direction((gpio_num_t)lcd->rst_pin, GPIO_MODE_OUTPUT_OD);

    // Initialize internal variables
    lcd->current_row         = 0;            // initial cursor row
    lcd->current_col         = 0;            // initial cursor col
    lcd->current_color       = WHITE;        // initial text color
    lcd->current_bckgcolor   = BLACK;        // initial background color
    lcd->current_orientation = IS_PORTRAIT;  // initial screen orientation
    lcd->current_hf = 1;
    lcd->current_wf = 1;
    uLCD_set_font(lcd, FONT_7X8);   // initial font (the only internal font)
                                    // sets max_row and max_col to 18 & 15
};


//******************************************************************************************************
void uLCD_writeBYTE(uLCD_4DGL *lcd, const char c)   // send a BYTE command to screen
{
#if DEBUGMODE
    printf("   In writeBYTE\n");
#endif
    uart_write_bytes(lcd->uart_num, (const char *) &c, 1);
#if DEBUGMODE
    printf("   Back from uart_write_bytes\n");
#endif
    ets_delay_us(150); // Independent of baud rate, a short delay is needed to allow LCD to process byte
    // With increasing baud rates, this delay dominates overall speed.
    // At 9600 baud, each byte takes about 1.04 ms to send, so 0.2 ms delay is small, and the effective
    // speed is about 8500 baud.    
    // At 115200 baud, each byte takes about 0.087 ms to send, so 0.2 ms delay is significant, 
    // and the effective speed is about 40000 baud.   
#if DEBUGMODE
    printf("   Char sent : 0x%02X\n",c);
#endif

}

//******************************************************************************************************
void uLCD_writeBYTEfast(uLCD_4DGL *lcd, const char c)   // send a BYTE command to screen
{
#if DEBUGMODE
    printf("   In writeBYTEfast\n");
#endif
    uart_write_bytes(lcd->uart_num, (const char *) &c, 1);
   //esp is too fast for LCD at high baud rates - but not in short commands
#if DEBUGMODE
    printf("   Char sent : 0x%02X\n",c);
#endif
}

//******************************************************************************************************
void uLCD_freeBUFFER(uLCD_4DGL *lcd)         // Clear serial buffer before writing command
{
    ESP_ERROR_CHECK(uart_flush_input(lcd->uart_num));
#if DEBUGMODE
    printf("   Buffer cleared\n");
#endif
}

//******************************************************************************************************
int uLCD_writeCOMMAND(uLCD_4DGL *lcd, char *command, int number)   // send several BYTES making a command and return an answer
{
#if DEBUGMODE
    printf("\n");
    printf("New COMMAND : 0x%02X\n", command[0]);
#endif
    int i, n;
    char resp = 0;
    uLCD_freeBUFFER(lcd);
    uLCD_writeBYTE(lcd, 0xFF);
    for (i = 0; i < number; i++) {
        if (i<16)
            uLCD_writeBYTEfast(lcd, command[i]); // send command to serial port
        else
            uLCD_writeBYTE(lcd, command[i]); // send command to serial port but slower
    }
    ets_delay_us(1000);               // wait a millisecond

    n = uart_read_bytes(lcd->uart_num, &resp, 1, 1000);
#if DEBUGMODE
    if ( n>0 )
    printf("   Answer received : %x\n", resp );
    else
    printf("   No Answer received\n");
#endif

     switch (resp) {
        case ACK :                                     // if OK return   1
            n =  1;
            break;
        case NAK :                                     // if NOK return -1
            n = -1;
            break;
        default :
            n =  0;                                 // else return   0
            break;
    } 
    return n;
}

//**************************************************************************
void uLCD_reset(uLCD_4DGL *lcd)    // Reset LCD display
{
    vTaskDelay(pdMS_TO_TICKS(10));  // wait 10 ms, allowing for possibility system just started
    gpio_set_level((gpio_num_t)lcd->rst_pin, 0); // put RESET pin to low
    ets_delay_us(10);              // wait 10 microseconds
    gpio_set_level((gpio_num_t)lcd->rst_pin, 1); // put RESET back to high
    vTaskDelay(pdMS_TO_TICKS(3000) );  // wait full 3 seconds for LCD to initialize
    uLCD_freeBUFFER(lcd);           // clear buffer of possible garbage
    lcd->current_row         = 0;            // initial cursor row
    lcd->current_col         = 0;            // initial cursor col

}

//******************************************************************************************************
int uLCD_writeCOMMANDnull(uLCD_4DGL *lcd, char *command, int number)   // send several BYTES making a command and return an answer
{
#if DEBUGMODE
    printf("\n");
    printf("New COMMAND : 0x%02X\n", command[0]);
#endif
    int i, n;
    char resp = 0;
    uLCD_freeBUFFER(lcd);   
    uLCD_writeBYTE(lcd, 0x00); //command has a null prefix byte
    for (i = 0; i < number; i++) {
        if (i<16) //don't overflow LCD UART buffer
            uLCD_writeBYTEfast(lcd, command[i]); // send command to serial port
        else
            uLCD_writeBYTE(lcd, command[i]); // send command to serial port with delay
    }
      
    if ( resp ) uart_read_bytes(lcd->uart_num, &resp, 1, 1000);
    switch (resp) {
        case ACK :                                     // if OK return   1
            n =  1;
            break;
        case NAK :                                     // if NOK return -1
            n = -1;
            break;
        default :
            n =  0;                                 // else return   0
            break;
    }
#if DEBUGMODE
    printf("   Answer received : %d\n",n);
#endif

    return n;
}

//**************************************************************************
void uLCD_cls(uLCD_4DGL *lcd)    // clear screen
{
    char command[1] = "";
    command[0] = CLS;
    uLCD_writeCOMMAND(lcd, command, 1);

    lcd->current_row=0;
    lcd->current_col=0;
    lcd->current_hf = 1;
    lcd->current_wf = 1;
    uLCD_set_font(lcd, FONT_7X8);                 // initial font
}

//**************************************************************************
int uLCD_version(uLCD_4DGL *lcd)    // get API version
{
    char command[2] = "";
    command[0] = '\x00';
    command[1] = VERSION;
    return uLCD_readVERSION(lcd, command, 2);
}

//**************************************************************************
void uLCD_baudrate(uLCD_4DGL *lcd, int speed)    // set screen baud rate
// speed values are standard baud rates like 9600, 115200, etc.
// Changes both screen and processor baud rates
{
    char command[3]= "";
    uLCD_writeBYTE(lcd, 0x00);
    command[0] = BAUDRATE;
    command[1] = 0;
    int newbaud = BAUD_9600;
    switch (speed) {
        case  110 :
            newbaud = BAUD_110;
            break;
        case  300 :
            newbaud = BAUD_300;
            break;
        case  600 :
            newbaud = BAUD_600;
            break;
        case 1200 :
            newbaud = BAUD_1200;
            break;
        case 2400 :
            newbaud = BAUD_2400;
            break;
        case 4800 :
            newbaud = BAUD_4800;
            break;
        case 9600 :
            newbaud = BAUD_9600;
            break;
        case 14400 :
            newbaud = BAUD_14400;
            break;
        case 19200 :
            newbaud = BAUD_19200;
            break;
        case 31250 :
            newbaud = BAUD_31250;
            break;
        case 38400 :
            newbaud = BAUD_38400;
            break;
        case 56000 :
            newbaud = BAUD_56000;
            break;
        case 57600 :
            newbaud = BAUD_57600;
            break;
        case 115200 :
            newbaud = BAUD_115200;
            break;
    // Use of higher baud rates is questionable, because LCD is too slow to process them
        case 128000 :
            newbaud = BAUD_128000;
            break;
        case 256000 :
            newbaud = BAUD_256000;
            break;
        case 300000 :
            newbaud = BAUD_300000;
            speed = 272727;
            break;
        case 375000 :
            newbaud = BAUD_375000;
            speed = 333333;
            break;
        case 500000 :
            newbaud = BAUD_500000;
            speed = 428571;
            break;
        case 600000 :
            newbaud = BAUD_600000;
            break;
        case 750000 : 
            newbaud = BAUD_750000;
            break;
        case 1000000 :  
            newbaud = BAUD_1000000;
            break;
        case 1500000 :
            newbaud = BAUD_1500000;
            break;
        case 3000000 :
            newbaud = BAUD_3000000;
            break;
        default   :
            newbaud = BAUD_9600;
            speed = 9600;
            break;
    }

    int i, n;
    char resp = 0;
#if DEBUGMODE
    printf("Changing uLCD baudrate to %d (%d)\n", speed, newbaud);
#endif

    uLCD_freeBUFFER(lcd);
    command[1] = (char) (newbaud >>8);
    command[2] = (char) (newbaud % 256);
    ets_delay_us(3000);                // wait a few milliseconds before changing baud
    for (i = 0; i <3; i++) uLCD_writeBYTEfast(lcd, command[i]);      // send command to serial port

    ets_delay_us(10000);                // wait 10 milliseconds before changing baud
    //don't change processor baud until all characters get sent out
    uart_set_baudrate(lcd->uart_num, speed);
    i=0;
        uart_get_buffered_data_len(lcd->uart_num, (size_t*)&n);
        #if DEBUGMODE
        printf("   First buffer check : %d\n",n);
        #endif
    uart_read_bytes(lcd->uart_num, &resp, 1, 1000);

    switch (resp) {
        case ACK :                                     // if OK return   1
            n =  1;
            break;
        case NAK :                                     // if NOK return -1
            n = -1;
            break;
        default :
            n =  0;                                 // else return   0
            break;
    }
    #if DEBUGMODE
    printf("   Answer received : %d\n",n);  // it is normal NOT to get an answer here
    #endif
    vTaskDelay(pdMS_TO_TICKS(50));            // Give extra time for response, then clear buffer
    uLCD_freeBUFFER(lcd);

}

//******************************************************************************************************
int uLCD_readVERSION(uLCD_4DGL *lcd, char *command, int number)   // read screen info and populate data
{

    int i, n, resp = 0;
    char response[5] = "";

    uLCD_freeBUFFER(lcd);

    for (i = 0; i < number; i++) uLCD_writeBYTE(lcd, command[i]);    // send all chars to serial port

    resp = uart_read_bytes(lcd->uart_num, &response, 5, 1000);

    switch (resp) {
        case 2 :                                           // if OK populate data and return 1
            lcd->revision  = ((response[0] << 8 ) + response[1]);
            n      = 1;
            break;
        default :
            n =  0;                                     // else return 0
            break;
    }
    return n;
}

//****************************************************************************************************
void uLCD_background_color(uLCD_4DGL *lcd, int color)              // set screen background color
{
    char command[3]= "";                                  // input color is in 24bits like 0xRRGGBB

    command[0] = BCKGDCOLOR;

    int red5   = (color >> (16 + 3)) & 0x1F;              // get red on 5 bits
    int green6 = (color >> (8 + 2))  & 0x3F;              // get green on 6 bits
    int blue5  = (color >> (0 + 3))  & 0x1F;              // get blue on 5 bits

    command[1] = ((red5 << 3)   + (green6 >> 3)) & 0xFF;  // first part of 16 bits color
    command[2] = ((green6 << 5) + (blue5 >>  0)) & 0xFF;  // second part of 16 bits color

    uLCD_writeCOMMAND(lcd, command, 3);
}

//****************************************************************************************************
void uLCD_textbackground_color(uLCD_4DGL *lcd, int color)              // set screen background color
{
    char command[3]= "";                                  // input color is in 24bits like 0xRRGGBB

    command[0] = TXTBCKGDCOLOR;

    int red5   = (color >> (16 + 3)) & 0x1F;              // get red on 5 bits
    int green6 = (color >> (8 + 2))  & 0x3F;              // get green on 6 bits
    int blue5  = (color >> (0 + 3))  & 0x1F;              // get blue on 5 bits

    command[1] = ((red5 << 3)   + (green6 >> 3)) & 0xFF;  // first part of 16 bits color
    command[2] = ((green6 << 5) + (blue5 >>  0)) & 0xFF;  // second part of 16 bits color

    uLCD_writeCOMMAND(lcd, command, 3);
}

//****************************************************************************************************
void uLCD_display_control(uLCD_4DGL *lcd, char mode)     // set screen mode to value
{
    char command[3]= "";

    command[0] = DISPCONTROL;
    command[1] = 0;
    command[2] = mode;

    if (mode ==  ORIENTATION) {
        switch (mode) {
            case LANDSCAPE :
                lcd->current_orientation = IS_LANDSCAPE;
                break;
            case LANDSCAPE_R :
                lcd->current_orientation = IS_LANDSCAPE;
                break;
            case PORTRAIT :
                lcd->current_orientation = IS_PORTRAIT;
                break;
            case PORTRAIT_R :
                lcd->current_orientation = IS_PORTRAIT;
                break;
        }
    }
    uLCD_writeCOMMAND(lcd, command, 3);
    uLCD_set_font(lcd, lcd->current_font);
}
//****************************************************************************************************
void uLCD_display_power(uLCD_4DGL *lcd, char mode)     // 0 turns off display, 1 turns it on
{
    char command[3]= "";

    command[0] = DISPPOWER;
    command[1] = 0;
    command[2] = mode;
    uLCD_writeCOMMAND(lcd, command, 3);
}


//******************************************************************************************************
int uLCD_getSTATUS(uLCD_4DGL *lcd, char *command, int number)   // read screen info and populate data
{

#if DEBUGMODE
    printf("\n");
    printf("New COMMAND : 0x%02X\n", command[0]);
#endif

    int i, n, resp;
    char response[5] = "";

    uLCD_freeBUFFER(lcd);

    for (i = 0; i < number; i++) uLCD_writeBYTE(lcd, command[i]);    // send all chars to serial port

    resp = uart_read_bytes(lcd->uart_num, &response, 4, 1000);

    switch (resp) {
        case 4 :
            n = (int)response[1];         // if OK populate data
            break;
        default :
            n =  -1;                      // else return   0
            break;
    }

#if DEBUGMODE
    printf("   Answer received : %d\n", resp);
#endif

    return n;
}

