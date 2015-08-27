#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "R_OS.h"
#include "R_MutableData.h"


struct R_MutableData {
	R_Type* type;
	R_Data data;
	uint8_t* allocated_buffer;
	size_t allocated_size;
};

static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Constructor(R_MutableData* self);
static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Destructor(R_MutableData* self);
static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Copier(R_MutableData* self, R_MutableData* new);
static R_JumpTable_Entry methods[] = {
  R_JumpTable_Entry_Make(R_Stringify, R_MutableData_stringify),
  R_JumpTable_Entry_Make(R_Equals, R_MutableData_isSame),
  R_JumpTable_Entry_NULL
};
R_Type_Def(R_MutableData, R_MutableData_Constructor, R_MutableData_Destructor, R_MutableData_Copier, methods);

static void R_FUNCTION_ATTRIBUTES R_MutableData_increaseAllocationIfNeeded(R_MutableData* self, size_t spaceNeeded);

static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Constructor(R_MutableData* self) {
	self->data.type = R_Type_Object(R_Data);
	self->data.bytes = self->allocated_buffer = NULL;
	self->data.size = self->allocated_size = 0;
	return self;
}
static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Destructor(R_MutableData* self) {
	os_free(self->allocated_buffer);
	self->data.bytes = self->allocated_buffer = NULL;
	self->data.size = self->allocated_size = 0;
	return self;
}
static R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_Copier(R_MutableData* self, R_MutableData* new) {
	R_MutableData_appendArray(new, self);
	return new;
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_reset(R_MutableData* self) {
	if (R_Type_IsNotOf(self, R_MutableData)) return NULL;
	self->data.bytes = self->allocated_buffer = (uint8_t*)os_realloc(self->allocated_buffer, 128*sizeof(uint8_t));
	self->allocated_size = 128;
	self->data.size = 0;
	return self;
}

static void R_FUNCTION_ATTRIBUTES R_MutableData_increaseAllocationIfNeeded(R_MutableData* self, size_t space_needed) {
	if (R_Type_IsNotOf(self, R_MutableData)) return;
	size_t bytes_used_in_buffer = (size_t)(self->data.bytes - self->allocated_buffer) + self->data.size;
	if (self->allocated_size < bytes_used_in_buffer + space_needed) {
		self->allocated_size = bytes_used_in_buffer + space_needed;
		size_t head_offset = self->data.bytes - self->allocated_buffer;
		self->allocated_buffer = (uint8_t*)os_realloc(self->allocated_buffer, self->allocated_size);
		self->data.bytes = self->allocated_buffer + head_offset;
	}
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendByte(R_MutableData* self, uint8_t byte) {
	if (R_Type_IsNotOf(self, R_MutableData)) return NULL;
	R_MutableData_increaseAllocationIfNeeded(self, sizeof(uint8_t));
	self->data.bytes[self->data.size++] = byte;
	return self;
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendCArray(R_MutableData* self, const uint8_t* bytes, size_t count) {
	if (R_Type_IsNotOf(self, R_MutableData) || bytes == NULL || count == 0) return NULL;
	R_MutableData_increaseAllocationIfNeeded(self, count*sizeof(uint8_t));
	os_memcpy(self->data.bytes+self->data.size, bytes, count*sizeof(uint8_t));
	self->data.size+=count*sizeof(uint8_t);
	return self;
}

size_t R_FUNCTION_ATTRIBUTES R_MutableData_size(const R_MutableData* self) {
	if (R_Type_IsNotOf(self, R_MutableData)) return 0;
	return self->data.size;
}

const uint8_t* R_FUNCTION_ATTRIBUTES R_MutableData_bytes(const R_MutableData* self) {
	if (R_Type_IsNotOf(self, R_MutableData)) return NULL;
	return self->data.bytes;
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_push(R_MutableData* self, uint8_t byte) {
  return R_MutableData_appendByte(self, byte);
}

uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_pop(R_MutableData* self) {
	if (R_Type_IsNotOf(self, R_MutableData)) return 0x00;
	return self->data.bytes[--self->data.size];
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_unshift(R_MutableData* self, uint8_t byte) {
	if (R_Type_IsNotOf(self, R_MutableData)) return NULL;
	if (self->data.bytes > self->allocated_buffer) {
		self->data.bytes--;
		*self->data.bytes = byte;
	}
	else {
		R_MutableData_increaseAllocationIfNeeded(self, sizeof(uint8_t));
		for (size_t i=self->data.size; i>0; i--) {
			self->data.bytes[i] = self->data.bytes[i-1];
		}
		self->data.bytes[0] = byte;
		self->data.size++;
	}
	return self;
}
uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_shift(R_MutableData* self) {
	if (R_Type_IsNotOf(self, R_MutableData) || self->data.size == 0) return 0x00;
	uint8_t byte = *self->data.bytes;
	self->data.bytes++;
	self->data.size--;
	if (self->data.size == 0) self->data.bytes = self->allocated_buffer;
	return byte;
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_moveSubArray(R_MutableData* self, R_MutableData* array, size_t start, size_t length) {
	if (R_Type_IsNotOf(self, R_MutableData) || R_Type_IsNotOf(array, R_MutableData)) return NULL;
	if (start+length > R_MutableData_size(array)) return NULL;
	if (length == 0) length = R_MutableData_size(array) - start;
	R_MutableData_appendCArray(self, R_MutableData_bytes(array)+start, length);

	size_t remainingBytes = R_MutableData_size(array) - (start + length);
	for (int i=0; i<remainingBytes; i++) {
		array->data.bytes[start+i] = array->data.bytes[start+length+i];
	}
	array->data.size-=length;

	return self;
}

uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_byte(const R_MutableData* self, size_t index) {
	if (R_Type_IsNotOf(self, R_MutableData) || index >= self->data.size) return 0;
	return self->data.bytes[index];
}

const R_Data* R_FUNCTION_ATTRIBUTES R_MutableData_data(const R_MutableData* self) {
	if (R_Type_IsNotOf(self, R_MutableData)) return NULL;
	return &self->data;
}
