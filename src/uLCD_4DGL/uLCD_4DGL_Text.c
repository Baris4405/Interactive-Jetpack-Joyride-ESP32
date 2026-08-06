//
// uLCD_4DGL is a class to drive 4D Systems TFT touch screens
//
// Copyright (C) <2010> Stephane ROCHON <stephane.rochon at free.fr>
// Modifed for Goldelox processor <2013> Jim Hamblen
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

//****************************************************************************************************
void uLCD_set_font_size(uLCD_4DGL* lcd, char width, char height)     // set font size
// This does not actually change the font, but figures out how many columns and rows fit on the screen
// based on the current font and the requested size multipliers
{
    if (lcd->current_orientation == IS_PORTRAIT) {
        lcd->current_fx = width;
        lcd->current_fy = height;
    } else {
        lcd->current_fy = height;
        lcd->current_fx = width;
    }
    lcd->max_col = lcd->current_w / (lcd->current_fx*lcd->current_wf);
    lcd->max_row = lcd->current_h / (lcd->current_fy*lcd->current_hf);
}

//****************************************************************************************************
void uLCD_set_font(uLCD_4DGL* lcd, char mode)     // set font - system or SD media
{
    char command[3]= "";
    command[0] = SETFONT;
    command[1] = 0;
    command[2] = mode;

    lcd->current_font = mode;

    if (lcd->current_orientation == IS_PORTRAIT) {
        lcd->current_w = SIZE_X;
        lcd->current_h = SIZE_Y;
    } else {
        lcd->current_w = SIZE_Y;
        lcd->current_h = SIZE_X;
    }

    switch (mode) {
        case FONT_5X7 :
            lcd->current_fx = 6;
            lcd->current_fy = 8;
            break;
        case FONT_7X8 :
            lcd->current_fx = 7;
            lcd->current_fy = 8;
            break;
        case FONT_8X8 :
            lcd->current_fx = 8;
            lcd->current_fy = 8;
            break;
        case FONT_8X12 :
            lcd->current_fx = 8;
            lcd->current_fy = 12;
            break;
        case FONT_12X16 :
            lcd->current_fx = 12;
            lcd->current_fy = 16;
            break;
        default:
            lcd->current_fx = 7;
            lcd->current_fy = 8;
    }

    lcd->max_col = lcd->current_w / (lcd->current_fx*lcd->current_wf);
    lcd->max_row = lcd->current_h / (lcd->current_fy*lcd->current_hf);

    uLCD_writeCOMMAND(lcd, command, 3);
}



//****************************************************************************************************
void uLCD_text_mode(uLCD_4DGL* lcd, char mode)     // set text background opacity mode
// 0 = transparent, 1 = opaque
{
    char command[3]= "";
    command[0] = TEXTMODE;
    command[1] = 0;
    command[2] = mode;

    uLCD_writeCOMMAND(lcd, command, 3);
}

//****************************************************************************************************
void uLCD_text_bold(uLCD_4DGL* lcd, char mode)     // set text mode
{
    char command[3]= "";
    command[0] = TEXTBOLD;
    command[1] = 0;
    command[2] = mode;

    uLCD_writeCOMMAND(lcd, command, 3);
}

//****************************************************************************************************
void uLCD_text_italic(uLCD_4DGL* lcd, char mode)     // set text mode
{
    char command[3]= "";
    command[0] = TEXTITALIC;
    command[1] = 0;
    command[2] = mode;

    uLCD_writeCOMMAND(lcd, command, 3);
}

//****************************************************************************************************
void uLCD_text_inverse(uLCD_4DGL* lcd,char mode)     // set text mode
{
    char command[3]= "";
    command[0] = TEXTINVERSE;
    command[1] = 0;
    command[2] = mode;

    uLCD_writeCOMMAND(lcd, command, 3);
}

//****************************************************************************************************
void uLCD_text_underline(uLCD_4DGL* lcd,char mode)     // set text mode
{
    char command[3]= "";
    command[0] = TEXTUNDERLINE;
    command[1] = 0;
    command[2] = mode;

    uLCD_writeCOMMAND(lcd, command, 3);
}

