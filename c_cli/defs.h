#ifndef DEFSH
#define DEFSH


/* NAMING CONVENTIONS:
 * int || char **  r - row
 * int || char *   c - column (not a char)
 * Board           b - Board
 * char **        sb - array to replace b.b during salvation
 * char           ch - a char (not a column)
 */


#include <stdlib.h>
#include <time.h>

#define SIZE   5
#define NBOMBS 7


// VALUES FOR Board.b AND sb
#define EDGE 'E'
#define UNKN '.'
#define NTHG ' '
#define BOMB '*'
#define TODI 'D' /*to discover*/

// VALUES FOR Board.mask
#define MASK_HIDDEN   0
#define MASK_VISIBLE  1
#define FLAG         'F'


// ERRORS NUMBERS
#define ERR_NOT_IN_BOARD        -1
#define ERR_ALREADY_DISCOVERED  -2
#define ERR_FLAGGED             -3
#define ERR_STRANGE_MASK        -4
#define ERR_BAD_OPTION          -5


typedef struct {
    // b and mask are size+2 x size+2
    // important indexes: [1 .. size]
    char ** b;

    char ** mask; 
    int size;
    int all_bombs;
    int remaining_bombs;
    int remaining_moves;
} Board;


#endif
