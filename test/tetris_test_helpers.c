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

    assert(tL_row <= bR_row && tL_col <= bR_col &&  \
        "top left and bottom right coords not valid");
    
    for (int r = tL_row; r <= bR_row; r++) {
        for (int c = tL_col; c < bR_col; c++) {
            tb->board[r][c] = fill_value;
        }
    }
    #ifdef DEBUG_T
    fprintf(gamelog, "filled cells from TL [%d,%d] to BR [%d,%d]\n", \
        tL_row, tL_col, bR_row, bR_col);

    #endif
}



/**
 * Setup conditions for testing check_valid_move
 * @param *tg pointer to TetrisGame
 * @param gen_height height of incomplete rows to generate
 * @param testpiece piece to use as active_piece
 * @param test_state test # to make debugging cases easier
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
    print_board_state(render_board, stdout, false);
    *test_state += 1;


}


/**
 * Print all tetrominos to terminal to verify correct configurations
*/
void print_all_tetrominos(void) {
    uint8_t num_tetrominos = NUM_TETROMINOS;     // number of currently impl tetrominos
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

/**
 * Check `row` for occupied cells to verify if its empty
 * @returns true if occupied cell found, false otherwise
*/
bool check_for_occ_cells_in_row(TetrisGame *tg, uint8_t row) {
    for (int i = 0; i < TETRIS_COLS; i++) {
        if (tg->board.board[row][i] != -1) 
            return true;
    }

    return false;
}

/**
 * Check all rows in tetrisgame and make sure none are found to be filled
*/
void check_no_filled_rows(TetrisGame *tg) {
    // check row states are correctly detected
    for (int i = 0; i < TETRIS_ROWS; i++) {
        TEST_ASSERT_FALSE_MESSAGE(check_filled_row(tg, i), "incorrectly detected row i as filled");
    }


}


/** 
 * Modify tg->gravity_tick_rate_usec and last_gravity_tick
 * so that the gravity tick check always passes
*/
void reset_game_gravity_time(TetrisGame *tg) {
    // bypass time check in game logic
    tg->gravity_tick_rate_usec = 0; 
    gettimeofday(&tg->last_gravity_tick_usec, NULL);
    tg->last_gravity_tick_usec.tv_sec -= 1;
    tg->last_gravity_tick_usec.tv_usec -= 100;
}


/**
 * Helper function to move active_piece down `rows_to_move` and then
 *  render board
 * function assumes `rows_to_move` rows below current piece position
 *  are valid locations to move to. 
*/
void move_piece_down(TetrisGame *tg, uint8_t rows_to_move) {
    for (int i = 0; i < rows_to_move; i++) {
        reset_game_gravity_time(tg);
        TEST_ASSERT_TRUE_MESSAGE(check_do_piece_gravity(tg), "PIECE GRAVITY CHECK FAILED");
    }
    render_active_board(tg);
}


