// /**
//  * monte_carlo.c
//  * Parallel Monte Carlo Pi Estimation.
//  * Uses fork() and shared memory (mmap) to aggregate results.
//  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 3) return printf("Usage: %s <procs> <points>\n", argv[0]);
    int nprocs = atoi(argv[1]);
    long total = atol(argv[2]);
    long *shared = mmap(NULL, sizeof(long), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    *shared = 0;

    for (int i = 0; i < nprocs; i++) {
        if (fork() == 0) {
            unsigned int seed = time(NULL) ^ getpid();
            long count = 0;
            for (long j = 0; j < total / nprocs; j++) {
                double x = (double)rand_r(&seed) / RAND_MAX;
                double y = (double)rand_r(&seed) / RAND_MAX;
                if (x*x + y*y <= 1.0) count++;
            }
            __sync_fetch_and_add(shared, count); // Atomic aggregation
            exit(0);
        }
    }
    while (wait(NULL) > 0);
    printf("Pi estimate: %f\n", 4.0 * *shared / total);
    return 0;
}