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

static R_String* R_String_Constructor(R_String* self);
static R_String* R_String_Destructor(R_String* self);
static R_String* R_String_Copier(R_String* self, R_String* new);
R_Type_Def(R_String, R_String_Constructor, R_String_Destructor, R_String_Copier, NULL);


static R_String* R_String_Constructor(R_String* self) {
	self->cstring = NULL;
	self->array = R_Type_New(R_ByteArray);

	return self;
}
static R_String* R_String_Destructor(R_String* self) {
	free(self->cstring);
	R_Type_Delete(self->array);
	return self;
}
static R_String* R_String_Copier(R_String* self, R_String* new) {
	R_String_appendString(new, self);
	return new;
}


R_String* R_String_reset(R_String* self) {
	if (self == NULL) return NULL;
	free(self->cstring);
	self->cstring = NULL;
	R_ByteArray_reset(self->array);

	return self;
}

const char* R_String_getString(R_String* self) {
	if (R_Type_IsNotOf(self, R_String)) return NULL;
	if (self->cstring != NULL) free(self->cstring);
	self->cstring = malloc((R_ByteArray_size(self->array)+1)*sizeof(char));
	memcpy(self->cstring, R_ByteArray_bytes(self->array), R_ByteArray_size(self->array));
	self->cstring[R_ByteArray_size(self->array)] = '\0';
	return self->cstring;
}

R_String* R_String_appendCString(R_String* self, const char* string) {
	return R_String_appendBytes(self, string, strlen(string));
}

R_String* R_String_appendBytes(R_String* self, const char* bytes, size_t byteCount) {
	if (self == NULL || bytes == NULL) return NULL;
	if (R_ByteArray_appendCArray(self->array, (const uint8_t*)bytes, byteCount) == NULL) return NULL;
	return self;
}

R_String* R_String_appendArray(R_String* self, const R_ByteArray* array) {
	if (self == NULL || array == NULL) return NULL;
	if (R_ByteArray_appendArray(self->array, array) == NULL) return NULL;
	return self;
}

size_t R_String_length(const R_String* self) {
	if (self == NULL) return 0;
	return R_ByteArray_size(self->array);
}

R_String* R_String_setString(R_String* self, const char* string) {
	if (self == NULL || string == NULL) return NULL;
	R_String_reset(self);
	return R_String_appendCString(self, string);
}

R_String* R_String_setSizedString(R_String* self, const char* string, size_t stringLength) {
	if (self == NULL || string == NULL) return NULL;
	R_String_reset(self);
	size_t length = strlen(string);
	length = (length > stringLength) ? stringLength : length;
	return R_String_appendBytes(self, string, length);
}

R_String* R_String_appendInt(R_String* self, int value) {
	if (self == NULL) return NULL;
	char characters[snprintf(NULL, 0, "%d", value)];
	sprintf(characters, "%d", value);
	return R_String_appendCString(self, characters);
}

R_String* R_String_appendFloat(R_String* self, float value) {
	if (self == NULL) return NULL;
	char characters[snprintf(NULL, 0, "%g", value)];
	sprintf(characters, "%g", value);
	return R_String_appendCString(self, characters);
}

R_String* R_String_appendString(R_String* self, R_String* string) {
	if (self == NULL || string == NULL) return NULL;
	if (R_ByteArray_appendArray(self->array, string->array) == NULL) return NULL;
	return self;
}

