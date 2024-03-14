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



void update_score(WINDOW *w, TetrisGame *g);

