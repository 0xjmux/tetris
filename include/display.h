#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include "tetris.h"

#ifdef __linux__
#include <stdio.h>
#include <ncurses.h>
#endif


void nc_init_colors(WINDOW *w);


#endif