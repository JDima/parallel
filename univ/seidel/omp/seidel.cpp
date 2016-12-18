#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
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

  #pragma omp parallel num_threads(numThreads) firstprivate(i, j, temp) shared(a, b, x, p)
  do
  {
    for (i = 0; i < n; i++)
      p[i] = x[i];
    #pragma omp for schedule(static, n/omp_get_num_threads())
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
  int n;
  double start_time, end_time, tick;
  n = atoi(argv[1]);
  std::vector< std::vector<double> > A(n, std::vector<double>(n));
  std::vector<double> b(n, 0);

  for(int i = 0; i < n; i++){
      b[i] = n * sqrt(n) + n;
      for(int j = 0; j < n; j++)
          A[i][j] = 1.0 + n * sqrt(n) * (i==j);
  }

  start_time = omp_get_wtime();
  std::vector<double> res = seidel(A, b);
  end_time = omp_get_wtime();

  std::cout << "Running time: " << end_time - start_time << " sec"<< std::endl;
}
