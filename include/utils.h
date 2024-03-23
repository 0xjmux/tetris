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

const char* get_piece_str(enum piece_type ptype);

void print_board_state(TetrisBoard tb, FILE *file, bool ini_out);

void sleep_millis(uint16_t millis);

// saving and restoring game state
bool restore_game_state(TetrisGame *tg, const char* filename, FILE *gamelog);
void reconstruct_board_from_str_row(TetrisBoard *tb, const char *name, const char *value);
void save_game_state(TetrisGame *tg, const char* filename);
void ini_save_board_to_file(FILE *file, TetrisBoard tb);





#endif