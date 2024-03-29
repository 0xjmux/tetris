/** 
 * Helper functions to be shared between driver_tetris and unit tests, 
 * including functionality to save and restore game state from .ini files
 * 
*/

#include "utils.h"

/**
 * Returns piece_type as string for debugging purposes
*/
const char* get_piece_str(enum piece_type ptype) {
    assert(ptype < 7 && "piece_type enum out of range!");
    const char* piece_str[] =  {"S_PIECE", "Z_PIECE", "T_PIECE", "L_PIECE", "J_PIECE", "SQ_PIECE", "I_PIECE"};
    return piece_str[ptype];
}


/**
 * Simple function to create a new piece with the specified parameters
*/
TetrisPiece create_tetris_piece(enum piece_type ptype, \
    int16_t row, int16_t col, uint8_t orientation) 
{
    assert(orientation >= 0 && orientation < 4 && "Orientation out of range");

    TetrisPiece new_piece = {.ptype = ptype, .orientation = orientation, \
        .loc.col = col, .loc.row = row , .falling=true};

    return new_piece;
}



// macros for ini parsing
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
#define MATCH_SECTION(n) strcmp(section, n) == 0
#define MATCH_KEY(n) strcmp(name, n) == 0

/**
 * Handler for reading .ini file
*/
static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    TetrisGame *tg = (TetrisGame*)user;

    // this is ugly but ig you cant do it with switch/case so here we go
    if (MATCH_SECTION("TETRIS_GAME_STRUCT")) {

        if (MATCH_KEY("game_over")) {
            tg->game_over = atoi(value);
        } else if (MATCH_KEY("score")) {
            tg->score = atoi(value);
        } else if (MATCH_KEY("level")) {
            tg->level = atoi(value);
        } else if (MATCH_KEY("lines_cleared_since_last_level")) {
            tg->lines_cleared_since_last_level = atoi(value);
        } else if (MATCH_KEY("gravity_tick_rate_usec")) {
            tg->gravity_tick_rate_usec = atoi(value);
        } else if (MATCH_KEY("last_gravity_tick_usec")) {
            // some manual manipulation needed here since both vals on one line
            char *timeval_str = strdup(value);

            tg->last_gravity_tick_usec.tv_sec = atoi(strtok(timeval_str,","));
            tg->last_gravity_tick_usec.tv_usec = atoi(strtok(NULL, ","));
            free(timeval_str);

        } else if (MATCH_KEY("active_board_highest_occupied_cell")) {
            tg->active_board.highest_occupied_cell = atoi(value);
        } else if (MATCH_KEY("board_highest_occupied_cell")) {
            tg->board.highest_occupied_cell = atoi(value);

        } else {
            printf("ERROR IN HANDLER DECODING TG INI: name=%s\n", name);
            return 0;
        }

    }
    else if (MATCH_SECTION("ACTIVE_PIECE")) {

        if (MATCH_KEY("ptype")) {
            tg->active_piece.ptype = atoi(value);
        } else if (MATCH_KEY("loc_row")) {
            tg->active_piece.loc.row = atoi(value);
        } else if (MATCH_KEY("loc_col")) {
            tg->active_piece.loc.col = atoi(value);
        } else if (MATCH_KEY("orientation")) {
            tg->active_piece.orientation = atoi(value);
        } else if (MATCH_KEY("falling")) {
            tg->active_piece.falling = atoi(value);
        } else {
            printf("ERROR IN HANDLER DECODING ACTIVE_PIECE INI: name=%s\n", name);
            return 0;
        }
    }

    // read board from file
    else if (MATCH_SECTION("active_board")) {
        reconstruct_board_from_str_row(&tg->active_board, name, value);

    }

    else if (MATCH_SECTION("board")) {
        reconstruct_board_from_str_row(&tg->board, name, value);

    }

    else if (MATCH_SECTION("BOARD_IMAGE")) {
        //we're ignoring the pretty print section
        ;
    }

    return 1;
}



/**
 * Restore game state saved to .ini file
 * @param TetrisGame* game object to save state to
 * @param savefile file handle to read from
 * @param gamelog log output location
 * @returns true if successful, false if not
*/
bool restore_game_state(TetrisGame *tg, const char* filename, FILE *gamelog) {

    uint8_t parse_ret = ini_parse(filename, handler, tg);
    if(parse_ret) {
        fprintf(gamelog, "can't load game save %s, error code %d!\n", filename, parse_ret);
        return false;
    }
    fprintf(gamelog, "Config loaded from %s!\n", filename);

    return true;

}

/**
 * Given board represented as string (from file), reconstruct tb.board 
 * based on the given values
*/
void reconstruct_board_from_str_row(TetrisBoard *tb, const char *name, const char *value) {

    // each cell is max 3 len, plus space for null byte
    #define MAX_STR_ROW_LEN TETRIS_COLS * 3 + 1
    #define MAX_ROW_NAME_LEN 10

        // curr row key
        char curr_row[MAX_ROW_NAME_LEN]; // = value;
        char str_row[MAX_STR_ROW_LEN];
        strcpy(str_row, value);
        

        // there are better ways of doing this but it'll only ever run on 
        //  "normal" computers and should be good enough
        for (int i = 0; i < TETRIS_ROWS; i++) {
            snprintf(curr_row, MAX_ROW_NAME_LEN, "row_%d", i);
            if (MATCH_KEY(curr_row)) {

                char *curr_cell = "";
                curr_cell = strtok(str_row, ",");
                tb->board[i][0] = atoi(curr_cell);
                for (int j = 1; j < TETRIS_COLS; j++) {
                    // go cell by cell and fill the array based on whats here
                    curr_cell = strtok(NULL, ",");
                    tb->board[i][j] = atoi(curr_cell);
                }

            }
        }

}





