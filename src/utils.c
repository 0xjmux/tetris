/** 
 * Helper functions to be shared between driver_tetris and unit tests
 * 
*/

#include "utils.h"

// macros for inih parsing
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
    //MATCH("SECTION", "KEY")
    if (MATCH_SECTION("TETRIS_GAME_STRUCT")) {

        if (MATCH_KEY("game_over")) {
            tg->game_over = atoi(value);
        } else if (MATCH_KEY("score")) {
            tg->score = atoi(value);
        } else if (MATCH_KEY("level")) {
            tg->level = atoi(value);
        } else if (MATCH_KEY("gravity_tick_rate_usec")) {
            tg->gravity_tick_rate_usec = atoi(value);

        } else if (MATCH_KEY("last_gravity_tick_usec")) {
            // some manual manipulation needed here since both vals on one line
            char *timeval_str = strdup(value);

            tg->last_gravity_tick_usec.tv_sec = atoi(strtok(timeval_str,","));
            tg->last_gravity_tick_usec.tv_usec = atoi(strtok(NULL, ","));

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

        // not the most efficient way of going about this, but it's cleaner at least
        reconstruct_board_from_str_row(&tg->active_board, name, value);

    }

    else if (MATCH_SECTION("board")) {
        reconstruct_board_from_str_row(&tg->board, name, value);

    }

    else if (MATCH_SECTION("BOARD_IMAGE")) {
        //we're ignoring the pretty print
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
    // for some reason ini_parse throws an error even on successful read. bruh
    if(ini_parse(filename, handler, tg)) {
        fprintf(gamelog, "can't load game save %s!\n", filename);
        // return false;
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
        

        // this is not a great way to do this but it'll only ever run on 
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

                // printf("copied row %d: %s\n", i, value);
            }
        }

}



/**
 * Print current board state to console
 * @param TetrisBoard
 * @param *file to print to - NULL for default
*/
void print_board_state(TetrisBoard tb, FILE *file) {
    if (file == NULL) 
        file = gamelog;
    // draw existing pieces on board
    fprintf(file, "Highest occupied cell: %d\n   ", tb.highest_occupied_cell);
    fprintf(file, "  ");
    for (int i = 0; i < TETRIS_COLS; i++) 
        fprintf(file, "%-2d  ",i);
    fprintf(file, "\n   ");

    for (int i = 0; i < TETRIS_COLS; i++) 
        fprintf(file, "----");
    fprintf(file, "----\n");

    for (int i = 0; i < TETRIS_ROWS; i++) {
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