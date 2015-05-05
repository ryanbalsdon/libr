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
	R_Type* type;
	uint8_t* buffer;
	uint8_t* head;
	size_t buffer_size;
	size_t array_size;
};

static R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_Constructor(R_ByteArray* self);
static R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_Destructor(R_ByteArray* self);
static R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_Copier(R_ByteArray* self, R_ByteArray* new);
static void* R_FUNCTION_ATTRIBUTES R_ByteArray_Methods(const R_Face* interface);
R_Type_Def(R_ByteArray, R_ByteArray_Constructor, R_ByteArray_Destructor, R_ByteArray_Copier, R_ByteArray_Methods);

static void R_FUNCTION_ATTRIBUTES R_ByteArray_increaseAllocationIfNeeded(R_ByteArray* self, size_t spaceNeeded);

static R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_Constructor(R_ByteArray* self) {
	self->head = self->buffer = NULL;
	self->buffer_size = self->array_size = 0;
	return self;
}
static R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_Destructor(R_ByteArray* self) {
	os_free(self->buffer);
	self->head = self->buffer = NULL;
	self->buffer_size = self->array_size = 0;
	return self;
}
static R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_Copier(R_ByteArray* self, R_ByteArray* new) {
	R_ByteArray_appendArray(new, self);
	return new;
}
static void* R_FUNCTION_ATTRIBUTES R_ByteArray_Methods(const R_Face* interface) {
	R_Face_DefJump(R_Puts, R_ByteArray_puts);
	R_Face_DefJump(R_Equals, R_ByteArray_isSame);
	return NULL;
}

R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_reset(R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	self->head = self->buffer = (uint8_t*)os_realloc(self->buffer, 128*sizeof(uint8_t));
	//memset(self->buffer, 0, 128*sizeof(uint8_t));
	self->buffer_size = 128;
	self->array_size = 0;
	return self;
}

static void R_FUNCTION_ATTRIBUTES R_ByteArray_increaseAllocationIfNeeded(R_ByteArray* self, size_t space_needed) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return;
	size_t bytes_used_in_buffer = (size_t)(self->head - self->buffer) + self->array_size;
	if (self->buffer_size < bytes_used_in_buffer + space_needed) {
		self->buffer_size = bytes_used_in_buffer + space_needed;
		size_t head_offset = self->head - self->buffer;
		self->buffer = (uint8_t*)os_realloc(self->buffer, self->buffer_size);
		self->head = self->buffer + head_offset;
	}
}

R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_appendByte(R_ByteArray* self, uint8_t byte) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	R_ByteArray_increaseAllocationIfNeeded(self, sizeof(uint8_t));
	self->head[self->array_size++] = byte;
	return self;
}
R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_appendCArray(R_ByteArray* self, const uint8_t* bytes, size_t count) {
	if (R_Type_IsNotOf(self, R_ByteArray) || bytes == NULL || count == 0) return NULL;
	R_ByteArray_increaseAllocationIfNeeded(self, count*sizeof(uint8_t));
	os_memcpy(self->head+self->array_size, bytes, count*sizeof(uint8_t));
	self->array_size+=count*sizeof(uint8_t);
	return self;
}

R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_appendArray(R_ByteArray* self, const R_ByteArray* array) {
	if (R_Type_IsNotOf(self, R_ByteArray) || R_Type_IsNotOf(array, R_ByteArray)) return NULL;
	return R_ByteArray_appendCArray(self, R_ByteArray_bytes(array), R_ByteArray_size(array));
}

size_t R_FUNCTION_ATTRIBUTES R_ByteArray_size(const R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return 0;
	return self->array_size;
}

const uint8_t* R_FUNCTION_ATTRIBUTES R_ByteArray_bytes(const R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	return self->head;
}

R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_push(R_ByteArray* self, uint8_t byte) {
  return R_ByteArray_appendByte(self, byte);
}

uint8_t R_FUNCTION_ATTRIBUTES R_ByteArray_pop(R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return 0x00;
	return self->head[--self->array_size];
}

R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_unshift(R_ByteArray* self, uint8_t byte) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	if (self->head > self->buffer) {
		self->buffer--;
		*self->buffer = byte;
	}
	else {
		R_ByteArray_increaseAllocationIfNeeded(self, sizeof(uint8_t));
		for (size_t i=self->array_size; i>0; i--) {
			self->head[i] = self->head[i-1];
		}
		self->head[0] = byte;
		self->array_size++;
	}
	return self;
}
uint8_t R_FUNCTION_ATTRIBUTES R_ByteArray_shift(R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray) || self->array_size == 0) return 0x00;
	uint8_t byte = *self->head;
	self->head++;
	self->array_size--;
	if (self->array_size == 0) self->head = self->buffer;
	return byte;
}

R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_moveSubArray(R_ByteArray* self, R_ByteArray* array, size_t start, size_t length) {
	if (R_Type_IsNotOf(self, R_ByteArray) || R_Type_IsNotOf(array, R_ByteArray)) return NULL;
	if (start+length > R_ByteArray_size(array)) return NULL;
	if (length == 0) length = R_ByteArray_size(array) - start;
	R_ByteArray_appendCArray(self, R_ByteArray_bytes(array)+start, length);

	size_t remainingBytes = R_ByteArray_size(array) - (start + length);
	for (int i=0; i<remainingBytes; i++) {
		array->head[start+i] = array->head[start+length+i];
	}
	array->array_size-=length;

	return self;
}

uint8_t R_FUNCTION_ATTRIBUTES R_ByteArray_byte(const R_ByteArray* self, size_t index) {
	if (R_Type_IsNotOf(self, R_ByteArray) || index >= self->array_size) return 0;
	return self->head[index];
}

int R_FUNCTION_ATTRIBUTES R_ByteArray_compare(const R_ByteArray* self, const R_ByteArray* comparor) {
	return R_ByteArray_compareWithCArray(self, R_ByteArray_bytes(comparor), R_ByteArray_size(comparor));
}

int R_FUNCTION_ATTRIBUTES R_ByteArray_compareWithCArray(const R_ByteArray* self, const uint8_t* comparor, size_t bytes) {
	if (R_Type_IsNotOf(self, R_ByteArray) || comparor == NULL) return -1;
	if (R_ByteArray_size(self) < bytes) return -1;
	if (R_ByteArray_size(self) > bytes) return -1;
	return os_memcmp(R_ByteArray_bytes(self), comparor, bytes);
}
