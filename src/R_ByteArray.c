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

static R_ByteArray* R_ByteArray_Constructor(R_ByteArray* self);
static R_ByteArray* R_ByteArray_Destructor(R_ByteArray* self);
static R_ByteArray* R_ByteArray_Copier(R_ByteArray* self, R_ByteArray* new);
static void* R_ByteArray_Methods(const R_Face* interface);
R_Type_Def(R_ByteArray, R_ByteArray_Constructor, R_ByteArray_Destructor, R_ByteArray_Copier, R_ByteArray_Methods);

static void R_ByteArray_increaseAllocationIfNeeded(R_ByteArray* self, size_t spaceNeeded);

static R_ByteArray* R_ByteArray_Constructor(R_ByteArray* self) {
	self->head = self->buffer = NULL;
	self->buffer_size = self->array_size = 0;
	return self;
}
static R_ByteArray* R_ByteArray_Destructor(R_ByteArray* self) {
	free(self->buffer);
	self->head = self->buffer = NULL;
	self->buffer_size = self->array_size = 0;
	return self;
}
static R_ByteArray* R_ByteArray_Copier(R_ByteArray* self, R_ByteArray* new) {
	R_ByteArray_appendArray(new, self);
	return new;
}
static void* R_ByteArray_Methods(const R_Face* interface) {
	R_Face_DefJump(R_Puts, R_ByteArray_puts);
	return NULL;
}

R_ByteArray* R_ByteArray_reset(R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	self->head = self->buffer = (uint8_t*)realloc(self->buffer, 128*sizeof(uint8_t));
	//memset(self->buffer, 0, 128*sizeof(uint8_t));
	self->buffer_size = 128;
	self->array_size = 0;
	return self;
}

static void R_ByteArray_increaseAllocationIfNeeded(R_ByteArray* self, size_t space_needed) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return;
	size_t bytes_used_in_buffer = (size_t)(self->head - self->buffer) + self->array_size;
	if (self->buffer_size < bytes_used_in_buffer + space_needed) {
		self->buffer_size = bytes_used_in_buffer + space_needed;
		size_t head_offset = self->head - self->buffer;
		self->buffer = (uint8_t*)realloc(self->buffer, self->buffer_size);
		self->head = self->buffer + head_offset;
	}
}

void R_ByteArray_puts(R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return;
	printf("{");
	for (int i=0; i<R_ByteArray_size(self); i++) {
		printf("0x%02X", R_ByteArray_byte(self, i));
		if (i < R_ByteArray_size(self)-1) printf(", ");
	}
	printf("}\n");
}

R_ByteArray* R_ByteArray_appendByte(R_ByteArray* self, uint8_t byte) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	R_ByteArray_increaseAllocationIfNeeded(self, sizeof(uint8_t));
	self->head[self->array_size++] = byte;
	return self;
}
R_ByteArray* R_ByteArray_appendCArray(R_ByteArray* self, const uint8_t* bytes, size_t count) {
	if (R_Type_IsNotOf(self, R_ByteArray) || bytes == NULL || count == 0) return NULL;
	R_ByteArray_increaseAllocationIfNeeded(self, count*sizeof(uint8_t));
	memcpy(self->head+self->array_size, bytes, count*sizeof(uint8_t));
	self->array_size+=count*sizeof(uint8_t);
	return self;
}
R_ByteArray* R_ByteArray_appendArray(R_ByteArray* self, const R_ByteArray* array) {
	if (R_Type_IsNotOf(self, R_ByteArray) || R_Type_IsNotOf(array, R_ByteArray)) return NULL;
	return R_ByteArray_appendCArray(self, R_ByteArray_bytes(array), R_ByteArray_size(array));
}

size_t R_ByteArray_size(const R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return 0;
	return self->array_size;
}
const uint8_t* R_ByteArray_bytes(const R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	return self->head;
}

R_ByteArray* R_ByteArray_push(R_ByteArray* self, uint8_t byte) {
	return R_ByteArray_appendByte(self, byte);
}
uint8_t R_ByteArray_pop(R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return 0x00;
	return self->head[--self->array_size];
}
R_ByteArray* R_ByteArray_unshift(R_ByteArray* self, uint8_t byte) {
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
uint8_t R_ByteArray_shift(R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray) || self->array_size == 0) return 0x00;
	uint8_t byte = *self->head;
	self->head++;
	self->array_size--;
	if (self->array_size == 0) self->head = self->buffer;
	return byte;
}

