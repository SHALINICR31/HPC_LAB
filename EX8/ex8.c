#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int n, i;
    
    printf("Enter number of elements: ");
    if (scanf("%d", &n) != 1) return 1;

    int *a = (int*)malloc(n * sizeof(int));
    int *b = (int*)malloc(n * sizeof(int));
    int *c = (int*)malloc(n * sizeof(int));

    printf("\n=== Array A ===\n");
    for (i = 0; i < n; i++) {
        a[i] = rand() % 10;
        printf("%d\t", a[i]);
    }

    printf("\n\n=== Array B ===\n");
    for (i = 0; i < n; i++) {
        b[i] = rand() % 10;
        printf("%d\t", b[i]);
    }

    printf("\n\n=== Parallel Computation ===");
    printf("\nMax threads available: %d\n", omp_get_max_threads());

    // Start timing
    double start_time = omp_get_wtime();

    #pragma omp parallel for 
    for (i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
       }
    for(i=0;i<10;i++){
	    printf("Thread %d: c[%d] = %d + %d = %d\n",
               omp_get_thread_num(), i, a[i], b[i], c[i]);
    }


    // End timing
    double end_time = omp_get_wtime();
    double execution_time = end_time - start_time;

    printf("\n=== Resulting Array C (Sum) ===\n");
    for (i = 0; i < 10; i++) {
        printf("%d ", c[i]);
    }

    printf("\n\n=== Execution Time ===\n");
    printf("Parallel section took: %.6f seconds\n", execution_time);

    free(a);
    free(b);
    free(c);
    return 0;
}
/*
III-A2@cil01:~/ex8$ ./a
Enter number of elements: 100

=== Array A ===
3	6	7	5	3	5	6	2	9	1	27	0	9	3	6	0	6	2	6	1	87	9	2	02	3	75	9	2	2	89	7	3	6	1	2	9	3	1	9	47	8	4	5	0	3	61	0	6	3	20	61	5	5	4	7	6	5	6	93	7	4	5	2	5	4	7	4	43	07	8	6	8	8	4	3	14	9	20	6	8	9	2	6	6	4	9	

=== Array B ===
5	0	4	8	7	1	7	2	7	2	26	1	0	6	1	5	9	4	9	0	91	7	7	11	5	97	7	6	7	36	5	6	3	9	4	8	1	2	9	39	0	8	8	5	0	96	3	8	5	61	15	9	8	4	8	1	0	3	04	4	4	4	7	6	3	1	7	59	62	1	7	8	5	7	4	18	5	97	5	3	8	8	3	1	8	9	

=== Parallel Computation ===
Max threads available: 6
Thread 0: c[0] = 3 + 5 = 8
Thread 0: c[1] = 6 + 0 = 6
Thread 0: c[2] = 7 + 4 = 11
Thread 0: c[3] = 5 + 8 = 13
Thread 0: c[4] = 3 + 7 = 10
Thread 0: c[5] = 5 + 1 = 6
Thread 0: c[6] = 6 + 7 = 13
Thread 0: c[7] = 2 + 2 = 4
Thread 0: c[8] = 9 + 7 = 16
Thread 0: c[9] = 1 + 2 = 3

=== Resulting Array C (Sum) ===
8 6 11 13 10 6 13 4 16 3 

=== Execution Time ===
Parallel section took: 0.005897 seconds
*/

