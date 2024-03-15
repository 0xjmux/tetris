#ifndef TETRIS_TEST_HELPERS_H
#define TETRIS_TEST_HELPERS_H

#include "tetris.h"

void add_incomplete_line_to_board(TetrisBoard *tb, uint8_t row);
void print_board_state(TetrisBoard tb);
void setup_N_lines_board(TetrisBoard *tb, uint8_t N);

// void setup_moveCheck(TetrisGame *tg, uint8_t gen_height, TetrisPiece testpiece);
void setup_moveCheck(TetrisGame *tg, uint8_t gen_height, \
    TetrisPiece testpiece, int *test_state);

#endif