R_ByteArray* R_ByteArray_moveSubArray(R_ByteArray* self, R_ByteArray* array, size_t start, size_t length) {
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

uint8_t R_ByteArray_byte(const R_ByteArray* self, size_t index) {
	if (R_Type_IsNotOf(self, R_ByteArray) || index >= self->array_size) return 0;
	return self->head[index];
}

uint8_t R_ByteArray_first(const R_ByteArray* self) {
	return R_ByteArray_byte(self, 0);
}

uint8_t R_ByteArray_last(const R_ByteArray* self) {
	return R_ByteArray_byte(self, self->array_size - 1);
}


int R_ByteArray_compare(const R_ByteArray* self, const R_ByteArray* comparor) {
	return R_ByteArray_compareWithCArray(self, R_ByteArray_bytes(comparor), R_ByteArray_size(comparor));
}

int R_ByteArray_compareWithCArray(const R_ByteArray* self, const uint8_t* comparor, size_t bytes) {
	if (R_Type_IsNotOf(self, R_ByteArray) || comparor == NULL) return -1;
	if (R_ByteArray_size(self) < bytes) return -1;
	if (R_ByteArray_size(self) > bytes) return -1;
	return memcmp(R_ByteArray_bytes(self), comparor, bytes);
}

static uint8_t hex_to_byte(char hex1, char hex2);
static uint8_t hex_to_nibble(char hex);
R_ByteArray* R_ByteArray_appendHexCString(R_ByteArray* self, const char* hex) {
	if (R_Type_IsNotOf(self, R_ByteArray) || hex == NULL) return NULL;
	size_t length = strlen(hex);
	int odd_offset = length%2;
	if (odd_offset) R_ByteArray_appendByte(self, hex_to_byte('0', hex[0]));
	for (int i=odd_offset; i<length; i+=2) {
		R_ByteArray_appendByte(self, hex_to_byte(hex[i], hex[i+1]));
	}
	return self;
}
R_ByteArray* R_ByteArray_appendHexString(R_ByteArray* self, R_String* hex) {
	if (R_Type_IsNotOf(self, R_ByteArray) || R_Type_IsNotOf(hex, R_String)) return NULL;
	int odd_offset = R_String_length(hex)%2;
	if (odd_offset) R_ByteArray_appendByte(self, hex_to_byte('0', R_String_getString(hex)[0]));
	for (int i=odd_offset; i<R_String_length(hex); i+=2) {
		R_ByteArray_appendByte(self, hex_to_byte(R_String_getString(hex)[i], R_String_getString(hex)[i+1]));
	}
	return self;
}
static uint8_t hex_to_byte(char hex1, char hex2) {
	return (hex_to_nibble(hex1) << 4) + hex_to_nibble(hex2);
}
static uint8_t hex_to_nibble(char hex) {
	if (hex >= '0' && hex <= '9')
		return 0x0 + (uint8_t)hex - (uint8_t)'0';
	else if (hex >= 'a' && hex <= 'f')
		return 0xa + (uint8_t)hex - (uint8_t)'a';
	else if (hex >= 'A' && hex <= 'F')
		return 0xA + (uint8_t)hex - (uint8_t)'A';
	else return 0x0;
}

R_ByteArray* R_ByteArray_setByte(R_ByteArray* self, uint8_t byte) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendByte(self, byte);
}
R_ByteArray* R_ByteArray_setCArray(R_ByteArray* self, const uint8_t* bytes, size_t count) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendCArray(self, bytes, count);
}
R_ByteArray* R_ByteArray_setArray(R_ByteArray* self, const R_ByteArray* array) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendArray(self, array);
}
R_ByteArray* R_ByteArray_setHexString(R_ByteArray* self, R_String* hex) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendHexString(self, hex);
}
R_ByteArray* R_ByteArray_setHexCString(R_ByteArray* self, const char* hex) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendHexCString(self, hex);
}
R_ByteArray* R_ByteArray_setUInt32(R_ByteArray* self, uint32_t value) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendUInt32(self, value);
}

R_ByteArray* R_ByteArray_appendUInt32(R_ByteArray* self, uint32_t value) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return NULL;
	if ((value & 0xFF000000) != 0) R_ByteArray_appendByte(self, (value & 0xFF000000) >> 24);
	if ((value & 0xFFFF0000) != 0) R_ByteArray_appendByte(self, (value & 0x00FF0000) >> 16);
	if ((value & 0xFFFFFF00) != 0) R_ByteArray_appendByte(self, (value & 0x0000FF00) >>  8);
	if ((value & 0xFFFFFFFF) != 0) R_ByteArray_appendByte(self, (value & 0x000000FF) >>  0);
	return self;
}

uint32_t R_ByteArray_getUInt32(R_ByteArray* self) {
	if (R_Type_IsNotOf(self, R_ByteArray)) return 0;
	uint32_t integer = (uint32_t)R_ByteArray_byte(self, 0);
	if (R_ByteArray_size(self) > 1) integer = R_ByteArray_byte(self, 1) + (integer << 8);
	if (R_ByteArray_size(self) > 2) integer = R_ByteArray_byte(self, 2) + (integer << 8);
	if (R_ByteArray_size(self) > 3) integer = R_ByteArray_byte(self, 3) + (integer << 8);
	return integer;
}

R_ByteArray* R_ByteArray_appendUInt32AsBCD(R_ByteArray* self, uint32_t value) {
	//Construct the array first in reverse, then append it to self
	R_ByteArray* reversed = R_Type_New(R_ByteArray);
	if (self == NULL || reversed == NULL) return NULL;
	do {
		uint8_t right_digit = value % 10;
		value /= 10;
		uint8_t left_digit = value % 10;
		value /= 10;
		R_ByteArray_appendByte(reversed, (left_digit << 4) | right_digit);
	} while (value > 0);

	for (int i=(int)R_ByteArray_size(reversed)-1; i>=0; i--) {
		R_ByteArray_appendByte(self, R_ByteArray_byte(reversed, i));
	}

	R_Type_Delete(reversed);
	return self;
}
