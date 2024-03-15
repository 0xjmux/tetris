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
 * Fill rectangle from top left coord to bottom right coord.
 * Used for creating filled blocks for testing. 
 * @param tL_row top left row
 * @param tL_col top left col
 * @param bR_row bottom right row
 * @param bR_col bottom right col
 * @param fill_value value to fill rectange with
*/
void fill_board_rectangle(TetrisBoard *tb, int8_t tL_row, int8_t tL_col, \
    int8_t bR_row, int8_t bR_col, int8_t fill_value) {

    assert(tL_row < bR_row && tL_col < bR_col &&  \
        "top left and bottom right coords not valid");
    
    for (int r = tL_row; r < bR_row; r++) {
        for (int c = tL_col; c < bR_col; c++) {
            tb->board[r][c] = fill_value;
        }
    }
}

/**
 * Print current board state to console
*/
void print_board_state(TetrisBoard tb) {
    // draw existing pieces on board
    printf("Highest occupied cell: %d\n   ", tb.highest_occupied_cell);
    printf("  ");
    for (int i = 0; i < TETRIS_COLS; i++) 
        printf("%-2d  ",i);
    printf("\n   ");

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
        printf("|\n");
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
    printf("Test case: %d\n", *test_state);
    print_board_state(render_board);
    *test_state += 1;


}


/**
 * Print all tetrominos to terminal to verify correct configurations
*/
void print_all_tetrominos(void) {
    uint8_t num_tetrominos = 3;     // number of currently impl tetrominos
    // uint8_t num_tetrominos = NUM_TETROMINOS;     // number of currently impl tetrominos
    // print piece definitions out to console for confirmation
    for (int i = 0; i < num_tetrominos; i++) {
        printf("Piece: %d\n", i);
        print_piece(TETROMINOS[i]);
    }
}

// print all piece orientations out to check they're valid
void print_piece(const tetris_location t[4][4]) {
    char piece[4][4];
    tetris_location loc;      // temp location

    // iterate through all 4 orientations
    for (int o = 0; o < 4; o++) {
        printf("Orientation %d: \n", o);
        // update temporary piece grid with this piece's placements
        for (int row = 0; row < 4; row++) {
            loc = t[o][row];
            piece[loc.row][loc.col] = 'X';
        }

        // print board
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                if (piece[row][col])
                    printf("%c", piece[row][col]);
                else
                    printf(" ");
            }
            printf("\n");
        }

        printf("\n");
        // clean out location array
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                piece[row][col] = '\0';
            }
        }
    }
}

