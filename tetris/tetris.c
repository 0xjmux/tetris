/**
 * Tetris Game library, platform and display agnostic
 * 
 * This code is designed to be as portable as possible. There are, 
 * however, some considerations for the platform it's ported to. 
 * Mainly, this concerns time interval handling inside check_do_piece_gravity()
 * and similar game tick related functions. 
 * 
 * I used `struct timeval` in this implementation, but this may not be available
 * on your microcontroller platform. All the math is done in microseconds
 * (because that's what timeval returns) as `long long`, so substitute your 
 * platform's RTC functionality and it should work as desired. 
 * 
 * @author Jacob Bokor, jacobbokor.com
 * @date 03/2024
*/

#include "tetris.h"

#ifdef DEBUG_T
// DEBUG_T is tetris game logic debug flag
// linker didn't like it being included normally
FILE *gamelog;
#endif


/**
 * Function to create a board WITHOUT using 
 * malloc and ptrs
*/
TetrisBoard init_board(void) {
    //assert_msg(TETRIS_ROWS < 256 && TETRIS_COLS < 256, "Board size too big!\n");
    TetrisBoard b; 
    for (int i = 0; i < TETRIS_ROWS; i++) {
        for (int j = 0; j < TETRIS_COLS; j++) {
            b.board[i][j] = -1;
        }
    }

    b.highest_occupied_cell = TETRIS_ROWS - 1;
    return b;
}


/**
 * Create a new tetris game struct
 * @returns TetrisGame* struct ptr
*/
TetrisGame* create_game(void) {
    TetrisGame *tg = malloc(sizeof(TetrisGame));

    tg->board = init_board();
    tg->active_board = init_board();
    tg->game_over = false;
    tg->level = 1;
    tg->score = 0;
    tg->gravity_tick_rate_usec = 200000;


    #ifdef DEBUG_T

        #ifndef TETRIS_UNIT_TEST_DEF
        // separate gamelog file to prevent ncurses printing issues
        gamelog = fopen("game.log", "w+");
        #else
        // for unit testing, assign game.log to stdout
        gamelog = stdout;
        #endif
    #endif

    return tg;
}

/**
 * Deallocate tetris game struct
*/
void end_game(TetrisGame *tg) {
    #ifdef DEBUG_T
    fprintf(gamelog, "Deallocating tetris game\n");
        #ifndef TETRIS_UNIT_TEST_DEF
        fclose(gamelog);
        #endif
    #endif


    free(tg);
}

/**
 * Process a single game tick
 * @param TetrisGame *tg
 * @param player_move move
*/
void tg_tick(TetrisGame *tg, enum player_move move) {

    // handle gravity, input, cleared lines, adjusting score, checking game over


    check_do_piece_gravity(tg);
    check_and_spawn_new_piece(tg);
    render_active_board(tg);

    #ifdef DEBUG_T
    // fprintf(gamelog, ".");
    // fflush(gamelog);
    #endif

    switch (move) {
        case T_NONE:
            break;

        case T_UP:
            if(test_piece_rotate(&tg->board, tg->active_piece))
                // fprintf(gamelog, "piece rotated (not impl yet)\n");
                tg->active_piece.orientation = (tg->active_piece.orientation + 1) % 4;
            break;

        case T_DOWN:
            if(check_valid_move(tg, move))
                tg->active_piece.loc.row += 1;
            break;
        case T_LEFT:
            if(check_valid_move(tg, move))
                tg->active_piece.loc.col -= 1;
            break;

        case T_RIGHT:
            if(check_valid_move(tg, move))
                tg->active_piece.loc.col += 1;
            break;
        
        case T_PLAYPAUSE:
            // IMPLEMENT
            break;
        
        // T_QUIT implemented by driver

        default:
            // we should not get here
            #ifdef DEBUG_T
            fprintf(gamelog, "tg_tick default case! uhoh  ");
            fprintf(gamelog, "player_move=%d \n", move);
            #endif
            assert(false && "reached default state of tg_tick");
            break;
    }


}


