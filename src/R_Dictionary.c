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
#include "R_Dictionary.h"
#include "R_ObjectArray.h"

typedef enum {
	R_Dictionary_Datum_Type_None = 0,
	R_Dictionary_Datum_Type_Data,
	R_Dictionary_Datum_Type_Object,
	R_Dictionary_Datum_Type_Array
} R_Dictionary_Datum_Type;

const int R_Dictionary_Datum_MaxKeyLength = 64;

typedef struct {
	R_Dictionary_Datum_Type type;
	char key[R_Dictionary_Datum_MaxKeyLength];
	R_String* string;
	R_Dictionary* object;
	R_ObjectArray* array;
} R_Dictionary_Datum;

R_Dictionary_Datum* R_Dictionary_Datum_alloc(void);
R_Dictionary_Datum* R_Dictionary_Datum_reset(R_Dictionary_Datum* self);
R_Dictionary_Datum* R_Dictionary_Datum_free(R_Dictionary_Datum* self);

R_Dictionary_Datum* R_Dictionary_Datum_alloc(void) {
	R_Dictionary_Datum* self = malloc(sizeof(R_Dictionary_Datum));
	memset(self, 0, sizeof(R_Dictionary_Datum));
	return self;
}
R_Dictionary_Datum* R_Dictionary_Datum_reset(R_Dictionary_Datum* self) {
	memset(self->key, 0, R_Dictionary_Datum_MaxKeyLength);
	if (self->array != NULL) {
		R_ObjectArray_removeManagedAll(self->array, (R_ObjectArray_Deallocator)R_Dictionary_free);
		R_ObjectArray_reset(self->array);
	}
	if (self->string != NULL) R_String_reset(self->string);
	self->type = R_Dictionary_Datum_Type_None;
	return self;
}
R_Dictionary_Datum* R_Dictionary_Datum_free(R_Dictionary_Datum* self) {
	if (self->object != NULL) R_Dictionary_free(self->object);
	if (self->array  != NULL) {
		R_ObjectArray_removeManagedAll(self->array, (R_ObjectArray_Deallocator)R_Dictionary_free);
		R_ObjectArray_free(self->array);
	}
	if (self->string != NULL) R_Type_Delete(self->string);
	free(self);
	return NULL;
}

R_Dictionary_Datum* R_Dictionary_getDatum(R_Dictionary* self, const char* key);
R_Dictionary_Datum* R_Dictionary_prepareDatumForSetter(R_Dictionary* self, const char* key);
bool R_Dictionary_findNextDatumInString(R_String* _input, R_String* nextDatumKey, R_String* nextDatumValue, R_String* remainder);
const char* R_Dictionary_findNextNonWhitespace(const char* input);
const char* R_Dictionary_findValueStart(const char* input);



struct R_Dictionary {
	R_ObjectArray* dictionary;
};

R_Dictionary* R_Dictionary_alloc(void) {
	R_Dictionary* self = (R_Dictionary*)malloc(sizeof(R_Dictionary));
	memset(self, 0, sizeof(R_Dictionary));
	self->dictionary = R_ObjectArray_alloc();

	return self;
}
R_Dictionary* R_Dictionary_reset(R_Dictionary* self) {
	R_ObjectArray_removeManagedAll(self->dictionary, (R_ObjectArray_Deallocator)R_Dictionary_Datum_free);
	R_ObjectArray_reset(self->dictionary);

	return self;
}
R_Dictionary* R_Dictionary_free(R_Dictionary* self) {
	R_ObjectArray_removeManagedAll(self->dictionary, (R_ObjectArray_Deallocator)R_Dictionary_Datum_free);
	R_ObjectArray_free(self->dictionary);
	free(self);
	return NULL;
}

bool R_Dictionary_doesKeyExist(R_Dictionary* self, const char* key) {
	if (R_Dictionary_getDatum(self, key) == NULL) return false;
	return true;
}

