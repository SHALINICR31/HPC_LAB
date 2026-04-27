
#include <string.h>
#include <mpi.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h> 

#define MAX 100

void toUpperCase(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

int isPalindrome(char str[]) {
    int i, j;
    for (i = 0, j = strlen(str) - 1; i < j; i++, j--) {
        if (str[i] != str[j]) {
            return 0;
        }
    }
    return 1;
}

int main(void) {
    char msg[MAX];
    int rank, size;
    char str[5][MAX] = {
        "GOOD MORNING",
        "GOOD EVENING",
        "GOOD NIGHT",
        "GOOD AFTERNOON",
        "BYE SEE U SOON"
    };

   
    clock_t start_time, end_time;
    start_time = clock();

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        MPI_Status status;
        for (int i = 1; i < size; i++) {
            MPI_Recv(msg, MAX, MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == 1) {
                toUpperCase(msg);
                printf("Process %d : Type 1 - Uppercase: %s\n", i, msg);
            } else if (status.MPI_TAG == 2) {
                if (isPalindrome(msg)) {
                    printf("Process %d : Type 2 - Palindrome\n", i);
                } else {
                    printf("Process %d : Type 2 - Not a Palindrome\n", i);
                }
            }
        }
    } else {
        strcpy(msg, str[rank]);    
        if (rank % 2 == 0) {
            MPI_Send(msg, strlen(msg) + 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
        } else {
            MPI_Send(msg, strlen(msg) + 1, MPI_CHAR, 0, 2, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();

   
    end_time = clock();
    double cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    
   
    if (rank != 0) {
        printf("Process %d execution time: %f seconds\n", rank, cpu_time_used);
    }

    return 0;
}
/*
[23bcs025@mepcolinux HPC]$cat mpiop.txt
II-A2@admin:~$ mpicc m.c
III-A2@admin:~$ mpiexec -n 5 ./a.out
Process 1 : Type 2 - Not a Palindrome
Process 2 : Type 1 - Uppercase: GOOD NIGHT
Process 3 : Type 2 - Not a Palindrome
Process 4 : Type 1 - Uppercase: BYE SEE U SOON
Process 1 execution time: 0.027044 seconds
Process 4 execution time: 0.027766 seconds
Process 2 execution time: 0.027408 seconds
Process 3 execution time: 0.027190 seconds
III-A2@admin:~$ ssh 23bcs025@172.16.16.200


[23bcs025@mepcolinux HPC]$exit
exit
*/


