/**
 * Helper functions for testing tetris game code
 * 
*/

#include "tetris_test_helpers.h"


/** 
 * Testing function to add incomplete lines to TetrsiBoard
 * to test move validity checking
*/
void add_incomplete_line_to_board(TetrisBoard *tb, uint8_t row) {
    // uint8_t highest_cell  = tg->board->highest_occupied_cell;
    
    // index of col to skip (so won't be a complete row)
    uint8_t skipped_col_index = rand() % TETRIS_COLS + 1;    
    // printf("adding incomplete line to row %d, col %d\n", row, skipped_col_index);
    for (int i = 0; i < TETRIS_COLS; i++) {
        if (i != skipped_col_index) {
            tb->board[row][i] = 1;
        }
    }
}

/**
 * Add N incomplete lines to TetrisBoard tb
*/
void setup_N_lines_board(TetrisBoard *tb, uint8_t N) {
    
    for (int i = TETRIS_ROWS -1 ; i > TETRIS_ROWS - N - 1; i--) {
        add_incomplete_line_to_board(tb, i);
    }
    tb->highest_occupied_cell = TETRIS_ROWS - N;
}

/**
 * Print current board state to console
*/
void print_board_state(TetrisBoard tb) {
    // draw existing pieces on board
    printf("Highest occupied cell: %d\n   ", tb.highest_occupied_cell);
    for (int i = 0; i < TETRIS_COLS; i++) 
        printf("----");
    printf("----\n");

    for (int i = 0; i < TETRIS_ROWS; i++) {
        printf("%-3d| ", i);
        for (int j = 0; j < TETRIS_COLS; j++) {
            if (tb.board[i][j] >= 0) {
                printf("%-3d ", tb.board[i][j]);
            }
            else {
                printf("    ");
            }
        }
        printf(" |\n");
    }
}

/**
 * Setup conditions for testing check_valid_move
 * @param *tg pointer to TetrisGame
 * @param gen_height height of incomplete rows to generate
 * @param testpiece pniece to use as acti
 * 
*/
void setup_moveCheck(TetrisGame *tg, uint8_t gen_height, \
    TetrisPiece testpiece, int *test_state) {

    // clear board contents
    tg->board = init_board();
    setup_N_lines_board(&tg->board, gen_height);

    tg->active_piece = testpiece;

    // render active board with state for debugging
    // technically not necessary for this test but very helpful
    //  to see the game state being tested
    TetrisBoard render_board = render_active_board(tg);
    printf("Test state: %d\n", *test_state);
    print_board_state(render_board);
    *test_state += 1;


}