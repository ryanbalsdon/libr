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
  uint8_t * array;       //The actual array
  int arrayAllocationSize;//How large the internal array is. This is always as-large or larger than ArraySize.
  int arraySize;          //How many objects the user has added to the array.
};

static R_ByteArray* R_ByteArray_Constructor(R_ByteArray* self);
static R_ByteArray* R_ByteArray_Destructor(R_ByteArray* self);
static void R_ByteArray_Copier(R_ByteArray* self, R_ByteArray* new);
R_Type_Def(R_ByteArray, R_ByteArray_Constructor, R_ByteArray_Destructor, R_ByteArray_Copier);

static void R_ByteArray_increaseAllocationIfNeeded(R_ByteArray* self, size_t spaceNeeded);

static R_ByteArray* R_ByteArray_Constructor(R_ByteArray* self) {
	self->array = NULL;
	self->arrayAllocationSize = 0;
	self->arraySize = 0;

	return self;
}
static R_ByteArray* R_ByteArray_Destructor(R_ByteArray* self) {
	free(self->array);
	return self;
}
static void R_ByteArray_Copier(R_ByteArray* self, R_ByteArray* new) {
	R_ByteArray_appendArray(new, self);
}

R_ByteArray* R_ByteArray_reset(R_ByteArray* self) {
	self->array = (uint8_t*)realloc(self->array, 128*sizeof(uint8_t));
	memset(self->array, 0, 128*sizeof(uint8_t));
	self->arrayAllocationSize = 128;
	self->arraySize = 0;

	return self;
}

static void R_ByteArray_increaseAllocationIfNeeded(R_ByteArray* self, size_t spaceNeeded) {
	if (self->arrayAllocationSize < self->arraySize + spaceNeeded) {
		self->arrayAllocationSize = self->arraySize + spaceNeeded;
		self->array = (uint8_t*)realloc(self->array, self->arrayAllocationSize);
	}
}

R_ByteArray* R_ByteArray_appendByte(R_ByteArray* self, uint8_t byte) {
	R_ByteArray_increaseAllocationIfNeeded(self, sizeof(uint8_t));
	self->array[self->arraySize++] = byte;
	return self;
}
R_ByteArray* R_ByteArray_appendCArray(R_ByteArray* self, const uint8_t* bytes, size_t count) {
	if (self == NULL || bytes == NULL || count == 0) return self;
	R_ByteArray_increaseAllocationIfNeeded(self, count*sizeof(uint8_t));
	memcpy(self->array+self->arraySize, bytes, count*sizeof(uint8_t));
	self->arraySize+=count*sizeof(uint8_t);
	return self;
}
R_ByteArray* R_ByteArray_appendArray(R_ByteArray* self, const R_ByteArray* array) {
	if (array == NULL || self == NULL) return self;
	return R_ByteArray_appendCArray(self, R_ByteArray_bytes(array), R_ByteArray_size(array));
}

size_t R_ByteArray_size(const R_ByteArray* self) {
	if (self == NULL) return 0;
	return self->arraySize;
}
const uint8_t* R_ByteArray_bytes(const R_ByteArray* self) {
	if (self == NULL) return NULL;
	return self->array;
}

R_ByteArray* R_ByteArray_push(R_ByteArray* self, uint8_t byte) {
	return R_ByteArray_appendByte(self, byte);
}
uint8_t R_ByteArray_pop(R_ByteArray* self) {
	return self->array[--self->arraySize];
}
R_ByteArray* R_ByteArray_unshift(R_ByteArray* self, uint8_t byte) {
	if (self == NULL) return NULL;
	R_ByteArray_increaseAllocationIfNeeded(self, sizeof(uint8_t));
	for (int i=self->arraySize; i>0; i--) {
		self->array[i] = self->array[i-1];
	}
	self->array[0] = byte;
	self->arraySize++;

	return self;
}
uint8_t R_ByteArray_shift(R_ByteArray* self) {
	if (self->arraySize>0) {
		uint8_t returnVal = self->array[0];
		for (int i=0; i<self->arraySize-1; i++) {
			self->array[i] = self->array[i+1];
		}
		self->arraySize--;
		return returnVal;
	}
	return 0x00;
}

