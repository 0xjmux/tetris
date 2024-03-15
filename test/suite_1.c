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
    int test_case = 0;

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


int main(void)
{
    UNITY_BEGIN();
    // these tests assume a 16x32 board
    assert(TETRIS_ROWS == 32 && TETRIS_COLS == 16);


    RUN_TEST(test_checkValidMove);

    return UNITY_END();
}
