// XFAIL: Linux
// RUN: %cxxamp -Xclang -fhsa-ext %s -o %t.out && %t.out
#include <iostream>
#include <random>
#include <future>
#include <vector>
#include <algorithm>
#include <utility>
#include <amp.h>

// An HSA version of C++AMP program
int main ()
{
  // define inputs and output
  const int vecSize = 2048;

  int table_a[vecSize];
  int table_b[vecSize];
  int table_c[vecSize];
  int *p_a = &table_a[0];
  int *p_b = &table_b[0];
  int *p_c = &table_c[0];

  // initialize test data
  std::random_device rd;
  std::uniform_int_distribution<int32_t> int_dist;
  for (int i = 0; i < vecSize; ++i) {
    table_a[i] = int_dist(rd);
    table_b[i] = int_dist(rd);
  }

  // the vector to store handles to each async pfe 
  std::vector<std::future<void>> futures;

  // divide the array into 4 quarters
  Concurrency::extent<1> e(vecSize / 4);

#define ASYNC_KERNEL_DISPATCH(x, y) \
  Concurrency::async_parallel_for_each( \
    e, \
    [=](Concurrency::index<1> idx) restrict(amp) { \
      for (int i = 0; i < 1024 * 1024; ++i) \
        p_c[idx[0] + vecSize/(x)*(y)] = p_a[idx[0] + vecSize/(x)*(y)] + p_b[idx[0] + vecSize/(x)*(y)]; \
  })

  // asynchronously launch each quarter
  futures.push_back(std::move(ASYNC_KERNEL_DISPATCH(4, 0)));
  futures.push_back(std::move(ASYNC_KERNEL_DISPATCH(4, 1)));
  futures.push_back(std::move(ASYNC_KERNEL_DISPATCH(4, 2)));
  futures.push_back(std::move(ASYNC_KERNEL_DISPATCH(4, 3)));

  // wait for all kernels to finish execution
  std::for_each(futures.cbegin(), futures.cend(), [](const std::future<void>& fut) { fut.wait(); });

  // verify
  int error = 0;
  for(unsigned i = 0; i < vecSize; i++) {
    error += table_c[i] - (table_a[i] + table_b[i]);
  }
  if (error == 0) {
    std::cout << "Verify success!\n";
  } else {
    std::cout << "Verify failed!\n";
  }

  return error != 0;
}

