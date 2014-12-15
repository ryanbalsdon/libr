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
#include "R_ObjectArray.h"

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
	R_ObjectArray* array = R_Type_New(R_ObjectArray_Type);
	R_Type_Delete(array);
	assert(R_Type_BytesAllocated == 0);
}

void test_integer(void) {
	assert(R_Type_BytesAllocated == 0);
	R_ObjectArray* array = R_Type_New(R_ObjectArray_Type);

	assert(Integer_Constructor_Called == 0);
	Integer* integer = R_ObjectArray_addObject(array, Integer_Type);
	assert(Integer_Constructor_Called == 1);
	assert(integer->integer == 42);
	integer->integer = 0;
	assert(R_ObjectArray_length(array) == 1);

	integer = R_ObjectArray_addObject(array, Integer_Type);
	assert(Integer_Constructor_Called == 2);
	assert(integer->integer == 42);
	integer->integer = 1;
	assert(R_ObjectArray_length(array) == 2);

	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,0))->integer) == 0);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,1))->integer) == 1);

	R_ObjectArray_removeIndex(array, 0);
	assert(Integer_Destructor_Called == 1);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,0))->integer) == 1);
	assert(R_ObjectArray_length(array) == 1);
	R_ObjectArray_removeAll(array);
	assert(Integer_Destructor_Called == 2);
	assert(R_ObjectArray_length(array) == 0);

	Integer_Destructor_Called = 0;
	Integer_Constructor_Called = 0;
	R_Type_Delete(array);

	assert(R_Type_BytesAllocated == 0);
}

void test_pop(void) {
	R_ObjectArray* array = R_Type_New(R_ObjectArray_Type);
	Integer* integerA = R_ObjectArray_addObject(array, Integer_Type);
	Integer* integerB = R_ObjectArray_addObject(array, Integer_Type);
	Integer* integerC = R_ObjectArray_addObject(array, Integer_Type);
	integerA->integer = 0;
	integerB->integer = 1;
	integerC->integer = 2;
	assert(R_ObjectArray_length(array) == 3);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,0))->integer) == 0);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,1))->integer) == 1);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,2))->integer) == 2);

	R_ObjectArray_pop(array);
	assert(R_ObjectArray_length(array) == 2);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,0))->integer) == 0);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,1))->integer) == 1);

	R_ObjectArray_pop(array);
	assert(R_ObjectArray_length(array) == 1);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,0))->integer) == 0);

	R_ObjectArray_pop(array);
	assert(R_ObjectArray_length(array) == 0);

	R_Type_Delete(array);
}

void test_swap(void) {
	R_ObjectArray* array = R_Type_New(R_ObjectArray_Type);
	Integer* integerA = R_ObjectArray_addObject(array, Integer_Type);
	Integer* integerB = R_ObjectArray_addObject(array, Integer_Type);
	Integer* integerC = R_ObjectArray_addObject(array, Integer_Type);
	integerA->integer = 0;
	integerB->integer = 1;
	integerC->integer = 2;
	assert(R_ObjectArray_length(array) == 3);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,0))->integer) == 0);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,1))->integer) == 1);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,2))->integer) == 2);

	R_ObjectArray_swap(array,0,1);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,0))->integer) == 1);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,1))->integer) == 0);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,2))->integer) == 2);

	R_ObjectArray_swap(array,2,0);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,0))->integer) == 2);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,1))->integer) == 0);
	assert((((Integer*)R_ObjectArray_pointerAtIndex(array,2))->integer) == 1);

	R_Type_Delete(array);
}

void test_cleanup(void) {
	assert(R_Type_BytesAllocated == 0);
	Integer_Destructor_Called = 0;
	Integer_Constructor_Called = 0;

	R_ObjectArray* array = R_Type_New(R_ObjectArray_Type);
	R_ObjectArray_addObject(array, Integer_Type);
	R_ObjectArray_addObject(array, Integer_Type);
	R_ObjectArray_addObject(array, Integer_Type);
	R_Type_Delete(array);
	assert(Integer_Constructor_Called == 3);
	assert(Integer_Destructor_Called == 3);
	assert(R_Type_BytesAllocated == 0);

}

int main(void) {
	assert(R_Type_BytesAllocated == 0);
	test_allocations();
	test_integer();
	test_pop();
	test_swap();
	test_cleanup();

	printf("Pass\n");
}