#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

int * scatterv(int nodes, int counts, int rem, double *sendbuf, double *recvbuf)
{
  int i, sum = 0;
  int *sendcounts = malloc(sizeof(int) * nodes);
  int *displs = malloc(sizeof(int) * nodes);
  for(i = 0; i < nodes; i++){
      sendcounts[i] = counts;
      if (rem > 0) {
          sendcounts[i]++;
          rem--;
      }
      displs[i] = sum;
      sum += sendcounts[i];
  }
  MPI_Scatterv(&sendbuf, sendcounts, displs, MPI_DOUBLE, &recvbuf, counts + 1, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
  return sendcounts;
}

int main(int argc, char *argv[]) {
  int n, nstrip;
  int rank, nodes, rowIndex;
  double start_time, end_time, tick;
  double *A, *a, *B, *b, *tmp, *xx, *x, *p;
  double temp, norm = 0, local_norm = 0;
  int i = 0, j = 0, iter = 0, sum;

  n = atoi(argv[1]);
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nodes);
  printf("%i Nodes %i\n", nodes);
  nstrip = n / nodes;
  a = (double*)calloc((n * n) / nodes, sizeof(double));
  b = (double*)calloc(nstrip, sizeof(double));

  if(rank == ROOT){
    A = (double*)calloc(n * n, sizeof(double));
    B = (double*)calloc(n, sizeof(double));
    for(i = 0; i < n; i++) {
        B[i] = n * sqrt(n) + n;
        rowIndex = i * n;
        for(j = 0; j < n; j++)
            A[rowIndex + j] = sqrt(n) + n * sqrt(n) * (i==j);
    }
  }
  int* nodeMatSize = scatterv(nodes, (n * n) / nodes, (n * n) % nodes, A, a);
  int* nodeVecSize = scatterv(nodes, nstrip, n % nodes, B, b);
  int* nodeVecOffsets = (int*)calloc(nodes, sizeof(int));
  sum = 0;
  for(i = 0; i < nodes; i++){
      nodeVecOffsets[i] = sum;
      sum += nodeVecSize[i];
  }

  x = (double*)calloc(n, sizeof(double));
  for (i = 0; i < n; i++)
    x[i] = i;
  start_time = MPI_Wtime();
  do
  {
    local_norm = 0;
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
    MPI_Allgatherv(MPI_IN_PLACE, nodeVecSize[rank], MPI_DOUBLE, &x[0], nodeVecSize, nodeVecOffsets, MPI_DOUBLE, MPI_COMM_WORLD);

    if(rank == ROOT)
      iter += 1;
  } while (norm >= 1e-2);
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
