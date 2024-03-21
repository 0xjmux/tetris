# Tetris Game Engine

This game engine is part of a larger project where I'm implementing a version of Tetris playable on WS2812B LED matrices via an ESP-Now remote. 

The game was branched off to it's own repository because the embedded project source tree only needs `tetris.h` and `tetris.c`, but all the other driver and testing files take up a decent amount of space - so while i'm developing it, it'll live here. 

### TODO:
* [ ] Finish tetris game logic
* [ ] fix gameover state not detected
* [ ] fix duplicate colors
* [ ] figure out what's causing a bunch of out-of-bounds numbers to show up in row 1 of board - i think this is a type issue with uint & int types!
    * [ ] this might be fixed with new type conversion functions? idk
* [ ] fix gameover state detected prematurely, causing crash
* [ ] fix score not incrementing on line clear
* [ ] add play/pause functionality
* [ ] fix moving left/right choppy feel
* [ ] figure out why check_and_clear being called on rows at very top of board - this preceeds premature gameover
    * this crash seems to happen on another piece clear

#### fixed
* [X] fix J piece rotation
* [X] Set up CI/CD with GH actions
* [X] fix full lines not being removed
* [X] fix crash on trying to clear line


> [!WARNING]
> This project is still in active development, bugs are still being found. When this is no longer the case, this notice will be removed. 


### Project Goals
This project has several goals. 
* Game needs to be runnable on low-powered devices using interrupts
* Driver code needed to make a given display or controller work should be kept as minimal as possible. Currently, the game exposes a 2D array of `int8_t` values representing different piece colors, and all the display implementation needs to do is render that to a grid of the board size. 
* Teach myself embedded test-driven development using techniques from James Greening's "Test-Driven Development for Embedded C" ([link](https://pragprog.com/titles/jgade/test-driven-development-for-embedded-c/))


This game is my first attempt at writing a component of medium complexity, and one of my primary goals is bettering my understanding of development best practices to accelerate my work on future projects. 


#### Some of what I've learned
mostly for myself to not forget
* CI build options in CMake, so that CI can build a different version depending on paths and such
