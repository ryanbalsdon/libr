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
#include <stdbool.h>
#include "R_Functional.h"
#include "R_ObjectArray.h"

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

void test_objectarray_iterator(void) {
  R_ObjectArray* array = R_Type_New(R_ObjectArray);
  R_Integer* integer = R_ObjectArray_add(array, R_Integer);
  integer->Integer = 0;
  integer = R_ObjectArray_add(array, R_Integer);
  integer->Integer = 1;
  integer = R_ObjectArray_add(array, R_Integer);
  integer->Integer = 2;

  bool integer0_found = false;
  bool integer1_found = false;
  bool integer2_found = false;

  R_ObjectArray_each(array, R_Integer, value) {
  	if (value->Integer == 0) integer0_found = true;
  	if (value->Integer == 1) integer1_found = true;
  	if (value->Integer == 2) integer2_found = true;
  }

  assert(integer0_found);
  assert(integer1_found);
  assert(integer2_found);

  R_Type_Delete(array);
}

int main(void) {
  test_functors();
  test_objectarray_iterator();


  assert(R_Type_BytesAllocated == 0);
  printf("Pass\n");
}