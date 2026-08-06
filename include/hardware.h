//=================================================================
// Copyright 2026 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//=================================================================

#pragma once

#include <stdio.h>
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "led_strip_types.h"
#include "esp_log.h"
#include "esp_err.h"
#include "Buttons.h"
#include "Nav_Switch.h"

// GPIO assignment
#define LED_GPIO_PIN  GPIO_NUM_8  // Neopixel (WS2812B) connected to GPIO8 on the ESP32 board
#define LCD_RESET_GPIO_PIN GPIO_NUM_18 // LCD RESET connected to GPIO18 on the ESP32 board
#define LCD_TX_GPIO_PIN GPIO_NUM_10 // LCD TX connected to GPIO10 on the ESP32 board (best for UART1)
#define LCD_RX_GPIO_PIN GPIO_NUM_9  // LCD RX connected to GPIO9 on the ESP32 board (best for UART1)
#define PB0_GPIO_PIN  GPIO_NUM_6  // Pushbutton 0 connected to GPIO6 on the ESP32 board
#define PB1_GPIO_PIN  GPIO_NUM_7  // Pushbutton 1 connected to GPIO7 on the ESP32 board 
#define PB2_GPIO_PIN  GPIO_NUM_0  // Pushbutton 2 connected to GPIO0 on the ESP32 board 
#define PB3_GPIO_PIN  GPIO_NUM_1  // Pushbutton 3 connected to GPIO1 on the ESP32 board 
#define SPEAKER_GPIO_PIN  GPIO_NUM_3 // Speaker connected to GPIO15 on the ESP32 board
#define NAV_UP_GPIO_PIN     GPIO_NUM_19  // Nav Switch UP connected to GPIO19 on the ESP32 board
#define NAV_DOWN_GPIO_PIN   GPIO_NUM_22  // Nav Switch DOWN connected to GPIO22 on the ESP32 board
#define NAV_LEFT_GPIO_PIN   GPIO_NUM_21  // Nav Switch LEFT connected to GPIO21 on the ESP32 board
#define NAV_RIGHT_GPIO_PIN  GPIO_NUM_23  // Nav Switch RIGHT connected to GPIO23 on the ESP32 board
#define NAV_CENTER_GPIO_PIN GPIO_NUM_20  // Nav Switch CENTER connected to GPIO20 on the ESP32 board

#define MAX_TIMER_COUNT 1000000 / (2 * 20) // Max timer count for 20 Hz square wave
#define MIN_TIMER_COUNT 1000000 / (2 * 10000) // Min timer count for 20 kHz square wave

typedef struct {   // sound structure for speaker configuration
   gpio_num_t speaker_gpio;  // GPIO pin for speaker output
   int resolution;         // Frequency resolution in Hz 
   int frequency;         // Frequency of the sound (square) wave
   int volume;           // Volume level (not implemented)
   int duration_ms;     // Duration of the sound in milliseconds (0 means continuous, if enabled)
   int enabled;         // Sound enabled flag
} sound;

// Structure that represents all the inputs to the game.
// If additional hardware is added, new elements should be added to this struct.
typedef struct {
    int up, down, left, right, center;     // Button presses
    int b0, b1, b2, b3;               // Pushbutton presses
} GameInputs;

void read_inputs(GameInputs* inputs, Buttons* buttons, Nav_Switch* myNav);

led_strip_handle_t configure_led(gpio_num_t led_gpio_pin);
int configure_sound(sound* my_sound, gptimer_handle_t* gptimer);

int sound_on(sound* my_sound, gptimer_handle_t* gptimer);

int tone(sound* my_sound, gptimer_handle_t* gptimer, int frequency );

int init_hardware();