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
#include "R_List.h"

struct Integer {
  R_Type* type;
  int integer;
};
typedef struct Integer Integer;
int Integer_Constructor_Called = 0;
Integer* Integer_Constructor(Integer* integer) {
  Integer_Constructor_Called++;
  integer->integer = 42;
  return integer;
}
int Integer_Destructor_Called = 0;
Integer* Integer_Destructor(Integer* integer) {
  Integer_Destructor_Called++;
  return integer;
}
R_Type_Def(Integer, Integer_Constructor, Integer_Destructor, NULL);

void test_allocations(void) {
	assert(R_Type_BytesAllocated == 0);
	R_List* array = R_Type_New(R_List);
	R_Type_Delete(array);
	assert(R_Type_BytesAllocated == 0);
}

void test_integer(void) {
	assert(R_Type_BytesAllocated == 0);
	R_List* array = R_Type_New(R_List);

	assert(Integer_Constructor_Called == 0);
	Integer* integer = R_List_add(array, Integer);
	assert(Integer_Constructor_Called == 1);
	assert(integer->integer == 42);
	integer->integer = 0;
	assert(R_List_length(array) == 1);

	integer = R_List_add(array, Integer);
	assert(Integer_Constructor_Called == 2);
	assert(integer->integer == 42);
	integer->integer = 1;
	assert(R_List_length(array) == 2);

	assert((((Integer*)R_List_pointerAtIndex(array,0))->integer) == 0);
	assert((((Integer*)R_List_pointerAtIndex(array,1))->integer) == 1);

	R_List_removeIndex(array, 0);
	assert(Integer_Destructor_Called == 1);
	assert((((Integer*)R_List_pointerAtIndex(array,0))->integer) == 1);
	assert(R_List_length(array) == 1);
	R_List_removeAll(array);
	assert(Integer_Destructor_Called == 2);
	assert(R_List_length(array) == 0);

	Integer_Destructor_Called = 0;
	Integer_Constructor_Called = 0;
	R_Type_Delete(array);

	assert(R_Type_BytesAllocated == 0);
}

void test_pop(void) {
	R_List* array = R_Type_New(R_List);
	Integer* integerA = R_List_add(array, Integer);
	Integer* integerB = R_List_add(array, Integer);
	Integer* integerC = R_List_add(array, Integer);
	integerA->integer = 0;
	integerB->integer = 1;
	integerC->integer = 2;
	assert(R_List_length(array) == 3);
	assert((((Integer*)R_List_pointerAtIndex(array,0))->integer) == 0);
	assert((((Integer*)R_List_pointerAtIndex(array,1))->integer) == 1);
	assert((((Integer*)R_List_pointerAtIndex(array,2))->integer) == 2);

	R_List_pop(array);
	assert(R_List_length(array) == 2);
	assert((((Integer*)R_List_pointerAtIndex(array,0))->integer) == 0);
	assert((((Integer*)R_List_pointerAtIndex(array,1))->integer) == 1);

	R_List_pop(array);
	assert(R_List_length(array) == 1);
	assert((((Integer*)R_List_pointerAtIndex(array,0))->integer) == 0);

	R_List_pop(array);
	assert(R_List_length(array) == 0);

	R_Type_Delete(array);
}

void test_swap(void) {
	R_List* array = R_Type_New(R_List);
	Integer* integerA = R_List_add(array, Integer);
	Integer* integerB = R_List_add(array, Integer);
	Integer* integerC = R_List_add(array, Integer);
	integerA->integer = 0;
	integerB->integer = 1;
	integerC->integer = 2;
	assert(R_List_length(array) == 3);
	assert((((Integer*)R_List_pointerAtIndex(array,0))->integer) == 0);
	assert((((Integer*)R_List_pointerAtIndex(array,1))->integer) == 1);
	assert((((Integer*)R_List_pointerAtIndex(array,2))->integer) == 2);

	R_List_swap(array,0,1);
	assert((((Integer*)R_List_pointerAtIndex(array,0))->integer) == 1);
	assert((((Integer*)R_List_pointerAtIndex(array,1))->integer) == 0);
	assert((((Integer*)R_List_pointerAtIndex(array,2))->integer) == 2);

	R_List_swap(array,2,0);
	assert((((Integer*)R_List_pointerAtIndex(array,0))->integer) == 2);
	assert((((Integer*)R_List_pointerAtIndex(array,1))->integer) == 0);
	assert((((Integer*)R_List_pointerAtIndex(array,2))->integer) == 1);

	R_Type_Delete(array);
}

void test_cleanup(void) {
	assert(R_Type_BytesAllocated == 0);
	Integer_Destructor_Called = 0;
	Integer_Constructor_Called = 0;

	R_List* array = R_Type_New(R_List);
	R_List_add(array, Integer);
	R_List_add(array, Integer);
	R_List_add(array, Integer);
	R_Type_Delete(array);
	assert(Integer_Constructor_Called == 3);
	assert(Integer_Destructor_Called == 3);
	assert(R_Type_BytesAllocated == 0);
}

void test_objectarray_iterator(void) {
  R_List* array = R_Type_New(R_List);
  R_Integer* integer = R_List_add(array, R_Integer);
  R_Integer_set(integer, 0);
  integer = R_List_add(array, R_Integer);
  R_Integer_set(integer, 1);
  integer = R_List_add(array, R_Integer);
  R_Integer_set(integer, 2);

  R_Functor* iterator = R_List_Iterator(R_Type_New(R_Functor), array);
  R_Integer* element = R_Functor_call(iterator);
  assert(R_Type_IsOf(element, R_Integer));
  assert(R_Integer_get(element) == 0);
  element = R_Functor_call(iterator);
  assert(R_Type_IsOf(element, R_Integer));
  assert(R_Integer_get(element) == 1);
  element = R_Functor_call(iterator);
  assert(R_Type_IsOf(element, R_Integer));
  assert(R_Integer_get(element) == 2);
  element = R_Functor_call(iterator);
  assert(element == NULL);

  R_Type_Delete(iterator);
  R_Type_Delete(array);
}

void test_objectarray_each(void) {
  R_List* array = R_Type_New(R_List);
  R_Integer* integer = R_List_add(array, R_Integer);
  R_Integer_set(integer, 0);
  integer = R_List_add(array, R_Integer);
  R_Integer_set(integer, 1);
  integer = R_List_add(array, R_Integer);
  R_Integer_set(integer, 2);

  bool integer0_found = false;
  bool integer1_found = false;
  bool integer2_found = false;

  R_List_each(array, R_Integer, value) {
  	if (R_Integer_get(value) == 0) integer0_found = true;
  	if (R_Integer_get(value) == 1) integer1_found = true;
  	if (R_Integer_get(value) == 2) integer2_found = true;
  }

  assert(integer0_found);
  assert(integer1_found);
  assert(integer2_found);

  R_Type_Delete(array);
}

int main(void) {
	test_allocations();
	test_integer();
	test_pop();
	test_swap();
	test_cleanup();
	test_objectarray_iterator();
	test_objectarray_each();

	assert(R_Type_BytesAllocated == 0);
	printf("Pass\n");
}