bool R_Dictionary_setString(R_Dictionary* self, const char* key, const char* data) {
	R_Dictionary_Datum* datum = R_Dictionary_prepareDatumForSetter(self, key);
	if (datum == NULL) return false;

	if (datum->string == NULL) datum->string = R_Type_New(R_String_Type);
	else R_String_reset(datum->string);
	R_String_appendCString(datum->string, data);
	datum->type = R_Dictionary_Datum_Type_Data;

	return true;
}

R_Dictionary_Datum* R_Dictionary_prepareDatumForSetter(R_Dictionary* self, const char* key) {
	if (strlen(key)+1 > R_Dictionary_Datum_MaxKeyLength) return NULL;
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum != NULL) return datum;
	datum = R_ObjectArray_addManagedPointer(self->dictionary, (R_ObjectArray_Allocator)R_Dictionary_Datum_alloc);
	strcpy(datum->key, key);
	return datum;
}

R_Dictionary_Datum* R_Dictionary_getDatum(R_Dictionary* self, const char* key) {
	for (int i=0; i<R_ObjectArray_length(self->dictionary); i++) {
		R_Dictionary_Datum* datum = R_ObjectArray_pointerAtIndex(self->dictionary, i);
		if (strcmp(datum->key, key) == 0) return datum;
	}
	return NULL;
}
const char* R_Dictionary_getString(R_Dictionary* self, const char* key) {
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL) return NULL;
	if (datum->type != R_Dictionary_Datum_Type_Data) return NULL;
	return R_String_getString(datum->string);
}

bool R_Dictionary_setInt(R_Dictionary* self, const char* key, int data) {
	R_Dictionary_Datum* datum = R_Dictionary_prepareDatumForSetter(self, key);
	if (datum == NULL) return false;
	if (datum->string == NULL) datum->string = R_Type_New(R_String_Type);
	else R_String_reset(datum->string);

	R_String_appendInt(datum->string, data);
	datum->type = R_Dictionary_Datum_Type_Data;
	return true;
}
int R_Dictionary_getInt(R_Dictionary* self, const char* key) {
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL) return 0;
	if (datum->type != R_Dictionary_Datum_Type_Data) return 0;
	return R_String_getInt(datum->string);
}

bool R_Dictionary_setFloat(R_Dictionary* self, const char* key, float data) {
	R_Dictionary_Datum* datum = R_Dictionary_prepareDatumForSetter(self, key);
	if (datum == NULL) return false;
	if (datum->string == NULL) datum->string = R_Type_New(R_String_Type);
	else R_String_reset(datum->string);

	R_String_appendFloat(datum->string, data);
	datum->type = R_Dictionary_Datum_Type_Data;
	return true;
}
float R_Dictionary_getFloat(R_Dictionary* self, const char* key) {
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL) return 0.0f;
	if (datum->type != R_Dictionary_Datum_Type_Data) return 0.0f;
	return R_String_getFloat(datum->string);
}

R_Dictionary* R_Dictionary_setObject(R_Dictionary* self, const char* key) {
	R_Dictionary_Datum* datum = R_Dictionary_prepareDatumForSetter(self, key);
	if (datum == NULL) return false;

	datum->object = R_Dictionary_alloc();
	datum->type = R_Dictionary_Datum_Type_Object;
	return datum->object;
}
R_Dictionary* R_Dictionary_getObject(R_Dictionary* self, const char* key) {
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL) return NULL;
	if (datum->type != R_Dictionary_Datum_Type_Object) return NULL;
	return datum->object;
}

