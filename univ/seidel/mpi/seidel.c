#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ROOT 0
#define MAX(a, b) ((a) > (b) ? (a) : (b))


void printx(int rank, double *x, int n)
{
  int i;
  printf("%i ", rank);
  for(i = 0; i < n; i++)
    printf("%f, ", x[i]);
  printf("\n");
}

void printintx(int rank, int *x, int n)
{
  int i;
  printf("%i ", rank);
  for(i = 0; i < n; i++)
    printf("%i, ", x[i]);
  printf("\n");
}


int main(int argc, char *argv[]) {
  int n, rank, nodes, rowIndex;
  double start_time, end_time, tick;
  double *A, *a, *B, *b, *tmp, *xx, *x, *p;
  double temp, norm = 0, local_norm = 0;
  int i = 0, j = 0, iter = 0, sum;

  n = atoi(argv[1]);
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nodes);

  if(rank == ROOT) {
    A = (double*)calloc(n * n, sizeof(double));
    B = (double*)calloc(n, sizeof(double));
    for(i = 0; i < n; i++) {
        B[i] = i + 1;
        rowIndex = i * n;
        for(j = 0; j < n; j++)
            A[rowIndex + j] = sqrt(j + 1) + n * sqrt(n) * (i==j);
    }
  }
  sum = 0;
  int *sendcounts = malloc(sizeof(int) * nodes);
  int *displs = malloc(sizeof(int) * nodes);
  int rem = n % nodes;
  for(i = 0; i < nodes; i++){
      sendcounts[i] = n / nodes;
      if (rem > 0) {
          sendcounts[i]++;
          rem--;
      }
      displs[i] = sum;
      sum += sendcounts[i];
  }
  b = (double*)calloc(sendcounts[rank], sizeof(double));
  MPI_Scatterv(B, sendcounts, displs, MPI_DOUBLE, b, sendcounts[rank], MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

  int* nodeVecSize = (int*)calloc(nodes, sizeof(int));
  int* nodeVecOffsets = (int*)calloc(nodes, sizeof(int));
  memcpy(nodeVecSize, sendcounts, nodes * sizeof(int));
  memcpy(nodeVecOffsets, displs, nodes * sizeof(int));

  for(i = 0; i < nodes; i++){
    sendcounts[i] *= n;
    displs[i] *= n;
  }
  a = (double*)calloc(sendcounts[rank], sizeof(double));
  MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE, a, sendcounts[rank], MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

  x = (double*)calloc(n, sizeof(double));
  for (i = 0; i < n; i++)
    x[i] = i;
  start_time = MPI_Wtime();
  do
  {
    local_norm = 0.0;
    for (i = 0; i < nodeVecSize[rank]; i++)
    {
      temp = 0.0;
      rowIndex = i * n;
      int ind = i + nodeVecOffsets[rank];
      for (j = 0; j < n; j++)
          temp += (a[rowIndex + j] * x[j]);
      double r = (b[i] - temp) / a[rowIndex + ind];
      x[ind] = x[ind] + r;
      local_norm = MAX(fabs(r), local_norm);
    }

    MPI_Allreduce(&local_norm, &norm, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allgatherv(MPI_IN_PLACE, nodeVecSize[rank], MPI_DOUBLE, x, nodeVecSize, nodeVecOffsets, MPI_DOUBLE, MPI_COMM_WORLD);

    if(rank == ROOT)
      iter += 1;
  } while (norm >= 1e-5);
  end_time = MPI_Wtime();

  MPI_Finalize();
  if(rank == ROOT) {
    printf("Iterations: %i\n", iter);
    printf("Discrepancy: %f\n", norm);
    printf("Running time: %f sec\n", end_time - start_time);
    // printx(rank, x, n);
  }
  return 0;
}
