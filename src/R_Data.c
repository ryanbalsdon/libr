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
#include "R_OS.h"
#include "R_Data.h"


struct R_Data {
	R_Type* type;
	uint8_t* buffer;
	uint8_t* head;
	size_t buffer_size;
	size_t array_size;
};

static R_Data* R_FUNCTION_ATTRIBUTES R_Data_Constructor(R_Data* self);
static R_Data* R_FUNCTION_ATTRIBUTES R_Data_Destructor(R_Data* self);
static R_Data* R_FUNCTION_ATTRIBUTES R_Data_Copier(R_Data* self, R_Data* new);
static void* R_FUNCTION_ATTRIBUTES R_Data_Methods(const R_Face* interface);
R_Type_Def(R_Data, R_Data_Constructor, R_Data_Destructor, R_Data_Copier, R_Data_Methods);

static void R_FUNCTION_ATTRIBUTES R_Data_increaseAllocationIfNeeded(R_Data* self, size_t spaceNeeded);

static R_Data* R_FUNCTION_ATTRIBUTES R_Data_Constructor(R_Data* self) {
	self->head = self->buffer = NULL;
	self->buffer_size = self->array_size = 0;
	return self;
}
static R_Data* R_FUNCTION_ATTRIBUTES R_Data_Destructor(R_Data* self) {
	os_free(self->buffer);
	self->head = self->buffer = NULL;
	self->buffer_size = self->array_size = 0;
	return self;
}
static R_Data* R_FUNCTION_ATTRIBUTES R_Data_Copier(R_Data* self, R_Data* new) {
	R_Data_appendArray(new, self);
	return new;
}
static void* R_FUNCTION_ATTRIBUTES R_Data_Methods(const R_Face* interface) {
	R_Face_DefJump(R_Puts, R_Data_puts);
	R_Face_DefJump(R_Equals, R_Data_isSame);
	return NULL;
}

R_Data* R_FUNCTION_ATTRIBUTES R_Data_reset(R_Data* self) {
	if (R_Type_IsNotOf(self, R_Data)) return NULL;
	self->head = self->buffer = (uint8_t*)os_realloc(self->buffer, 128*sizeof(uint8_t));
	//memset(self->buffer, 0, 128*sizeof(uint8_t));
	self->buffer_size = 128;
	self->array_size = 0;
	return self;
}

static void R_FUNCTION_ATTRIBUTES R_Data_increaseAllocationIfNeeded(R_Data* self, size_t space_needed) {
	if (R_Type_IsNotOf(self, R_Data)) return;
	size_t bytes_used_in_buffer = (size_t)(self->head - self->buffer) + self->array_size;
	if (self->buffer_size < bytes_used_in_buffer + space_needed) {
		self->buffer_size = bytes_used_in_buffer + space_needed;
		size_t head_offset = self->head - self->buffer;
		self->buffer = (uint8_t*)os_realloc(self->buffer, self->buffer_size);
		self->head = self->buffer + head_offset;
	}
}

R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendByte(R_Data* self, uint8_t byte) {
	if (R_Type_IsNotOf(self, R_Data)) return NULL;
	R_Data_increaseAllocationIfNeeded(self, sizeof(uint8_t));
	self->head[self->array_size++] = byte;
	return self;
}
R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendCArray(R_Data* self, const uint8_t* bytes, size_t count) {
	if (R_Type_IsNotOf(self, R_Data) || bytes == NULL || count == 0) return NULL;
	R_Data_increaseAllocationIfNeeded(self, count*sizeof(uint8_t));
	os_memcpy(self->head+self->array_size, bytes, count*sizeof(uint8_t));
	self->array_size+=count*sizeof(uint8_t);
	return self;
}

R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendArray(R_Data* self, const R_Data* array) {
	if (R_Type_IsNotOf(self, R_Data) || R_Type_IsNotOf(array, R_Data)) return NULL;
	return R_Data_appendCArray(self, R_Data_bytes(array), R_Data_size(array));
}

size_t R_FUNCTION_ATTRIBUTES R_Data_size(const R_Data* self) {
	if (R_Type_IsNotOf(self, R_Data)) return 0;
	return self->array_size;
}

const uint8_t* R_FUNCTION_ATTRIBUTES R_Data_bytes(const R_Data* self) {
	if (R_Type_IsNotOf(self, R_Data)) return NULL;
	return self->head;
}

R_Data* R_FUNCTION_ATTRIBUTES R_Data_push(R_Data* self, uint8_t byte) {
  return R_Data_appendByte(self, byte);
}

uint8_t R_FUNCTION_ATTRIBUTES R_Data_pop(R_Data* self) {
	if (R_Type_IsNotOf(self, R_Data)) return 0x00;
	return self->head[--self->array_size];
}

R_Data* R_FUNCTION_ATTRIBUTES R_Data_unshift(R_Data* self, uint8_t byte) {
	if (R_Type_IsNotOf(self, R_Data)) return NULL;
	if (self->head > self->buffer) {
		self->buffer--;
		*self->buffer = byte;
	}
	else {
		R_Data_increaseAllocationIfNeeded(self, sizeof(uint8_t));
		for (size_t i=self->array_size; i>0; i--) {
			self->head[i] = self->head[i-1];
		}
		self->head[0] = byte;
		self->array_size++;
	}
	return self;
}
uint8_t R_FUNCTION_ATTRIBUTES R_Data_shift(R_Data* self) {
	if (R_Type_IsNotOf(self, R_Data) || self->array_size == 0) return 0x00;
	uint8_t byte = *self->head;
	self->head++;
	self->array_size--;
	if (self->array_size == 0) self->head = self->buffer;
	return byte;
}

R_Data* R_FUNCTION_ATTRIBUTES R_Data_moveSubArray(R_Data* self, R_Data* array, size_t start, size_t length) {
	if (R_Type_IsNotOf(self, R_Data) || R_Type_IsNotOf(array, R_Data)) return NULL;
	if (start+length > R_Data_size(array)) return NULL;
	if (length == 0) length = R_Data_size(array) - start;
	R_Data_appendCArray(self, R_Data_bytes(array)+start, length);

	size_t remainingBytes = R_Data_size(array) - (start + length);
	for (int i=0; i<remainingBytes; i++) {
		array->head[start+i] = array->head[start+length+i];
	}
	array->array_size-=length;

	return self;
}

uint8_t R_FUNCTION_ATTRIBUTES R_Data_byte(const R_Data* self, size_t index) {
	if (R_Type_IsNotOf(self, R_Data) || index >= self->array_size) return 0;
	return self->head[index];
}

int R_FUNCTION_ATTRIBUTES R_Data_compare(const R_Data* self, const R_Data* comparor) {
	return R_Data_compareWithCArray(self, R_Data_bytes(comparor), R_Data_size(comparor));
}

int R_FUNCTION_ATTRIBUTES R_Data_compareWithCArray(const R_Data* self, const uint8_t* comparor, size_t bytes) {
	if (R_Type_IsNotOf(self, R_Data) || comparor == NULL) return -1;
	if (R_Data_size(self) < bytes) return -1;
	if (R_Data_size(self) > bytes) return -1;
	return os_memcmp(R_Data_bytes(self), comparor, bytes);
}
