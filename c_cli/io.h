#ifndef IOH
#define IOH


#include <stdio.h>
#include <ctype.h>
#include <string.h>

void welcome();
void show_board(Board *b);

int  get_move(int *row, int *col, int size);

void show_errors(int err);
void show_game_over();
void show_winner();

void pmsg(int i);

#endif
