#ifndef UTILS_H
#define UTILS_H

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


bool restore_game_state(TetrisGame *tg, const char* filename, FILE *gamelog);


void reconstruct_board_from_str_row(TetrisBoard *tb, const char *name, const char *value);
void print_board_state(TetrisBoard tb, FILE *file, bool ini_out);

void sleep_millis(uint16_t millis);




#endif