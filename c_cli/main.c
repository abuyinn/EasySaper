#include "defs.h"
#include "io.h"

////////////////////////
// CREATING AND DELETING
////////////////////////

// return array len x len
char ** make_tab2x2(int len, char def_val){
    char** board = (char**)malloc(len*sizeof(char*));
    for (char** p=board; p<board+len; ++p){
        *p = (char*)malloc(len*sizeof(char));
        for (char *p2=*p; p2<*p+len; ++p2)
            *p2=def_val;
    }
    return board;
}

void plant_bombs(Board *b){
    int planted=0;
    int line=0, col=0;
    srand(time(NULL));

    while (planted < b->bombs){
        // insert a bomb to free cell in [1 .. size]
        line = rand() % b->size + 1;
        col  = rand() % b->size + 1;
        if (b->b[line][col]==NTHG){
            b->b[line][col]=BOMB;
            planted++;
        }
    }
}

// count the amount of bombs around
void count_cells(Board *b){
    char **bo=b->b;
    for (int l=1; l<=b->size; ++l){/* line */
        for (int c=1; c<=b->size; ++c){ /* col */
            if (bo[l][c]!=BOMB){
                // empty, so we have to count bombs here
                // we have guards in edges
                // ...so we don't care about go over array
                bo[l][c] = '0' +
                    (bo[l-1][c-1]==BOMB) +
                    (bo[l-1][c  ]==BOMB) +
                    (bo[l-1][c+1]==BOMB) +

                    (bo[l  ][c-1]==BOMB) +
                    (bo[l  ][c+1]==BOMB) +

                    (bo[l+1][c-1]==BOMB) +
                    (bo[l+1][c  ]==BOMB) +
                    (bo[l+1][c+1]==BOMB);
                if (bo[l][c]=='0')
                    bo[l][c]=NTHG;
            }
        }
    }
}

Board * make_board(int size){
    Board *b=(Board*) malloc(sizeof(Board));
    // arrays will have long: (size+2)x(size+2)
    b->b      = make_tab2x2(size+2,NTHG);
    b->mask = make_tab2x2(size+2,0);
    b->size   = size;
    b->bombs  = NBOMBS;
    b->remaining_bombs = b->bombs;
    b->remaining_moves = size*size-b->bombs;
    plant_bombs(b);
    count_cells(b);
    return b;
}

void del_board(Board *b){
    for (char **p=b->b, **m=b->mask;
            p < b->b+b->size+2;
                free(*p++), free(*m++))
        ;
    free(b->b);
    free(b->mask);
    free(b);
}


///////////
// CHECKING
///////////

int is_in_board(Board *b, int line, int col){
    return line>0 && col>0 && line<=b->size && col<=b->size;
}

// returns:
//  1 - cell is ready to discover
// -1 - cell is outside the board
// -2 - cell is visible
// -3 - cell is flagged
// -4 - strange value of mask[line][col]
int is_move_ok(Board *b, int line, int col){
    if (! is_in_board(b,line,col))
        return -1;
    switch (b->mask[line][col]){
        case 0:     return  1;
        case 1:     return -2;
        case FLAG:  return -3;
        default:    return -4;
    }
}

// returns:
//  1 - cell is ready to be unflagged (is   flagged)
//  2 - cell is ready to be   flagged (is unflagged and undiscovered)
// -5 - cell is already discovered
// -6 - strange value of mask[line][col]
int is_flagged(Board *b, int line, int col){
    switch (b->mask[line][col]){
        case FLAG:  return  1;
        case 0:     return  2;
        case 1:     return -5;
        default:    return -6;
    }
}


///////////////////////////
// DISCOVERING AND FLAGGING
///////////////////////////

int discover_cell(Board *b, int line, int col);

int discover_empty_area(Board *b, int line, int col){
    if (b->b[line][col]==NTHG){
        // there is no bomb around
        discover_cell(b,line-1,col-1);
        discover_cell(b,line-1,col  );
        discover_cell(b,line-1,col+1);
        
        discover_cell(b,line  ,col-1);
        discover_cell(b,line  ,col+1);
        
        discover_cell(b,line+1,col-1);
        discover_cell(b,line+1,col  );
        discover_cell(b,line+1,col+1);
        return 0;
    }
    return -1;
}

int discover_cell(Board *b, int line, int col){
    if (is_move_ok(b, line, col)==1){
        b->mask[line][col]=1;
        if (b->b[line][col]!=BOMB)
            b->remaining_moves--;
        discover_empty_area(b,line,col);
        return b->b[line][col];
    } else
        return -1;
}

int flag_cell(Board *b, int line, int col){
    if (! is_in_board(b, line, col))
        return -1;
    
    switch (b->mask[line][col]){
        case 0:
            b->mask[line][col]=FLAG;
            b->remaining_bombs--;
            return 0;
        case 1:
            return -2;
        case FLAG:
            b->mask[line][col]=0;
            b->remaining_bombs++;
            return 0;
        default:
            return -3;
    }
}


///////
// MOVE
///////
int move(Board *b){
    int line=0, col=0, flag=0, err=0;

    do {
        show_errors(err);
        flag = get_move(&line, &col, b->size);
    } while ( 
              (! ((err=is_move_ok(b,line,col))==1 && flag==0) ) &&
              (! ((err=is_flagged(b,line,col))>0  && flag==FLAG))  
            );

    if (flag==FLAG)
        return flag_cell(b,line,col);
    else return discover_cell(b,line,col);
}


///////
// MAIN
///////
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
    if (b->remaining_moves==0)
        show_winner();
    else
        show_game_over();

    // clear
    del_board(b);

    return 0;
}
