/**
 * Tetris Game library, platform and display agnostic
 * @author Jacob Bokor, jacobbokor.com
 * @date 03/2024
*/

#include "tetris.h"

#ifdef DEBUG_T
// DEBUG_T is tetris game logic debug flag
// linker didn't like it being included normally
FILE *gamelog;
#endif


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
 * Allocate and init TetrisBoard->board array with all zeros
*/
TetrisBoard* create_board(void) {
    //assert_msg(TETRIS_ROWS < 256 && TETRIS_COLS < 256, "Board size too big!\n");
    TetrisBoard *b = malloc(sizeof(TetrisBoard));
    assert(b);
    for (int i = 0; i < TETRIS_ROWS; i++) {
        for (int j = 0; j < TETRIS_COLS; j++) {
            b->board[i][j] = -1;
        }
    }

    b->highest_occupied_cell = TETRIS_ROWS - 1;
    return b;
}

void delete_board(TetrisBoard *b) {
    free(b);
}

/**
 * Create a new tetris game struct
 * @returns TetrisGame* struct ptr
*/
TetrisGame* create_game(void) {
    TetrisGame *tg = malloc(sizeof(TetrisGame));

    tg->board = create_board();
    tg->active_board = create_board();
    tg->game_over = false;
    tg->level = 1;
    tg->score = 0;
    tg->gravity_tick_rate = 1000;


    #ifdef DEBUG_T
    // separate gamelog file to prevent ncurses printing issues
    gamelog = fopen("game.log", "w+");
    #endif


    return tg;

}

/**
 * Deallocate tetris game struct
*/
void end_game(TetrisGame *tg) {
    #ifdef DEBUG_T
    fprintf(gamelog, "Deallocating tetris game\n");
    #endif

    // if board is still allocated, dealloc it
    // (this ref-deref might cause issues)
    if(tg->board != NULL)
        free(tg->board);


    free(tg);

    #ifdef DEBUG_T
    fclose(gamelog);
    #endif
}

/**
 * Process a single game tick
 * @param TetrisGame *tg
 * @param player_move move
 * @returns true if successful, false if gameover ??? idk if this is how i wanna do it
*/
void tg_tick(TetrisGame *tg, enum player_move move) {

    // handle gravity, input, cleared lines, adjusting score, checking game over


    render_active_board(tg);
    #ifdef DEBUG_T
    fprintf(gamelog, ".");
    fflush(gamelog);
    #endif


    // if piece has fallen, then create_rand_piece()

}


/**
 * combine active_piece and existing board stack into active_board, 
 * which is then used by display system to display the actual game.
 * This function assumes placement of *tp in board is valid
*/
// TetrisBoard render_active_board(TetrisGame *tg, TetrisPiece *tp) {
TetrisBoard render_active_board(TetrisGame *tg) {
    TetrisBoard gameboard = *tg->board;
    TetrisPiece tp = tg->active_piece;

    // relative locations of active piece based on orientation
    // tetris_location test_cell = TETROMINOS[tp.ptype][tp.orientation][0];
    tetris_location tp_cells[NUM_CELLS_IN_TETROMINO];
    // = TETROMINOS[tp.ptype][tp.orientation];
    memcpy(tp_cells, TETROMINOS[tp.ptype][tp.orientation], NUM_CELLS_IN_TETROMINO * sizeof(tetris_location));


    #ifdef DEBUG_T  // print out contents of tp_cells
        fprintf(gamelog, "relative cells to tetris_loc: tp_cells: [ ");
        for(int i = 0; i < 4; i++)
            fprintf(gamelog, "[%d, %d] ", tp_cells[i].row, tp_cells[i].col);

        fprintf(gamelog, "]\n");
        fflush(gamelog);
    #endif

    // starting at tetris_location fill cells using relative locations from TETROMINOS
    for(int i = 0; i < 4; i++) {
        tetris_location curr_offset = tp_cells[i];
        // update board to reflect placement of piece
        gameboard.board[tp.loc.row + curr_offset.row] \
            [tp.loc.col + curr_offset.col] = tp.ptype;


    }

    *tg->active_board = gameboard;

    return gameboard;
}

/** 
 * Updates game score based on # lines cleared, along with
 * level and gravity tick rate
*/
void tg_update_score(TetrisGame *tg, uint8_t lines_cleared) {
    assert(lines_cleared < 5 && "Number of lines_cleared too large\n");


    // calculate score increase
    tg->score += tg->level * points_per_level_cleared[lines_cleared];

    // update current highest cell
    tg->board->highest_occupied_cell -= lines_cleared;

    // every 10 lines, the level increases
    static uint8_t lines_cleared_since_last_level;
    lines_cleared_since_last_level += lines_cleared;

    if (lines_cleared_since_last_level >= 10) {
        tg->level += 1;
        lines_cleared_since_last_level = lines_cleared_since_last_level % 10;
        assert(lines_cleared_since_last_level < 10);

        // when the level increases, the gravity tick also speeds up
        //    (but only so far)
        if (tg->gravity_tick_rate > 200) {
            tg->gravity_tick_rate -= 100;
        }
    }

    #ifdef DEBUG_T
    fprintf(gamelog, "Score updated for %d lines cleared. Score = %d, Level = %d \
    Lines cleared since last level=%d", lines_cleared, tg->score, tg->level, 
    lines_cleared_since_last_level);
    fflush(gamelog);
    #endif

}

