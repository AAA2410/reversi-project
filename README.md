# reversi-project

The code uses the terminal/command line to simulate a game of Othello/Reversi (see rules here: https://en.wikipedia.org/wiki/Reversi)

The user can input a board size, and chooses whether the computer plays black or white (black goes first). 
The inputs are taken based on the column lettering as rowcolumn (e.g. input ad drops it into row a column d).

To compile, use the C99 standard for compilation.

  gcc -std=c99 -c game_ai.c
  
  gcc game_ai.o -o game.exe
  
  ./game.exe
