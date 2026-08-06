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

typedef struct {
    int up_pin;
    int down_pin;
    int left_pin;
    int right_pin;
    int center_pin;
} Nav_Switch;

void init_Nav_Switch(Nav_Switch* ns, int up_pin, int down_pin, int left_pin, int right_pin, int center_pin);
int ns_read(Nav_Switch* ns);
int up(Nav_Switch* ns);
int down(Nav_Switch* ns);
int left(Nav_Switch* ns);
int right(Nav_Switch* ns);
int center(Nav_Switch* ns);
