/**
 * Tetris Game library header file
 * @author Jacob Bokor, jacobbokor.com
 * @date 03/2024
*/

#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __linux__
#include <stdio.h>
#include <stdlib.h>         // used for malloc(), free()
#include <assert.h>
#include <string.h>         // memcpy
#endif

// TETRIS GAME LOGIC DEBUG FLAG
#define DEBUG_T 1

#ifdef DEBUG_T
// separate game log file to prevent ncurses printing issues
extern FILE *gamelog;
#endif

// how many rows and columns is the board?
// max allowed is 256,256 because anything larger is unreasonable
#define TETRIS_ROWS 32
#define TETRIS_COLS 16


// how many different piece types and orientations
#define NUM_TETROMINOS 7
#define NUM_ORIENTATIONS 4
#define NUM_CELLS_IN_TETROMINO 4

// piece descriptions
// S, Z, T, L, reverse L (J), square, long bar (I)
enum piece_type {S_PIECE, Z_PIECE, T_PIECE, L_PIECE, J_PIECE, SQ_PIECE, I_PIECE};

enum piece_colors {S_CELL_COLOR, Z_CELL_COLOR, T_CELL_COLOR, L_CELL_COLOR, J_CELL_COLOR, SQ_CELL_COLOR, I_CELL_COLOR};
#define BG_COLOR -1

// Define possible moves that can be taken by player
enum player_move {T_NONE, T_UP, T_DOWN, T_LEFT, T_RIGHT, T_PLAYPAUSE, T_QUIT};

// Points per level cleared, combos not implemented
// See: https://tetris.wiki/Scoring
static const uint16_t points_per_level_cleared[] = {0, 100, 300, 500, 800};

/**
 * row,col location on tetris board, from top right. 
 * Negative numbers allowed.
*/
typedef struct tetris_location {
    int8_t row;
    int8_t col;
} tetris_location;

// tetris piece descriptions - defined here to prevent multiple definition
extern const tetris_location TETROMINOS[NUM_TETROMINOS][NUM_ORIENTATIONS][NUM_CELLS_IN_TETROMINO];


/**
 * Struct representing a single Tetromino
 * @param ptype - enum type of piece
 * @param tetris_location loc - piece position
 * @param orientation - piece orientation
*/
typedef struct TetrisPiece {
    enum piece_type ptype;
    tetris_location loc;
    uint8_t orientation;    // orientation, in range 0-3
} TetrisPiece;

/**
 * Represents the game board
 * @param board 2D int array representing board
 *  -1 means unoccupied, >0 indicates cell color by piece_colors[]
 * @param highest_occupied_row tallest point in current stack, tracked to 
 *  avoid needless recomputation
 * 
*/
typedef struct TetrisBoard {
    int8_t board[TETRIS_ROWS][TETRIS_COLS];
    uint8_t highest_occupied_cell;

} TetrisBoard;


TetrisBoard init_board(void);


/**
 * Tetris Game Struct
 * @param board 2D struct array of set pieces on board
 * @param active_board 2D struct array representing entire board 
 *  (including falling piece)
 * @param game_over bool true if game over, false if not
 * @param gravity_tick_rate millis between each gravity tick
 * @param score player's current score
 * @param level current level
*/
typedef struct TetrisGame {
    TetrisBoard board;
    TetrisBoard active_board;
    TetrisPiece active_piece;
    bool game_over;
    uint16_t gravity_tick_rate;
    uint32_t score;
    uint32_t level;
} TetrisGame;


TetrisGame* create_game(void);
void end_game(TetrisGame *tg);
void tg_tick(TetrisGame *tg, enum player_move move);

bool check_valid_move(TetrisGame *tg, uint8_t player_move);


TetrisBoard render_active_board(TetrisGame *tg);
TetrisPiece create_rand_piece(TetrisGame *tg);
TetrisPiece create_tetris_piece(enum piece_type ptype, int8_t row, int8_t col, uint8_t orientation);

bool test_piece_offset(TetrisBoard *tb, const tetris_location global_loc, const tetris_location move_offset);
bool test_piece_rotate(TetrisBoard *tb, const TetrisPiece tp);


// function definitions
// void print_piece(tetris_location** t);
void print_piece(const tetris_location t[4][4]);



#endif