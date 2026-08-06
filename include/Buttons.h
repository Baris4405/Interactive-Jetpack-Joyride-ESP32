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
    int button0_pin;
    int button1_pin;
    int button2_pin;
    int button3_pin;
    int pressed[4];
} Buttons;

void init_Buttons(Buttons* ns, int button0_pin, int button1_pin, int button2_pin, int button3_pin);
int pb_readall(Buttons* ns);
int pb0(Buttons* ns);
int pb1(Buttons* ns);
int pb2(Buttons* ns);
int pb3(Buttons* ns);
