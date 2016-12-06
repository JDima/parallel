#include <omp.h>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <chrono>
#include <time.h>

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
  if (sqrt(norm) >= 1e-3)
    return false;
  return true;
}

std::vector<double> seidel(std::vector< std::vector<double> > &a, std::vector<double> &b)
{
  int n = b.size();
  std::vector<double> x(b.size(), 1.0);
  std::vector<double> p(b.size(), 1.0);
  int i = 0;
  do
  {
    for (int i = 0; i < n; i++)
      p[i] = x[i];

    #pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
      double var = 0.0;
      for (int j = 0; j < i; j++)
        var += (a[i][j] * x[j]);
      for (int j = i + 1; j < n; j++)
        var += (a[i][j] * p[j]);
      x[i] = (b[i] - var) / a[i][i];
    }
  } while (!converge(x, p));
  return x;
}


int main() {
  using namespace std::chrono;

  std::vector< std::vector<double> > a = {{16.0, 3.0}, {7.0, -11.0}};
  std::vector<double> b = {11.0, 13.0};

  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  std::vector<double> res = seidel(a, b);
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>( t2 - t1 ).count();

  std::cout << "Solution: ";
  for(auto x : res)
    std::cout << x << " ";
  std::cout << "\nRunning time: " << duration << " ms"<< std::endl;
}
