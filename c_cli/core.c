#include "defs.h"
#include "io.h"
#include "solver.h"
#include "rearrange.h"

///////////
// COUNTERS
///////////

// for each not-BOMB-cell x:
//     count the amount of bombs around x
//     insert it to x
//
// it looks clearer without pointers
void count_bombs_around_all(char **b, int size){
    for (int r=1; r<=size; ++r){
        for (int c=1; c<=size; ++c){
            if (b[r][c]!=BOMB){
                // empty, so we have to count bombs here
                // we have guards in edges
                // ...so we don't care about go over array
                b[r][c] = '0' +
                    (b[r-1][c-1]==BOMB) +
                    (b[r-1][c  ]==BOMB) +
                    (b[r-1][c+1]==BOMB) +
                    (b[r  ][c-1]==BOMB) +
                    (b[r  ][c+1]==BOMB) +
                    (b[r+1][c-1]==BOMB) +
                    (b[r+1][c  ]==BOMB) +
                    (b[r+1][c+1]==BOMB);
                if (b[r][c]=='0')
                    b[r][c]=NTHG;
            }
        }
    }
}

// count all chars ch in b
// and return it
int count_all_ch(char **b, int size, char ch){
    int cnt=0;
    for (char **r=b+1; *((*r)+1)!=EDGE; ++r){
        for (char *c=(*r)+1; *c!=EDGE; ++c){
            if (*c==ch)
                ++cnt;
        }
    }
    return cnt;
}

int count_ch_around(char **b, int r, int c, char ch){
    return (b[r-1][c-1]==ch) + (b[r-1][c]==ch) + (b[r-1][c+1]==ch) +
           (b[r  ][c-1]==ch) +                   (b[r  ][c+1]==ch) +
           (b[r+1][c-1]==ch) + (b[r+1][c]==ch) + (b[r+1][c+1]==ch);
}


////////////////////////
// CREATING AND DELETING
////////////////////////

// returns array len x len
// EDGE    on the edges
// def_val in the middle
char ** make_arr2x2(int len, char def_val){
    char** board = (char**)malloc(len*sizeof(char*));

    // row 0 and len-1:
    *board         = (char*)malloc(len*sizeof(char));
    *(board+len-1) = (char*)malloc(len*sizeof(char));
    for (char *c1=*board, *c2=*(board+len-1);
            c1<*board+len; ++c1,++c2)
        *c1 = *c2 = EDGE;

    // row 1 ... len-2:
    for (char **r=board+1; r<board+len-1; ++r){
        *r = (char*)malloc(len*sizeof(char));
        **r = *(*r+len-1) = EDGE;
        for (char *c=*r+1; c<*r+len-1; ++c)
            *c = def_val;
    }

    return board;
}

void plant_bombs(Board *b){
    int planted=0,  r=0,c=0;
    srand(time(NULL));

    while (planted < b->all_bombs){
        // insert a bomb to free cell in [1..size][1..size]
        r = rand() % b->size + 1;
        c = rand() % b->size + 1;
        if (b->b[r][c]!=BOMB){
            b->b[r][c]=BOMB;
            planted++;
        }
    }
}

Board * make_board(int size){
    Board *b=(Board*) malloc(sizeof(Board));
    // arrays will have length: (size+2)x(size+2)
    b->b    = make_arr2x2(size+2,UNKN);
    b->mask = make_arr2x2(size+2,MASK_HIDDEN);
    b->size = size;
    b->all_bombs = NBOMBS;
    b->remaining_bombs = b->all_bombs;
    b->remaining_moves = size*size - b->all_bombs;
    plant_bombs(b);
    count_bombs_around_all(b->b, b->size);
    return b;
}

// del array len x len
void del_char_arr(char **b, int len){
    for (char **p=b;  p < b+len;  free(*p++))
        ;
    free(b);
}

void del_board(Board *b){
    del_char_arr(b->b,    b->size+2);
    del_char_arr(b->mask, b->size+2);
    free(b);
}


///////////
// CHECKING
///////////

int is_in_board(Board *b, int r, int c){
    return r>0 && c>0 && r<=b->size && c<=b->size;
}

// returns:
//  1 - cell is ready to discover
// -1 - cell is outside the board
// -2 - cell is already discovered
// -3 - cell is flagged
// -4 - strange value of mask[row][col]
int is_move_ok(Board *b, int r, int c){
    if (! is_in_board(b,r,c))
        return ERR_NOT_IN_BOARD;
    switch (b->mask[r][c]){
        case MASK_HIDDEN:  return 1;
        case MASK_VISIBLE: return ERR_ALREADY_DISCOVERED;
        case FLAG:         return ERR_FLAGGED;
        default:           return ERR_STRANGE_MASK;
    }
}

