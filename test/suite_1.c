/**
 * Tetris Game engine test suite 1
*/

#include <unity.h>

#include "tetris.h"
#include "tetris_test_helpers.h"

TetrisGame *tg;
TetrisBoard tb;

/**
 * Runs before each test
*/
void setUp(void) {
    tg = create_game();
    tb = init_board();

}

/**
 * Runs after each test
*/
void tearDown(void) {
    end_game(tg);
}


/**
 * Test that check_valid_move() works as it's supposed to
*/
void test_checkValidMove(void) {
//create_tetris_piece(enum piece_type ptype, int8_t row, int8_t col, uint8_t orientation);
    int test_case = 0;      // keep track of which case we're testing for easier debugging

    // T piece in middle of falling area
    setup_moveCheck(tg, 2, create_tetris_piece(T_PIECE, 14, 1, 0), &test_case);
    TEST_ASSERT_TRUE(check_valid_move(tg, T_LEFT));
    TEST_ASSERT_TRUE(check_valid_move(tg, T_RIGHT));
    TEST_ASSERT_TRUE(check_valid_move(tg, T_DOWN));
    
    // T piece on left edge of board
    setup_moveCheck(tg, 2, create_tetris_piece(T_PIECE, 14, 0, 1), &test_case);
    TEST_ASSERT_FALSE(check_valid_move(tg, T_LEFT));
    TEST_ASSERT_TRUE(check_valid_move(tg, T_RIGHT));
    TEST_ASSERT_TRUE(check_valid_move(tg, T_DOWN));
    // T piece on left and bottom of board
    setup_moveCheck(tg, 5, create_tetris_piece(T_PIECE, TETRIS_ROWS - 7, -1, 3), &test_case);
    TEST_ASSERT_FALSE(check_valid_move(tg, T_LEFT));
    TEST_ASSERT_TRUE_MESSAGE(check_valid_move(tg, T_RIGHT), "test moving right while on left edge");
    TEST_ASSERT_FALSE(check_valid_move(tg, T_DOWN));

    // T piece on right and bottom of board
    setup_moveCheck(tg, 5, create_tetris_piece(T_PIECE, TETRIS_ROWS - 7, \
        TETRIS_COLS - 3, 3), &test_case);
    TEST_ASSERT_TRUE(check_valid_move(tg, T_LEFT));
    TEST_ASSERT_FALSE_MESSAGE(check_valid_move(tg, T_RIGHT), "test moving right while on right edge");
    TEST_ASSERT_FALSE(check_valid_move(tg, T_DOWN));


    // S PIECE
    // S piece middle of falling area
    setup_moveCheck(tg, 2, create_tetris_piece(S_PIECE, 14, 1, 0), &test_case);
    TEST_ASSERT_TRUE(check_valid_move(tg, T_LEFT));
    TEST_ASSERT_TRUE(check_valid_move(tg, T_RIGHT));
    TEST_ASSERT_TRUE(check_valid_move(tg, T_DOWN));
    // S piece on left edge of board
    setup_moveCheck(tg, 2, create_tetris_piece(S_PIECE, 14, 0, 1), &test_case);
    TEST_ASSERT_FALSE(check_valid_move(tg, T_LEFT));
    TEST_ASSERT_TRUE(check_valid_move(tg, T_RIGHT));
    TEST_ASSERT_TRUE(check_valid_move(tg, T_DOWN));
    // S piece on left and bottom of board  #6
    setup_moveCheck(tg, 5, create_tetris_piece(S_PIECE, TETRIS_ROWS - 7, 0, 3), &test_case);
    TEST_ASSERT_FALSE(check_valid_move(tg, T_LEFT));
    TEST_ASSERT_TRUE_MESSAGE(check_valid_move(tg, T_RIGHT), "test moving right while on left edge");
    TEST_ASSERT_FALSE(check_valid_move(tg, T_DOWN));

    // S piece on right and bottom of board #7
    setup_moveCheck(tg, 5, create_tetris_piece(S_PIECE, TETRIS_ROWS - 7, \
        TETRIS_COLS - 3, 3), &test_case);
    TEST_ASSERT_TRUE(check_valid_move(tg, T_LEFT));
    TEST_ASSERT_FALSE_MESSAGE(check_valid_move(tg, T_RIGHT), "test moving right while on right edge");
    TEST_ASSERT_FALSE(check_valid_move(tg, T_DOWN));

    // Z PIECE
}

