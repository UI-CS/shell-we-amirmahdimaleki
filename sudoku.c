// /**
//  * sudoku.c
//  * Parallel Sudoku Validator using Pthreads.
//  * Validates a fixed 9x9 grid using 11 threads.
//  */

// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>

// #define SIZE 9

// // Hardcoded Sudoku Puzzle (Valid Solution)
// int sudoku[SIZE][SIZE] = {
//     {6, 2, 4, 5, 3, 9, 1, 8, 7},
//     {5, 1, 9, 7, 2, 8, 6, 3, 4},
//     {8, 3, 7, 6, 1, 4, 2, 9, 5},
//     {1, 4, 3, 8, 6, 5, 7, 2, 9},
//     {9, 5, 8, 2, 4, 7, 3, 6, 1},
//     {7, 6, 2, 3, 9, 1, 4, 5, 8},
//     {3, 7, 1, 9, 5, 6, 8, 4, 2},
//     {4, 9, 6, 1, 8, 2, 5, 7, 3},
//     {2, 8, 5, 4, 7, 3, 9, 1, 6}
// };

// // Result array for threads
// // Indices: 0 (Rows), 1 (Cols), 2-10 (Subgrids)
// int valid[11] = {0};

// typedef struct {
//     int row;
//     int col;
//     int thread_idx;
// } parameters;

// /**
//  * Helper to check if an array of 9 numbers contains 1-9 exactly once.
//  */
// int check_validity(int *arr) {
//     int check[10] = {0};
//     for (int i = 0; i < 9; i++) {
//         int num = arr[i];
//         if (num < 1 || num > 9 || check[num] == 1) return 0;
//         check[num] = 1;
//     }
//     return 1;
// }

// /**
//  * Thread function to check all rows
//  */
// void *validate_rows(void *param) {
//     parameters *p = (parameters *)param;
//     for (int i = 0; i < SIZE; i++) {
//         int row_data[9];
//         for (int j = 0; j < SIZE; j++) {
//             row_data[j] = sudoku[i][j];
//         }
//         if (!check_validity(row_data)) {
//             valid[p->thread_idx] = 0; // Invalid
//             pthread_exit(NULL);
//         }
//     }
//     valid[p->thread_idx] = 1; // Valid
//     pthread_exit(NULL);
// }

// /**
//  * Thread function to check all columns
//  */
// void *validate_cols(void *param) {
//     parameters *p = (parameters *)param;
//     for (int j = 0; j < SIZE; j++) {
//         int col_data[9];
//         for (int i = 0; i < SIZE; i++) {
//             col_data[i] = sudoku[i][j];
//         }
//         if (!check_validity(col_data)) {
//             valid[p->thread_idx] = 0; // Invalid
//             pthread_exit(NULL);
//         }
//     }
//     valid[p->thread_idx] = 1; // Valid
//     pthread_exit(NULL);
// }

// /**
//  * Thread function to check a specific 3x3 subgrid
//  */
// void *validate_subgrid(void *param) {
//     parameters *p = (parameters *)param;
//     int row_start = p->row;
//     int col_start = p->col;
//     int subgrid_data[9];
//     int count = 0;

//     for (int i = row_start; i < row_start + 3; i++) {
//         for (int j = col_start; j < col_start + 3; j++) {
//             subgrid_data[count++] = sudoku[i][j];
//         }
//     }

//     valid[p->thread_idx] = check_validity(subgrid_data);
//     pthread_exit(NULL);
// }

// int main() {
//     pthread_t threads[11];
//     parameters *thread_params[11];

//     printf("Sudoku Validator: Checking validity...\n");

//     // 1. Thread for Rows
//     thread_params[0] = (parameters *)malloc(sizeof(parameters));
//     thread_params[0]->thread_idx = 0;
//     pthread_create(&threads[0], NULL, validate_rows, (void *)thread_params[0]);

//     // 2. Thread for Columns
//     thread_params[1] = (parameters *)malloc(sizeof(parameters));
//     thread_params[1]->thread_idx = 1;
//     pthread_create(&threads[1], NULL, validate_cols, (void *)thread_params[1]);

//     // 3. Threads for 9 Subgrids
//     int thread_count = 2;
//     for (int i = 0; i < SIZE; i += 3) {
//         for (int j = 0; j < SIZE; j += 3) {
//             thread_params[thread_count] = (parameters *)malloc(sizeof(parameters));
//             thread_params[thread_count]->row = i;
//             thread_params[thread_count]->col = j;
//             thread_params[thread_count]->thread_idx = thread_count;
//             pthread_create(&threads[thread_count], NULL, validate_subgrid, (void *)thread_params[thread_count]);
//             thread_count++;
//         }
//     }

//     // Wait for all threads
//     for (int i = 0; i < 11; i++) {
//         pthread_join(threads[i], NULL);
//         free(thread_params[i]);
//     }

//     // Check results
//     int is_valid = 1;
//     for (int i = 0; i < 11; i++) {
//         if (valid[i] == 0) {
//             is_valid = 0;
//             printf("Validation failed at thread index %d\n", i);
//         }
//     }

//     if (is_valid) {
//         printf("Sudoku Puzzle is VALID.\n");
//     } else {
//         printf("Sudoku Puzzle is INVALID.\n");
//     }

//     return 0;
// }

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