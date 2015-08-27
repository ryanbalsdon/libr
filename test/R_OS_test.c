#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "R_OS.h"

void test_atoi(void);
void test_atof(void);

int main(void) {
  test_atoi();
  test_atof();
  printf("PASS\n");
}

void test_atoi(void) {
  assert(os_atoi_alt("0") == 0);
  assert(os_atoi_alt("1") == 1);
  assert(os_atoi_alt("123456") == 123456);
  assert(os_atoi_alt("+245") == +245);
  assert(os_atoi_alt("-245") == -245);
  assert(os_atoi_alt("+912") == 912);
}

void test_atof(void) {
  assert(os_atof_alt("0") == 0.0);
  assert(os_atof_alt("0.0") == 0.0);
  assert(os_atof_alt("1.0") == 1.0);
  assert(os_atof_alt("0.0") == 0.0);
  assert(os_atof_alt("100") == 100.0);
  assert(os_atof_alt("100.1001") == 100.1001);
  assert(os_atof_alt("10e2") == 10e2);
  assert(os_atof_alt("1E3") == 1E3);
  assert(os_atof_alt("2e-4") > 2.0e-4);
  assert(os_atof_alt("2e-4") < 2.1e-4);
  assert(os_atof_alt("9.9e2") == 9.9e2);
  assert(os_atof_alt("-1e0") == -1e0);
}