/**
 * Test T_PIECE rotation around board
*/
void test_T_testPieceRotate(void) {
    int test_case = 1;
    enum piece_type piecetype = T_PIECE;
    TetrisPiece tp;

    // Test rotation of all orientations of T piece in middle of falling area
    tp = create_tetris_piece(piecetype, 14, 1, 0);
    setup_moveCheck(tg, 2, tp, &test_case);
    TEST_ASSERT_TRUE(test_piece_rotate(&tg->board, tp));
    tp = create_tetris_piece(piecetype, 14, 1, 1);
    setup_moveCheck(tg, 2, tp, &test_case);
    TEST_ASSERT_TRUE(test_piece_rotate(&tg->board, tp));
    tp = create_tetris_piece(piecetype, 14, 1, 2);
    setup_moveCheck(tg, 2, tp, &test_case);
    TEST_ASSERT_TRUE(test_piece_rotate(&tg->board, tp));
    tp = create_tetris_piece(piecetype, 14, 1, 3);
    setup_moveCheck(tg, 2, tp, &test_case);
    TEST_ASSERT_TRUE(test_piece_rotate(&tg->board, tp));

    // test rotation of T at bottom left of board (cases 5-8)
    tp = create_tetris_piece(piecetype, TETRIS_ROWS - 7, 0, 0);
    setup_moveCheck(tg, 5, tp, &test_case);
    TEST_ASSERT_TRUE(test_piece_rotate(&tg->board, tp));
    tp = create_tetris_piece(piecetype, TETRIS_ROWS - 7, 0, 1);
    setup_moveCheck(tg, 5, tp, &test_case);
    TEST_ASSERT_TRUE(test_piece_rotate(&tg->board, tp));
    tp = create_tetris_piece(piecetype, TETRIS_ROWS - 7, 0, 2);
    setup_moveCheck(tg, 5, tp, &test_case);
    TEST_ASSERT_TRUE(test_piece_rotate(&tg->board, tp));
    tp = create_tetris_piece(piecetype, TETRIS_ROWS - 7, -1, 3);
    setup_moveCheck(tg, 5, tp, &test_case);
    TEST_ASSERT_FALSE(test_piece_rotate(&tg->board, tp));

    //T piece rotate into slot at bottom of board
    tp = create_tetris_piece(piecetype, TETRIS_ROWS - 6, 3, 0);
    setup_moveCheck(tg, 4, tp, &test_case);
    // fill in rectangle around where piece needs to be
    fill_board_rectangle(&tg->active_board, TETRIS_ROWS - 5, 0, TETRIS_ROWS-5,4,1);
    fill_board_rectangle(&tg->board, TETRIS_ROWS - 5, 0, TETRIS_ROWS-5,4,1);
    fill_board_rectangle(&tg->active_board, TETRIS_ROWS - 5, 5, TETRIS_ROWS-5,TETRIS_COLS ,1);
    fill_board_rectangle(&tg->board, TETRIS_ROWS - 5, 5, TETRIS_ROWS-5,TETRIS_COLS ,1);
    print_board_state(tg->active_board);
    TEST_ASSERT_TRUE(test_piece_rotate(&tg->board, tp));
    TEST_ASSERT_FALSE_MESSAGE(check_valid_move(tg, T_LEFT), "translated into occupied cell");
    TEST_ASSERT_FALSE(check_valid_move(tg, T_RIGHT));
    TEST_ASSERT_FALSE(check_valid_move(tg, T_DOWN));
    tp = create_tetris_piece(piecetype, TETRIS_ROWS - 6, 3, 1);
    setup_moveCheck(tg, 4, tp, &test_case);
    print_board_state(tg->active_board);
    TEST_ASSERT_TRUE(test_piece_rotate(&tg->board, tp));
}

/**
 * Test clearing rows and moving them down
*/
void test_clearRows(void) {
    TetrisPiece tp;
    int test_case = 1;

    tp = create_tetris_piece(T_PIECE, 14, 1, 0);
    setup_moveCheck(tg, 5, tp, &test_case);
    // rows indexed from top, so this is rows 2&3
    clear_rows(tg, 28, 2);
    print_board_state(tg->board);
    TEST_ASSERT_EQUAL_INT(TETRIS_ROWS - 3, tg->board.highest_occupied_cell);
    TEST_ASSERT_FALSE_MESSAGE(check_for_occ_cells_in_row(tg, TETRIS_ROWS - 4), \
        "failed to correctly move rows down after clear");

    // test moving down vertical rectangle structure
    setup_moveCheck(tg, 5, tp, &test_case);
    fill_board_rectangle(&tg->board, 19, 6,  26, 10, 1);
    tg->board.highest_occupied_cell = 19;
    clear_rows(tg, 28, 2);
    print_board_state(tg->board);
    TEST_ASSERT_EQUAL_INT(TETRIS_ROWS - 11, tg->board.highest_occupied_cell);
    TEST_ASSERT_FALSE_MESSAGE(check_for_occ_cells_in_row(tg, TETRIS_ROWS - 12), \
        "failed to correctly move rows down after rect shift");

    // test if a filled row is detected as filled
    setup_moveCheck(tg, 5, tp, &test_case);
    fill_board_rectangle(&tg->board, 25, 0,  26, TETRIS_COLS, 1);
    print_board_state(tg->board);
    tg->board.highest_occupied_cell = 25;
    // check from row=25 to to bottom of board
    TEST_ASSERT_TRUE_MESSAGE(check_filled_row(tg, 25), "filled row not detected as filled");
    TEST_ASSERT_TRUE_MESSAGE(check_filled_row(tg, 26), "filled row not detected as filled");
    TEST_ASSERT_FALSE_MESSAGE(check_filled_row(tg, 27), "not filled row detected as filled");
    TEST_ASSERT_FALSE_MESSAGE(check_filled_row(tg, 28), "not filled row detected as filled");
    TEST_ASSERT_FALSE_MESSAGE(check_filled_row(tg, 29), "not filled row detected as filled");
    TEST_ASSERT_FALSE_MESSAGE(check_filled_row(tg, 30), "not filled row detected as filled");
    clear_rows(tg, 25, 2);
    for (int i = 25; i < TETRIS_ROWS; i++) {
        TEST_ASSERT_FALSE(check_filled_row(tg, i));
    }

}


void test_checkSpawnNewPiece(void) {
    // test piece falling case

    // test piece stopped falling case


    // test piece stopped falling invalid case



}

int main(void)
{
    UNITY_BEGIN();
    // these tests assume a 16x32 board, i haven't tested otherwise
    assert(TETRIS_ROWS == 32 && TETRIS_COLS == 16);

    RUN_TEST(test_checkValidMove);
    RUN_TEST(test_T_testPieceRotate);
    RUN_TEST(test_clearRows);
    // test_checkSpawnNewPiece

    return UNITY_END();
}
