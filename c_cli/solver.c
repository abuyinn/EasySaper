#include "defs.h"
#include "core.h"
#include "solver.h"
#include <ctype.h>


// all things shown to user in b write to sb
// also mark all the rest in board as UNKN
void rewrite_shown(Board *b, char **sb){
    for (int r=1; r<=b->size; ++r){
        for (int c=1; c<=b->size; ++c){
            sb[r][c] = (b->mask[r][c]==MASK_VISIBLE ? b->b[r][c] : UNKN);
        }
    }
}

int mark_ch1_around_as_ch2(char **sb,int r,int c,char ch1,char ch2){
    int changes=0;
    if (sb[r-1][c-1]==ch1)  sb[r-1][c-1]=ch2, ++changes;
    if (sb[r-1][c  ]==ch1)  sb[r-1][c  ]=ch2, ++changes;
    if (sb[r-1][c+1]==ch1)  sb[r-1][c+1]=ch2, ++changes;
    if (sb[r  ][c-1]==ch1)  sb[r  ][c-1]=ch2, ++changes;
    if (sb[r  ][c+1]==ch1)  sb[r  ][c+1]=ch2, ++changes;
    if (sb[r+1][c-1]==ch1)  sb[r+1][c-1]=ch2, ++changes;
    if (sb[r+1][c  ]==ch1)  sb[r+1][c  ]=ch2, ++changes;
    if (sb[r+1][c+1]==ch1)  sb[r+1][c+1]=ch2, ++changes;
    return changes;
}

// marking all UNKN cells around sb[r][c] as BOMB
int mark_bombs_around(char **sb, int r, int c){
    return mark_ch1_around_as_ch2(sb,r,c,UNKN,BOMB);
}

// if around cell are n bombs and n undiscovered+bombs:
//      mark all undiscovered as bombs
int solver_mark_bombs(char **sb, int size){
    int i=0, changes=0;
    for (int r=1; r<=size; ++r)
        for (int c=1; c<=size; ++c){
            if ( isdigit(i=sb[r][c]) &&
                 ((i-'0')==count_ch_around(sb,r,c,UNKN)
                          +count_ch_around(sb,r,c,BOMB)) )
                changes += mark_bombs_around(sb,r,c);
        }
    return changes;
}

// mark cells where for sure is no BOMB as TODI
int solver_mark_cells_to_discover(char **sb, int size){
    int i=0, changes=0;
    for (int r=1; r<=size; ++r){
        for (int c=1; c<=size; ++c)
            if (  isdigit(i=sb[r][c]) &&
                    (i-'0')==count_ch_around(sb,r,c,BOMB)  ){
                changes += mark_ch1_around_as_ch2(sb,r,c,UNKN,TODI);
            }
    }
    return changes;
}

void solve_certain(char **sb, int size){
    while(  solver_mark_bombs(sb,size) + 
            solver_mark_cells_to_discover(sb,size) )
        ;
}

// in sb:
// - rewrite all discovered cells
// - mark as BOMB all bombs
// - mark as TODI all cells safe to discover
// according to information shown to user
// NOT MORE
char ** basic_solver(Board *b){
    char **sb = make_arr2x2(b->size+2,EDGE);
    rewrite_shown(b,sb);
    solve_certain(sb,b->size);
    // TODO:
    // if solved all around discovered
    // check UNKN > all_bombs - bombs
    // and try to plant
    
    return sb;
}

