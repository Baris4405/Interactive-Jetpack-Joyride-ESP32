//
// uLCD_4DGL is a class to drive 4D Systems LCD screens
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


//Media Commands

//******************************************************************************************************
int uLCD_4DGL_media_init(uLCD_4DGL* lcd)
{
    int resp = 0;
    char response[2];
    char command[1] = "";
    command[0] = MINIT;
    uLCD_writeCOMMAND(lcd, command, 1);
    resp = uart_read_bytes(lcd->uart_num, &response, 2, 1000);
    if (resp == 2) {
        resp = (int)((response[0] << 8) + response[1]);
    } else {
        resp = -1;
        }
    return resp;
}

//******************************************************************************************************
void uLCD_4DGL_set_byte_address(uLCD_4DGL* lcd, int hi, int lo)
{
    char command[5]= "";
    command[0] = SBADDRESS;

    command[1] = (hi >> 8) & 0xFF;
    command[2] = hi & 0xFF;

    command[3] = (lo >> 8) & 0xFF;
    command[4] = lo & 0xFF;
    uLCD_writeCOMMAND(lcd, command, 5);
}

//******************************************************************************************************
void uLCD_4DGL_set_sector_address(uLCD_4DGL* lcd, int hi, int lo)
{

    char command[5]= "";
    command[0] = SSADDRESS;

    command[1] = (hi >> 8) & 0xFF;
    command[2] = hi & 0xFF;

    command[3] = (lo >> 8) & 0xFF;
    command[4] = lo & 0xFF;
    uLCD_writeCOMMAND(lcd, command, 5);
}

//******************************************************************************************************
char uLCD_4DGL_read_byte(uLCD_4DGL* lcd)
{
    char resp = 0;
    char response;
    char command[1] = "";
    command[0] = READBYTE;
    uLCD_writeCOMMAND(lcd, command, 1);
    resp = uart_read_bytes(lcd->uart_num, &response, 1, 1000);
    if (resp == 1) {
        resp = response;
    } else {
        resp = -1;
    }
    return resp;
}

//******************************************************************************************************
int  uLCD_4DGL_read_word(uLCD_4DGL* lcd)
{
    int resp=0;
    char response[2];
    char command[1] = "";
    command[0] = READWORD;
    uLCD_writeCOMMAND(lcd, command, 1);
    resp = uart_read_bytes(lcd->uart_num, &response, 2, 1000);
    if (resp == 2) {
        resp = (int)((response[0] << 8) + response[1]);
    } else {
        resp = -1;
    }
    return resp;
}

//******************************************************************************************************
void uLCD_4DGL_write_byte(uLCD_4DGL* lcd, int value)
{
    char command[3]= "";

    command[0] = WRITEBYTE;

    command[1] = (value >> 8) & 0xFF;
    command[2] = value & 0xFF;
    uLCD_writeCOMMAND(lcd, command, 3);
}

//******************************************************************************************************
void uLCD_4DGL_write_word(uLCD_4DGL* lcd, int value)
{
    char command[3]= "";

    command[0] = WRITEWORD;

    command[1] = (value >> 8) & 0xFF;
    command[2] = value & 0xFF;
    uLCD_writeCOMMAND(lcd, command, 3);
}

//******************************************************************************************************
void uLCD_4DGL_flush_media(uLCD_4DGL* lcd )
{
    char command[1] = "";
    command[0] = FLUSHMEDIA;
    uLCD_writeCOMMAND(lcd, command, 1);
}

//******************************************************************************************************
void uLCD_4DGL_display_image(uLCD_4DGL* lcd, int x, int y)
{
    char command[6]= "";
    command[0] = DISPLAYIMAGE;

    command[1] = (x >> 8) & 0xFF;
    command[2] = x & 0xFF;

    command[3] = (y >> 8) & 0xFF;
    command[4] = y & 0xFF;
    uLCD_writeCOMMAND(lcd, command, 5);
}

//******************************************************************************************************
void uLCD_4DGL_display_video(uLCD_4DGL* lcd, int x, int y)
{
    char command[5]= "";
    command[0] = DISPLAYVIDEO;

    command[1] = (x >> 8) & 0xFF;
    command[2] = x & 0xFF;

    command[3] = (y >> 8) & 0xFF;
    command[4] = y & 0xFF;
    uLCD_writeCOMMAND(lcd, command, 5);
}

//******************************************************************************************************
void uLCD_4DGL_display_frame(uLCD_4DGL* lcd, int x, int y, int framenum)
{
    char command[7]= "";

    command[0] = DISPLAYFRAME;

    command[1] = (x >> 8) & 0xFF;
    command[2] = x & 0xFF;

    command[3] = (y >> 8) & 0xFF;
    command[4] = y & 0xFF;

    command[5] = (framenum >> 8) & 0xFF;
    command[6] = framenum & 0xFF;
    uLCD_writeCOMMAND(lcd, command,7);
}

