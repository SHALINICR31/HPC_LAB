#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

int main(){
    int n, val;
    printf("Enter number of elements: ");
    scanf("%d", &n);
    printf("Enter value to add: ");
    scanf("%d", &val);

    int* A;
    A = (int*)malloc(n * sizeof(int));

    for(int i = 0; i < n; i++){
        A[i] = rand() % 10;
    }

    printf("\nOriginal Array elements:");
    for(int i = 0; i < n; i++){
        printf("%d\t", A[i]);
    }

    printf("\n\n=== Parallel Processing ===");
    printf("\nTotal threads used: %d\n", omp_get_max_threads());

    #pragma omp parallel for 
    for (int i = 0; i < n; i++){
        int original = A[i];
        A[i] += val;
        printf("Thread %d: A[%d] = %d + %d = %d\n", 
               omp_get_thread_num(), i, original, val, A[i]);
    }

    printf("\nArray after adding %d:", val);
    for(int i = 0; i < n; i++){
        printf("%d\t", A[i]);
    }

    free(A);
    return 0;
}

/*

Enter number of elements: 5

=== Array A ===
3	6	7	5	3	

=== Array B ===
5	6	2	9	1	

=== Parallel Computation (4 Threads) ===
Thread 1: c[1] = 6 + 6 = 12
Thread 3: c[3] = 5 + 9 = 14
Thread 2: c[2] = 7 + 2 = 9
Thread 4: c[4] = 3 + 1 = 4
Thread 0: c[0] = 3 + 5 = 8


[23bcs025@mepcolinux ex8]$exit
exit

*/