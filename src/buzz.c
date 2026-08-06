// ============================================
// The buzz class file
//
// Copyright 2026 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
//==================================================================


#include "buzz.h"

void buzz_init (Buzz * b)
{
b->loc_x = 5; // Starting x coordinate for buzz
b->loc_y = 7; // Starting y coordinate for buzz
b->loc_x_prev = 5; // Previous x coordinate for buzz
b->loc_y_prev = 7; // Previous y coordinate for buzz
b->score = 0; // Starting score for buzz
b->lives = 1; // Starting lives for buzz
b->attack_energy = 0; // Starting attack energy for buzz
b->speed = 1; // Starting speed for buzz

    // TODO: Implement
    // 1. Set starting and previous coordinates for your buzz player 
    // 2. Set starting lives, score, and attack energy
    // 3. Initialize any other variables you added to the Buzz struct
}