/**
 * combine active_piece and existing board stack into active_board, 
 * which is then used by display system to display the actual game.
 * This function assumes placement of *tp in board is valid
*/
// TetrisBoard render_active_board(TetrisGame *tg, TetrisPiece *tp) {
TetrisBoard render_active_board(TetrisGame *tg) {
    TetrisBoard gameboard = tg->board;
    TetrisPiece tp = tg->active_piece;

    // relative locations of active piece based on orientation
    tetris_location tp_cells[NUM_CELLS_IN_TETROMINO];
    for (int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
        tp_cells[i] = TETROMINOS[tp.ptype][tp.orientation][i];
    }


    #ifdef DEBUG_T  // print out contents of tp_cells
       // fprintf(gamelog, "relative cells to tetris_loc: tp_cells: [ ");
       // for(int i = 0; i < 4; i++)
       //     fprintf(gamelog, "[%d, %d] ", tp_cells[i].row, tp_cells[i].col);

       // fprintf(gamelog, "]\n");
       // fflush(gamelog);
    #endif

    // starting at tetris_location fill cells using relative locations from TETROMINOS
    for(int i = 0; i < 4; i++) {
        tetris_location curr_offset = tp_cells[i];
        // update board to reflect placement of piece
        gameboard.board[tp.loc.row + curr_offset.row] \
            [tp.loc.col + curr_offset.col] = tp.ptype;

    }

    tg->active_board = gameboard;

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
    tg->board.highest_occupied_cell -= lines_cleared;

    // every 10 lines, the level increases
    static uint8_t lines_cleared_since_last_level;
    lines_cleared_since_last_level += lines_cleared;

    if (lines_cleared_since_last_level >= 10) {
        tg->level += 1;
        lines_cleared_since_last_level = lines_cleared_since_last_level % 10;
        assert(lines_cleared_since_last_level < 10);

        // when the level increases, the gravity tick also speeds up
        //    (but only so far)
        if (tg->gravity_tick_rate_usec > 200000) {
            tg->gravity_tick_rate_usec -= 100000;
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


    new_piece.ptype = rand() % NUM_TETROMINOS + 1;
    new_piece.orientation = 0;
    new_piece.loc.col = TETRIS_COLS / 2;
    new_piece.loc.row = 2;      // CHANGE TO 1 ONCE DONE!!!!!
    new_piece.falling = true;

    tg->active_piece = new_piece;
    return new_piece;
}

/**
 * Simple function to create a new piece with the specified parameters
*/
TetrisPiece create_tetris_piece(enum piece_type ptype, \
    int16_t row, int16_t col, uint8_t orientation) {
    assert(orientation > -1 && orientation < 4 && "Orientation out of range");
    TetrisPiece new_piece = {.ptype = ptype, .orientation = orientation, \
        .loc.col = col, .loc.row = row , .falling=true};
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
    memcpy(tp_cells, TETROMINOS[tp.ptype][tp.orientation], sizeof(tp_cells));
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

        case T_UP:
            #ifdef DEBUG_T
                fprintf(gamelog, "rotate move should not be called from check_valid_move()!");
                fflush(gamelog);
                fprintf(gamelog, "player_move=%d \n", player_move);
            #endif
            assert(false && "rotate move should not be passed to check_valid_move()!");

            break;



        case T_DOWN:
            const tetris_location down_offset = {.row = 1, .col = 0};
            for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
                move_valid = move_valid && test_piece_offset(&tg->board, tp_cells[i], down_offset);
            }
//            // if we can't move down, we must have reached the bottom
//            if (!move_valid) {
//                tg->active_piece.falling = false;
//            }
            break;
        case T_LEFT:
            const tetris_location left_offset = {.row = 0, .col = -1};
            // test all 4 cells in tetronimo; if any are false, move is invalid
            for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
                move_valid = move_valid && test_piece_offset(&tg->board, tp_cells[i], left_offset);
            }
            break;

        case T_RIGHT:
            const tetris_location right_offset = {.row = 0, .col = 1};
            for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
                move_valid = move_valid && test_piece_offset(&tg->board, tp_cells[i], right_offset);
            }
            break;

        default:
            #ifdef DEBUG_T
            fprintf(gamelog, "check_valid_move default case, not good!  ");
            fprintf(gamelog, "player_move=%d \n", player_move);
            #endif
            break;
    }


    return move_valid;
}

/**
 * Helper func to test if single cell translation of `offset` from 
 * `global_loc` is valid position for current piece
*/
bool test_piece_offset(TetrisBoard *tb, const tetris_location global_loc, const tetris_location move_offset) {
    tetris_location testing_loc;    // location we want to move to
    testing_loc.row = global_loc.row + move_offset.row;
    testing_loc.col = global_loc.col + move_offset.col;

    // if cell is unoccupied and not out of range
    if (tb->board[testing_loc.row][testing_loc.col] == -1 && \
        testing_loc.row > -1 && testing_loc.row < TETRIS_ROWS && \
        testing_loc.col > -1 && testing_loc.col < TETRIS_COLS)
        return true;
    else
        return false;
}

