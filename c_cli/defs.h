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

// errors numbers
#define ERR_NOT_IN_BOARD        -1
#define ERR_ALREADY_DISCOVERED  -2
#define ERR_FLAGGED             -3
#define ERR_STRANGE_MASK        -4
#define ERR_BAD_OPTION          -5


typedef struct {
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
