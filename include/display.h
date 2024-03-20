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

void print_board_state(TetrisBoard tb, FILE *file);
void save_game_state(TetrisGame *tg);

void save_board_to_file(FILE *file, TetrisBoard tb);

#endif