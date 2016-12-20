#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ROOT 0
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void printx(double *x, int n)
{
  int i;
  for(i = 0; i < n; i++)
    printf("%2.2f, ", x[i]);
  printf("\n");
}


void scatterv(int n, int counts, int rem, double *sendbuf, double *recvbuf)
{
  int i;
  for(i = 0; i < n; i++){
      sendcounts[i] = counts;
      if (rem > 0) {
          sendcounts[i]++;
          rem--;
      }

      displs[i] = sum;
      sum += sendcounts[i];
  }
  MPI_Scatterv(sendbuf, sendcounts, displs, MPI_DOUBLE, recvbuf, counts, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
}

int main(int argc, char *argv[]) {
  int n, nstrip, diagIndex;
  int rank, nodes, rowIndex;
  double start_time, end_time, tick;
  double *A, *a, *B, *b, *tmp, *xx, *x, *p;
  double temp, norm, local_norm = 0;
  int i = 0, j = 0, iter = 0;

  n = atoi(argv[1]);
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nodes);

  nstrip = n / nodes;
  a = (double*)calloc(n * nstrip, sizeof(double));
  b = (double*)calloc(nstrip, sizeof(double));

  sendcounts = malloc(sizeof(int)*size);
  displs = malloc(sizeof(int)*size);

  if(rank == ROOT){
    // printf("%i\n", rank);
    // printf("%i\n", nstrip);
    A = (double*)calloc(n * n, sizeof(double));
    B = (double*)calloc(n, sizeof(double));
    for(i = 0; i < n; i++){
        B[i] = n * sqrt(n) + n;
        rowIndex = i * n;
        for(j = 0; j < n; j++)
            A[rowIndex + j] = sqrt(n) + n * sqrt(n) * (i==j);
    }
  }
  scatterv(n, n*nstrip, (n * n) % nodes , A, a);
  scatterv(n, nstrip, n % nodes, B, b);

  // MPI_Scatter(A, n*nstrip, displs, MPI_DOUBLE, a, n*nstrip, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
  // MPI_Scatter(B, nstrip, displs, MPI_DOUBLE, b, nstrip, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

  xx = (double*)calloc(n, sizeof(double));
  p = (double*)calloc(n, sizeof(double));
  x = (double*)calloc(nstrip, sizeof(double));

  start_time = MPI_Wtime();
  do
  {
    local_norm = 0;
    for (i = 0; i < n; i++)
      p[i] = xx[i];
    for (i = 0; i < nstrip; i++)
    {
      temp = 0.0;
      rowIndex = i * n;
      diagIndex = i + rank * nstrip;
      for (j = 0; j < i; j++)
        if(diagIndex != j)
          temp += (a[rowIndex + j] * x[j]);
      for (j = i + 1; j < n; j++)
        if(diagIndex != j)
          temp += (a[rowIndex + j] * p[j]);
      x[i] = (b[i] - temp) / a[rowIndex + diagIndex];

      local_norm = MAX(fabs(x[i] - p[i]), local_norm);
      iter++;
    }
    MPI_Gather(x, nstrip, MPI_DOUBLE, xx, nstrip, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    if(rank == ROOT) {
      iter += 1;
    }
    MPI_Allreduce(&local_norm, &norm, 1, MPI_FLOAT, MPI_MAX, 0,MPI_COMM_WORLD);
    // MPI_Bcast(&norm, 1, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(xx, n, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
  } while (norm >= 1e-5);
  end_time = MPI_Wtime();

  MPI_Finalize();
  if(rank == ROOT) {
    printf("Iterations: %i sec\n", iter);
    printf("Discrepancy: %f sec\n", norm);
    printf("Running time: %f sec\n", end_time - start_time);
  }
  return 0;
}
