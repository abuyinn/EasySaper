#ifndef DEFSH
#define DEFSH


#include <stdlib.h>
#include <time.h>

#define SIZE   5
#define NBOMBS 5

#define UNKN '.'
#define NTHG ' '
#define FLAG 'F'
#define BOMB '*'


typedef struct board {
    // b and b_mask are size+2 x size+2
    // important indexes: [1 .. size]
    char ** b;

    // mask:
    // 0    - hidden
    // 1    - shown
    // FLAG - flagged
    char ** mask; 
    int size;
    int bombs;
    int remaining_bombs;
    int remaining_moves;
} Board;


#endif