/**
 * Test if next rotation of piece tp is valid
*/
bool test_piece_rotate(TetrisBoard *tb, const TetrisPiece tp) {
    tetris_location tp_cells[NUM_CELLS_IN_TETROMINO];
    // copy offsets for next orientation of piece into tp_cells
    memcpy(tp_cells, TETROMINOS[tp.ptype][(tp.orientation + 1) % 4], sizeof(tp_cells));
    // change those offsets to global piece locations
    for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
        tp_cells[i].row += tp.loc.row;
        tp_cells[i].col += tp.loc.col;
    }

    #ifdef DEBUG_T
    fprintf(gamelog, "Current orientation=%d, orientation after rotation = %d\n", \
        tp.orientation, ((tp.orientation+1) % 4));
    fflush(gamelog);
    #endif

    bool result = true;
    tetris_location testing_loc = tp_cells[0];

    for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
        testing_loc = tp_cells[i];
        // if cell is unoccupied and not out of range
        if (tb->board[testing_loc.row][testing_loc.col] == -1 && \
            testing_loc.row > -1 && testing_loc.row < TETRIS_ROWS && \
            testing_loc.col > -1 && testing_loc.col < TETRIS_COLS)
            result = result && true;
        else
            result = false;
    }

    return result;
}


/**
 * On placing a piece down, the rows where it landed 
 * need to be checked to see if they're full so they can
 * be cleared and the score can be increased
*/
bool check_filled_row(TetrisGame *tg, const uint8_t row) {
    
    for (int c = 0; c < TETRIS_COLS; c++) {
        if(tg->board.board[row][c] == -1)
            return false;
    }
    return true;
}


/** 
 * Check if piece needs to be dropped down by comparing game 
 * time to tick, and if so update piece location accordingly
 * @returns true if piece moved down, false otherwise
 *  if piece is unable to be moved down (reached bottom), 
 *  returns false & active_piece.falling changed to false
 * @attention time intervals are highly architecture specific; it's probable
 * that this function will need to be modified when porting to other platforms
*/
bool check_do_piece_gravity(TetrisGame *tg) {
    // if time has passed tick interval

    // get curr system time
    struct timeval curr_time_usec;
    gettimeofday(&curr_time_usec, NULL);

    // check if it's time for piece to be moved down
    int32_t time_diff_usec = get_elapsed_us(tg->last_gravity_tick_usec, curr_time_usec);
    if ( time_diff_usec > tg->gravity_tick_rate_usec) {

        // if can move down
        if(check_valid_move(tg, T_DOWN)) {
            tg->active_piece.loc.row += 1; // move location down
            #ifdef DEBUG_T
            fprintf(gamelog, "Gravity tick activated systime=%ld, last tick=%ld: piece moved down\n", \
                curr_time_usec.tv_usec, tg->last_gravity_tick_usec.tv_usec);
            #endif
            tg->last_gravity_tick_usec = curr_time_usec;  // update gravity tick

        }
        else {
            // if can't move piece down, change falling to false and ret false
            #ifdef DEBUG_T
            fprintf(gamelog, "Gravity tick activated: move down failed\n");
            #endif
            tg->active_piece.falling = false;
            return false;
        }
    }    
    else  
        return false;

    return true;
}


/**
 * Clear row `row` and move all cells above it down one; 
 * filling in now empty spots with BG_COLOR
 * this function assumes `row` has already been checked to be filled
 * @param tg TetrisGame
 * @param top_row top row of the rows being cleared
 * @param num_rows number of rows to clear
*/
void clear_rows(TetrisGame *tg, uint8_t top_row, uint8_t num_rows) {
    // starting at `row`, go up until you reach cell with value -1 
    // or the top of the board
    //assert(num_rows <= 4 && top_row < TETRIS_ROWS);
    assert(num_rows <= 4);
    assert(top_row < TETRIS_ROWS);

    //  for each col on the board
    for (int col = 0; col < TETRIS_COLS; col++) {
        // starting at bottom row, move everything above it down
        // by num_rows
        for (int i = top_row + num_rows - 1; i > 1; i--) {
            // possible edge case here with very top row?
            // get next row and move it down
            tg->board.board[i][col] = tg->board.board[i-num_rows][col];
        }
    }

    // move highest occupied cell down by how many rows were cleared
    tg->board.highest_occupied_cell += num_rows;
}


