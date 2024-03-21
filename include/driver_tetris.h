
#ifndef DRIVER_TETRIS
#define DRIVER_TETRIS

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>   // sleep, timespec
#include <sys/time.h>

#include <assert.h>
#include <ncurses.h>

#include "ini.h"

#include "tetris.h"
#include "display.h"



// sane defaults for the curses window
// if this was for more than testing i'd determine it dynamically
//  using ENV vars
#define C_LINES 50
#define C_COLS 100

// make blocks 2 columns wide, makes everything nicer
#define BLOCK_WIDTH 2

#define SCORE_WIN_ROWS 10
#define SCORE_WIN_COLS 30

#define SCREEN_REFRESH_INTERVAL_USEC 25000 // 50ms


// ncursew macros to print out filled and empty cells
#define ADD_BLOCK(w,x) waddch((w),' '|A_REVERSE|COLOR_PAIR(x));     \
                       waddch((w),' '|A_REVERSE|COLOR_PAIR(x))
#define ADD_EMPTY(w) waddch((w), ' '); waddch((w), ' ')

void display_board(WINDOW *w, TetrisBoard *b);
void update_score(WINDOW *w, TetrisGame *tg);

// Debug functions
void print_keypress(enum player_move move);


// void refresh_debug_var_window(WINDOW *w, enum player_move move);
void refresh_debug_var_window(WINDOW *w);
#endif