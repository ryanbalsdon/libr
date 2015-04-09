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
#include "R_Functor.h"


void* Integer_Doubler(R_Integer* integer) {
	R_Integer_set(integer, R_Integer_get(integer)*2);
	return integer;
}
R_Functor* Integer_Doubler_Functor_Init(R_Functor* functor, int initial) {
	R_Integer* state = R_Type_New(R_Integer);
	functor->state = R_Integer_set(state, initial);
	functor->function = (R_Functor_Function)Integer_Doubler;
	return functor;
}

void test_functors(void) {
	R_Functor* functor_6 = R_Type_New(R_Functor);
	Integer_Doubler_Functor_Init(functor_6, 6);
	R_Integer* value = R_Functor_call(functor_6);
	assert(R_Integer_get(value) == 12);
	value = R_Functor_call(functor_6);
	assert(R_Integer_get(value) == 24);
	R_Type_Delete(functor_6);

	R_Functor* functor_10 = Integer_Doubler_Functor_Init(R_Type_New(R_Functor), 100);
	value = R_Functor_call(functor_10);
	assert(R_Integer_get(value) == 200);
	R_Type_Delete(functor_10);
}

typedef struct {
  R_Type* type;
  size_t current;
  size_t max;
} TestIterator;
R_Type_Def(TestIterator, NULL, NULL, NULL);

void* Iterator_Advancer(TestIterator* iterator) {
  iterator->current++;
  if (iterator->current > iterator->max) return NULL;
  return iterator;
}
R_Functor* Iterator_Init(R_Functor* functor, int count) {
  TestIterator* state = R_Type_New(TestIterator);
  state->max = count;
  functor->state = state;
  functor->function = (R_Functor_Function)Iterator_Advancer;
  return functor;
}

void test_iterators(void) {
  R_Functor* functor_1 = R_Type_New(R_Functor);
  Iterator_Init(functor_1, 1);
  assert(R_Type_IsOf(R_Functor_call(functor_1), TestIterator));
  assert(R_Functor_call(functor_1) == NULL);
  R_Type_Delete(functor_1);

  R_Functor* functor_10 = R_Type_New(R_Functor);
  Iterator_Init(functor_10, 10);
  for (int i=0; i<10; i++) assert(R_Type_IsOf(R_Functor_call(functor_10), TestIterator));
  assert(R_Functor_call(functor_10) == NULL);
  R_Type_Delete(functor_10);

  R_Functor* functor_100 = R_Type_New(R_Functor);
  Iterator_Init(functor_100, 100);
  int loop_count = 0;
  R_Functor_iterate(functor_100, TestIterator, instance) {
    assert(R_Type_IsOf(instance, TestIterator));
    loop_count++;
  }
  assert(R_Functor_call(functor_100) == NULL);
  assert(loop_count == 100);
  R_Type_Delete(functor_100);
}

int main(void) {
  test_functors();
  test_iterators();

  assert(R_Type_BytesAllocated == 0);
  printf("Pass\n");
}