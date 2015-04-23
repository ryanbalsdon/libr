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
#include "R_String.h"

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
R_Type_Def(Integer, Integer_Constructor, Integer_Destructor, NULL, NULL);

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

  bool integer0_found = false;
  bool integer1_found = false;
  bool integer2_found = false;

  R_Functor* iterator = R_List_Iterator(R_Type_New(R_Functor), array);
  R_Integer* element = NULL;
  while ((element = R_Functor_call(iterator)) != NULL) {
    assert(R_Type_IsOf(element, R_Integer));
    if (R_Integer_get(element) == 0) integer0_found = true;
    if (R_Integer_get(element) == 1) integer1_found = true;
    if (R_Integer_get(element) == 2) integer2_found = true;
  }

  assert(integer0_found);
  assert(integer1_found);
  assert(integer2_found);

  iterator = R_List_Iterator(iterator, array);
  while ((element = R_Functor_call(iterator)) != NULL) {
    R_List_removePointer(array, element);
  }
  assert(R_List_size(array) == 0);

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
    assert(R_Type_IsOf(value, R_Integer));
    if (R_Integer_get(value) == 0) integer0_found = true;
    if (R_Integer_get(value) == 1) integer1_found = true;
    if (R_Integer_get(value) == 2) integer2_found = true;
  }

  assert(integer0_found);
  assert(integer1_found);
  assert(integer2_found);

  R_Type_Delete(array);
}

void test_add_copy(void) {
  R_List* array = R_Type_New(R_List);

  R_Integer* integer = R_List_add(array, R_Integer);
  R_Integer_set(integer, 42);
  integer = R_List_addCopy(array, integer);

  assert(R_List_size(array) == 2);
  assert(R_List_pointerAtIndex(array, 0) != R_List_pointerAtIndex(array, 1));
  assert(R_Integer_get(R_List_pointerAtIndex(array, 0)) == R_Integer_get(R_List_pointerAtIndex(array, 1)));

  R_Type_Delete(array);
}

void test_transfer(void) {
  R_List* array = R_Type_New(R_List);

  R_Integer* integer = R_Type_New(R_Integer);
  R_Integer_set(integer, 42);
  assert(integer == R_List_transferOwnership(array, integer));

  assert(R_List_size(array) == 1);
  assert(R_List_pointerAtIndex(array, 0) == integer);
  assert(R_Type_IsOf(integer, R_Integer));
  assert(R_Integer_get(integer) == 42);

  R_Type_Delete(array);
}

void test_copy(void) {
  R_List* array = R_Type_New(R_List);
  R_Integer* integer = R_List_add(array, R_Integer);
  R_Integer_set(integer, 0);
  integer = R_List_add(array, R_Integer);
  R_Integer_set(integer, 1);
  integer = R_List_add(array, R_Integer);
  R_Integer_set(integer, 2);
  integer = R_List_add(array, R_Integer);
  R_Integer_set(integer, 3);

  R_List* array_copy = R_Type_Copy(array);
  R_Type_Delete(array);

  assert(array_copy != NULL);
  assert(R_List_size(array_copy) == 4);
  assert(R_Integer_get(R_List_pointerAtIndex(array_copy, 0)) == 0);
  assert(R_Integer_get(R_List_pointerAtIndex(array_copy, 1)) == 1);
  assert(R_Integer_get(R_List_pointerAtIndex(array_copy, 2)) == 2);
  assert(R_Integer_get(R_List_pointerAtIndex(array_copy, 3)) == 3);

  R_Type_Delete(array_copy);
}

void test_append(void) {
  R_List* list_a = R_Type_New(R_List);
  R_List* list_b = R_Type_New(R_List);
  R_Integer_set(R_List_add(list_a, R_Integer), 0);
  R_Integer_set(R_List_add(list_a, R_Integer), 1);
  R_Integer_set(R_List_add(list_b, R_Integer), 10);
  R_Integer_set(R_List_add(list_b, R_Integer), 11);

  assert(R_List_appendList(list_a, list_b) == list_a);
  assert(R_List_size(list_a) == 4);
  assert(R_Integer_get(R_List_pointerAtIndex(list_a, 0)) == 0);
  assert(R_Integer_get(R_List_pointerAtIndex(list_a, 1)) == 1);
  assert(R_Integer_get(R_List_pointerAtIndex(list_a, 2)) == 10);
  assert(R_Integer_get(R_List_pointerAtIndex(list_a, 3)) == 11);

  assert(R_List_appendList(list_b, list_b) == list_b);
  assert(R_List_size(list_b) == 4);
  assert(R_Integer_get(R_List_pointerAtIndex(list_b, 0)) == 10);
  assert(R_Integer_get(R_List_pointerAtIndex(list_b, 1)) == 11);
  assert(R_Integer_get(R_List_pointerAtIndex(list_b, 2)) == 10);
  assert(R_Integer_get(R_List_pointerAtIndex(list_b, 3)) == 11);

  assert(R_List_pointerAtIndex(list_b, 2) != R_List_pointerAtIndex(list_b, 0));

  R_Type_Delete(list_a);
  R_Type_Delete(list_b);
}

void test_puts(void) {
  R_List* list = R_Type_New(R_List);
  R_String_setString(R_List_add(list, R_String), "hello");
  R_String_setString(R_List_add(list, R_String), "world");
  R_Puts(list);
  R_Type_Delete(list);
}

int main(void) {
	test_allocations();
	test_integer();
	test_pop();
	test_swap();
	test_cleanup();
	test_transfer();
	test_objectarray_iterator();
	test_objectarray_each();
	test_add_copy();
	test_copy();
	test_append();
  test_puts();

	assert(R_Type_BytesAllocated == 0);
	printf("Pass\n");
}
