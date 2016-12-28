// #include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ROOT 0
#define MIN(a, b) ((a) < (b) ? (a) : (b))

double h, a, b;
int n, matSize, procMatSize, procVecSize, vecSize;
int rank, nodes;

double f(double x, double y) {
  return x + y;
}

double solution(double x, double y) {
  return (x * x * x + y * y * y) / 6;
}

int main(int argc, char *argv[]) {
  double startTime, endTime;
  double dmax, d, temp;
  double* u;
  int i, j, ind;
  n = atoi(argv[1]);

  // MPI_Init(&argc, &argv);
  //
  // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  // MPI_Comm_size(MPI_COMM_WORLD, &nodes);
  nodes = 4;
  printf("%i\n", n);
  a = 0.0;
  b = 1.0;
  matSize = (n + 2) * (n + 2);
  vecSize = n + 2;
  h = (b - a) / (vecSize - 1);
  procMatSize = matSize / nodes;
  procVecSize = n / nodes;

  u = (double*)calloc(matSize, sizeof(double));

  for(i = 0; i < vecSize; i++) {
    for(j = 0; j < vecSize; j++) {
      ind = i * vecSize + j;
      u[ind] = solution(a + i * h, a + j * h);
    }
  }

  for(i = 1; i < vecSize - 1; i++) {
    for(j = 1; j < vecSize - 1; j++) {
      ind = i * vecSize + j;
      u[ind] = 0;
    }
  }

  do
  {
    printf("iter\n");
    dmax = 0; // максимальное изменение значений u
    for (i = 1; i < n + 1; i++ ) {
      for (j = 1; j < n + 1; j++ ) {
        ind = i * vecSize + j;
        temp = u[ind];
        u[ind] = 0.25 * (u[ind - vecSize] + u[ind + vecSize] + u[ind - 1]
        + u[ind + 1] - h * h * f(a + i * h, a + j * h));
        d = fabs(temp - u[ind]);
        if (dmax < d)
          dmax = d;
      }
    }
  } while ( dmax > 1e-5 );

  for (i = 1; i < n + 1; i++ ) {
    printf("%f ", a + i * h);
    for (j = 1; j < n + 1; j++ ) {
      ind = i * vecSize + j;
      // printf("%f ", solution(a + i * h, a + j * h));
      // printf("%f ", u[ind]);
      printf("%f ", fabs(solution(a + i * h, a + j * h) - u[ind]));
    }
    printf("\n");
  }
  // startTime = MPI_Wtime();
  // std::vector<double> res = seidel(A, b);
  // endTime = MPI_Wtime();
  // MPI_Finalize();
  //
  // std::cout << "Running time: " << endTime - startTime << " sec"<< std::endl;
}
