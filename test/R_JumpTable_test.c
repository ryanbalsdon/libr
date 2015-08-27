#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "R_JumpTable.h"

R_JumpTable_DefineKey(doubler);
R_JumpTable_DeclareFunction(doubler, int, int);

R_JumpTable_DefineKey(adder);
R_JumpTable_DeclareFunction(adder, int, int, int);

R_JumpTable_DefineKey(method_that_is_not_in_the_list);

int doubler_implementation(int);
int doubler_implementation(int in) {
  return in*2;
}

int adder_implementation(int,int);
int adder_implementation(int a, int b) {
  return a+b;
}

R_JumpTable_Entry table1[] = {R_JumpTable_Entry_Make(doubler, doubler_implementation), R_JumpTable_Entry_NULL};
R_JumpTable_Entry table2[] = {
  R_JumpTable_Entry_Make(doubler, doubler_implementation), 
  R_JumpTable_Entry_Make(adder, adder_implementation), 
  R_JumpTable_Entry_NULL
};


int main(void) {
  assert(doubler_implementation == R_JumpTable_get(table1, R_JumpTable_Key(doubler)));
  assert(NULL                   == R_JumpTable_get(table1, R_JumpTable_Key(method_that_is_not_in_the_list)));

  assert(R_JumpTable_call(table1, doubler, 2) == 4);

  assert(R_JumpTable_call(table2, doubler, 124) == 248);
  assert(R_JumpTable_call(table2, adder, 164, 52) == 216);

  printf("Pass\n");
}
