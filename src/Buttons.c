//=================================================================
// Copyright 2026 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//=================================================================

#include "Buttons.h"

// Initialize 
// Normal values: 
// button0_pin GPIO_NUM_6
// button1_pin GPIO_NUM_7
// button2_pin GPIO_NUM_0
// button3_pin GPIO_NUM_1


void init_Buttons(Buttons* buttons, int button0_pin, int button1_pin, int button2_pin, int button3_pin) {
    buttons->button0_pin = button0_pin;
    buttons->button1_pin = button1_pin;
    buttons->button2_pin = button2_pin;
    buttons->button3_pin = button3_pin;      
    
    gpio_set_direction((gpio_num_t)button0_pin, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)button1_pin, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)button2_pin, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)button3_pin, GPIO_MODE_INPUT);

    gpio_set_pull_mode((gpio_num_t)button0_pin, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode((gpio_num_t)button1_pin, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode((gpio_num_t)button2_pin, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode((gpio_num_t)button3_pin, GPIO_PULLUP_ONLY);
}

int pb_readall(Buttons* pbs) {
    int value = 0;
    value |= (!gpio_get_level((gpio_num_t)pbs->button0_pin)) << 0;
    value |= (!gpio_get_level((gpio_num_t)pbs->button1_pin)) << 1;
    value |= (!gpio_get_level((gpio_num_t)pbs->button2_pin)) << 2;
    value |= (!gpio_get_level((gpio_num_t)pbs->button3_pin)) << 3;
    return value;
}

int pb0(Buttons* pbs) {
    return !gpio_get_level((gpio_num_t)pbs->button0_pin);
}
int pb1(Buttons* pbs) {
    return !gpio_get_level((gpio_num_t)pbs->button1_pin);
}
int pb2(Buttons* pbs) {
    return !gpio_get_level((gpio_num_t)pbs->button2_pin);
}
int pb3(Buttons* pbs) {
    return !gpio_get_level((gpio_num_t)pbs->button3_pin);
}