// returns:
//  1 - cell is ready to be unflagged (is   flagged)
//  2 - cell is ready to be   flagged (is unflagged and undiscovered)
// -1 - cell is not in the board
// -2 - cell is already discovered
// -4 - strange value of mask[row][col]
int is_flagged(Board *b, int r, int c){
    if (! is_in_board(b,r,c))
        return ERR_NOT_IN_BOARD;
    switch (b->mask[r][c]){
        case FLAG:         return 1;
        case MASK_HIDDEN:  return 2;
        case MASK_VISIBLE: return ERR_ALREADY_DISCOVERED;
        default:           return ERR_STRANGE_MASK;
    }
}

// returns:
// 0 if exists cell x with number n
//      and there are more than n BOMBs around x
//   or a cell have strange value
//   or there are too much bombs
// 1 if numbers in cells are correct or too high
int is_sb_partial_ok(char **sb, int size, int all_bombs){
    int bombs=0,bombs_ar;
    for (int r=1; r<=size; ++r){
        for (int c=1; c<=size; ++c){
            switch (sb[r][c]){
                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8':
                    if ((bombs_ar=count_ch_around(sb,r,c,BOMB)) >
                            (sb[r][c]-'0')){
                        // too much bombs
                        /*printf("Error: is_sb_partial_ok: sb[%d][%d]=%c\t"
                                "bombs_around: %d\t"
                                "too much bombs\n",
                                r,c,sb[r][c], bombs_ar);*/
                        return 0;
                    } else if ((bombs_ar+count_ch_around(sb,r,c,UNKN)) <
                            (sb[r][c]-'0')){
                        // no way to have enough bombs
                        /*printf("Error: is_sb_partial_ok: sb[%d][%d]=%c\t"
                                "bombs_around: %d\t"
                                "no way to have enough bombs\n",
                                r,c,sb[r][c], bombs_ar);*/
                        return 0;
                    }
                    break;
                case EDGE:
                    //printf("Error: is_sb_partial_ok: EDGE\n\n");
                    return 0;
                case UNKN:
                    break;
                case NTHG:
                    if (count_ch_around(sb,r,c,BOMB)!=0)
                        return 0;
                    break;
                case BOMB:
                    ++bombs;
                    break;
                case TODI:
                    break;
                default:
                    break;
            }
        }
    }
    if (bombs<=all_bombs){
        //printf("\nis_sb_partial_ok: OK\n");
        return 1;
    }
    //printf("Error: is_sb_ok: NO, too much bombs: %d / %d\n",
    //        bombs, all_bombs);
    return 0;
}


// returns 1 if sb is correct and complete saper array
int is_sb_ok(char **sb, int size, int all_bombs){
    int bombs=0,bomb_ar;
    for (int r=1; r<=size; ++r){
        for (int c=1; c<=size; ++c){
            switch (sb[r][c]){
                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8':
                    if ((bomb_ar=count_ch_around(sb,r,c,BOMB)) !=
                            (sb[r][c]-'0')){
                        //printf("Error: is_sb_ok: sb[%d][%d]=%c\n"
                        //        "bombs_around: %d",
                        //        r,c,sb[r][c],bomb_ar);
                        return 0;
                    }
                    break;
                case EDGE:
                    //printf("Error: is_sb_ok: EDGE\n\n");
                    return 0;
                case UNKN:
                    //printf("Error: is_sb_ok: UNKN\n\n");
                    return 0;
                case NTHG:
                    if (count_ch_around(sb,r,c,BOMB)!=0)
                        return 0;
                    break;
                case FLAG:
                    //printf("Error: is_sb_ok: FLAG\n\n");
                    return 0;
                case BOMB:
                    ++bombs;
                    break;
                case TODI:
                    //printf("Error: is_sb_ok: TODI\n\n");
                    return 0;
                default:
                    //printf("Error: is_sb_ok: %c\n\n",sb[r][c]);
                    return 0;
            }
        }
    }
    if (bombs==all_bombs){
        //printf("\nis_sb_ok: OK\n\n");
        return 1;
    }
    //printf("Error: is_sb_ok: too few bombs: %d / %d\n\n",
    //        bombs, all_bombs);
    return 0;
}

