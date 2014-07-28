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
#include "R_MutablePointerArray.h"

int allocatorWasRun = 0;
int* allocator() {
	allocatorWasRun++;
	return malloc(sizeof(int));
}
int deallocatorWasRun = 0;
int* deallocator(int* pointer) {
	deallocatorWasRun++;
	free(pointer);
	return NULL;
}

int main(void) {
	//printf("Alloc Test 1\n");
	R_MutablePointerArray* array = NULL;
	array = R_MutablePointerArray_alloc();
	array = R_MutablePointerArray_free(array);
	assert(array == NULL);

	//printf("Alloc Test 2\n");
	array = R_MutablePointerArray_free(R_MutablePointerArray_reset(R_MutablePointerArray_alloc()));
	assert(array==NULL);

	//printf("Add Tests\n");
	array = R_MutablePointerArray_alloc();
	assert(R_MutablePointerArray_length(array) == 0);
	char testStringA[] = "TestA";
	char testStringB[] = "TestB: The Ultra Test";
	char testStringC[] = "CCCCCC";
	char* pointer = R_MutablePointerArray_addPointer(array, sizeof(testStringA));
	assert(R_MutablePointerArray_length(array) == 1);
	strcpy(pointer, testStringA);
	pointer = R_MutablePointerArray_addPointer(array, sizeof(testStringB));
	assert(R_MutablePointerArray_length(array) == 2);
	strcpy(pointer, testStringB);
	pointer = R_MutablePointerArray_addPointer(array, sizeof(testStringB));
	assert(R_MutablePointerArray_length(array) == 3);
	strcpy(pointer, testStringC);

	//printf("Retrieve Tests\n");
	pointer = R_MutablePointerArray_pointerAtIndex(array, 0);
	assert(strcmp(pointer, testStringA) == 0);
	pointer = R_MutablePointerArray_pointerAtIndex(array, 1);
	assert(strcmp(pointer, testStringB) == 0);
	pointer = R_MutablePointerArray_lastPointer(array);
	assert(strcmp(pointer, testStringC) == 0);
	assert(R_MutablePointerArray_indexOfPointer(array, pointer) == 2);

	//printf("Swap Tests\n");
	R_MutablePointerArray_swap(array,0,1);
	pointer = R_MutablePointerArray_pointerAtIndex(array, 0);
	assert(strcmp(pointer, testStringB) == 0);
	pointer = R_MutablePointerArray_pointerAtIndex(array, 1);
	assert(strcmp(pointer, testStringA) == 0);
	pointer = R_MutablePointerArray_pointerAtIndex(array, 2);
	assert(strcmp(pointer, testStringC) == 0);

	//printf("Remove Tests\n");
	R_MutablePointerArray_pop(array);
	assert(R_MutablePointerArray_length(array) == 2);
	pointer = R_MutablePointerArray_pointerAtIndex(array, 0);
	assert(strcmp(pointer, testStringB) == 0);
	pointer = R_MutablePointerArray_pointerAtIndex(array, 1);
	assert(strcmp(pointer, testStringA) == 0);
	pointer = R_MutablePointerArray_addPointer(array, sizeof(testStringC));
	strcpy(pointer, testStringC);

	R_MutablePointerArray_removePointer(array, R_MutablePointerArray_pointerAtIndex(array,1));
	assert(R_MutablePointerArray_length(array) == 2);
	pointer = R_MutablePointerArray_pointerAtIndex(array, 0);
	assert(strcmp(pointer, testStringB) == 0);
	pointer = R_MutablePointerArray_pointerAtIndex(array, 1);
	assert(strcmp(pointer, testStringC) == 0);
	pointer = R_MutablePointerArray_addPointer(array, sizeof(testStringA));
	strcpy(pointer, testStringA);

	R_MutablePointerArray_removeIndex(array,0);
	assert(R_MutablePointerArray_length(array) == 2);
	pointer = R_MutablePointerArray_pointerAtIndex(array, 0);
	assert(strcmp(pointer, testStringC) == 0);
	pointer = R_MutablePointerArray_pointerAtIndex(array, 1);
	assert(strcmp(pointer, testStringA) == 0);
	pointer = R_MutablePointerArray_addPointer(array, sizeof(testStringB));
	strcpy(pointer, testStringB);

	R_MutablePointerArray_removeAll(array);
	assert(R_MutablePointerArray_length(array) == 0);

	//printf("Manager Tests\n");
	array = R_MutablePointerArray_reset(array);
	assert(R_MutablePointerArray_length(array) == 0);
	int* managedPointer = R_MutablePointerArray_addManagedPointer(array, (R_MutablePointerArray_Allocator)allocator);
	assert(allocatorWasRun == 1);
	assert(R_MutablePointerArray_length(array) == 1);
	*managedPointer = 5;
	managedPointer = R_MutablePointerArray_addManagedPointer(array, (R_MutablePointerArray_Allocator)allocator);
	assert(allocatorWasRun == 2);
	assert(R_MutablePointerArray_length(array) == 2);
	*managedPointer = 10;
	assert(*(int*)(R_MutablePointerArray_pointerAtIndex(array, 0)) == 5);
	assert(*(int*)(R_MutablePointerArray_pointerAtIndex(array, 1)) == 10);

	R_MutablePointerArray_removeManagedIndex(array, (R_MutablePointerArray_Deallocator)deallocator, 0);
	assert(R_MutablePointerArray_length(array) == 1);
	assert(*(int*)(R_MutablePointerArray_pointerAtIndex(array, 0)) == 10);
	assert(deallocatorWasRun == 1);
	managedPointer = R_MutablePointerArray_addManagedPointer(array, (R_MutablePointerArray_Allocator)allocator);
	*managedPointer = 15;
	assert(allocatorWasRun == 3);
	assert(R_MutablePointerArray_length(array) == 2);
	R_MutablePointerArray_removeManagedPointer(array, (R_MutablePointerArray_Deallocator)deallocator, managedPointer);
	assert(deallocatorWasRun == 2);
	assert(R_MutablePointerArray_length(array) == 1);
	assert(*(int*)(R_MutablePointerArray_pointerAtIndex(array, 0)) == 10);
	managedPointer = R_MutablePointerArray_addManagedPointer(array, (R_MutablePointerArray_Allocator)allocator);
	*managedPointer = 20;
	assert(allocatorWasRun == 4);
	assert(R_MutablePointerArray_length(array) == 2);
	R_MutablePointerArray_removeManagedAll(array, (R_MutablePointerArray_Deallocator)deallocator);
	assert(deallocatorWasRun == 4);
	assert(R_MutablePointerArray_length(array) == 0);


	printf("Pass\n");
}