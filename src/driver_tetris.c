/**
 * This file acts as a driver for the tetris game so it can be run on 
 * an x86 machine to test functionality before porting to RTOS
 * @file driver_tetris.c
 * @brief Driver for tetris game so it can be run on Linux workstations for testing
 * @version 1.0
 * @author Jacob Bokor
 * @date 03/2024
 *
 */

#include "driver_tetris.h"
#include "utils.h"
#include "tetris.h"

// show extra window with debugging information
// #define DEBUG_T_WIN 1
#ifdef DEBUG_T_WIN
WINDOW *dbg_win;
#endif

// needs to be here for debug_win
TetrisGame *tg;

int main(void) {


    // set up ncurses
    WINDOW *g_win, *s_win;      // game and score windows
    initscr();
    cbreak();
    noecho();       // dont echo key presses to screen
    keypad(stdscr, TRUE);    // enable use of arrow and function keys
    curs_set(0);   // don't show cursor on screen 
    timeout(0);         // dont block on getch()
    nodelay(stdscr, TRUE);
    // getmaxyx(stdscr, row, col);      // get window size to int row, int col
    
    refresh();

    const int winheight = TETRIS_ROWS;
    const int winwidth = BLOCK_WIDTH * TETRIS_COLS;
    g_win = newwin(winheight + 2, winwidth + 2, 2, 2);        // creates game window

    nc_init_colors();
    s_win = newwin(SCORE_WIN_ROWS, SCORE_WIN_COLS, 2, winwidth + 5); // score window
    box(g_win, 0,0);
    box(s_win, 0,0);

    wrefresh(g_win);
    wrefresh(s_win);

    // CREATE DEBUG WINDOW
    #ifdef DEBUG_T_WIN
    dbg_win = newwin(winheight + 2 - SCORE_WIN_ROWS,  SCORE_WIN_COLS, \
     2 + SCORE_WIN_ROWS, winwidth + 5); // score window
    box(dbg_win, 0, 0);
    wrefresh(dbg_win);

    #endif

    

    tg = create_game();
    enum player_move move = T_NONE;
    create_rand_piece(tg);      // create first piece

    #ifdef DEBUG_T
    fprintf(gamelog, "========================================\n");
    fprintf(gamelog, "====       Starting new game!       ====\n");
    fprintf(gamelog, "========================================\n");
    fflush(gamelog);
    #endif

    // while game is running and player hasn't tried to quit
    while (!tg->game_over && move != T_QUIT) {


        // this function handles basically everything about the internal game state. 
        // all the driver really has to do is pass `move` along to it and then print out
        //  the tg->active_board array in whatever format is desired
        tg_tick(tg, move);

        // display board
        display_board(g_win, tg->active_board);
        update_score(s_win, tg);

        switch(getch()) {
            case KEY_UP:
                move = T_UP;
                break;
            case KEY_DOWN:
                move = T_DOWN;
                break;
            case KEY_LEFT:
                move = T_LEFT;
                break;
            case KEY_RIGHT:
                move = T_RIGHT;
                break;
            case ' ':   // SPACE pauses game
                // move = T_PLAYPAUSE;
                // align "PAUSED" text in game window ; window, y, x
                wmove(g_win, (TETRIS_ROWS / 10), (TETRIS_COLS * BLOCK_WIDTH / 2) -2);
                wprintw(g_win, "PAUSED");
                wrefresh(g_win);
                // change getch() back to blocking so pause holds until resumed
                timeout(-1);
                getch();
                timeout(0);
                move = T_NONE;

                break;

            // Quit game
            case 'q':
                wclear(g_win);
                box(g_win,0,0);
                wmove(g_win, (TETRIS_ROWS / 10), (TETRIS_COLS * BLOCK_WIDTH / 2) -4);
                wprintw(g_win, "QUIT? [Yy/Nn]");
                wrefresh(g_win);
                // change getch() back to blocking so pause holds until resumed
                timeout(-1);
                char response = getch();

                if (response == 'y' || response == 'Y' || \
                    response == ' ' || response == 'q') {
                    move = T_QUIT;
                    break;
                }
                else {
                    move = T_NONE;
                }

                timeout(0);
                break;
            // save game to disk
            case 'p':
                move = T_NONE;
                save_game_state(tg, "gamestate.ini");
                mvwprintw(s_win, 4,1, "GAME STATE SAVED\n");
                #ifdef DEBUG_T
                fprintf(gamelog, "game state saved to file gamestate.ini\n");
                #endif
                wnoutrefresh(s_win);
                 
                break;
            // load game from disk
            case 'l':
                move = T_NONE;
                // IMPLEMENT
                break;
            default:
                move = T_NONE;
        }
        sleep_millis(10);       // delay for 10ms (will be replaced with ISR)

        // print keypress for debugging
        #ifdef DEBUG_T
        if (move != T_NONE) print_keypress(move);
        #endif
    }
    #ifdef DEBUG_T
    // for debugging, save state when game exits
    save_game_state(tg, "final-gamestate.ini");
    #endif


    // if we're here, game is over; dealloc tg
    end_game(tg);
    endwin();

    printf("Game over! Level=%d, Score=%d\n", tg->level, tg->score);

}

