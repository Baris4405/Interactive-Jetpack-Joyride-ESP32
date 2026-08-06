// ============================================
// The header file for Graphics class file.
//
// Copyright 2026 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
//==================================================================


#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "globals.h"

/**
 * Takes a string image and draws it to the screen. The string is 121 characters
 * long, and represents an 11x11 tile in row-major ordering (across, then down,
 * like a regular multi-dimensional array). The available colors are:
 *      R = Red
 *      Y = Yellow
 *      G = Green
 *      D = Brown ("dirt")
 *      5 = Light grey (50%)
 *      3 = Dark grey (30%)
 *      Any other character is black
 * More colors can be easily added by following the pattern already given.
 */
void draw_img(int u, int v, const char* img);

/**
 * DrawFunc functions. 
 * These can be used as the MapItem draw functions.
 */
void draw_nothing(int u, int v);
void draw_wall(int u, int v);
void draw_nectar(int u, int v);
void draw_buzz(int u, int v);
void draw_heal(int u, int v);
void draw_poison(int u, int v);
void draw_stinger(int u, int v);  
void draw_upper_status();
void draw_lower_status();

//advanced feature function prototype for directional sprites
void draw_buzz_direction(int u, int v, int dir);

//advanced feature function prototype for pollen
void draw_pollen_tile(int u, int v);

#endif // GRAPHICS_H