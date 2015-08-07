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
#include "R_MutableData.h"


struct R_MutableData {
	R_Type* type;
	uint8_t* buffer;
	uint8_t* head;
	size_t buffer_size;
	size_t array_size;
};

static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Constructor(R_MutableData* self);
static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Destructor(R_MutableData* self);
static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Copier(R_MutableData* self, R_MutableData* new);
static R_JumpTable_Entry methods[] = {
  R_JumpTable_Entry_Make(R_Puts, R_MutableData_puts),
  R_JumpTable_Entry_Make(R_Equals, R_MutableData_isSame),
  R_JumpTable_Entry_NULL
};
R_Type_Def(R_MutableData, R_MutableData_Constructor, R_MutableData_Destructor, R_MutableData_Copier, methods);

static void R_FUNCTION_ATTRIBUTES R_MutableData_increaseAllocationIfNeeded(R_MutableData* self, size_t spaceNeeded);

static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Constructor(R_MutableData* self) {
	self->head = self->buffer = NULL;
	self->buffer_size = self->array_size = 0;
	return self;
}
static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Destructor(R_MutableData* self) {
	os_free(self->buffer);
	self->head = self->buffer = NULL;
	self->buffer_size = self->array_size = 0;
	return self;
}
static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Copier(R_MutableData* self, R_MutableData* new) {
	R_MutableData_appendArray(new, self);
	return new;
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_reset(R_MutableData* self) {
	if (R_Type_IsNotOf(self, R_MutableData)) return NULL;
	self->head = self->buffer = (uint8_t*)os_realloc(self->buffer, 128*sizeof(uint8_t));
	//memset(self->buffer, 0, 128*sizeof(uint8_t));
	self->buffer_size = 128;
	self->array_size = 0;
	return self;
}

static void R_FUNCTION_ATTRIBUTES R_MutableData_increaseAllocationIfNeeded(R_MutableData* self, size_t space_needed) {
	if (R_Type_IsNotOf(self, R_MutableData)) return;
	size_t bytes_used_in_buffer = (size_t)(self->head - self->buffer) + self->array_size;
	if (self->buffer_size < bytes_used_in_buffer + space_needed) {
		self->buffer_size = bytes_used_in_buffer + space_needed;
		size_t head_offset = self->head - self->buffer;
		self->buffer = (uint8_t*)os_realloc(self->buffer, self->buffer_size);
		self->head = self->buffer + head_offset;
	}
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendByte(R_MutableData* self, uint8_t byte) {
	if (R_Type_IsNotOf(self, R_MutableData)) return NULL;
	R_MutableData_increaseAllocationIfNeeded(self, sizeof(uint8_t));
	self->head[self->array_size++] = byte;
	return self;
}
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendCArray(R_MutableData* self, const uint8_t* bytes, size_t count) {
	if (R_Type_IsNotOf(self, R_MutableData) || bytes == NULL || count == 0) return NULL;
	R_MutableData_increaseAllocationIfNeeded(self, count*sizeof(uint8_t));
	os_memcpy(self->head+self->array_size, bytes, count*sizeof(uint8_t));
	self->array_size+=count*sizeof(uint8_t);
	return self;
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendArray(R_MutableData* self, const R_MutableData* array) {
	if (R_Type_IsNotOf(self, R_MutableData) || R_Type_IsNotOf(array, R_MutableData)) return NULL;
	return R_MutableData_appendCArray(self, R_MutableData_bytes(array), R_MutableData_size(array));
}

size_t R_FUNCTION_ATTRIBUTES R_MutableData_size(const R_MutableData* self) {
	if (R_Type_IsNotOf(self, R_MutableData)) return 0;
	return self->array_size;
}

const uint8_t* R_FUNCTION_ATTRIBUTES R_MutableData_bytes(const R_MutableData* self) {
	if (R_Type_IsNotOf(self, R_MutableData)) return NULL;
	return self->head;
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_push(R_MutableData* self, uint8_t byte) {
  return R_MutableData_appendByte(self, byte);
}

uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_pop(R_MutableData* self) {
	if (R_Type_IsNotOf(self, R_MutableData)) return 0x00;
	return self->head[--self->array_size];
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_unshift(R_MutableData* self, uint8_t byte) {
	if (R_Type_IsNotOf(self, R_MutableData)) return NULL;
	if (self->head > self->buffer) {
		self->buffer--;
		*self->buffer = byte;
	}
	else {
		R_MutableData_increaseAllocationIfNeeded(self, sizeof(uint8_t));
		for (size_t i=self->array_size; i>0; i--) {
			self->head[i] = self->head[i-1];
		}
		self->head[0] = byte;
		self->array_size++;
	}
	return self;
}
uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_shift(R_MutableData* self) {
	if (R_Type_IsNotOf(self, R_MutableData) || self->array_size == 0) return 0x00;
	uint8_t byte = *self->head;
	self->head++;
	self->array_size--;
	if (self->array_size == 0) self->head = self->buffer;
	return byte;
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_moveSubArray(R_MutableData* self, R_MutableData* array, size_t start, size_t length) {
	if (R_Type_IsNotOf(self, R_MutableData) || R_Type_IsNotOf(array, R_MutableData)) return NULL;
	if (start+length > R_MutableData_size(array)) return NULL;
	if (length == 0) length = R_MutableData_size(array) - start;
	R_MutableData_appendCArray(self, R_MutableData_bytes(array)+start, length);

	size_t remainingBytes = R_MutableData_size(array) - (start + length);
	for (int i=0; i<remainingBytes; i++) {
		array->head[start+i] = array->head[start+length+i];
	}
	array->array_size-=length;

	return self;
}

uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_byte(const R_MutableData* self, size_t index) {
	if (R_Type_IsNotOf(self, R_MutableData) || index >= self->array_size) return 0;
	return self->head[index];
}

int R_FUNCTION_ATTRIBUTES R_MutableData_compare(const R_MutableData* self, const R_MutableData* comparor) {
	return R_MutableData_compareWithCArray(self, R_MutableData_bytes(comparor), R_MutableData_size(comparor));
}

int R_FUNCTION_ATTRIBUTES R_MutableData_compareWithCArray(const R_MutableData* self, const uint8_t* comparor, size_t bytes) {
	if (R_Type_IsNotOf(self, R_MutableData) || comparor == NULL) return -1;
	if (R_MutableData_size(self) < bytes) return -1;
	if (R_MutableData_size(self) > bytes) return -1;
	return os_memcmp(R_MutableData_bytes(self), comparor, bytes);
}