//****************************************************************************************************
void uLCD_text_width(uLCD_4DGL* lcd, char width)     // set text width
{
    char command[3]= "";
    command[0] = TEXTWIDTH;
    command[1] = 0;
    command[2] = width;
    lcd->current_wf = width;
    lcd->max_col = lcd->current_w / (lcd->current_fx*lcd->current_wf);
    uLCD_writeCOMMAND(lcd, command, 3);
}

//****************************************************************************************************
void uLCD_text_height(uLCD_4DGL* lcd,char height)     // set text height
{
    char command[3]= "";
    command[0] = TEXTHEIGHT;
    command[1] = 0;
    command[2] = height;
    lcd->current_hf = height;
    lcd->max_row = lcd->current_h / (lcd->current_fy*lcd->current_hf);
    uLCD_writeCOMMAND(lcd, command, 3);
}


//****************************************************************************************************
void uLCD_text_char(uLCD_4DGL* lcd, char c, char col, char row, int color)     // draw a text char
{
    char command[6]= "";
    command[0] = 0xE4; //move cursor
    command[1] = 0;
    command[2] = row;
    command[3] = 0;
    command[4] = col;
    uLCD_writeCOMMAND(lcd, command, 5);

    command[0] = 0x7F;  //set color

    int red5   = (color >> (16 + 3)) & 0x1F;              // get red on 5 bits
    int green6 = (color >> (8 + 2))  & 0x3F;              // get green on 6 bits
    int blue5  = (color >> (0 + 3))  & 0x1F;              // get blue on 5 bits

    command[1] = ((red5 << 3)   + (green6 >> 3)) & 0xFF;  // first part of 16 bits color
    command[2] = ((green6 << 5) + (blue5 >>  0)) & 0xFF;  // second part of 16 bits color
    uLCD_writeCOMMAND(lcd, command, 3);

    command[0] = TEXTCHAR;  //print char
    command[1] = 0;
    command[2] = c;
    uLCD_writeCOMMAND(lcd, command, 3);
}


//****************************************************************************************************
void uLCD_text_string(uLCD_4DGL* lcd, char *s, char col, char row, char font, int color)     // draw a text string
{
    char command[1000]= "";
    int size = strlen(s);
    int i = 0;

    uLCD_set_font(lcd, font);

    command[0] = 0xE4; //move cursor
    command[1] = 0;
    command[2] = row;
    command[3] = 0;
    command[4] = col;
    uLCD_writeCOMMAND(lcd, command, 5);

    command[0] = 0x7F;  //set color
    int red5   = (color >> (16 + 3)) & 0x1F;              // get red on 5 bits
    int green6 = (color >> (8 + 2))  & 0x3F;              // get green on 6 bits
    int blue5  = (color >> (0 + 3))  & 0x1F;              // get blue on 5 bits

    command[1] = ((red5 << 3)   + (green6 >> 3)) & 0xFF;  // first part of 16 bits color
    command[2] = ((green6 << 5) + (blue5 >>  0)) & 0xFF;  // second part of 16 bits color
    uLCD_writeCOMMAND(lcd, command, 3);

    command[0] = TEXTSTRING;
    for (i=0; i<size; i++) command[1+i] = s[i];
    command[1+size] = 0;
    uLCD_writeCOMMANDnull(lcd, command, 2 + size);
}



//****************************************************************************************************
void uLCD_locate(uLCD_4DGL* lcd,char col, char row)     // place text curssor at col, row
{
    char command[5] = "";
    lcd->current_col = col;
    lcd->current_row = row;
    command[0] = MOVECURSOR; //move cursor
    command[1] = 0;
    command[2] = lcd->current_row;
    command[3] = 0;
    command[4] = lcd->current_col;
    uLCD_writeCOMMAND(lcd, command, 5);
}

