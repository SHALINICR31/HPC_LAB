#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define MAX_NAME 20

typedef struct {
    char name[MAX_NAME];
    int roll;
    int marks;
    char grade;
} Student;

char get_grade(int m) {
    if (m >= 80) return 'A';
    if (m >= 60) return 'B';
    if (m >= 40) return 'C';
    return 'F';
}

int main(int argc, char** argv) {
    int rank, size;
    double local_start, local_end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

   
    local_start = MPI_Wtime();

    
    MPI_Datatype student_type;
    int lengths[4] = {MAX_NAME, 1, 1, 1};
    MPI_Aint offsets[4];
    MPI_Datatype types[4] = {MPI_CHAR, MPI_INT, MPI_INT, MPI_CHAR};

    offsets[0] = offsetof(Student, name);
    offsets[1] = offsetof(Student, roll);
    offsets[2] = offsetof(Student, marks);
    offsets[3] = offsetof(Student, grade);

    MPI_Type_create_struct(4, lengths, offsets, types, &student_type);
    MPI_Type_commit(&student_type);

    Student *all_students = NULL;
    Student local_student;

    if (rank == 0) {
        all_students = (Student*)malloc(size * sizeof(Student));
        FILE *fp = fopen("students.txt", "r");
        if (!fp) {
            fprintf(stderr, "Error: Could not open students.txt\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        printf("Master (Rank 0): Reading student data from file...\n");
        for (int i = 0; i < size; i++) {
            if(fscanf(fp, "%s %d %d", all_students[i].name, &all_students[i].roll, &all_students[i].marks) == EOF) break;
        }
        fclose(fp);
        printf("Master (Rank 0): Scattering data to all processes...\n\n");
    }

    
    MPI_Scatter(all_students, 1, student_type, &local_student, 1, student_type, 0, MPI_COMM_WORLD);
    printf("Process %d: Received data for %s (Roll: %d)\n", rank, local_student.name, local_student.roll);

   
    local_student.grade = get_grade(local_student.marks);
    printf("Process %d: Calculated Grade '%c' for %s\n", rank, local_student.grade, local_student.name);

   
    printf("Process %d: Sending result back to Master...\n", rank);
    MPI_Gather(&local_student, 1, student_type, all_students, 1, student_type, 0, MPI_COMM_WORLD);

    local_end = MPI_Wtime();
    
    
  

    if (rank == 0) {
        printf("\nMaster (Rank 0): All data gathered. Printing final results:\n");
        printf("--------------------------------------------------\n");
        printf("%-15s %-10s %-10s %-5s\n", "NAME", "ROLL", "MARKS", "GRADE");
        printf("--------------------------------------------------\n");
        for (int i = 0; i < size; i++) {
            printf("%-15s %-10d %-10d %-5c\n", 
                   all_students[i].name, all_students[i].roll, all_students[i].marks, all_students[i].grade);
        }
        printf("--------------------------------------------------\n");
        free(all_students);
    }

    
    printf("Process %d: Completed in %f seconds\n", rank, local_end - local_start);

    MPI_Type_free(&student_type);
    MPI_Finalize();
    return 0;
}

/*
III-A2@admin:~$ mpicc mpi4a.c
III-A2@admin:~$ mpiexec -n 4 ./a.out
Master (Rank 0): Reading student data from file...
Master (Rank 0): Scattering data to all processes...

Process 0: Received data for Alice (Roll: 101)
Process 0: Calculated Grade 'A' for Alice
Process 0: Sending result back to Master...
Process 1: Received data for Bob (Roll: 102)
Process 1: Calculated Grade 'B' for Bob
Process 1: Sending result back to Master...
Process 2: Received data for Charlie (Roll: 103)
Process 2: Calculated Grade 'C' for Charlie
Process 2: Sending result back to Master...
Process 3: Received data for David (Roll: 104)
Process 3: Calculated Grade 'A' for David
Process 3: Sending result back to Master...
Process 3: Completed in 0.000257 seconds

Master (Rank 0): All data gathered. Printing final results:
--------------------------------------------------
NAME            ROLL       MARKS      GRADE
--------------------------------------------------
Alice           101        85         A    
Bob             102        72         B    
Charlie         103        45         C    
David           104        92         A    
--------------------------------------------------
Process 0: Completed in 0.000284 seconds
Process 1: Completed in 0.000188 seconds
Process 2: Completed in 0.000261 seconds

[23bcs025@mepcolinux ex4]$cat students.txt
alice 101 85
bob 102 72
charlie 103 45
david 104 92
[23bcs025@mepcolinux ex4]$exit
exit

*/