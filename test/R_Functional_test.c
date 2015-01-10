/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "R_Functional.h"

void Integer_Doubler(R_Integer* integer) {
	integer->Integer *= 2;
}
R_Functor* Integer_Doubler_Functor_Init(R_Functor* functor, int initial) {
	R_Integer* state = R_Type_New(R_Integer);
	state->Integer = initial;
	functor->state = state;
	functor->function = (R_Functor_Function)Integer_Doubler;
	return functor;
}

void test_functors(void) {
	R_Functor* functor_6 = R_Type_New(R_Functor);
	Integer_Doubler_Functor_Init(functor_6, 6);
	R_Integer* value = R_Functor_call(functor_6);
	assert(value->Integer == 12);
	value = R_Functor_call(functor_6);
	assert(value->Integer == 24);
	R_Type_Delete(functor_6);

	R_Functor* functor_10 = Integer_Doubler_Functor_Init(R_Type_New(R_Functor), 100);
	value = R_Functor_call(functor_10);
	assert(value->Integer == 200);
	R_Type_Delete(functor_10);
}

int main(void) {
  test_functors();

  assert(R_Type_BytesAllocated == 0);
  printf("Pass\n");
}