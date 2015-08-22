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
#include "R_MutableString.h"

struct R_MutableString {
	R_Type* type;
	char* cstring;              //A buffer to store a cstring conversion, if it's needed
	R_MutableData* array;         //Array of characters
};

static R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_Constructor(R_MutableString* self);
static R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_Destructor(R_MutableString* self);
static R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_Copier(R_MutableString* self, R_MutableString* new);
static R_JumpTable_Entry methods[] = {
  R_JumpTable_Entry_Make(R_Stringify, R_MutableString_stringify), 
  R_JumpTable_Entry_Make(R_Equals, R_MutableString_isSame),
  R_JumpTable_Entry_NULL
};
R_Type_Def(R_MutableString, R_MutableString_Constructor, R_MutableString_Destructor, R_MutableString_Copier, methods);


static R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_Constructor(R_MutableString* self) {
	self->cstring = NULL;
	self->array = R_Type_New(R_MutableData);

	return self;
}
static R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_Destructor(R_MutableString* self) {
	os_free(self->cstring);
	R_Type_Delete(self->array);
	return self;
}
static R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_Copier(R_MutableString* self, R_MutableString* new) {
	R_MutableString_appendString(new, self);
	return new;
}


R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_reset(R_MutableString* self) {
	if (self == NULL) return NULL;
	os_free(self->cstring);
	self->cstring = NULL;
	R_MutableData_reset(self->array);

	return self;
}

const char* R_FUNCTION_ATTRIBUTES R_MutableString_getString(R_MutableString* self) {
	if (R_Type_IsNotOf(self, R_MutableString)) return NULL;
	if (self->cstring != NULL) os_free(self->cstring);
	self->cstring = (char*)os_malloc((R_MutableData_size(self->array)+1)*sizeof(char));
	os_memcpy(self->cstring, R_MutableData_bytes(self->array), R_MutableData_size(self->array));
	self->cstring[R_MutableData_size(self->array)] = '\0';
	return self->cstring;
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendBytes(R_MutableString* self, const char* bytes, size_t byteCount) {
	if (self == NULL || bytes == NULL) return NULL;
	if (R_MutableData_appendCArray(self->array, (const uint8_t*)bytes, byteCount) == NULL) return NULL;
	return self;
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendArray(R_MutableString* self, const R_MutableData* array) {
	if (self == NULL || array == NULL) return NULL;
	if (R_MutableData_appendArray(self->array, array) == NULL) return NULL;
	return self;
}

size_t R_FUNCTION_ATTRIBUTES R_MutableString_length(const R_MutableString* self) {
	if (self == NULL) return 0;
	return R_MutableData_size(self->array);
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendArrayAsHex(R_MutableString* self, const R_MutableData* array) {
	if (self == NULL || array == NULL) return NULL;
	for (int i=0; i<R_MutableData_size(array); i++) {
		char hexDigitF0 = (R_MutableData_byte(array,i) & 0xF0) >> 4;
		char hexDigit0F = (R_MutableData_byte(array,i) & 0x0F);
		if (hexDigitF0 >= 0x0 && hexDigitF0 <= 0x9) hexDigitF0 += ('0' - 0x0);
		else if (hexDigitF0 >= 0xa && hexDigitF0 <= 0xf) hexDigitF0 += ('A' - 0xA);
		if (hexDigit0F >= 0x0 && hexDigit0F <= 0x9) hexDigit0F += ('0' - 0x0);
		else if (hexDigit0F >= 0xa && hexDigit0F <= 0xf) hexDigit0F += ('A' - 0xA);
		R_MutableData_push(self->array, hexDigitF0);
		R_MutableData_push(self->array, hexDigit0F);
	}

	return self;
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_getSubstring(R_MutableString* self, R_MutableString* output, size_t startingIndex, size_t length) {
	if (self == NULL || output == NULL) return NULL;
	R_MutableData* to_copy = R_Type_Copy(self->array);
	R_MutableString_reset(output);
	R_MutableData_moveSubArray(output->array, to_copy, startingIndex, length);
	R_Type_Delete(to_copy);
	return output;
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_moveSubstring(R_MutableString* self, R_MutableString* output, size_t startingIndex, size_t length) {
	if (self == NULL || output == NULL) return NULL;
	R_MutableString_reset(output);
	R_MutableData_moveSubArray(output->array, self->array, startingIndex, length);
	return output;
}

bool R_FUNCTION_ATTRIBUTES R_MutableString_isSame(R_MutableString* self, R_MutableString* comparor) {
	if (self == NULL || comparor == NULL) return false;
	if (R_MutableData_compare(self->array, comparor->array) != 0) return false;
	return true;
}

bool R_FUNCTION_ATTRIBUTES R_MutableString_compare(const R_MutableString* self, const char* comparor) {
	if (R_Type_IsNotOf(self, R_MutableString) || comparor == NULL) return false;
	return (R_MutableData_compareWithCArray(self->array, (uint8_t*)comparor, os_strlen(comparor)) == 0);
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendString(R_MutableString* self, R_MutableString* string) {
  if (self == NULL || string == NULL) return NULL;
  if (R_MutableData_appendArray(self->array, string->array) == NULL) return NULL;
  return self;
}

char R_FUNCTION_ATTRIBUTES R_MutableString_first(const R_MutableString* self) {
	if (self == NULL) return '\0';
	return R_MutableData_first(self->array);
}
char R_FUNCTION_ATTRIBUTES R_MutableString_shift(R_MutableString* self) {
	if (self == NULL) return '\0';
	return R_MutableData_shift(self->array);
}
char R_FUNCTION_ATTRIBUTES R_MutableString_last(const R_MutableString* self) {
	if (self == NULL) return '\0';
	return R_MutableData_last(self->array);
}
char R_FUNCTION_ATTRIBUTES R_MutableString_pop(R_MutableString* self) {
	if (self == NULL) return '\0';
	return R_MutableData_pop(self->array);
}
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_push(R_MutableString* self, char character) {
	if (self == NULL) return NULL;
	if (R_MutableData_push(self->array, character) == NULL) return NULL;
	return self;
}

char R_FUNCTION_ATTRIBUTES R_MutableString_character(R_MutableString* self, size_t index) {
	if (R_Type_IsNotOf(self, R_MutableString)) return '\0';
	if (index >= R_MutableString_length(self)) return '\0';
	return (char)R_MutableData_byte(self->array, index);
}

const R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableString_bytes(R_MutableString* self) {
	if (R_Type_IsNotOf(self, R_MutableString)) return NULL;
	return self->array;
}
