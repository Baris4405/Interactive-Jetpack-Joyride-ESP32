//=================================================================
// Copyright 2026 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//=================================================================

#include "Nav_Switch.h"

// Initialize (replaces constructor of previous C++ version)
// Normal values: 
// up_pin GPIO_NUM_19
// down_pin GPIO_NUM_22
// left_pin GPIO_NUM_21
// right_pin GPIO_NUM_23
// center_pin GPIO_NUM_20

void init_Nav_Switch(Nav_Switch* ns, int up_pin, int down_pin, int left_pin, int right_pin, int center_pin) {
    ns->up_pin = up_pin;
    ns->down_pin = down_pin;
    ns->left_pin = left_pin;
    ns->right_pin = right_pin;
    ns->center_pin = center_pin;

    gpio_set_direction((gpio_num_t)ns->up_pin, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)ns->down_pin, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)ns->left_pin, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)ns->right_pin, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)ns->center_pin, GPIO_MODE_INPUT);

    gpio_set_pull_mode((gpio_num_t)ns->up_pin, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode((gpio_num_t)ns->down_pin, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode((gpio_num_t)ns->left_pin, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode((gpio_num_t)ns->right_pin, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode((gpio_num_t)ns->center_pin, GPIO_PULLUP_ONLY);
}


int ns_read(Nav_Switch* ns) {
    int value = 0;
    value |= (!gpio_get_level((gpio_num_t)ns->up_pin)) << 0;
    value |= (!gpio_get_level((gpio_num_t)ns->down_pin)) << 1;
    value |= (!gpio_get_level((gpio_num_t)ns->left_pin)) << 2;
    value |= (!gpio_get_level((gpio_num_t)ns->right_pin)) << 3;
    value |= (!gpio_get_level((gpio_num_t)ns->center_pin)) << 4;
    return value;
}

int up(Nav_Switch* ns) {
    return !gpio_get_level((gpio_num_t)ns->up_pin);
}
int down(Nav_Switch* ns) {
    return !gpio_get_level((gpio_num_t)ns->down_pin);
}
int left(Nav_Switch* ns) {
    return !gpio_get_level((gpio_num_t)ns->left_pin);
}
int right(Nav_Switch* ns) {
    return !gpio_get_level((gpio_num_t)ns->right_pin);
}
int center(Nav_Switch* ns) {
    return !gpio_get_level((gpio_num_t)ns->center_pin);
}
