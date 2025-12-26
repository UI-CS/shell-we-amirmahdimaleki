
//  * sudoku.c
//  * Parallel Sudoku Validator using Pthreads.
//  * Validates a fixed 9x9 grid 
//  */

#include <stdio.h>
#include <pthread.h>

int grid[9][9] = {
    {5,3,4,6,7,8,9,1,2}, {6,7,2,1,9,5,3,4,8}, {1,9,8,3,4,2,5,6,7},
    {8,5,9,7,6,1,4,2,3}, {4,2,6,8,5,3,7,9,1}, {7,1,3,9,2,4,8,5,6},
    {9,6,1,5,3,7,2,8,4}, {2,8,7,4,1,9,6,3,5}, {3,4,5,2,8,6,1,7,9}
};
int valid[11];
typedef struct { int r, c, id; } prm;

void *check_rows(void *p) {
    prm *d = (prm *)p;
    for (int i=0; i<9; i++) {
        int m=0; for (int j=0; j<9; j++) m |= (1 << grid[i][j]);
        if (m != 1022) { valid[d->id]=0; pthread_exit(NULL); }
    }
    valid[d->id]=1; pthread_exit(NULL);
}
void *check_cols(void *p) {
    prm *d = (prm *)p;
    for (int j=0; j<9; j++) {
        int m=0; for (int i=0; i<9; i++) m |= (1 << grid[i][j]);
        if (m != 1022) { valid[d->id]=0; pthread_exit(NULL); }
    }
    valid[d->id]=1; pthread_exit(NULL);
}
void *check_sub(void *p) {
    prm *d = (prm *)p;
    int m=0;
    for (int i=d->r; i<d->r+3; i++) for (int j=d->c; j<d->c+3; j++) m |= (1 << grid[i][j]);
    valid[d->id] = (m == 1022); pthread_exit(NULL);
}

int main() {
    pthread_t t[11]; prm p[11];
    p[0].id=0; pthread_create(&t[0], NULL, check_rows, &p[0]);
    p[1].id=1; pthread_create(&t[1], NULL, check_cols, &p[1]);
    for (int i=0; i<3; i++) for (int j=0; j<3; j++) {
        int idx = 2+i*3+j; p[idx] = (prm){i*3, j*3, idx};
        pthread_create(&t[idx], NULL, check_sub, &p[idx]);
    }
    for (int i=0; i<11; i++) pthread_join(t[i], NULL);
    for (int i=0; i<11; i++) if (!valid[i]) { printf("Invalid!\n"); return 0; }
    printf("Sudoku is Valid!\n"); return 0;
}