/**
 * Check board piece to see if still falling. If it's stopped
 * falling, then add it to the board, and spawn a new rand piece
 * @returns true when new piece spawned, false otherwise
 * 
*/
bool check_and_spawn_new_piece(TetrisGame *tg) {

    // if the piece is still falling, it hasn't landed yet; 
    //  we do nothing
    if (tg->active_piece.falling)
        return false;

    // if we're here, we must have landed
    TetrisPiece tp = tg->active_piece; // last active piece
    #ifdef DEBUG_T
    fprintf(gamelog, "Piece stopped falling at loc row=%d, col=%d", tp.loc.row, tp.loc.col);
    #endif

    // set up active piece and array of offsets from current loc
    tetris_location tp_cells[NUM_CELLS_IN_TETROMINO];
    memcpy(tp_cells, TETROMINOS[tp.ptype][tp.orientation], sizeof(tp_cells));

    // put global piece locations in tp_cells
    for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
        tp_cells[i].row += tp.loc.row;
        tp_cells[i].col += tp.loc.col;
    }

    // add piece to board
    for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
        // set global locations on board equal to piece color
        tg->board.board[tp_cells[i].row][tp_cells[i].col] = tp.ptype;
    }



    // some slight duplicate work here but implementation of 
    // not doing dupliate work was complex enough that this is prob better
    // in short, this only checks rows where piece ended up, but will 
    // check all 4 rows; which might mean checking the same row twice
    int16_t rows_to_clear[4];
    uint8_t rows_idx = 0;
    int16_t row_with_offset;
    int16_t new_max_row = TETRIS_ROWS;
    for(int i = 0; i < NUM_CELLS_IN_TETROMINO; i++) {
        row_with_offset = tp_cells[i].row;

        assert(row_with_offset < TETRIS_ROWS && "global row out of bounds");

        // if row is full, add it to list of rows to clear
        if(check_filled_row(tg, row_with_offset)) {
            rows_to_clear[rows_idx] = row_with_offset;
            rows_idx += 1;
        }

        // test each piece location to see if this cell is the new highest occupied cell
        if (row_with_offset < new_max_row) {
            new_max_row = row_with_offset;
        }
    }

    // update highest occupied cell based on this piece
    assert(new_max_row > -1 && new_max_row < TETRIS_ROWS && "new tallest cell out of bounds");
    tg->board.highest_occupied_cell = new_max_row;

    // if we have rows to clear:
    if (rows_idx > 0) {
        // find top row in rows_to_clear and clear rows
        int16_t top_row = smallest_in_arr(rows_to_clear, rows_idx + 1);
        #ifdef DEBUG_T
            fprintf(gamelog, "clearing %d rows with top_row=%d\n", rows_idx + 1, top_row);
            fflush(gamelog);
        #endif

        clear_rows(tg, top_row, rows_idx + 1);
    }

    // NOW, WE SPAWN NEW PIECE
    create_rand_piece(tg);

    return true;
}


/**
 * Check if we've reached a game over state
 * @returns true if game over, false otherwise. Sets
 * tg.game_over on true state. 
*/
bool check_game_over(TetrisGame *tg) {
    // currently there's an issue that when the stack gets to the top
    // the board highest maxes out at 2 and it doesn't detect the 
    // game over condition. it keeps adding pieces on top of each other, 
    // whcih is a mess.

    return false;

}


///**
// * Very simple inline function for checking if an int 
// * is already present in a short array of values
//*/
//inline bool val_in_arr(const int val, const int arr[], const size_t arr_len) {
//    for (int i = 0; i < arr_len; i++) {
//        if (arr[i] == val)
//            return true;
//    }
//    return false;
//
//}

/**
 * Very simple helper function to return smallest value in array
 * (uint8_t only, used for row clearing operation)
*/
int16_t smallest_in_arr(int16_t arr[], const size_t arr_size) {
    int smallestVal = INT16_MAX;    
    for (int i = 0; i < arr_size; i++) {
        if (arr[i] < smallestVal) {
            smallestVal = arr[i];
        }
    }
    return smallestVal;
}


/**
 * Get difference between before and after in microseconds, accounting for 
 * the fact the seconds place rolls over
 * 
 * @note timeval seems like the most portable way to get time, 
 * so it looks like i'm going with that 
*/
inline int32_t get_elapsed_us(struct timeval before, struct timeval after) {
    int32_t elapsed_us;

    if (after.tv_usec >= before.tv_usec) {
        elapsed_us = after.tv_usec - before.tv_usec;
    }
    else {
        elapsed_us = 100000 + after.tv_usec - before.tv_usec;
    }
    return elapsed_us;
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
   }, 
   // L
   {{{0,0}, {1,0}, {2,0}, {2,1}},
   {{0,1}, {1,-1}, {1,0}, {1,1}},
   {{0,0}, {0,1}, {1,0}, {2,0}},
   {{0,0}, {1,0}, {1,1}, {1,2}}
   },
   // J
   {{{0,1}, {1,1}, {2,0}, {2,1}},
   {{0,-1}, {0,0}, {0,1}, {1,1}},
   {{0,0}, {0,1}, {1,0}, {2,0}},
   {{0,0}, {1,0}, {1,1}, {1,2}}
   },
   // SQUARE
   {{{0,0}, {0,1}, {1,0}, {1,1}},
   {{0,0}, {0,1}, {1,0}, {1,1}},
   {{0,0}, {0,1}, {1,0}, {1,1}},
   {{0,0}, {0,1}, {1,0}, {1,1}}
   },
   // I
   {{{0,0}, {0,1}, {0,2}, {0,3}},
   {{0,0}, {1,0}, {2,0}, {3,0}},
   {{0,0}, {0,1}, {0,2}, {0,3}},
   {{0,0}, {1,0}, {2,0}, {3,0}}
   }
};



