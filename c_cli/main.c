#include "defs.h"
#include "io.h"
#include "core.h"

int main(void){
    Board *b=make_board(SIZE);

    // hello!
    welcome();
    show_board(b);

    // playing...
    while (move(b)!=BOMB && b->remaining_moves){
        show_board(b);
    }

    // result
    show_board(b);
    show_result(b->remaining_moves);

    // clear
    del_board(b);

    return 0;
}