/**
 * This function helps with debugging edge cases. 
 * Will save the board to a file `gamestate.ini` 
*/
void save_game_state(TetrisGame *tg, const char* filename) {
        FILE *savefile;
        savefile = fopen(filename, "w+");


        // pretty-print board state to file for ease of differentiation
        fprintf(savefile, "[BOARD_IMAGE]\n");
        print_board_state(tg->active_board, savefile, true);

        // save TetrisGame state
        fprintf(savefile, "\n\n[TETRIS_GAME_STRUCT]\n");
        fprintf(savefile, "active_board_highest_occupied_cell = %d\n", tg->active_board.highest_occupied_cell);
        fprintf(savefile, "board_highest_occupied_cell = %d\n", tg->board.highest_occupied_cell);
        fprintf(savefile, "\n");
        fflush(savefile);

        fprintf(savefile, "game_over = %d\n", tg->game_over);
        fprintf(savefile, "score = %d\n", tg->score);
        fprintf(savefile, "level = %d\n", tg->level);
        fprintf(savefile, "lines_cleared_since_last_level = %d\n", tg->lines_cleared_since_last_level);
        fprintf(savefile, "gravity_tick_rate_usec = %d\n", tg->gravity_tick_rate_usec);
        fprintf(savefile, "last_gravity_tick_usec = %ld,%ld\n", tg->last_gravity_tick_usec.tv_sec, tg->last_gravity_tick_usec.tv_usec);

        fprintf(savefile, "\n[ACTIVE_PIECE]\n");
        fprintf(savefile, "ptype = %d\n", tg->active_piece.ptype);
        fprintf(savefile, "loc_row = %d\n", tg->active_piece.loc.row);
        fprintf(savefile, "loc_col = %d\n", tg->active_piece.loc.col);
        fprintf(savefile, "orientation = %d\n", tg->active_piece.orientation);
        fprintf(savefile, "falling = %d\n", tg->active_piece.falling);


        fprintf(savefile, "\n[active_board]\n");
        ini_save_board_to_file(savefile, tg->active_board);

        fprintf(savefile, "\n[board]\n");
        ini_save_board_to_file(savefile, tg->board);

        printf("\ngame state saved!\n");

        fclose(savefile);

}



/**
 * Save board to ini file in easier to read format. 
 * Each board will have its own section to make this less painful
*/
void ini_save_board_to_file(FILE *file, TetrisBoard tb) {

    for (int i = 0; i < TETRIS_ROWS; i++) {
        fprintf(file, "row_%d = ", i);
        for (int j = 0; j < TETRIS_COLS - 1; j++) {
            fprintf(file, "%2d,", tb.board[i][j]);
        }
        fprintf(file, "%2d\n", tb.board[i][TETRIS_COLS - 1]);
        
    }
    fflush(file);
}


/**
 * Print current board state to console
 * @param TetrisBoard
 * @param *file to print to - NULL for default (gamelog)
 * @param ini_out - if printing to an .ini file, prepend each line with comment char ";"
*/
void print_board_state(TetrisBoard tb, FILE *file, bool ini_out) {
    if (file == NULL) {
        #ifdef DEBUG_T
            file = gamelog;
        #else  
            // if this is a debug statement but debug is disabled, exit now
            file = NULL;
            return;
        #endif
    }


    // draw existing pieces on board
    if (ini_out) fprintf(file, "; ");
    fprintf(file, "Highest occupied cell: %d\n", tb.highest_occupied_cell);

    // print col numbers
    if (ini_out) fprintf(file, "; ");
    fprintf(file, "  ");
    for (int i = 0; i < TETRIS_COLS; i++) 
        fprintf(file, "%-2d  ",i);
    fprintf(file, "\n");

    // print separator
    fprintf(file, "   ");
    if (ini_out) fprintf(file, "; ");
    for (int i = 0; i < TETRIS_COLS; i++) 
        fprintf(file, "----");
    fprintf(file, "----\n");

    // print board itself
    for (int i = 0; i < TETRIS_ROWS; i++) {
        if (ini_out) fprintf(file, "; ");
        // print row number
        fprintf(file, "%-3d| ", i);
        for (int j = 0; j < TETRIS_COLS; j++) {
            if (tb.board[i][j] >= 0) {
                fprintf(file, "%-3d ", tb.board[i][j]);
            }
            else {
                fprintf(file, "    ");
            }
        }
        fprintf(file, "|\n");
    }
    fflush(file);
}


/**
 * POSIX sleep for `millis` milliseconds
 * `man 2 nanosleep`
*/
void sleep_millis(uint16_t millis) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = millis * 1000; // * 1000;
    nanosleep(&ts, NULL);
}