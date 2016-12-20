#include <omp.h>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <iostream>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void printx(std::vector<double> &vec)
{
  for(int i = 0; i < vec.size(); i++)
    std::cout << vec[i] << " ";
  std::cout << std::endl;
}

std::vector<double> seidel(std::vector< std::vector<double> > &a, std::vector<double> &b)
{
  int i = 0, j = 0;
  double norm, temp = 0.0;
  int numThreads = 8, n = b.size(), iters = 0;
  std::vector<double> x(n);
  for (i = 0; i < n; i++)
    x[i] = float(i);
  std::vector<double> p(n);
  do
  {
    iters += 1;
    norm = 0.0;
    #pragma omp parallel num_threads(numThreads) shared(a, b, x, p, norm) private(i, j, temp)
    {
      #pragma omp for
      for (i = 0; i < n; i++)
        p[i] = x[i];
      #pragma omp for reduction(max : norm)
      for (i = 0; i < n; i++)
      {
        temp = 0.0;
        for (j = 0; j < i; j++)
          temp += (a[i][j] * x[j]);
        for (j = i + 1; j < n; j++)
          temp += (a[i][j] * p[j]);

        x[i] = (b[i] - temp) / a[i][i];

        norm = MAX(fabs(x[i] - p[i]), norm);
      }
    }
  } while (norm >= 1e-5);

  std::cout << "Iterations: " << iters << std::endl;
  std::cout << "Discrepancy: " << norm << std::endl;
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
          A[i][j] = sqrt(n) + n * sqrt(n) * (i==j);
  }

  start_time = omp_get_wtime();
  std::vector<double> res = seidel(A, b);
  end_time = omp_get_wtime();
  // printx(res);
  std::cout << "Running time: " << end_time - start_time << " sec"<< std::endl;
}
