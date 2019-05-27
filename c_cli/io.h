#ifndef IOH
#define IOH


#include <stdio.h>
#include <ctype.h>

void welcome();
void show_board(Board *b);

int  get_move(int *line, int *col, int size);

void show_errors(int err);
void show_game_over();
void show_winner();


#endif
