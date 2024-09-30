#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 1024

/* Global variables: accessible to all threads */
int thread_count;
int m, n; // Dimensions of the matrix
double **A, *x, *y;

void *Pth_mat_vect(void* rank) {
    long my_rank = (long) rank;
    int i, j;
    int local_m = m / thread_count;
    int my_first_row = my_rank * local_m;
    int my_last_row = (my_rank + 1) * local_m - 1;

    for (i = my_first_row; i <= my_last_row; i++) {
        y[i] = 0.0;
        for (j = 0; j < n; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    long thread;
    pthread_t* thread_handles;
    int i, j;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of threads>\n", argv[0]);
        exit(1);
    }
    thread_count = strtol(argv[1], NULL, 10);
    if (thread_count <= 0 || thread_count > MAX_THREADS) {
        fprintf(stderr, "Number of threads must be between 1 and %d\n", MAX_THREADS);
        exit(1);
    }

    /* Initialize the matrix and vectors */
    m = 4; // Number of rows in the matrix
    n = 4; // Number of columns in the matrix

    A = malloc(m * sizeof(double *));
    for (i = 0; i < m; i++) {
        A[i] = malloc(n * sizeof(double));
    }

    x = malloc(n * sizeof(double));
    y = malloc(m * sizeof(double));

    /* Fill the matrix and vector with some values */
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            A[i][j] = i + j;
        }
    }

    for (i = 0; i < n; i++) {
        x[i] = i;
    }

    thread_handles = malloc(thread_count * sizeof(pthread_t));
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, Pth_mat_vect, (void*) thread);
    }

    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    /* Print the result vector */
    printf("Result vector:\n");
    for (i = 0; i < m; i++) {
        printf("%f ", y[i]);
    }
    printf("\n");

    /* Free allocated memory */
    free(thread_handles);
    free(x);
    free(y);
    for (i = 0; i < m; i++) {
        free(A[i]);
    }
    free(A);

    return 0;
}
