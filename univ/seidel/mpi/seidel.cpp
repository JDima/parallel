#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ROOT 0

void printx(double *x, int n)
{
  for(int i = 0; i < n; i++)
    printf("%2.2f, ", x[i]);
  printf("\n");
}


double converge(double *xk, double *xkp, int n)
{
  double norm = 0.0;
  for (int i = 0; i < n; i++) {
    norm += (xk[i] - xkp[i]) * (xk[i] - xkp[i]);
  }
  return sqrt(norm);
}

int main(int argc, char *argv[]) {
  int n, nstrip, diagIndex;
  int rank, nodes, rowIndex;
  double start_time, end_time, tick;
  double *A, *a, *B, *b, *tmp;
  n = atoi(argv[1]);
  double *xx = (double*)calloc(n, sizeof(double));
  double *p = (double*)calloc(n, sizeof(double));
  double temp, norm;
  int i = 0, j = 0, iter = 0;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nodes);

  nstrip = n/nodes;
  a = (double*)calloc(n * nstrip, sizeof(double));
  b = (double*)calloc(nstrip, sizeof(double));
  if(rank == ROOT){
    // printf("%i\n", rank);
    // printf("%i\n", nstrip);
    A = (double*)calloc(n * n, sizeof(double));
    B = (double*)calloc(n, sizeof(double));
    for(int i = 0; i < n; i++){
        B[i] = n * sqrt(n) + n;
        rowIndex = i * n;
        for(int j = 0; j < n; j++)
            A[rowIndex + j] = 1.0 + n * sqrt(n) * (i==j);
    }
  }
  double *x = (double*)calloc(nstrip, sizeof(double));
  MPI_Scatter(A, n*nstrip, MPI_DOUBLE, a, n*nstrip, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
  MPI_Scatter(B, nstrip, MPI_DOUBLE, b, nstrip, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
  start_time = MPI_Wtime();
  do
  {
    for (i = 0; i < n; i++)
      p[i] = xx[i];
    for (i = 0; i < nstrip; i++)
    {
      temp = 0.0;
      rowIndex = i * n;
      diagIndex = i+rank*nstrip;
      for (j = 0; j < i; j++)
        if(diagIndex != j)
          temp += (a[rowIndex + j] * x[j]);
      for (j = i + 1; j < n; j++)
        if(diagIndex != j)
          temp += (a[rowIndex + j] * p[j]);
      x[i] = (b[i] - temp) / a[rowIndex + diagIndex];
      iter++;
    }
    MPI_Gather(x, nstrip, MPI_DOUBLE, xx, nstrip, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    if(rank == ROOT){
      norm = converge(xx, p, n);
      // printf("%f\n", norm);
    }
    MPI_Bcast(&norm, 1, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(xx, n, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
  } while (norm >= 1e-5);
  // double* res = seidel(A, b, n, nstrip, rank);
  end_time = MPI_Wtime();
  MPI_Finalize();
  if(rank == ROOT){
    // printx(xx, n);
    printf("Running time: %f sec\n", end_time - start_time);
  }
  return 0;
}
