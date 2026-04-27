#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main()
{
    int i, j, rows, cols;
    int fd[2];
    double serial_time, parent_time, child_time, parallel_time;
    struct timespec s1, s2, p1, p2;

    printf("Enter number of rows: ");
    scanf("%d", &rows);

    printf("Enter number of columns: ");
    scanf("%d", &cols);

    int A[rows][cols], B[rows][cols];
    int C[rows][cols], D[rows][cols];

    srand(time(NULL));

    /* -------- GENERATE MATRICES -------- */
    if (rows <= 10 && cols <= 10) {
        printf("\nMatrix A:\n");
        for(i = 0; i < rows; i++)
        {
            for(j = 0; j < cols; j++)
            {
                A[i][j] = rand() % 10;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }

        printf("\nMatrix B:\n");
        for(i = 0; i < rows; i++)
        {
            for(j = 0; j < cols; j++)
            {
                B[i][j] = rand() % 10;
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
    } else {
        for(i = 0; i < rows; i++)
        {
            for(j = 0; j < cols; j++)
            {
                A[i][j] = rand() % 10; // Generate values but do not print
            }
        }
        for(i = 0; i < rows; i++)
        {
            for(j = 0; j < cols; j++)
            {
                B[i][j] = rand() % 10; // Generate values but do not print
            }
        }
    }

    /* -------- SERIAL EXECUTION -------- */
    clock_gettime(CLOCK_MONOTONIC, &s1);

    for(i = 0; i < rows; i++)
        for(j = 0; j < cols; j++)
            C[i][j] = A[i][j] + B[i][j];

    for(i = 0; i < rows; i++)
        for(j = 0; j < cols; j++)
            D[i][j] = A[i][j] - B[i][j];

    clock_gettime(CLOCK_MONOTONIC, &s2);

    serial_time =
        (s2.tv_sec - s1.tv_sec) * 1000.0 +
        (s2.tv_nsec - s1.tv_nsec) / 1e6;

    if (rows <= 10 && cols <= 10) {
        printf("\nSerial Addition Result (C = A + B):\n");
        for(i = 0; i < rows; i++)
        {
            for(j = 0; j < cols; j++)
                printf("%d ", C[i][j]);
            printf("\n");
        }

        printf("\nSerial Subtraction Result (D = A - B):\n");
        for(i = 0; i < rows; i++)
        {
            for(j = 0; j < cols; j++)
                printf("%d ", D[i][j]);
            printf("\n");
        }
    }

    /* -------- PARALLEL EXECUTION -------- */
    pipe(fd);
    pid_t pid = fork();

    if(pid == 0)   /* CHILD → SUBTRACTION */
    {
        close(fd[0]);

        clock_gettime(CLOCK_MONOTONIC, &p1);

        for(i = 0; i < rows; i++)
            for(j = 0; j < cols; j++)
                D[i][j] = A[i][j] - B[i][j];

        clock_gettime(CLOCK_MONOTONIC, &p2);

        child_time =
            (p2.tv_sec - p1.tv_sec) * 1000.0 +
            (p2.tv_nsec - p1.tv_nsec) / 1e6;

        write(fd[1], &child_time, sizeof(double));
        close(fd[1]);

        if (rows <= 10 && cols <= 10) {
            printf("\nParallel Subtraction Result (Child):\n");
            for(i = 0; i < rows; i++)
            {
                for(j = 0; j < cols; j++)
                    printf("%d ", D[i][j]);
                printf("\n");
            }
        }
        return 0;
    }
    else          /* PARENT → ADDITION */
    {
        close(fd[1]);

        clock_gettime(CLOCK_MONOTONIC, &p1);

        for(i = 0; i < rows; i++)
            for(j = 0; j < cols; j++)
                C[i][j] = A[i][j] + B[i][j];

        clock_gettime(CLOCK_MONOTONIC, &p2);

        parent_time =
            (p2.tv_sec - p1.tv_sec) * 1000.0 +
            (p2.tv_nsec - p1.tv_nsec) / 1e6;

        read(fd[0], &child_time, sizeof(double));
        close(fd[0]);
        wait(NULL);

        if (rows <= 10 && cols <= 10) {
            printf("\nParallel Addition Result (Parent):\n");
            for(i = 0; i < rows; i++)
            {
                for(j = 0; j < cols; j++)
                    printf("%d ", C[i][j]);
                printf("\n");
            }
        }

        parallel_time = (parent_time > child_time)
                        ? parent_time : child_time;
    }

    printf("\nSerial Execution Time   : %.4f ms", serial_time);
    printf("\nParallel Execution Time : %.4f ms\n", parallel_time);

    return 0;
}
/*
[23bcs025@mepcolinux HPC]$cc e1b.c
[23bcs025@mepcolinux HPC]$cc e1b.

Enter number of rows: 3
Enter number of columns: 3

Matrix A:
5 3 7 
1 5 6 
2 9 8 

Matrix B:
8 5 6 
7 2 5 
3 6 2 

Serial Addition Result (C = A + B):
13 8 13 
8 7 11 
5 15 10 

Serial Subtraction Result (D = A - B):
-3 -2 1 
-6 3 1 
-1 3 6 

Parallel Subtraction Result (Child):
-3 -2 1 
-6 3 1 
-1 3 6 

Parallel Addition Result (Parent):
13 8 13 
8 7 11 
5 15 10 

Serial Execution Time   : 0.0005 ms
Parallel Execution Time : 0.0007 ms
[23bcs025@mepcolinux HPC]$./a.out
Enter number of rows: 50
Enter number of columns: 50

Serial Execution Time   : 0.0533 ms
Parallel Execution Time : 0.0375 ms
[23bcs025@mepcolinux HPC]$./a.out
Enter number of rows: 100
Enter number of columns: 100

Serial Execution Time   : 0.2971 ms
Parallel Execution Time : 0.1654 ms
[23bcs025@mepcolinux HPC]$exit
exit

*/