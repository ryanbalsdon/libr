/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "R_ByteArray.h"


struct R_ByteArray {
	uint8_t * array;       //The actual array
    int arrayAllocationSize;//How large the internal array is. This is always as-large or larger than ArraySize.
    int arraySize;          //How many objects the user has added to the array.
};

static void R_ByteArray_increaseAllocationIfNeeded(R_ByteArray* self, size_t spaceNeeded);

R_ByteArray* R_ByteArray_alloc(void) {
	R_ByteArray* self = (R_ByteArray*)malloc(sizeof(R_ByteArray));
	self->array = (uint8_t*)malloc(128*sizeof(uint8_t));
    memset(self->array, 0, 128*sizeof(uint8_t));
    self->arrayAllocationSize = 128;
    self->arraySize = 0;

    return self;
}
R_ByteArray* R_ByteArray_reset(R_ByteArray* self) {
	self->array = (uint8_t*)realloc(self->array, 128*sizeof(uint8_t));
    memset(self->array, 0, 128*sizeof(uint8_t));
    self->arrayAllocationSize = 128;
    self->arraySize = 0;

    return self;
}
R_ByteArray* R_ByteArray_free(R_ByteArray* self) {
	free(self->array);
	free(self);
	return NULL;
}

static void R_ByteArray_increaseAllocationIfNeeded(R_ByteArray* self, size_t spaceNeeded) {
	while (self->arrayAllocationSize < self->arraySize + spaceNeeded) {
		//double allocation
		self->arrayAllocationSize *= 2;
		self->array = (uint8_t*)realloc(self->array, self->arrayAllocationSize);
	}
}

R_ByteArray* R_ByteArray_appendByte(R_ByteArray* self, uint8_t byte) {
	R_ByteArray_increaseAllocationIfNeeded(self, sizeof(uint8_t));
	self->array[self->arraySize++] = byte;
	return self;
}
R_ByteArray* R_ByteArray_appendCArray(R_ByteArray* self, const uint8_t* bytes, size_t count) {
	R_ByteArray_increaseAllocationIfNeeded(self, count*sizeof(uint8_t));
	memcpy(self->array+self->arraySize, bytes, count*sizeof(uint8_t));
	self->arraySize+=count*sizeof(uint8_t);
	return self;
}
R_ByteArray* R_ByteArray_appendArray(R_ByteArray* self, R_ByteArray* array) {
	return R_ByteArray_appendCArray(self, R_ByteArray_bytes(array), R_ByteArray_size(array));
}

size_t R_ByteArray_size(R_ByteArray* self) {
	return self->arraySize;
}
const uint8_t* R_ByteArray_bytes(R_ByteArray* self) {
	return self->array;
}