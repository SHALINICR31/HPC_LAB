#include<stdio.h>
#include<omp.h>

double f(double x){
   return x*x;
}

int main(){
   int n, i, thread_count;
   double a, b, h;
   double sum, temp_sum = 0.0;

   printf("Enter Lower Limit(a):\n");
   scanf("%lf",&a);

   printf("\nEnter Upper Limit(b):\n");
   scanf("%lf",&b);

   printf("\nEnter number of Trapezoids:\n");
   scanf("%d",&n);

   printf("\nEnter Thread Count:\n");
   scanf("%d",&thread_count);

   h = (b - a)/n;
   printf("\nStep 1: h = (b-a)/n = (%lf - %lf)/%d = %lf\n", b, a, n, h);

   sum = (f(a) + f(b)) / 2.0;
   printf("\nStep 2: (f(a)+f(b))/2 = (%lf + %lf)/2 = %lf\n", f(a), f(b), sum);

   printf("\nStep 3: Calculating f(xi) values:\n");

   double start = omp_get_wtime();

#pragma omp parallel for num_threads(thread_count) reduction(+:temp_sum)
   for(i = 1; i < n; i++){
      double x = a + i * h;
      double fx = f(x);

#pragma omp critical
      {
         printf("i=%d, x=%lf, f(x)=%lf\n", i, x, fx);
      }

      temp_sum += fx;
   }

   printf("\nStep 4: Sum of intermediate values = %lf\n", temp_sum);

   sum += temp_sum;
   printf("\nStep 5: Total sum = %lf\n", sum);


   double integral = sum * h;
   printf("\nStep 6: Integral = sum * h = %lf * %lf = %lf\n", sum, h, integral);

   double end = omp_get_wtime();

   printf("\nEstimated Integral = %lf\n", integral);
   printf("Execution time : %lf\n",end-start);

   return 0;
}
/*
[23bcs025@mepcolinux ex9]$ex9op.c
bash: ex9op.c: command not found

Enter Lower Limit(a):
0

Enter Upper Limit(b):
5

Enter number of Trapezoids:
5

Enter Thread Count:
5

Step 1: h = (b-a)/n = (5.000000 - 0.000000)/5 = 1.000000

Step 2: (f(a)+f(b))/2 = (0.000000 + 25.000000)/2 = 12.500000

Step 3: Calculating f(xi) values:
i=4, x=4.000000, f(x)=16.000000
i=3, x=3.000000, f(x)=9.000000
i=1, x=1.000000, f(x)=1.000000
i=2, x=2.000000, f(x)=4.000000

Step 4: Sum of intermediate values = 30.000000

Step 5: Total sum = 42.500000

Step 6: Integral = sum * h = 42.500000 * 1.000000 = 42.500000

Estimated Integral = 42.500000
Execution time : 0.022013

[23bcs025@mepcolinux ex9]$exit
exit

*/