R_Dictionary* R_Dictionary_addToArray(R_Dictionary* self, const char* key) {
	R_Dictionary_Datum* datum = R_Dictionary_prepareDatumForSetter(self, key);
	if (datum == NULL) return false;

	if (datum->array == NULL) datum->array = R_ObjectArray_alloc();
	datum->type = R_Dictionary_Datum_Type_Array;
	return R_ObjectArray_addManagedPointer(datum->array, (R_ObjectArray_Allocator)R_Dictionary_alloc);
}
int R_Dictionary_getArraySize(R_Dictionary* self, const char* key) {
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL) return 0;
	if (datum->type != R_Dictionary_Datum_Type_Array) return 0;
	return R_ObjectArray_length(datum->array);
}
R_Dictionary* R_Dictionary_getArrayIndex(R_Dictionary* self, const char* key, int index) {
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL) return NULL;
	if (datum->type != R_Dictionary_Datum_Type_Array) return NULL;
	return (R_Dictionary*)R_ObjectArray_pointerAtIndex(datum->array, index);
}

R_String* R_Dictionary_serialize(R_Dictionary* self, R_String* string) {
	R_String_appendCString(string, "{");

	for (int keyIndex=0; keyIndex<R_ObjectArray_length(self->dictionary); keyIndex++) {
		R_Dictionary_Datum* datum = R_ObjectArray_pointerAtIndex(self->dictionary, keyIndex);
		if (datum == NULL) return NULL;
		//R_String_appendString(string, "");
		R_String_appendCString(string, datum->key);
		R_String_appendCString(string, "=");
		if (datum->type == R_Dictionary_Datum_Type_Data) {
			R_String_appendCString(string, "'");
			R_String_appendString(string, datum->string);
			R_String_appendCString(string, "';");
		}
		else if (datum->type == R_Dictionary_Datum_Type_Object) {
			R_Dictionary_serialize(datum->object, string);
		}
		else if (datum->type == R_Dictionary_Datum_Type_Array) {
			R_String_appendCString(string, "[");
			for (int i=0; i<R_ObjectArray_length(datum->array); i++) {
				R_Dictionary* arrayMember = (R_Dictionary*)R_ObjectArray_pointerAtIndex(datum->array, i);
				R_Dictionary_serialize(arrayMember, string);
			}
			R_String_appendCString(string, "];");
		}
	}

	R_String_appendCString(string, "};");
	return string;
}

R_Dictionary* R_String_objectize(R_Dictionary* self, R_String* string) {
	R_String* thisObjectString = R_Type_New(R_String_Type);
	if (R_String_getEnclosedString(string, '{', '}', thisObjectString) == NULL ) {
		R_Type_Delete(thisObjectString);
		printf("no braces\n");
		return NULL;
	}

	R_String* thisDatumKeyString = R_Type_New(R_String_Type);
	R_String* thisDatumValueString = R_Type_New(R_String_Type);
	while (R_Dictionary_findNextDatumInString(thisObjectString, thisDatumKeyString, thisDatumValueString, thisObjectString)) {
		char dataType = R_String_findFirstToken(thisDatumValueString, "'{[");
		if (dataType == '\'') {
			R_String* thisDatumNumberString = R_Type_New(R_String_Type);
			if (R_String_getEnclosedString(thisDatumValueString, '\'', '\'', thisDatumNumberString) == NULL ) {
				R_Type_Delete(thisDatumNumberString);
				R_Type_Delete(thisDatumValueString);
				R_Type_Delete(thisDatumKeyString);
				R_Type_Delete(thisObjectString);
				printf("no single quotes\n");
				return NULL;
			}

			R_Dictionary_setString(self, R_String_getString(thisDatumKeyString), R_String_getString(thisDatumNumberString));
			R_Type_Delete(thisDatumNumberString);
		}
		else if (dataType == '{') {
			R_Dictionary* thisDatumObject = R_Dictionary_setObject(self, R_String_getString(thisDatumKeyString));
			if (R_String_objectize(thisDatumObject, thisDatumValueString) == NULL) {
				R_Type_Delete(thisDatumValueString);
				R_Type_Delete(thisDatumKeyString);
				R_Type_Delete(thisObjectString);
				printf("failed objectizing child\n");
				return NULL;
			}
		}
		else if (dataType == '[') {
			R_String* thisDatumArrayEntryString = R_Type_New(R_String_Type);
			while (R_String_splitBracedString(thisDatumValueString, '{', '}', NULL, thisDatumArrayEntryString, NULL, thisDatumValueString) == true) {
				R_Dictionary* thisDatumObject = R_Dictionary_addToArray(self, R_String_getString(thisDatumKeyString));
				if (R_String_objectize(thisDatumObject, thisDatumArrayEntryString) == NULL) {
					R_Type_Delete(thisDatumArrayEntryString);
					R_Type_Delete(thisDatumValueString);
					R_Type_Delete(thisDatumKeyString);
					R_Type_Delete(thisObjectString);
					printf("failed objectizing array entry\n");
					return NULL;
				}
			}
			R_Type_Delete(thisDatumArrayEntryString);
		}
	}

	R_Type_Delete(thisDatumValueString);
	R_Type_Delete(thisDatumKeyString);
	R_Type_Delete(thisObjectString);

	return self;
}

