#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

int main() {
    int N;
    printf("Enter the value of N for NxN matrix: ");
    scanf("%d", &N);
    int i, j, k;

    // Allocate memory for matrices
    int **A = (int **)malloc(N * sizeof(int *));
    int **B = (int **)malloc(N * sizeof(int *));
    int **C = (int **)malloc(N * sizeof(int *));
    int (*p2c)[2] = (int (*)[2])malloc(N * sizeof(int[2]));

    for (i = 0; i < N; i++) {
        A[i] = (int *)malloc(N * sizeof(int));
        B[i] = (int *)malloc(N * sizeof(int));
        C[i] = (int *)malloc(N * sizeof(int));
    }

    // Seed for random number generation
    srand(time(0));
    
    // Generate random matrix A
    if (N <= 10) {
        printf("Matrix A:\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                A[i][j] = rand() % 10;
                printf("%d\t", A[i][j]);
            }
            printf("\n");
        }
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                A[i][j] = rand() % 10; // Still generate values but do not print
            }
        }
    }

    // Generate random matrix B
    if (N <= 10) {
        printf("Matrix B:\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                B[i][j] = rand() % 10;
                printf("%d\t", B[i][j]);
            }
            printf("\n");
        }
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                B[i][j] = rand() % 10; // Still generate values but do not print
            }
        }
    }

    // Create pipes
    for (i = 0; i < N; i++) {
        pipe(p2c[i]);
    }

    // Create child processes
    for (i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            int *result = (int *)malloc(N * sizeof(int));
            struct timeval start, end;
            double time_used;
            gettimeofday(&start, NULL);
            for (j = 0; j < N; j++) {
                result[j] = 0;
                for (k = 0; k < N; k++) {
                    result[j] += A[i][k] * B[k][j];
                }
            }
            gettimeofday(&end, NULL);

            time_used = (end.tv_sec - start.tv_sec) * 1e6;
            time_used = (time_used + (end.tv_usec - start.tv_usec)) * 1e-6;

            close(p2c[i][0]); // Close read end
            write(p2c[i][1], result, N * sizeof(int));
            write(p2c[i][1], &time_used, sizeof(time_used));
            close(p2c[i][1]); // Close write end
            free(result);
            return 0; // Exit child process
        }
    }

    // Parent process
    double max_time = 0;
    for (i = 0; i < N; i++) {
        int *result = (int *)malloc(N * sizeof(int));
        double child_time;
        close(p2c[i][1]); // Close write end
        read(p2c[i][0], result, N * sizeof(int));
        read(p2c[i][0], &child_time, sizeof(child_time));
        for (j = 0; j < N; j++) {
            C[i][j] = result[j];
        }
        if (child_time > max_time)
            max_time = child_time;
        close(p2c[i][0]); // Close read end
        free(result);
    }

    // Wait for all child processes to finish
    for (i = 0; i < N; i++) {
        wait(NULL);
    }
    
    struct timeval s, e;
    double time_used;
    gettimeofday(&s, NULL);
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            C[i][j] = 0;
            for (k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    gettimeofday(&e, NULL);
    time_used = (e.tv_sec - s.tv_sec) * 1e6;
    time_used = (time_used + (e.tv_usec - s.tv_usec)) * 1e-6;

    // Convert to milliseconds
    max_time *= 1000;
    time_used *= 1000;

    printf("PARALLEL EXECUTION: %f milliseconds\n", max_time);
    printf("SERIAL EXECUTION: %f milliseconds\n", time_used);
    
    // Print the resultant matrix only if N <= 10
    if (N <= 10) {
        printf("Resultant matrix:\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf("%d\t", C[i][j]);
            }
            printf("\n");
        }
    } else {
        //printf("Resultant matrix is not printed because N is greater than 10.\n");
    }

    // Free allocated memory
    for (i = 0; i < N; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
    free(p2c);

    return 0;
}
/*
Enter the value of N for NxN matrix: 3
Matrix A:
4	6	3	
3	2	8	
2	0	6	
Matrix B:
0	4	2	
2	1	6	
4	9	8	
PARALLEL EXECUTION: 0.001000 milliseconds
SERIAL EXECUTION: 0.001000 milliseconds
Resultant matrix:
24	49	68	
36	86	82	
24	62	52	

Enter the value of N for NxN matrix: 11
PARALLEL EXECUTION: 0.002000 milliseconds
SERIAL EXECUTION: 0.022000 milliseconds
[23bcs025@mepcolinux HPC]$./a.out
Enter the value of N for NxN matrix: 50
PARALLEL EXECUTION: 0.040000 milliseconds
SERIAL EXECUTION: 1.377000 milliseconds
[23bcs025@mepcolinux HPC]$./a.out
Enter the value of N for NxN matrix: 100
PARALLEL EXECUTION: 2.280000 milliseconds
SERIAL EXECUTION: 15.922000 milliseconds
[23bcs025@mepcolinux HPC]$./a.out
Enter the value of N for NxN matrix: 1000


PARALLEL EXECUTION: 53.289000 milliseconds
SERIAL EXECUTION: 21571.955000 milliseconds
[23bcs025@mepcolinux HPC]$
[23bcs025@mepcolinux HPC]$

*/
