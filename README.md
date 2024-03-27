# Tetris Game Engine

This game engine is part of a larger project where I'm implementing a version of Tetris playable on WS2812B LED matrices via an ESP-Now remote. 

The game was branched off to its own repository because the embedded project source tree only needs `tetris.h` and `tetris.c`, but all the other driver and testing files take up a lot of space.

### TODO:
* [ ] fix duplicate colors
    * this is an ncurses color issue with TERM, the game logic is setting it correctly
* [ ] maybe rewrite render_active_board to no-copy to increase speed?

#### fixed
* [X] fix J piece rotation
* [X] Set up CI/CD with GH actions
* [X] fix full lines not being removed
* [X] fix crash on trying to clear line
* [X] fix gameover state detected prematurely, causing crash [Issue #1](https://github.com/0xjmux/tetris/issues/1)
* [X] fix score not incrementing on line clear
* [X] maybe fixed: figure out why check_and_clear being called on rows at very top of board
    * this preceeds premature gameover, this crash seems to happen on another piece clear
* [X] figure out what's causing a bunch of out-of-bounds numbers to show up in row 1 of board - i think this is a type issue with uint & int types!
* [X] add play/pause functionality
* [X] Finish tetris game logic
* [X] fix gameover state not detected
* [X] add confirmation on quit to driver



### About
* One of the main goals of this project is to gain experience developing and debugging components on a workstation that can then be easily ported to MCU platforms. As such, the linux driver for this game is filled with debugging features to make hunting bugs in the game logic as easy as possible, since that same process will be much more difficult when in the resource-constrained environment of an MCU. 
* Some features include
    * Saving entire game state to a file at any point by pressing `p`. Game state can then be restored, but these saves are largely used for unit testing purposes since having a start point of a game that you know preceded a crash can be very helpful when trying to force the game into invalid states. 
    * Some of these files can be found in [test/files/](./test/files/)
    * Game state is also saved to a file on gameover, which was done to debug issues that cause premature gameover conditions. 
* Everything is written from scratch, with the exception of the graphics library (ncurses), Unit test harness (Unity), and `.ini` file parser, inih. 
* 

### Project Goals
This project has several goals. 
* Game needs to be runnable on low-powered devices using interrupts
* Driver code needed to make a given display or controller work should be kept as minimal as possible. Currently, the game exposes a 2D array of `int8_t` values representing different piece colors, and all the display implementation needs to do is render that to a grid of the board size. 
* Teach myself embedded test-driven development using techniques from James Greening's "Test-Driven Development for Embedded C" ([link](https://pragprog.com/titles/jgade/test-driven-development-for-embedded-c/))


This game is my first attempt at writing a component of medium complexity, and one of my primary goals is bettering my understanding of development best practices to accelerate my work on future projects. 


### Some of what I've learned
mostly for myself to not forget
* CI build options in CMake, so that CI can build a different version depending on paths and such

### Compilation
#### Game Driver (runs in terminal on Linux workstations)
```sh
cmake . -B build && cmake --build build
./build/tetris_driver
```

#### Unit Tests
```sh
cmake . -B build -DTARGET_GROUP=test && cmake --build build
./build/test_tetris
```

#### Debugging 
If debugging flags are enabled, two game state files will show up in the current directory when the game is finished: `game.log` and `final-gamestate.ini`. The `game.log` is a log of actions taken during the game to speed up tracing logic problems; `final-gamestate.ini` contains a human & machine readable save of the entire game state at gameover, allowing easier debugging of premature exit conditions (which was one of the bigger bugs I had to find). The log file automatically updates during gameplay, so a live log of what's happening in-game can be watched in a separate terminal session by doing `tail -f game.log`. 

One of the main reasons I set up the `.ini` file saving functionality was for unit testing. This can be seen in the `test_clearRowsDumpedGame()` functions inside `test/suite_1.c`. The game state is restored and then used to test edge cases and look for weird behavior, all starting from an actual state reached in-game. 


#### Implementation and Controls
* For your own microcontroller implementation, see `src/driver_tetris.c` as an example; it shows you everything that's needed for a complete implementation. The game is designed to be as self-contained as possible, so past passing along inputs and rendering the board array there's not much you need to do. 

Essentially, this is it:
```c
// start game, define player_move variable, and create first piece
TetrisGame *tg = create_game();
enum player_move move = T_NONE;
create_rand_piece(tg);     

while([game not over]) {
    // do game tick
    tg_tick(tg, move);

    // process player input
    // wait for 10 ms before doing it again
}
```

The game board itself is rendered to a 2D array `int8_t board[TETRIS_ROWS][TETRIS_COLS]` accessible via `tg->active_board.board`. All your display implementation needs to do is render this array into the associated colors for whatever display format is desired. 

The code is documented using Doxygen style comments. Custom types are documented in `tetris.h`, and functions are preceded by short explanations in `tetris.c`. On inclusion into your project, your IDE's LSP server should automatically show these descriptions on hover. 

##### Linux Game Controls
These are the controls for my implementation (`driver_tetris.c`) for POSIX terminals via ncurses. 
```
Use arrow keys to move
SPACE pauses game
'q' quits
'p' prints current game state to .ini file (for debugging purposes)
```


#### Flags
* There are many compilation flags to enable/disable features, mostly for debugging. I've also created several compilation flags that enable extra output on CI builds so it's easier to see what went wrong from the build report console. The default options should be fine, but for finer tuning you can see the options available to you across the project's `CMakeLists.txt` files. 


##### Other
I have these aliases in my `~/.zshrc` to make the testing process more fluid. 
```sh
alias cmakeclean='rm -rf CMakeCache.txt cmake_install.cmake Makefile CMakeFiles build'
alias cmakeregen='cmake . -B build && cmake --build build'
alias cmaketest='cmake . -B build -DTARGET_GROUP=test && cmake --build build'
```

