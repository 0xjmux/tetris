#ifndef TETRIS_TEST_HELPERS_H
#define TETRIS_TEST_HELPERS_H

#include "tetris.h"
#include <time.h>

void add_incomplete_line_to_board(TetrisBoard *tb, uint8_t row);
void print_board_state(TetrisBoard tb);
void setup_N_lines_board(TetrisBoard *tb, uint8_t N);

// void setup_moveCheck(TetrisGame *tg, uint8_t gen_height, TetrisPiece testpiece);
void setup_moveCheck(TetrisGame *tg, uint8_t gen_height, \
    TetrisPiece testpiece, int *test_state);
void fill_board_rectangle(TetrisBoard *tb, int8_t tL_row, int8_t tL_col, \
    int8_t bR_row, int8_t bR_col, int8_t fill_value);


bool check_for_occ_cells_in_row(TetrisGame *tg, uint8_t row);

void print_all_tetrominos(void);
void print_piece(const tetris_location t[4][4]);

void sleep_millis(uint16_t millis);


#endif