#include "defs.h"
#include "io.h"
#include "core.h"
#include "solver.h"
#include "rearrange.h"
#include <stdio.h>


// TODO:
// refactoring
// changing to pointers
// tests


typedef struct {
    int r;
    int c;
} Point;


int get_all_unkn_points(char **sb, int size, Point *points){
    int cnt=0;
    for (int r=1; r<=size; ++r){
        for (int c=1; c<=size; ++c){
            if (sb[r][c]==UNKN){
                points->r=r;
                points->c=c;
                ++points;
                ++cnt;
            }
        }
    }
    points->r=-1;
    points->c=-1;
    return cnt;
}

// save to cad
// all UNKN cells located around discovered cell(s)
void get_cad(char **b, int size, Point *cad){
    for (int r=1; r<=size; ++r){
        for (int c=1; c<=size; ++c){
            if (b[r][c]==UNKN && is_around_discovered(b,r,c)){
                cad->r=r;
                cad->c=c;
                ++cad;
            }
        }
    }
    cad->r=-1;
    cad->c=-1;
}

void print_points(Point *p, char *label){
    printf("points %s:\n",label);
    for (; p->r!=-1; ++p)
        printf("%d %d\n",p->r,p->c);
    printf("\n");
}

int recursive_planting(char **sb,int size,Point*p,int bombs,int all_bombs);

int try_plant_a_bomb(char **sb,int size,Point*p,int bombs,int all_bombs){
    sb[p->r][p->c] = BOMB;
    if (is_sb_partial_ok(sb,size,all_bombs)){
        if (bombs+1<all_bombs){
            if (recursive_planting(sb,size,p+1,bombs+1,all_bombs))
                return 1;
        } else if (bombs+1==all_bombs){
            return 1;
        }
    }
    return 0;
}

int try_without_a_bomb(char **sb,int size,Point*p,int bombs,int all_bombs){
    sb[p->r][p->c] = TODI;
    if (is_sb_partial_ok(sb,size,all_bombs)){
        if (recursive_planting(sb,size,p+1,bombs,all_bombs)){
            return 1;
        }
    }
    return 0;
}

// planting bombs in sb
// cad = UNKN cells around discovered cells
int recursive_planting(char **sb,int size,Point*cad,int bombs,int all_bombs){
    // plant bombs in all important cells
    if (cad->r != -1){
        char old_val = sb[cad->r][cad->c];

        if (rand()%2){  /* random whats first */
            if (try_plant_a_bomb  (sb,size,cad,bombs,all_bombs)) return 1;
            if (try_without_a_bomb(sb,size,cad,bombs,all_bombs)) return 1;
        } else {
            if (try_without_a_bomb(sb,size,cad,bombs,all_bombs)) return 1;
            if (try_plant_a_bomb  (sb,size,cad,bombs,all_bombs)) return 1;
        }

        sb[cad->r][cad->c] = old_val;
        return 0;
    }

    // cad are empty (bottom level of recursion)
    // but maybe there are cells not around discovered (cnad) that should have bombs
    Point cnad[size*size];                            // UNKN cells not around discovered
    int num_cnad = get_all_unkn_points(sb,size,cnad); // number of cnad
    bombs = count_all_ch(sb,size,BOMB);

    if (num_cnad==0 && bombs==all_bombs){
        // all done
        return 1;
    }

    if (num_cnad >= all_bombs-bombs){
        // still some bombs to plant
        // random plant bombs
        while (bombs < all_bombs){
            int i = rand() % num_cnad;
            if (cnad[i].r != -1){
                sb[cnad[i].r][cnad[i].c]=BOMB;
                cnad[i].r = -1;
                ++bombs;
            }
        }
        //printf("\nrecursive planting: successfull\n");
        return 1;
    }

    return 0;
}



// returns:
// 0 - rearranging faild
// 1 - rearranging success and:
//      sb is correct and complete board.b
//          with all shown cells the same as previous
//      sb[r][c] != BOMB
int rearrange(char **sb, int r, int c, int size, int all_bombs){

    int bombs=count_all_ch(sb,size,BOMB);

    // 1: if solver found all bombs, return success
    if (bombs == all_bombs){
        //printf("\nRearrange: bombs == all_bombs\n\n");
        count_bombs_around_all(sb,size);
        return 1;
    }

    // 2. if solver found too much bombs, fail
    if (bombs > all_bombs){
        printf( "\nRearrange: error: too much bombs inserted "
                "(implementation error)!\n\n");
        return 0;
    }
    
    // 3: try to plant rest of bombs
    //printf("\nRearrange: bombs < all_bombs\n\n"
    //        "starting board:\n");
    //show_b(sb,size);

    // 4: assume in sb[r][c] is no BOMB...
    sb[r][c]=TODI;
    solve_certain(sb,size);
    //show_b(sb,size);

    Point cad[size*size]; // UNKN cells around discovered cell(s)
    get_cad(sb,size,cad);
    //print_points(cad, "cad");

    // 5: plant bombs that will fit shown numbers to user
    recursive_planting(sb,size,cad,bombs,all_bombs);
    count_bombs_around_all(sb,size);

    // 6: check result and return success or fail
    if (is_sb_ok(sb,size,all_bombs)){
        //printf("\nRearrange: verification: OK\n");
        //show_b(sb,size);
        return 1;
    } else {
        //printf("\nRearrange: verification: BAD\n");
        //show_b(sb,size);
        return 0;
    }
}

