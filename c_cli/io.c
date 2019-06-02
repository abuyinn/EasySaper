#include "defs.h"
#include "io.h"

#define MAXLEN 100  /* maximum input length */

void legend(){
    printf( "A legend:\n"
            "\t%c - Unknown, you can discover it.\n"
            "\t%c - Nothing, 0 bombs.\n"
            "\t%c - Your flag.\n"
            "\t%c - A bomb.\n\n",UNKN,NTHG,FLAG,BOMB);
}

void about_moves(){
    printf( "Moves:\n"
            "\tTo discover cell, type its row, space, and cell,\n"
            "\t\tfor example: 1 3\n"
            "\tTo flag/unflag cell, type %c and then"
                "its row, space and cell,\n"
            "\t\tfor example: %c 1 2\n\n"
            ,FLAG,FLAG);
}

void welcome(){
    printf("Welcome in EasySaper game.\n");
    printf("Version: Alpha\n\n");
    legend();
    about_moves();
}

void show_board(Board *b){
    // first line
    printf("\n    ");
    for (int i=1; i<=b->size; ++i)
        printf("%5d%s",i,(i==b->size)?"\n\n\n":"");

    // next lines
    int size1=1;
    for (char **p=b->b+1, **m=b->mask+1;
            size1<=b->size;
                ++p,++m,++size1){
        printf("%3d   ", size1);
        for (char *pp=(*p)+1, *mm=(*m)+1;
                *pp != EDGE; ++pp,++mm){
            printf("  %c  ", (*mm==1) ? *pp :
                    ((*mm==FLAG)?FLAG:UNKN)  );
        }
        printf("\n\n");
    }
    printf("Bombs:\t%3d\n", b->remaining_bombs);
    printf("Moves:\t%3d\n", b->remaining_moves);
    printf("\n");
}

int get_move(int *row, int *col, int size){
    char str[MAXLEN+1]={'\0'};
    char c=0;

    while(strlen(str)<2){   // while str=='\n'
        printf("Your move (row column): ");
        fgets(str,MAXLEN,stdin);
    }
    if ( (sscanf(str, "%c %d %d",&c,row,col)==3) && isalpha(c) ){
        return c;
    }
    if (sscanf(str,    "%d %d",   row,col) == 2){
        return 0;
    }
    return -1;
}

void show_errors(int err){
    switch (err){
        case 0:
        case 1:
        case 2:
            return;
        case ERR_NOT_IN_BOARD:
            printf("Error: cell is outside the board!\n\n");
            return;
        case ERR_ALREADY_DISCOVERED:
            printf("Error: cell is already discovered!\n\n");
            return;
        case ERR_FLAGGED:
            printf("Error: cell is flagged so you can't discover it now!"
                    "\n\n");
            return;
        case ERR_STRANGE_MASK:
            printf("Error: strange mask (implementation error)!\n\n");
            return;
        case ERR_BAD_OPTION:
            printf("Error: bad option!\n\n");
            return;
        default:
            printf("Error: unhandled error %d!\n\n",err);
            return;
    }
}

void show_result(int remaining_moves){
    if (remaining_moves==0){
        // WIN
        printf("Congratulations! You win!\n\n");
    } else {
        // LOSS
        printf("You're dead.\n\n");
    }
}

// only for debugging
void show_b(char **b, int size){
    printf(">>>>>>show_b>>>>>>>>");
    // first line
    printf("\n    ");
    for (int i=1; i<=size; ++i)
        printf("%5d%s",i,(i==size)?"\n\n":"");

    // next lines
    int size1=1;
    for (char **p=b+1;  size1<=size;  ++p,++size1){
        printf("%3d   ", size1);
        for (char *pp=(*p)+1;  *pp!=EDGE; ++pp){
            printf("  %c  ", *pp  );
        }
        printf("\n");
    }
    printf("<<<<<<show_b<<<<<<<<\n");
}


void show_all_b(char **b, int size){
    printf(">>>>>>show_all_b>>>> size: %d\n",size);
    for (int r=0; r<size; ++r){
        for (int c=0; c<size; ++c){
            printf("%c ",b[r][c]);
        }
        printf("\n");
    }
    printf("<<<<<<show_b<<<<<<<<\n");
}