/**
 * Creates and initializes a new active piece
*/
TetrisPiece create_rand_piece(TetrisGame *tg) {
    // create new piece and place in middle center
    TetrisPiece new_piece;
    new_piece.ptype = rand() / 7 + 1;
    new_piece.orientation = 0;
    new_piece.loc.col = TETRIS_COLS / 2;
    new_piece.loc.row = 2;      // CHANGE TO 1 ONCE DONE!!!!!

    tg->active_piece = new_piece;
    return new_piece;
}


/**
 * Check if a given player move of T_NONE, T_LEFT, T_RIGHT, 
 * T_DOWN, T_UP (rotate) is valid
*/
bool check_valid_move(TetrisGame *tg, uint8_t player_move){
    TetrisPiece tp = tg->active_piece;
    tetris_location tp_cells[NUM_CELLS_IN_TETROMINO];
    // copy offsets for current piece into tp_cells
    // @todo is this copying too much??? CHECK!!!!! 
    memcpy(tp_cells, TETROMINOS[tp.ptype][tp.orientation], NUM_CELLS_IN_TETROMINO * sizeof(tetris_location));
    // change those offsets to global piece locations
    for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
        tp_cells[i].row += tp.loc.row;
        tp_cells[i].col += tp.loc.col;
    }
    #ifdef DEBUG_T
        fprintf(gamelog, "Global piece locations are: {");
        for(int i = 0; i < 4; i++)
            fprintf(gamelog, "[%d, %d] ", tp_cells[i].row, tp_cells[i].col);

        fprintf(gamelog, "}\n");
        fflush(gamelog);
    #endif

    bool move_valid = true;

    switch (player_move) {
        case T_NONE:
            return true;
            break;
        case T_LEFT:
            const tetris_location left_offset = {.row = 0, .col = -1};
            // test all 4 cells in tetronimo; if any are false, move is invalid
            for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
                move_valid = move_valid && test_piece_offset(tg->board, tp_cells[i], left_offset);
            }

        case T_RIGHT:
            const tetris_location right_offset = {.row = 0, .col = 1};
            for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
                move_valid = move_valid && test_piece_offset(tg->board, tp_cells[i], right_offset);
            }

        case T_DOWN:
            const tetris_location down_offset = {.row = 1, .col = 0};
            for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
                move_valid = move_valid && test_piece_offset(tg->board, tp_cells[i], down_offset);
            }

        default:
            #ifdef DEBUG_T
            fprintf(gamelog, "check_valid_translation invalid case!\n");
            #endif
            break;
    }


    return move_valid;
}

/**
 * Helper func to test if `offset` from 
 * `global_loc` is empty 
*/
bool test_piece_offset(TetrisBoard *tb, const tetris_location global_loc, const tetris_location move_offset) {
    tetris_location testing_loc;    // location we want to move to
    testing_loc.row = global_loc.row + move_offset.row;
    testing_loc.col = global_loc.col + move_offset.col;

    if (tb->board[testing_loc.row][testing_loc.col] == -1)
        return true;
    else
        return false;
}


void move_piece_left(TetrisGame *tg) {

}

void move_piece_right(TetrisGame *tg) {

}


/**
 * Clear row `row` and move all cells above it down one; 
 * filling in now empty spots with BG_COLOR
*/
void clear_row(TetrisGame *tg, uint8_t row) {

}


/**
 * a "Tetromino", or piece on the tetris board. 
 * [piece_type][orientation][row,col offset location]
 * The pairs in this array represent the offset from the top left required to 
 *  rotate the piece
*/
const tetris_location TETROMINOS[NUM_TETROMINOS][NUM_ORIENTATIONS][NUM_CELLS_IN_TETROMINO] = { 
   // S
   {{{0,0}, {1,0}, {1,1}, {2,1}},
   {{1,0}, {1,1}, {0,1}, {0,2}},
   {{0,0}, {1,0}, {1,1}, {2,1}},
   {{1,0}, {1,1}, {0,1}, {0,2}}
   },
   // Z
   {{{0,1}, {1,0}, {1,1}, {2,0}},
   {{0,0}, {0,1}, {1,1}, {1,2}},
   {{0,1}, {1,0}, {1,1}, {2,0}},
   {{0,0}, {0,1}, {1,1}, {1,2}}
   },
    // T
   {{{0,0}, {0,1}, {0,2}, {1,1}},
   {{0,0}, {0,1}, {1,1}, {-1,1}},
   {{1,0}, {1,1}, {0,1}, {1,2}},
   {{0,1}, {1,1}, {-1,1}, {0,2}}
   } //,
//    // L
//    {{{}, {}, {}, {}},
//    {{}, {}, {}, {}},
//    {{}, {}, {}, {}},
//    {{}, {}, {}, {}}
//    },
   
//    // J
//    {{{}, {}, {}, {}},
//    {{}, {}, {}, {}},
//    {{}, {}, {}, {}},
//    {{}, {}, {}, {}}
//    },
//    // SQUARE
//    {{{}, {}, {}, {}},
//    {{}, {}, {}, {}},
//    {{}, {}, {}, {}},
//    {{}, {}, {}, {}}
//    },
//    // I
//    {{{}, {}, {}, {}},
//    {{}, {}, {}, {}},
//    {{}, {}, {}, {}},
//    {{}, {}, {}, {}}
//    }
};



