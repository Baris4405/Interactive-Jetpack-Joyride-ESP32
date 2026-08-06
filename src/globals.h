// ============================================
// The header file for general project settings
//
// Copyright 2026 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
//==================================================================


#ifndef GLOBAL_H
#define GLOBAL_H

// Include all the hardware libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_rom_sys.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/gptimer.h"
#include "soc/uart_pins.h"
#include "hardware.h"
#include "led_strip.h"
#include "Buttons.h"
#include "uLCD_4DGL.h"  
#include "Nav_Switch.h"

// LCD screen object
extern uLCD_4DGL myLCD;

// Speaker object and its associated timer
extern sound speaker;
// gptimer_handle_t gptimer = NULL;

extern Buttons buttons;    // Initialize the push buttons
extern Nav_Switch myNav;   // Initialize the navigation switch

// Global settings
#define F_DEBUG   1                     // Debug flag

#define BACKGROUND_COLOR 0x000000       // Black

#define PI 3.1415926

#define NO_RESULT 0
#define NO_ACTION 0
#define B1 1
#define B2 2
#define B3 3
#define GO_UP 4
#define GO_LEFT 5
#define GO_DOWN 6
#define GO_RIGHT 7
#define CENTER_PRESSED 8
#define MOVED 9
#define GOT_OBJ 10
#define GAME_OVER 11
#define WIN 12

// === [define the macro of error handle function] ===
// when the condition (c) is not true, assert the program and show error code
#define ASSERT_P(c,e) do { \
    if(!(c)){ \
        printf("\nERROR:%d\n",e); \
        while(1); \
    } \
} while (0)

// === [error code] ===
#define ERROR_NONE 0 // All good in the hood
#define ERROR_MEH -1 // This is how errors are done

#endif //GLOBAL_H