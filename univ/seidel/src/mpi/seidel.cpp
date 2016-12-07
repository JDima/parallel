#include <mpi.h>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <iostream>

void printx(std::vector<double> &vec)
{
  for(int i = 0; i < vec.size(); i++)
    std::cout << vec[i] << " ";
  std::cout << std::endl;
}


bool converge(std::vector<double> &xk, std::vector<double> &xkp)
{
  int n = xk.size();
  double norm = 0.0;
  for (int i = 0; i < n; i++) {
    norm += (xk[i] - xkp[i]) * (xk[i] - xkp[i]);
  }
  if (sqrt(norm) >= 1e-5)
    return false;
  return true;
}

std::vector<double> seidel(std::vector< std::vector<double> > &a, std::vector<double> &b)
{
  double temp;
  int n = b.size();
  int i = 0, j = 0;
  int numThreads = 8;

  std::vector<double> x(b.size(), 1.0);
  std::vector<double> p(b.size(), 1.0);

  do
  {
    for (i = 0; i < n; i++)
      p[i] = x[i];
    for (i = 0; i < n; i++)
    {
      temp = 0.0;
      for (j = 0; j < i; j++)
        temp += (a[i][j] * x[j]);
      for (j = i + 1; j < n; j++)
        temp += (a[i][j] * p[j]);
      x[i] = (b[i] - temp) / a[i][i];
    }
  } while (!converge(x, p));
  return x;
}


int main(int argc, char *argv[]) {
  int n = 2000;
  int rank, nodes;
  double start_time, end_time, tick;
  std::vector< std::vector<double> > A(n, std::vector<double>(n));
  std::vector<double> b(n, 0);

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nodes);
  
  for(int i = 0; i < n; i++){
      b[i] = n * sqrt(n) + n;
      for(int j = 0; j < n; j++)
          A[i][j] = 1.0 + n * sqrt(n) * (i==j);
  }
  start_time = MPI_Wtime();
  std::vector<double> res = seidel(A, b);
  end_time = MPI_Wtime();
  MPI_Finalize();

  std::cout << "Running time: " << end_time - start_time << " sec"<< std::endl;
}
