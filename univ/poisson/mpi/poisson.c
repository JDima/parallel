#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ROOT 0
#define TAGXUP 13
#define TAGXDOWN 14
#define MAX(a, b) ((a) > (b) ? (a) : (b))

double h, a, b;
int *matSendCounts, *vecSendCounts;
int *matDispls, *vecDispls;
int n, matSize, procMatSize, procVecSize, vecSize;
int rank, nodes;

void printintx(int rank, int *x, int n)
{
  int i;
  printf("%i ", rank);
  for(i = 0; i < n; i++)
    printf("%i, ", x[i]);
  printf("\n");
}


double f(double x, double y) {
  return x + y;
}

double solution(double x, double y) {
  return (x * x * x + y * y * y) / 6;
}

double * initU() {
  int i, j, ind;
  double * U = (double*)calloc(matSize, sizeof(double));
  for(i = 0; i < vecSize; i++) {
    for(j = 0; j < vecSize; j++) {
      ind = i * vecSize + j;
      U[ind] = solution(a + i * h, a + j * h);
    }
  }

  for(i = 1; i < vecSize - 1; i++) {
    for(j = 1; j < vecSize - 1; j++) {
      ind = i * vecSize + j;
      U[ind] = 0;
    }
  }
  return U;
}

void initCounts() {
  int vecSum = 0, i;
  int rem = vecSize % nodes;
  matSendCounts = malloc(sizeof(int) * nodes);
  vecSendCounts = malloc(sizeof(int) * nodes);
  matDispls = malloc(sizeof(int) * nodes);
  vecDispls = malloc(sizeof(int) * nodes);

  for(i = 0; i < nodes; i++){
      vecSendCounts[i] = procVecSize;
      if (rem > 0) {
          vecSendCounts[i]++;
          rem--;
      }
      vecDispls[i] = vecSum;
      matDispls[i] = vecSize * vecSum;
      matSendCounts[i] = vecSize * vecSendCounts[i];
      vecSum += vecSendCounts[i];
  }
}

int initConst() {
  a = 0.0;
  b = 1.0;
  matSize = (n + 2) * (n + 2);
  vecSize = n + 2;
  h = (b - a) / (vecSize - 1);
  procMatSize = matSize / nodes;
  procVecSize = vecSize / nodes;
}

double* init() {
  double *U, *u;
  initConst();
  if(rank == ROOT)
    U = initU();
  initCounts();
  u = (double*)calloc(matSendCounts[rank], sizeof(double));
  MPI_Scatterv(U, matSendCounts, matDispls, MPI_DOUBLE, u, matSendCounts[rank], MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
  return u;
}

double calcu(double left, double right, double up, double down, int i, int j) {
  return 0.25 * (left + right + up + down - h * h * f(a + i * h, a + j * h));
}

int main(int argc, char *argv[]) {
  double startTime, endTime;
  double temp, local_norm = 0.0, norm = 0.0;
  double* u, *U;
  int i, j, ind, iter = 0;

  n = atoi(argv[1]);

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nodes);
  u = init();
  startTime = MPI_Wtime();
  double *uup = (double*)calloc(vecSize, sizeof(double));
  double *udown = (double*)calloc(vecSize, sizeof(double));
  do
  {
    local_norm = 0.0;
    if(rank > ROOT)
      MPI_Send(u, vecSize, MPI_DOUBLE, rank - 1, TAGXDOWN, MPI_COMM_WORLD);
    if(rank < nodes-1)
      MPI_Send(&u[matSendCounts[rank] - vecSize], vecSize, MPI_DOUBLE, rank + 1, TAGXUP, MPI_COMM_WORLD);

    if(rank > ROOT)
      MPI_Recv(uup, vecSize, MPI_DOUBLE, rank - 1, TAGXUP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if(rank < nodes - 1)
      MPI_Recv(udown, vecSize, MPI_DOUBLE, rank + 1, TAGXDOWN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if (rank > ROOT){
      i = 0;
      for (j = 1; j < vecSize - 1; j++) {
        ind = i * vecSize + j;
        temp = u[ind];
        u[ind] = calcu(u[ind - 1], u[ind + 1], uup[j], u[ind + vecSize], vecDispls[rank] + i, j);
        local_norm = MAX(fabs(temp - u[ind]), local_norm);
      }
    }

    for(i = 1; i < vecSendCounts[rank] - 1; i++) {
      for(j = 1; j < vecSize - 1; j++) {
        ind = i * vecSize + j;
        temp = u[ind];
        u[ind] = calcu(u[ind - 1], u[ind + 1], u[ind - vecSize], u[ind + vecSize], vecDispls[rank] + i, j);
        local_norm = MAX(fabs(temp - u[ind]), local_norm);
      }
    }

    if (rank < nodes - 1){
      i = vecSendCounts[rank] - 1;
      for (j = 1; j < vecSize - 1; j++) {
        ind = i * vecSize + j;
        temp = u[ind];
        u[ind] = calcu(u[ind - 1], u[ind + 1], u[ind - vecSize], udown[j], vecDispls[rank] + i, j);
        local_norm = MAX(fabs(temp - u[ind]), local_norm);
      }
    }
    MPI_Allreduce(&local_norm, &norm, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    if(rank == ROOT)
      iter += 1;
  } while ( norm > 1e-5 );
  endTime = MPI_Wtime();

  if (rank == ROOT)
    U = (double*)calloc(matSize, sizeof(double));
  MPI_Gatherv(u, matSendCounts[rank], MPI_DOUBLE, U, matSendCounts, matDispls, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
  if (rank == ROOT) {
    for (i = 1; i < vecSize - 1; i++) {
      // printf("%f ", a + i * h);
      for (j = 1; j < vecSize - 1; j++) {
        ind = i * vecSize + j;
        // printf("%f ", solution(a + i * h, a + j * h));
        // printf("%f ", U[ind]);
        printf("%f ", fabs(solution(a + i * h, a + j * h) - U[ind]));
      }
      printf("\n");
    }
  }
  MPI_Finalize();
  if(rank == ROOT) {
    printf("Iterations: %i\n", iter);
    printf("Discrepancy: %f\n", norm);
    printf("Running time: %f sec\n", endTime - startTime);
    // printx(rank, x, n);
  }
  return 0;
}
