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

void save_game_state(TetrisGame *tg, const char* filename);

// void old_save_board_to_file(FILE *file, TetrisBoard tb);
void ini_save_board_to_file(FILE *file, TetrisBoard tb);

#endif