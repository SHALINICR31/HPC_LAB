
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include<time.h>

int main() {
    int rank, size, num, n = 8;
    int *array = NULL;
    int *sub_array;
    int local_n, local_count = 0, total_count = 0;
    double exec;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    clock_t start_time,end_time;
    local_n = n / size;
    sub_array = (int *)malloc(local_n * sizeof(int));

 
    if (rank == 0) {
        array = (int *)malloc(n * sizeof(int));

     
        printf("Array elements: ");
        for (int i = 0; i < n; i++) {
            array[i] = rand() % 10;
            printf("%d ", array[i]);
        }
        printf("\n");

     
        printf("Enter number to find frequency: ");
        fflush(stdout); 
        scanf("%d", &num);
        printf("\n");
    }

    
    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    
    MPI_Scatter(array, local_n, MPI_INT, sub_array, local_n, MPI_INT, 0, MPI_COMM_WORLD);
    
    start_time =clock();
    printf("Process %d received sub-array: ", rank);
    for (int i = 0; i < local_n; i++) {
        printf("%d ", sub_array[i]);
    }
    printf("\n");

    
    for (int i = 0; i < local_n; i++) {
        if (sub_array[i] == num) {
            local_count++;
        }
    }
    end_time=clock();
    exec=((double)(end_time - start_time))/CLOCKS_PER_SEC;
    printf("Process %d  occurrences : %d  found %d exec %lf \n", rank,num, local_count,exec);

    
    MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    
    if (rank == 0) {
        printf("Total frequency of %d is %d\n", num, total_count);
    }

    
    if (rank == 0) {
        free(array);
    }
    free(sub_array);
    
    MPI_Finalize();
    return 0;
}

/*
ada@ada:~$ mpicc coll.c
ada@ada:~$ mpiexec -n 4 ./a.out
Array elements: 3 6 7 5 3 5 6 2 
Enter number to find frequency: 3

Process 0 received sub-array: 3 6 
Process 0  occurrences : 3  found 1 exec 0.000011 
Total frequency of 3 is 2
Process 1 received sub-array: 7 5 
Process 1  occurrences : 3  found 0 exec 0.000016 
Process 2 received sub-array: 3 5 
Process 2  occurrences : 3  found 1 exec 0.000020 
Process 3 received sub-array: 6 2 
Process 3  occurrences : 3  found 0 exec 0.000017 

*/