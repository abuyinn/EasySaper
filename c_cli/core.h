#ifndef COREH
#define COREH


// for main
Board * make_board(int size);
void    del_board(Board *b);
int     move(Board *b);

// for solver
int     count_ch_around(char **sb, int r, int c, char ch);
char ** make_arr2x2(int len, char def_val);

// for rearrange
int     is_sb_partial_ok(char **sb, int size, int all_bombs);
int     is_sb_ok(char **sb, int size, int all_bombs);
int     is_around_discovered(char **b, int r, int c);
void    count_bombs_around_all(char **bo, int size);
int     count_all_ch(char **b, int size, char ch);


#endif
