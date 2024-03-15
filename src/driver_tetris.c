/**
 * This file acts as a driver for the tetris game so it can be run on 
 * an x86 machine to test functionality before porting to RTOS
 * @file      
 * @brief     
 * @version   
 * @author    
 * @date      
 *
 */

#include "driver_tetris.h"
#include "display.h"
#include "tetris.h"


int main(void) {


    // set up ncurses
    WINDOW *g_win, *s_win;      // game and score windows
    initscr();
    cbreak();
    noecho();       // dont echo key presses to screen
    keypad(stdscr, TRUE);    // enable use of arrow and function keys
    curs_set(0);   // don't show cursor on screen 
    timeout(0);         // dont block on getch()
    // getmaxyx(stdscr, row, col);      // get window size to int row, int col
    
    // refresh();

    // create new ncurses window // NCURSES INIT DEBUGISH CODE
    // const int winheight = BLOCK_WIDTH * TETRIS_ROWS;
    const int winheight = TETRIS_ROWS;
    const int winwidth = BLOCK_WIDTH * TETRIS_COLS;
    // WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
    g_win = newwin(winheight + 2, winwidth + 2, 2, 2);        // creates game window
    nc_init_colors(g_win);
    s_win = newwin(SCORE_WIN_ROWS, SCORE_WIN_COLS, 2, winwidth + 5); // score window
    box(g_win, 0,0);
    box(s_win, 0,0);



    wrefresh(g_win);
    wrefresh(s_win);

    TetrisGame *tg = create_game();
    // tg->board = create_board();
    enum player_move move = T_NONE;

    // while game is running and player hasn't tried to quit
    while (!tg->game_over && move != T_QUIT) {


        tg_tick(tg, move);

        // display board
        display_board(g_win, &tg->board);
        update_score(s_win, tg);

        sleep_millis(10);       // delay for 10ms (will be replaced with ISR)
        

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
            case ' ':
                move = T_PLAYPAUSE;
                break;
            case 'q':
                move = T_QUIT;
                break;
            default:
                move = T_NONE;
        }

        // print keypress for debugging
        #ifdef DEBUG_T
        if (move != T_NONE) print_keypress(move);
        #endif
    }

    // if we're here, game is over; dealloc tg
    end_game(tg);
        
    // TODO ask player if they want to play again


    endwin();

}

/**
 * Draw board array presented by tetris game
*/
void display_board(WINDOW *w, TetrisBoard *tb) {
    box(w, 0,0);


    // draw existing pieces on board
    for (int i = 0; i < TETRIS_ROWS; i++) {
        // move ncurses cursor
        wmove(w, 1 + i, 1);
        for (int j = 0; j < TETRIS_COLS; j++) {

            if (tb->board[i][j] >= 0) {
                ADD_BLOCK(w, tb->board[i][j]);
            }
            else {
                ADD_EMPTY(w);
            }

        }
    }

    wrefresh(w);

}


// void draw_piece(WINDOW *w, TetrisBoard *tb)


void update_score(WINDOW *w, TetrisGame *tg) {
    wclear(w);
    box(w,0,0);
    wmove(w, 1, 1);
    mvwprintw(w, 1,1, "Score: %d\n", tg->score);
    mvwprintw(w, 2,1, "Level: %d\n", tg->level);

    wnoutrefresh(w);

}



/** 
 * Debug function to print out player move
*/
void print_keypress(enum player_move move) {
    // translate enum value into human-readable move
    char const* move_str[] =  {"T_NONE", "T_UP", "T_DOWN", "T_LEFT", \
    "T_RIGHT", "T_PLAYPAUSE", "T_QUIT"};
    fprintf(gamelog, "Received move: %s\n", move_str[move]);
    fflush(gamelog);

}

/**
 * POSIX sleep for `millis` milliseconds
*/
void sleep_millis(uint16_t millis) {
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = millis * 1000 * 1000;
    nanosleep(&t, NULL);
}
