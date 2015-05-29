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
#include "R_String.h"

struct R_String {
	R_Type* type;
	char* cstring;              //A buffer to store a cstring conversion, if it's needed
	R_ByteArray* array;         //Array of characters
};

static R_String* R_FUNCTION_ATTRIBUTES R_String_Constructor(R_String* self);
static R_String* R_FUNCTION_ATTRIBUTES R_String_Destructor(R_String* self);
static R_String* R_FUNCTION_ATTRIBUTES R_String_Copier(R_String* self, R_String* new);
static void* R_FUNCTION_ATTRIBUTES R_String_Methods(const R_Face* interface);
R_Type_Def(R_String, R_String_Constructor, R_String_Destructor, R_String_Copier, R_String_Methods);


static R_String* R_FUNCTION_ATTRIBUTES R_String_Constructor(R_String* self) {
	self->cstring = NULL;
	self->array = R_Type_New(R_ByteArray);

	return self;
}
static R_String* R_FUNCTION_ATTRIBUTES R_String_Destructor(R_String* self) {
	os_free(self->cstring);
	R_Type_Delete(self->array);
	return self;
}
static R_String* R_FUNCTION_ATTRIBUTES R_String_Copier(R_String* self, R_String* new) {
	R_String_appendString(new, self);
	return new;
}

static void* R_FUNCTION_ATTRIBUTES R_String_Methods(const R_Face* interface) {
	R_Face_DefJump(R_Puts, R_String_puts);
	R_Face_DefJump(R_Equals, R_String_isSame);
	return NULL;
}


R_String* R_FUNCTION_ATTRIBUTES R_String_reset(R_String* self) {
	if (self == NULL) return NULL;
	os_free(self->cstring);
	self->cstring = NULL;
	R_ByteArray_reset(self->array);

	return self;
}

const char* R_FUNCTION_ATTRIBUTES R_String_getString(R_String* self) {
	if (R_Type_IsNotOf(self, R_String)) return NULL;
	if (self->cstring != NULL) os_free(self->cstring);
	self->cstring = (char*)os_malloc((R_ByteArray_size(self->array)+1)*sizeof(char));
	os_memcpy(self->cstring, R_ByteArray_bytes(self->array), R_ByteArray_size(self->array));
	self->cstring[R_ByteArray_size(self->array)] = '\0';
	return self->cstring;
}

R_String* R_FUNCTION_ATTRIBUTES R_String_appendBytes(R_String* self, const char* bytes, size_t byteCount) {
	if (self == NULL || bytes == NULL) return NULL;
	if (R_ByteArray_appendCArray(self->array, (const uint8_t*)bytes, byteCount) == NULL) return NULL;
	return self;
}

R_String* R_FUNCTION_ATTRIBUTES R_String_appendArray(R_String* self, const R_ByteArray* array) {
	if (self == NULL || array == NULL) return NULL;
	if (R_ByteArray_appendArray(self->array, array) == NULL) return NULL;
	return self;
}

size_t R_FUNCTION_ATTRIBUTES R_String_length(const R_String* self) {
	if (self == NULL) return 0;
	return R_ByteArray_size(self->array);
}

R_String* R_FUNCTION_ATTRIBUTES R_String_appendArrayAsHex(R_String* self, const R_ByteArray* array) {
	if (self == NULL || array == NULL) return NULL;
	for (int i=0; i<R_ByteArray_size(array); i++) {
		char hexDigitF0 = (R_ByteArray_byte(array,i) & 0xF0) >> 4;
		char hexDigit0F = (R_ByteArray_byte(array,i) & 0x0F);
		if (hexDigitF0 >= 0x0 && hexDigitF0 <= 0x9) hexDigitF0 += ('0' - 0x0);
		else if (hexDigitF0 >= 0xa && hexDigitF0 <= 0xf) hexDigitF0 += ('A' - 0xA);
		if (hexDigit0F >= 0x0 && hexDigit0F <= 0x9) hexDigit0F += ('0' - 0x0);
		else if (hexDigit0F >= 0xa && hexDigit0F <= 0xf) hexDigit0F += ('A' - 0xA);
		R_ByteArray_push(self->array, hexDigitF0);
		R_ByteArray_push(self->array, hexDigit0F);
	}

	return self;
}

R_String* R_FUNCTION_ATTRIBUTES R_String_getSubstring(R_String* self, R_String* output, size_t startingIndex, size_t length) {
	if (self == NULL || output == NULL) return NULL;
	R_ByteArray* to_copy = R_Type_Copy(self->array);
	R_String_reset(output);
	R_ByteArray_moveSubArray(output->array, to_copy, startingIndex, length);
	R_Type_Delete(to_copy);
	return output;
}

R_String* R_FUNCTION_ATTRIBUTES R_String_moveSubstring(R_String* self, R_String* output, size_t startingIndex, size_t length) {
	if (self == NULL || output == NULL) return NULL;
	R_String_reset(output);
	R_ByteArray_moveSubArray(output->array, self->array, startingIndex, length);
	return output;
}

bool R_FUNCTION_ATTRIBUTES R_String_isSame(R_String* self, R_String* comparor) {
	if (self == NULL || comparor == NULL) return false;
	if (R_ByteArray_compare(self->array, comparor->array) != 0) return false;
	return true;
}

bool R_FUNCTION_ATTRIBUTES R_String_compare(const R_String* self, const char* comparor) {
	if (R_Type_IsNotOf(self, R_String) || comparor == NULL) return false;
	return (R_ByteArray_compareWithCArray(self->array, (uint8_t*)comparor, os_strlen(comparor)) == 0);
}

R_String* R_FUNCTION_ATTRIBUTES R_String_appendString(R_String* self, R_String* string) {
  if (self == NULL || string == NULL) return NULL;
  if (R_ByteArray_appendArray(self->array, string->array) == NULL) return NULL;
  return self;
}

char R_FUNCTION_ATTRIBUTES R_String_first(const R_String* self) {
	if (self == NULL) return '\0';
	return R_ByteArray_first(self->array);
}
char R_FUNCTION_ATTRIBUTES R_String_shift(R_String* self) {
	if (self == NULL) return '\0';
	return R_ByteArray_shift(self->array);
}
char R_FUNCTION_ATTRIBUTES R_String_last(const R_String* self) {
	if (self == NULL) return '\0';
	return R_ByteArray_last(self->array);
}
char R_FUNCTION_ATTRIBUTES R_String_pop(R_String* self) {
	if (self == NULL) return '\0';
	return R_ByteArray_pop(self->array);
}
R_String* R_FUNCTION_ATTRIBUTES R_String_push(R_String* self, char character) {
	if (self == NULL) return NULL;
	if (R_ByteArray_push(self->array, character) == NULL) return NULL;
	return self;
}

char R_FUNCTION_ATTRIBUTES R_String_character(R_String* self, size_t index) {
	if (R_Type_IsNotOf(self, R_String)) return '\0';
	if (index >= R_String_length(self)) return '\0';
	return (char)R_ByteArray_byte(self->array, index);
}

const R_ByteArray* R_FUNCTION_ATTRIBUTES R_String_bytes(R_String* self) {
	if (R_Type_IsNotOf(self, R_String)) return NULL;
	return self->array;
}