size_t R_ByteArray_moveSubArray(R_ByteArray* self, R_ByteArray* array, size_t start, size_t length) {
	if (self == NULL || array == NULL) return 0;
	if (start+length > R_ByteArray_size(array)) return 0;
	if (length == 0) length = R_ByteArray_size(array) - start;
	R_ByteArray_appendCArray(self, R_ByteArray_bytes(array)+start, length);

	size_t remainingBytes = R_ByteArray_size(array) - (start + length);
	for (int i=0; i<remainingBytes; i++) {
		array->array[start+i] = array->array[start+length+i];
	}
	array->arraySize-=length;

	return length;
}

uint8_t R_ByteArray_byte(const R_ByteArray* self, size_t index) {
	if (self == NULL || index >= self->arraySize) return 0;
	return self->array[index];
}

uint8_t R_ByteArray_first(const R_ByteArray* self) {
	return R_ByteArray_byte(self, 0);
}

uint8_t R_ByteArray_last(const R_ByteArray* self) {
	return R_ByteArray_byte(self, self->arraySize - 1);
}


int R_ByteArray_compare(R_ByteArray* self, const R_ByteArray* comparor) {
	size_t lowerSize = (R_ByteArray_size(self) < R_ByteArray_size(comparor)) ? R_ByteArray_size(self) : R_ByteArray_size(comparor);
	int decision = memcmp(R_ByteArray_bytes(self), R_ByteArray_bytes(comparor), lowerSize);
	if (decision != 0) return decision;
	if (R_ByteArray_size(self) == R_ByteArray_size(comparor)) return 0;
	if (R_ByteArray_size(self) == lowerSize) return -1;
	return 1;
}

static uint8_t hex_to_byte(char hex1, char hex2);
static uint8_t hex_to_nibble(char hex);
R_ByteArray* R_ByteArray_appendHexCString(R_ByteArray* self, const char* hex) {
	size_t length = strlen(hex);
	for (int i=0; i<length; i+=2) {
		if (length > i+1)
			R_ByteArray_appendByte(self, hex_to_byte(hex[i], hex[i+1]));
		else
			R_ByteArray_appendByte(self, hex_to_byte(hex[i], '0'));
	}
	return self;
}
R_ByteArray* R_ByteArray_appendHexString(R_ByteArray* self, R_String* hex) {
	for (int i=0; i<R_String_length(hex); i+=2) {
		if (R_String_length(hex) > i+1)
			R_ByteArray_appendByte(self, hex_to_byte(R_String_getString(hex)[i], R_String_getString(hex)[i+1]));
		else
			R_ByteArray_appendByte(self, hex_to_byte(R_String_getString(hex)[i], '0'));
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
	if (self == NULL) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendByte(self, byte);
}
R_ByteArray* R_ByteArray_setCArray(R_ByteArray* self, const uint8_t* bytes, size_t count) {
	if (self == NULL) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendCArray(self, bytes, count);
}
R_ByteArray* R_ByteArray_setArray(R_ByteArray* self, const R_ByteArray* array) {
	if (self == NULL) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendArray(self, array);
}
R_ByteArray* R_ByteArray_setHexString(R_ByteArray* self, R_String* hex) {
	if (self == NULL) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendHexString(self, hex);
}
R_ByteArray* R_ByteArray_setHexCString(R_ByteArray* self, const char* hex) {
	if (self == NULL) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendHexCString(self, hex);
}
R_ByteArray* R_ByteArray_setUInt32(R_ByteArray* self, uint32_t value) {
	if (self == NULL) return NULL;
	R_ByteArray_reset(self);
	return R_ByteArray_appendUInt32(self, value);
}

R_ByteArray* R_ByteArray_appendUInt32(R_ByteArray* self, uint32_t value) {
	if (self == NULL) return NULL;
	if ((value & 0xFF000000) != 0) R_ByteArray_appendByte(self, (value & 0xFF000000) >> 24);
	if ((value & 0xFFFF0000) != 0) R_ByteArray_appendByte(self, (value & 0x00FF0000) >> 16);
	if ((value & 0xFFFFFF00) != 0) R_ByteArray_appendByte(self, (value & 0x0000FF00) >>  8);
	if ((value & 0xFFFFFFFF) != 0) R_ByteArray_appendByte(self, (value & 0x000000FF) >>  0);
	return self;
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