bool R_Dictionary_findNextDatumInString(R_String* _input, R_String* nextDatumKey, R_String* nextDatumValue, R_String* _remainder) {
	const char* input = R_String_getString(_input);

	const char* keyStart = R_Dictionary_findNextNonWhitespace(input);
	if (keyStart == NULL) return false;
	const char* keyEnd = strchr(keyStart, '=');
	if (keyEnd == NULL) return false;


	const char* valueStart = R_Dictionary_findValueStart(keyEnd);
	if (valueStart == NULL) return false;


	if (valueStart[0] == '\'') {
		//this is a data value
		const char* valueEnd = strchr(valueStart, ';');
		if (valueEnd == NULL) return false;

		if (R_String_setSizedString(nextDatumKey, keyStart, keyEnd-keyStart) == NULL) return false;
		if (R_String_setSizedString(nextDatumValue, valueStart, valueEnd-valueStart) == NULL) return false;
		if (R_String_setString(_remainder, valueEnd) == NULL) return false;
		return true;
	}
	else if (valueStart[0] == '{') {
		//this is an object value
		R_String* valueString = R_Type_New(R_String_Type);
		R_String_setString(valueString, valueStart);
		if (R_String_setSizedString(nextDatumKey, keyStart, keyEnd-keyStart) == NULL) {
			R_Type_Delete(valueString);
			return false;
		}
		if (R_String_splitBracedString(valueString, '{', '}', NULL, nextDatumValue, NULL, _remainder) == false) {
			R_Type_Delete(valueString);
			return false;
		}
		R_Type_Delete(valueString);
		return true;
	}
	else if (valueStart[0] == '[') {
		//this is an array value
		R_String* valueString = R_Type_New(R_String_Type);
		R_String_setString(valueString, valueStart);
		if (R_String_setSizedString(nextDatumKey, keyStart, keyEnd-keyStart) == NULL) {
			R_Type_Delete(valueString);
			return false;
		}
		if (R_String_splitBracedString(valueString, '[', ']', NULL, nextDatumValue, NULL, _remainder) == false) {
			R_Type_Delete(valueString);
			return false;
		}
		R_Type_Delete(valueString);
		return true;
	}

	return false;
}

const char* R_Dictionary_findNextNonWhitespace(const char* input) {
	for (int start=0; start<strlen(input); start++) {
		if (input[start] != ' ' &&
			input[start] != '\t' &&
			input[start] != '\r' &&
			input[start] != '\n' &&
			input[start] != ';')
		{
			return input+start;
		}
	}

	return NULL;
}

const char* R_Dictionary_findValueStart(const char* input) {
	for (int start=0; start<strlen(input); start++) {
		if (input[start] == '\'' ||
			input[start] == '{' ||
			input[start] == '[')
		{
			return input+start;
		}
	}

	return NULL;
}



