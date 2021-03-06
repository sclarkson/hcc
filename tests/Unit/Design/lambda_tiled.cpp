// RUN: %cxxamp %s -o %t.out && %t.out
#include <hc.hpp>
#include <stdlib.h>
#include <iostream>
int main(void){
  const int vecSize = 128;

  // Alloc & init input data
  hc::extent<1> e(vecSize);
  hc::tiled_extent<1> et(e.tile(16));
  hc::tiled_extent<1> et2 = e.tile(16);
  assert(et.tile_dim[0] == 16);
  assert(et2.tile_dim[0] == 16);
  hc::array<int, 1> a(vecSize);
  hc::array<int, 1> b(vecSize);
  hc::array<int, 1> c(vecSize);
  int sum = 0;
  hc::array_view<int> ga(a);
  hc::array_view<int> gb(b);
  hc::array_view<int> gc(c);
  for (hc::index<1> i(0); i[0] < vecSize; i++) {
    ga[i] = 100.0f * rand() / RAND_MAX;
    gb[i] = 100.0f * rand() / RAND_MAX;
    sum += a[i] + b[i];
  }

  hc::parallel_for_each(
    et,
    [=](hc::tiled_index<1> idx) [[hc]] {
    gc[idx] = ga[idx]+gb[idx];
  });

  int error = 0;
  for(unsigned i = 0; i < vecSize; i++) {
    error += gc[i] - (ga[i] + gb[i]);
  }
  return error != 0;
}
