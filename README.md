# Tetris Game Engine

This game engine is part of a larger project, where I'm implementing a version of Tetris playable on WS2812B LED matrices via an ESP-Now remote. 

The game was branched off to it's own repository because the embedded project source tree only needs `tetris.h` and `tetris.c`, but all the other driver and testing files take up a decent amount of space - so while i'm developing it, it'll live here. 

### Project Goals
This project has several goals. 
* Game needs to be runnable on low-powered devices using interrupts
* Driver code needed to make a given display or controller work should be kept as minimal as possible. Currently, the game exposes an array of `uint8_t` values representing different piece colors, and all the display implementation needs to do is render that to a grid of the board size. 
* Try techniques from James Greening's "Test-Driven Development for Embedded C" [link](https://pragprog.com/titles/jgade/test-driven-development-for-embedded-c/)


This game is my first attempt at writing a component of medium complexity, and one of my primary goals is gaining a complete understanding of development best practices to accelerate my work on future projects. 
