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
    int row=0, col=0;
    srand(time(NULL));

    while (planted < b->bombs){
        // insert a bomb to free cell in [1 .. size]
        row = rand() % b->size + 1;
        col  = rand() % b->size + 1;
        if (b->b[row][col]==NTHG){
            b->b[row][col]=BOMB;
            planted++;
        }
    }
}

// count the amount of bombs around
void count_cells(Board *b){
    char **bo=b->b;
    for (int r=1; r<=b->size; ++r){     /* row */
        for (int c=1; c<=b->size; ++c){ /* col */
            if (bo[r][c]!=BOMB){
                // empty, so we have to count bombs here
                // we have guards in edges
                // ...so we don't care about go over array
                bo[r][c] = '0' +
                    (bo[r-1][c-1]==BOMB) +
                    (bo[r-1][c  ]==BOMB) +
                    (bo[r-1][c+1]==BOMB) +

                    (bo[r  ][c-1]==BOMB) +
                    (bo[r  ][c+1]==BOMB) +

                    (bo[r+1][c-1]==BOMB) +
                    (bo[r+1][c  ]==BOMB) +
                    (bo[r+1][c+1]==BOMB);
                if (bo[r][c]=='0')
                    bo[r][c]=NTHG;
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

int is_in_board(Board *b, int row, int col){
    return row>0 && col>0 && row<=b->size && col<=b->size;
}

// returns:
//  1 - cell is ready to discover
// -1 - cell is outside the board
// -2 - cell is already discovered
// -3 - cell is flagged
// -4 - strange value of mask[row][col]
int is_move_ok(Board *b, int row, int col){
    if (! is_in_board(b,row,col))
        return ERR_NOT_IN_BOARD;
    switch (b->mask[row][col]){
        case 0:     return  1;
        case 1:     return ERR_ALREADY_DISCOVERED;
        case FLAG:  return ERR_FLAGGED;
        default:    return ERR_STRANGE_MASK;
    }
}

// returns:
//  1 - cell is ready to be unflagged (is   flagged)
//  2 - cell is ready to be   flagged (is unflagged and undiscovered)
// -1 - cell is not in the board
// -2 - cell is already discovered
// -4 - strange value of mask[row][col]
int is_flagged(Board *b, int row, int col){
    if (! is_in_board(b,row,col))
        return ERR_NOT_IN_BOARD;
    switch (b->mask[row][col]){
        case FLAG:  return  1;
        case 0:     return  2;
        case 1:     return ERR_ALREADY_DISCOVERED;
        default:    return ERR_STRANGE_MASK;
    }
}


///////////////////////////
// DISCOVERING AND FLAGGING
///////////////////////////

int discover_cell(Board *b, int row, int col);

int discover_empty_area(Board *b, int row, int col){
    if (b->b[row][col]==NTHG){
        // there is no bomb around
        discover_cell(b,row-1,col-1);
        discover_cell(b,row-1,col  );
        discover_cell(b,row-1,col+1);
        
        discover_cell(b,row  ,col-1);
        discover_cell(b,row  ,col+1);
        
        discover_cell(b,row+1,col-1);
        discover_cell(b,row+1,col  );
        discover_cell(b,row+1,col+1);
        return 0;
    }
    return -1;
}

int discover_cell(Board *b, int row, int col){
    if (is_move_ok(b, row, col)==1){
        b->mask[row][col]=1;
        if (b->b[row][col]!=BOMB)
            b->remaining_moves--;
        discover_empty_area(b,row,col);
        return b->b[row][col];
    } else
        return -1;
}

int flag_cell(Board *b, int row, int col){
    if (! is_in_board(b, row, col))
        return -1;
    
    switch (b->mask[row][col]){
        case 0:
            b->mask[row][col]=FLAG;
            b->remaining_bombs--;
            return 0;
        case 1:
            return -2;
        case FLAG:
            b->mask[row][col]=0;
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
    int row=0, col=0, flag=0, err=0;

    do {
        if (  (flag = get_move(&row, &col, b->size))  <  0  ){
            continue;
        }

        switch (flag){
            case 0:
                if ( (err=is_move_ok(b,row,col))==1 ){
                    // Discovering...
                    return discover_cell(b,row,col);
                }
                break;
            case FLAG:
                if ((err=is_flagged(b,row,col))>0){
                    // Flagging
                    return flag_cell(b,row,col);
                }
                break;
            default:
                err=ERR_BAD_OPTION;
                break;
        }

        show_errors(err);
    } while (1);
}

