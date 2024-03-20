/**
 * Display handling code
*/

#include "display.h"
#include "tetris.h"


/**
 * Initialize tetromino colors for ncurses
*/
void nc_init_colors(WINDOW *w) {
    start_color();

    init_pair(BG_COLOR, COLOR_BLACK, COLOR_BLACK);

    init_pair(S_CELL_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(Z_CELL_COLOR, COLOR_RED, COLOR_BLACK);
    init_pair(T_CELL_COLOR, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(L_CELL_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(J_CELL_COLOR, COLOR_BLUE, COLOR_BLACK);
    init_pair(SQ_CELL_COLOR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(I_CELL_COLOR, COLOR_CYAN, COLOR_BLACK);
}




/**
 * This function helps with debugging edge cases. 
 * Will save the board to a file `gamestate.ini` 
*/
void save_game_state(TetrisGame *tg) {
        FILE *savefile;
        savefile = fopen("gamestate.ini", "w+");


        // pretty-print board state to file for ease of differentiation
        fprintf(savefile, "[BOARD_IMAGE]\n");
        print_board_state(tg->active_board, savefile);

        // save TetrisGame state
        fprintf(savefile, "\n\n[TETRIS_GAME_STRUCT]\n");
        fprintf(savefile, "active_board = ");
        save_board_to_file(savefile, tg->active_board);
        fprintf(savefile, "active_board.highest_occupied_cell = %d\n", tg->active_board.highest_occupied_cell);
        fprintf(savefile, "\nboard = ");
        save_board_to_file(savefile, tg->board);
        fprintf(savefile, "board.highest_occupied_cell = %d\n", tg->board.highest_occupied_cell);
        fprintf(savefile, "\n");
        fflush(savefile);

        fprintf(savefile, "game_over = %d\n", tg->game_over);
        fprintf(savefile, "score = %d\n", tg->score);
        fprintf(savefile, "level = %d\n", tg->level);
        fprintf(savefile, "gravity_tick_rate_usec = %d\n", tg->gravity_tick_rate_usec);
        fprintf(savefile, "last_gravity_tick_usec = {%ld,%ld}\n", tg->last_gravity_tick_usec.tv_sec, tg->last_gravity_tick_usec.tv_usec);

        
        fprintf(savefile, "\n[ACTIVE_PIECE]\n");
        fprintf(savefile, "ptype = %d\n", tg->active_piece.ptype);
        fprintf(savefile, "loc_row = %d\n", tg->active_piece.loc.row);
        fprintf(savefile, "loc_col = %d\n", tg->active_piece.loc.col);
        fprintf(savefile, "orientation = %d\n", tg->active_piece.orientation);
        fprintf(savefile, "falling = %d\n", tg->active_piece.falling);

        printf("game state saved!\n");

        fclose(savefile);

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
 * Handler for reading .ini file
*/
// static int handler(void* user, const char* section, const char* name,
//                    const char* value)
// {
//     TetrisGame *tg = (TetrisGame*)user;

//     #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
//     if (MATCH("protocol", "version")) {
//         pconfig->version = atoi(value);
//     } else if (MATCH("user", "name")) {
//         pconfig->name = strdup(value);
//     } else if (MATCH("user", "email")) {
//         pconfig->email = strdup(value);
//     } else {
//         return 0;  /* unknown section/name, error */
//     }
//     return 1;
// }



void save_board_to_file(FILE *file, TetrisBoard tb) {
    fprintf(file, "{");

    for (int i = 0; i < TETRIS_ROWS; i++) {
        // fprintf(file, "%-3d| ", i);
        fprintf(file, "{");
        for (int j = 0; j < TETRIS_COLS - 1; j++) {
            fprintf(file, "%d,", tb.board[i][j]);
        }
        fprintf(file, "%d}", tb.board[i][TETRIS_COLS - 1]);
        
        // dont print comma on last row
        if (i < TETRIS_ROWS - 1)
            fprintf(file,", ");
    }
    fprintf(file, "}\n");
    fflush(file);
}