/**
 * Draw board array presented by tetris game
*/
void display_board(WINDOW *w, TetrisBoard tb) {

    static struct timeval last_update;
    struct timeval curr_time_usec;
    gettimeofday(&curr_time_usec, NULL);

    int32_t usec_timediff = curr_time_usec.tv_usec - last_update.tv_usec; 

    // if it flows below zero, (seconds ticks over, but us doesn't) just run it
    if (usec_timediff > SCREEN_REFRESH_INTERVAL_USEC || usec_timediff < 0) {
        werase(w);
        box(w, 0,0);
        // draw existing pieces on board
        for (int i = 0; i < TETRIS_ROWS; i++) {
            // move ncurses cursor
            wmove(w, 1 + i, 1);
            for (int j = 0; j < TETRIS_COLS; j++) {

                if (tb.board[i][j] >= 0) {
                    ADD_BLOCK(w, tb.board[i][j]);
                }
                else {
                    ADD_EMPTY(w);
                }

            }
        }

        last_update = curr_time_usec;
        wrefresh(w);
        // wnoutrefresh(w);

        #ifdef DEBUG_T
        // fprintf(gamelog, "display_board()\n");
        // print_board_state(*tb, gamelog);
        // fflush(gamelog);
        #endif

        #ifdef DEBUG_T_WIN
        refresh_debug_var_window(dbg_win);


        #endif

    }
 }


void update_score(WINDOW *w, TetrisGame *tg) {
    wclear(w);
    wmove(w, 1, 1);
    mvwprintw(w, 1,1, "Score: %d\n", tg->score);
    mvwprintw(w, 2,1, "Level: %d\n", tg->level);
    mvwprintw(w, 3,1, "Lines until next Level: %d\n", 10 - tg->lines_cleared_since_last_level);
    box(w,0,0);

    wnoutrefresh(w);

}



/** 
 * Debug function to print out player move
 * Only works when DEBUG_T def enabled
*/
void print_keypress(enum player_move move) {
    // translate enum value into human-readable move
    #ifdef DEBUG_T
        char const* move_str[] =  {"T_NONE", "T_UP", "T_DOWN", "T_LEFT", \
        "T_RIGHT", "T_PLAYPAUSE", "T_QUIT"};
        fprintf(gamelog, "Received move: %s\n", move_str[move]);
        fflush(gamelog);
    #endif
}



void refresh_debug_var_window(WINDOW *w) {
    wclear(w);
    box(w,0,0);
    wmove(w, 1, 1);
    TetrisPiece tp = tg->active_piece;

    mvwprintw(w, 1,1, "DEBUG INFO:\n");
    // mvwprintw(w, 2,1, "Current move: %s\n", move_str[move]);
    // mvwprintw(w, 3,1, "Piece: %s\n", piece_str[tp.ptype]);
    mvwprintw(w, 3,1, "Piece: %s\n", get_piece_str(tp.ptype));
    mvwprintw(w, 4,1, "Piece Row: %d   Piece col: %d\n", tp.loc.row, tp.loc.col);
    mvwprintw(w, 5,1, "Orientation: %d   Falling?: %d\n", tp.orientation, tp.falling);
    mvwprintw(w, 6,1, "BOARD INFO: highest cell: %d\n", tg->board.highest_occupied_cell);

    wnoutrefresh(w);

}


/**
 * Initialize tetromino colors for ncurses
*/
// void nc_init_colors(WINDOW *w) {
void nc_init_colors(void) {
    start_color();
    if (has_colors() == false) {
        #ifdef DEBUG_T
            fprintf(stdout, "Terminal doesn't have color support!\n");
        #endif
    }

    init_pair(BG_COLOR, COLOR_BLACK, COLOR_BLACK);

    // bug: S and L have same color - this is an ncurses issue
    //  and because i'm using all 8 colors there's not much 
    //  I can do about it
    init_pair(S_CELL_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(Z_CELL_COLOR, COLOR_RED, COLOR_BLACK);
    init_pair(T_CELL_COLOR, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(L_CELL_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(J_CELL_COLOR, COLOR_BLUE, COLOR_BLACK);
    init_pair(SQ_CELL_COLOR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(I_CELL_COLOR, COLOR_CYAN, COLOR_BLACK);
    

}

