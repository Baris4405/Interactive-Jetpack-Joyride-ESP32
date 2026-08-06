// ============================================
// The header file for buzz class file.
//
// Copyright 2026 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
//==================================================================


#ifndef BUZZ_H
#define BUZZ_H


// Buzz Structure
typedef struct {
    int loc_x, loc_y, loc_x_prev, loc_y_prev; // Location of the buzz
    int score; // Current score of the buzz
    int lives; // Current lives of the buzz
    int attack_energy; // Energy available for attacks (each attack consumes a variable amount)
    int speed;    // add any other variables you want to keep track of for your buzz player here!
} Buzz;

// Initialize an Buzz structure
// You want to assign initial values to each of the variables defined above
// in the Buzz structure.
void buzz_init (Buzz* buzz);

#endif // BUZZ_H