R_String* R_String_appendArrayAsHex(R_String* self, const R_ByteArray* array) {
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

int R_String_getInt(R_String* self) {
	if (self == NULL) return 0;
	int output = 0;
	if (sscanf(R_String_getString(self), "%d", &output) == 1) return output;
	return 0;
}
float R_String_getFloat(R_String* self) {
	if (self == NULL) return 0.0f;
	float output = 0.0f;
	if (sscanf(R_String_getString(self), "%g", &output) == 1) return output;
	return 0.0f;
}

R_String* R_String_getSubstring(R_String* self, R_String* output, size_t startingIndex, size_t length) {
	if (self == NULL || output == NULL) return NULL;
	R_ByteArray* to_copy = R_Type_Copy(self->array);
	R_String_reset(output);
	R_ByteArray_moveSubArray(output->array, to_copy, startingIndex, length);
	R_Type_Delete(to_copy);
	return output;
}

R_String* R_String_moveSubstring(R_String* self, R_String* output, size_t startingIndex, size_t length) {
	if (self == NULL || output == NULL) return NULL;
	R_String_reset(output);
	R_ByteArray_moveSubArray(output->array, self->array, startingIndex, length);
	return output;
}

bool R_String_isSame(R_String* self, R_String* comparor) {
	if (self == NULL || comparor == NULL) return false;
	if (R_ByteArray_compare(self->array, comparor->array) != 0) return false;
	return true;
}

bool R_String_isEmpty(R_String* self) {
	if (R_String_length(self) == 0) return true;
	return false;
}

bool R_String_compare(const R_String* self, const char* comparor) {
	if (R_Type_IsNotOf(self, R_String) || comparor == NULL) return false;
	return (R_ByteArray_compareWithCArray(self->array, (uint8_t*)comparor, strlen(comparor)) == 0);
}

R_String* R_String_appendStringAsJson(R_String* self, R_String* string) {
	if (self == NULL || string == NULL) return NULL;
	R_String_appendCString(self, "\"");

	for (int i=0; i<R_String_length(string); i++) {
		char character = R_String_getString(string)[i];
		if (character == '"')  {R_String_appendCString(self, "\\\""); continue;}
		if (character == '\\') {R_String_appendCString(self, "\\\\"); continue;}
		if (character == '/')  {R_String_appendCString(self, "\\/");  continue;}
		if (character == '\b') {R_String_appendCString(self, "\\b");  continue;}
		if (character == '\f') {R_String_appendCString(self, "\\f");  continue;}
		if (character == '\n') {R_String_appendCString(self, "\\n");  continue;}
		if (character == '\r') {R_String_appendCString(self, "\\r");  continue;}
		if (character == '\t') {R_String_appendCString(self, "\\t");  continue;}
		if (character < 0x1f) continue;
		R_String_appendBytes(self, &character, 1);
	}

	R_String_appendCString(self, "\"");
	return self;
}

char R_String_first(const R_String* self) {
	if (self == NULL) return '\0';
	return R_ByteArray_first(self->array);
}
char R_String_shift(R_String* self) {
	if (self == NULL) return '\0';
	return R_ByteArray_shift(self->array);
}
char R_String_last(const R_String* self) {
	if (self == NULL) return '\0';
	return R_ByteArray_last(self->array);
}
char R_String_pop(R_String* self) {
	if (self == NULL) return '\0';
	return R_ByteArray_pop(self->array);
}
R_String* R_String_push(R_String* self, char character) {
	if (self == NULL) return NULL;
	if (R_ByteArray_push(self->array, character) == NULL) return NULL;
	return self;
}

char R_String_character(R_String* self, size_t index) {
	if (R_Type_IsNotOf(self, R_String)) return '\0';
	if (index >= R_String_length(self)) return '\0';
	return (char)R_ByteArray_byte(self->array, index);
}

static bool R_String_trim_isWhiteSpace(char character);
R_String* R_String_trim(R_String* self) {
	if (self == NULL) return NULL;
	while(R_String_length(self) > 0 && R_String_trim_isWhiteSpace(R_String_first(self))) R_String_shift(self);
	while(R_String_length(self) > 0 && R_String_trim_isWhiteSpace(R_String_last(self))) R_String_pop(self);
	return self;
}

static bool R_String_trim_isWhiteSpace(char character) {
	switch (character) {
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			return true;
	}
	return false;
}

const R_ByteArray* R_String_bytes(R_String* self) {
	if (R_Type_IsNotOf(self, R_String)) return NULL;
	return self->array;
}

int R_String_find(R_String* self, const char* substring) {
	if (R_Type_IsNotOf(self, R_String) || substring == NULL) return -1;
	const char* string = R_String_cstring(self);
	char* result = strstr(string, substring);
	if (result == NULL) return -1;
	return (int)(result - string);
}

R_List* R_String_split(R_String* self, const char* seperator, R_List* output) {
	if (R_Type_IsNotOf(self, R_String) || R_Type_IsNotOf(output, R_List)) return NULL;
	if (R_String_length(self) == 0) return NULL;
	if (seperator == NULL) seperator = "\n";

	R_String* copy = R_Type_Copy(self);

	while (R_String_length(copy) > 0) {
		R_String* this_splice = R_List_add(output, R_String);
		int splice_length = R_String_find(copy, seperator);
		if (splice_length < 0) {
			R_String_appendString(this_splice, copy);
			R_Type_Delete(copy);
			return output;
		}
		if (splice_length > 0) R_String_moveSubstring(copy, this_splice, 0, splice_length);
		for (int i=0; i<strlen(seperator); i++) R_String_shift(copy);
	}

	R_Type_Delete(copy);
	return output;
}

R_String* R_String_join(R_String* self, const char* seperator, R_List* input) {
	if (R_Type_IsNotOf(self, R_String) || R_Type_IsNotOf(input, R_List)) return NULL;
	if (seperator == NULL) seperator = "";
	R_List_each(input, R_String, string) {
		if (R_Type_IsNotOf(string, R_String)) continue;
		R_String_appendString(self, string);
		if (string != R_List_last(input)) R_String_appendCString(self, seperator);
	}
	return self;
}

static uint8_t R_String_appendArrayAsBase64_base64FromIndex(uint8_t index);
R_String* R_String_appendArrayAsBase64(R_String* self, const R_ByteArray* array) {
	if (R_Type_IsNotOf(self, R_String) || R_Type_IsNotOf(array, R_ByteArray)) return NULL;
	for(int i=0; i<R_ByteArray_size(array); i+=3) {
		uint8_t byte1 = ((R_ByteArray_byte(array,   i) & 0xFC) >> 2);
		uint8_t byte2 = ((R_ByteArray_byte(array,   i) & 0x03) << 4) + ((R_ByteArray_byte(array, i+1) & 0xF0) >> 4);
		uint8_t byte3 = ((R_ByteArray_byte(array, i+1) & 0x0F) << 2) + ((R_ByteArray_byte(array, i+2) & 0xC0) >> 6);
		uint8_t byte4 = (R_ByteArray_byte(array, i+2) & 0x3F);
		if (R_String_push(self, R_String_appendArrayAsBase64_base64FromIndex(byte1)) == NULL) return NULL;
		if (R_String_push(self, R_String_appendArrayAsBase64_base64FromIndex(byte2)) == NULL) return NULL;
		if (R_ByteArray_length(array) > i+1) {
			if (R_String_push(self, R_String_appendArrayAsBase64_base64FromIndex(byte3)) == NULL) return NULL;
		}
		else {
			if (R_String_push(self, '=') == NULL) return NULL;
		}
		if (R_ByteArray_length(array) > i+2) {
			if (R_String_push(self, R_String_appendArrayAsBase64_base64FromIndex(byte4)) == NULL) return NULL;
		}
		else {
			if (R_String_push(self, '=') == NULL) return NULL;
		}
	}
	return self;
}

static uint8_t R_String_appendArrayAsBase64_base64FromIndex(uint8_t index) {
	if (index <= 25) return 'A'+index;
	if (index <= 51) return 'a'+(index-26);
	if (index <= 61) return '0'+(index-52);
	if (index == 62) return '+';
	if (index == 63) return '/';
	return '=';
}