int is_around_discovered(char **b, int r, int c){
    if (isdigit(b[r-1][c-1])) return 1;
    if (isdigit(b[r-1][c  ])) return 1;
    if (isdigit(b[r-1][c+1])) return 1;
    if (isdigit(b[r  ][c-1])) return 1;
    if (isdigit(b[r  ][c+1])) return 1;
    if (isdigit(b[r+1][c-1])) return 1;
    if (isdigit(b[r+1][c  ])) return 1;
    if (isdigit(b[r+1][c+1])) return 1;
    return 0;
}


////////////
// SALVATION
////////////

// if a user choose to discover cell with bomb,
//  try to imperceptibly change location of bombs
//
// returns:
// 0 - if the bomb have to be in [row][col]
// 1 - when:
//      - changed bomb(s) position in b->b
//      - there is no bomb in [row][col]
//      - b->b is correct and complete
int salvation(Board *b, int r, int c){
    //printf("\n>>>> S A L V A T I O N\n");


    // 1: try to solve it
    char **sb = basic_solver(b);


    // 2: fail if in [r][c] have to be bomb
    if (sb[r][c]==BOMB){
        //printf( "Salvation: "
        //        "basic solver told: there is a bomb.\n\n");
        //show_b(sb,b->size);
        //printf("\n<<<< S A L V A T I O N  0\n");
        return 0;
    }


    // 3: try to rearrange (find new locations of bombs)
    //printf( "Salvation: "
    //        "basic solver don't know. Trying to rearrange.\n\n");
    if (rearrange(sb, r,c, b->size, b->all_bombs)){
        //printf( "Salvation: "
        //        "rearrange successfull, copying data...\n\n");
        del_char_arr(b->b, b->size+2);
        b->b=sb;
        //printf("\n<<<< S A L V A T I O N  1\n");
        return 1;
    }


    // 4: fail if you can't find proper locations
    //printf( "Salvation: "
    //        "rearranging failed to plant bombs properly\n");
    //printf("\n<<<< S A L V A T I O N  0\n");
    return 0;
}



///////////////////////////
// DISCOVERING AND FLAGGING
///////////////////////////

int discover_cell(Board *b, int r, int c);

int discover_empty_area(Board *b, int r, int c){
    if (b->b[r][c]==NTHG){
        // there is no bomb around
        discover_cell(b,r-1,c-1);
        discover_cell(b,r-1,c  );
        discover_cell(b,r-1,c+1);
        discover_cell(b,r  ,c-1);
        discover_cell(b,r  ,c+1);
        discover_cell(b,r+1,c-1);
        discover_cell(b,r+1,c  );
        discover_cell(b,r+1,c+1);
        return 0;
    }
    return -1;
}


int discover_cell(Board *b, int r, int c){
    if (is_move_ok(b, r, c)==1){
        if (b->b[r][c]==BOMB){
            if (salvation(b, r, c)){
                //printf("\nsalvation successfull\n\n");
                b->remaining_moves--;
            } else {
                ;//printf("\nsalvation unsuccessfull\n\n");
            }
        } else
            b->remaining_moves--;
        b->mask[r][c]=MASK_VISIBLE;
        discover_empty_area(b,r,c);
        return b->b[r][c];
    } else
        return -1;
}

int flag_cell(Board *b, int r, int c){
    if (! is_in_board(b, r, c))
        return ERR_NOT_IN_BOARD;
    
    switch (b->mask[r][c]){
        case MASK_HIDDEN:
            b->mask[r][c]=FLAG;
            b->remaining_bombs--;
            return 0;
        case MASK_VISIBLE:
            return ERR_ALREADY_DISCOVERED;
        case FLAG:
            b->mask[r][c]=MASK_HIDDEN;
            b->remaining_bombs++;
            return 0;
        default:
            return ERR_STRANGE_MASK;
    }
}


///////
// MOVE
///////
int move(Board *b){
    int r=0, c=0, flag=0, err=0;

    do {
        if ( (flag = get_move(&r,&c,b->size))  >=  0 ){
            switch (flag){
                case 0:
                    if ( (err=is_move_ok(b,r,c))==1 ){
                        // discovering...
                        return discover_cell(b,r,c);
                    }
                    break;
                case FLAG:
                    if ((err=is_flagged(b,r,c))>0){
                        // (un)flagging
                        return flag_cell(b,r,c);
                    }
                    break;
                default:
                    err=ERR_BAD_OPTION;
                    break;
            }
        }

        show_errors(err);
    } while (1);
}