//****************************************************************************************************
void uLCD_text_color(uLCD_4DGL* lcd, int color)     // set text color
{
    char command[5] = "";
    lcd->current_color = color;
    command[0] = TXTFGGDCOLOR;  //set color

    int red5   = (color >> (16 + 3)) & 0x1F;              // get red on 5 bits
    int green6 = (color >> (8 + 2))  & 0x3F;              // get green on 6 bits
    int blue5  = (color >> (0 + 3))  & 0x1F;              // get blue on 5 bits

    command[1] = ((red5 << 3)   + (green6 >> 3)) & 0xFF;  // first part of 16 bits color
    command[2] = ((green6 << 5) + (blue5 >>  0)) & 0xFF;  // second part of 16 bits color
    uLCD_writeCOMMAND(lcd, command, 3);
}

//****************************************************************************************************
void uLCD_putc(uLCD_4DGL* lcd,char c)      // place char at current cursor position
//used by virtual printf function _putc
{
    char command[6] ="";
    if(c<0x20) {
        if(c=='\n') {
            lcd->current_col = 0;
            lcd->current_row++;
            command[0] = MOVECURSOR; //move cursor to start of next line
            command[1] = 0;
            command[2] = lcd->current_row;
            command[3] = 0;
            command[4] = lcd->current_col;
            uLCD_writeCOMMAND(lcd, command, 5);
        }
        if(c=='\r') {
            lcd->current_col = 0;
            command[0] = MOVECURSOR; //move cursor to start of line
            command[1] = 0;
            command[2] = lcd->current_row;
            command[3] = 0;
            command[4] = lcd->current_col;
            uLCD_writeCOMMAND(lcd, command, 5);
        }
        if(c=='\f') {
            uLCD_cls(lcd); //clear screen on form feed
        }
    } else {
        command[0] = PUTCHAR;
        command[1] = 0x00;
        command[2] = c;
        uLCD_writeCOMMAND(lcd, command,3);
        lcd->current_col++;
    }
    if (lcd->current_col == lcd->max_col) {
        lcd->current_col = 0;
        lcd->current_row++;
        command[0] = MOVECURSOR; //move cursor to next line
        command[1] = 0;
        command[2] = lcd->current_row;
        command[3] = 0;
        command[4] = lcd->current_col;
        uLCD_writeCOMMAND(lcd, command, 5);
    }
    if (lcd->current_row == lcd->max_row) {
        lcd->current_row = 0;
        command[0] = MOVECURSOR; //move cursor back to start
        command[1] = 0;
        command[2] = lcd->current_row;
        command[3] = 0;
        command[4] = lcd->current_col;
        uLCD_writeCOMMAND(lcd, command, 5);
    }
}


//****************************************************************************************************
void uLCD_puts(uLCD_4DGL* lcd, char *s, int fast)     // place string at current cursor position
// if fast=0, carriage return, newline, and form feed will work, AND it will automatically insert
//   newlines when last column is reached
// if fast<>0, it will be slightly faster, but text will extend past right side if too long.
{
    if (!fast)
    {
        int len = strlen(s);
        for (int i = 0; i < len; i++) {
            uLCD_putc( lcd, s[i] );
        }
    } 
    else
    {
        uLCD_text_string(lcd, s, lcd->current_col, lcd->current_row, lcd->current_font, lcd->current_color);

        lcd->current_col += strlen(s);

        if (lcd->current_col >= lcd->max_col) {
            lcd->current_row++;
            lcd->current_col = 0;
            uLCD_locate(lcd, lcd->current_col, lcd->current_row);
        }
        if (lcd->current_row >= lcd->max_row) {
            lcd->current_row = lcd->max_row;
            uLCD_locate(lcd, lcd->current_col, lcd->current_row);
        }
    }
        
}

void uLCD_printf(uLCD_4DGL* lcd, char *s, ...)   // place FORMATTED string at current cursor position
{
    va_list argptr;
    va_start(argptr, s);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), s, argptr);
    va_end(argptr);
    uLCD_puts(lcd, buffer, 0);
}
