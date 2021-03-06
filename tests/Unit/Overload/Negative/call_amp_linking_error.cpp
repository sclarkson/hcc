// RUN: %cxxamp %s -o %t.out 2>&1 | %FileCheck --strict-whitespace %s

//////////////////////////////////////////////////////////////////////////////////
// Do not delete or add any line; it is referred to by absolute line number in the
// FileCheck lines below
//////////////////////////////////////////////////////////////////////////////////
#include <hc.hpp>
using namespace hc;

int f1() [[hc]] {return 1;} 
int f2() [[hc]] {
  return f1();
}

int CPU_Func() [[cpu]]
{
  return f2();
}
// CHECK:call_amp_linking_error.cpp:[[@LINE-2]]:12: error:  'f2':  no overloaded function has restriction specifiers that are compatible with the ambient context 'CPU_Func'
// CHECK-NEXT:  return f2();
// CHECK-NEXT:           ^


int main()
{
  return 1; // Should not compile
}
