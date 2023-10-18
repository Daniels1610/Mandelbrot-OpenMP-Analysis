// getMandelbrot-SGuided
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NPOINTS 1000
#define MAXITER 1000

int numoutside = 0;
int threads[] = {1,2,3,4,5,6,7,8};
int numThreads = 0;
struct d_complex{
   double r;
   double i;
};
struct get_mandelbrot_values {
    double area;
    double error;
    double run_time;
};
typedef struct d_complex d_complex;
typedef struct get_mandelbrot_values get_mandelbrot_values;

void testpoint(d_complex c);
get_mandelbrot_values get_mandelbrot();

int main(){
  printf("Correct answer should be around 1.510659\n");

  for (int x = 0; x < 8; x++){
    numThreads = threads[x];
    printf("THREAD NUM: %d\n", numThreads);
    for (int times = 0; times < 5; times++){
      get_mandelbrot_values result = get_mandelbrot();
      printf("Mandelbrot Set Area: %2.6f Error: +/- %2.6f Run Time: %lf\n",result.area, result.error, result.run_time);
    }
  }
}

get_mandelbrot_values get_mandelbrot(){
  omp_set_num_threads(numThreads);
  d_complex c; get_mandelbrot_values get_mandelbrot_return_values; 
  double area = 0, error = 0, eps = 1.0e-5; numoutside = 0;
  double start_time = omp_get_wtime();
  
  int j = 0;
  #pragma omp parallel for schedule(guided) private(j) firstprivate(c, eps) 
  for (int i = 0; i < NPOINTS; i++) {
    for (j = 0; j < NPOINTS; j++) {
      c.r = -2.0+2.5*(double)(i)/(double)(NPOINTS)+eps;
      c.i = 1.125*(double)(j)/(double)(NPOINTS)+eps;
      testpoint(c);
    }
  }
  
  area = 2.0*2.5*1.125*(double)(NPOINTS*NPOINTS-numoutside)/(double)(NPOINTS*NPOINTS);

  get_mandelbrot_return_values.area = area;
  get_mandelbrot_return_values.error = area/(double)NPOINTS;
  get_mandelbrot_return_values.run_time = omp_get_wtime() - start_time;
  return get_mandelbrot_return_values;
}

void testpoint(struct d_complex c){
  struct d_complex z;
  double temp;
  z=c;
  for (int iter = 0; iter<MAXITER; iter++){
    temp = (z.r*z.r)-(z.i*z.i)+c.r;
    z.i = z.r*z.i*2+c.i;
    z.r = temp;
    if ((z.r*z.r+z.i*z.i)>4.0) {
      #pragma omp atomic
      numoutside++;
      break;
    }
  }
}