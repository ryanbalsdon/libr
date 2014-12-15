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


typedef struct {
	R_Type* type;
	R_String* key;
	void* value;
} R_Dictionary_Datum;

R_Dictionary_Datum* R_Dictionary_Datum_Constructor(R_Dictionary_Datum* self) {
	self->key = R_Type_New(R_String_Type);
	return self;
}

R_Dictionary_Datum* R_Dictionary_Datum_Destructor(R_Dictionary_Datum* self) {
	R_Type_Delete(self->key);
	if (self->value != NULL) R_Type_Delete(self->value);
	return self;
}
R_Type_Def(R_Dictionary_Datum, R_Dictionary_Datum_Constructor, R_Dictionary_Datum_Destructor, NULL);

struct R_Dictionary {
	R_Type* type;
	R_ObjectArray* dictionary;
};

R_Dictionary* R_Dictionary_Constructor(R_Dictionary* self) {
	self->dictionary = R_Type_New(R_ObjectArray_Type);
	return self;
}

R_Dictionary* R_Dictionary_Destructor(R_Dictionary* self) {
	R_Type_Delete(self->dictionary);
	return self;
}
R_Type_Def(R_Dictionary, R_Dictionary_Constructor, R_Dictionary_Destructor, NULL);

R_Dictionary_Datum* R_Dictionary_getDatum(R_Dictionary* self, const char* key);
R_Dictionary_Datum* R_Dictionary_prepareDatumForSetter(R_Dictionary* self, const char* key);
R_Dictionary_Datum* R_Dictionary_prepareDatumForStringSetter(R_Dictionary* self, const char* key);
bool R_Dictionary_findNextDatumInString(R_String* _input, R_String* nextDatumKey, R_String* nextDatumValue, R_String* remainder);
const char* R_Dictionary_findNextNonWhitespace(const char* input);
const char* R_Dictionary_findValueStart(const char* input);


bool R_Dictionary_doesKeyExist(R_Dictionary* self, const char* key) {
	if (R_Dictionary_getDatum(self, key) == NULL) return false;
	return true;
}

bool R_Dictionary_setString(R_Dictionary* self, const char* key, const char* data) {
	if (self == NULL || key == NULL || data == NULL) return false;
	R_Dictionary_Datum* datum = R_Dictionary_prepareDatumForStringSetter(self, key);
	if (datum == NULL) return false;
	R_String_appendCString(datum->value, data);
	return true;
}

R_Dictionary_Datum* R_Dictionary_prepareDatumForStringSetter(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return NULL;
	R_Dictionary_Datum* datum = R_Dictionary_prepareDatumForSetter(self, key);
	if (datum == NULL) return false;
	if (datum->value != NULL) R_Type_Delete(datum->value);
	datum->value = R_Type_New(R_String_Type);
	if (datum->value == NULL) return NULL;
	return datum;
}

R_Dictionary_Datum* R_Dictionary_prepareDatumForSetter(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return NULL;
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum != NULL) return datum;
	datum = R_ObjectArray_addObject(self->dictionary, R_Dictionary_Datum_Type);
	R_String_setString(datum->key, key);
	return datum;
}

R_Dictionary_Datum* R_Dictionary_getDatum(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return NULL;
	R_String* r_key = R_Type_New(R_String_Type);
	if (r_key == NULL) return NULL;
	R_String_setString(r_key, key);

	for (int i=0; i<R_ObjectArray_length(self->dictionary); i++) {
		R_Dictionary_Datum* datum = R_ObjectArray_pointerAtIndex(self->dictionary, i);
		if (R_String_isSame(r_key, datum->key)) return R_Type_Delete(r_key), datum;
	}
	return R_Type_Delete(r_key), NULL;
}
const char* R_Dictionary_getString(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return NULL;
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL || datum->value == NULL || R_Type_Type(datum->value) != R_String_Type) return NULL;
	return R_String_getString(datum->value);
}

bool R_Dictionary_setInt(R_Dictionary* self, const char* key, int data) {
	if (self == NULL || key == NULL) return false;
	R_Dictionary_Datum* datum = R_Dictionary_prepareDatumForStringSetter(self, key);
	if (datum == NULL) return false;
	R_String_appendInt(datum->value, data);
	return true;
}
int R_Dictionary_getInt(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return false;
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL || datum->value == NULL || R_Type_Type(datum->value) != R_String_Type) return 0;
	return R_String_getInt(datum->value);
}

bool R_Dictionary_setFloat(R_Dictionary* self, const char* key, float data) {
	if (self == NULL || key == NULL) return false;
	R_Dictionary_Datum* datum = R_Dictionary_prepareDatumForStringSetter(self, key);
	if (datum == NULL) return false;
	R_String_appendFloat(datum->value, data);
	return true;
}
float R_Dictionary_getFloat(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return false;
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL || datum->value == NULL || R_Type_Type(datum->value) != R_String_Type) return 0;
	return R_String_getFloat(datum->value);
}

R_Dictionary* R_Dictionary_setObject(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return NULL;
	R_Dictionary_Datum* datum = R_Dictionary_prepareDatumForSetter(self, key);
	if (datum == NULL) return NULL;
	if (datum->value != NULL) R_Type_Delete(datum->value);
	datum->value = R_Type_New(R_Dictionary_Type);
	return datum->value;
}

R_Dictionary* R_Dictionary_getObject(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return NULL;
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL || datum->value == NULL || R_Type_Type(datum->value) != R_Dictionary_Type) return NULL;
	return datum->value;
}

R_Dictionary* R_Dictionary_addToArray(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return NULL;
	R_Dictionary_Datum* datum = R_Dictionary_prepareDatumForSetter(self, key);
	if (datum == NULL) return NULL;
	if (datum->value != NULL && R_Type_Type(datum->value) != R_ObjectArray_Type) {
		R_Type_Delete(datum->value);
		datum->value = NULL;
	}
	if (datum->value == NULL) datum->value = R_Type_New(R_ObjectArray_Type);
	return R_ObjectArray_addObject(datum->value, R_Dictionary_Type);
}

int R_Dictionary_getArraySize(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return 0;
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL || datum->value == NULL || R_Type_Type(datum->value) != R_ObjectArray_Type) return 0;
	return R_ObjectArray_length(datum->value);
}

R_Dictionary* R_Dictionary_getArrayIndex(R_Dictionary* self, const char* key, size_t index) {
	if (self == NULL || key == NULL) return NULL;
	R_Dictionary_Datum* datum = R_Dictionary_getDatum(self, key);
	if (datum == NULL || datum->value == NULL || R_Type_Type(datum->value) == NULL) return NULL;
	return (R_Dictionary*)R_ObjectArray_pointerAtIndex(datum->value, index);
}

R_String* R_Dictionary_serialize(R_Dictionary* self, R_String* string) {
	if (self == NULL || string == NULL) return NULL;
	R_String_appendCString(string, "{");

	for (int keyIndex=0; keyIndex<R_ObjectArray_length(self->dictionary); keyIndex++) {
		R_Dictionary_Datum* datum = (R_Dictionary_Datum*)R_ObjectArray_pointerAtIndex(self->dictionary, keyIndex);
		if (datum == NULL) return NULL;
		R_String_appendString(string, datum->key);
		R_String_appendCString(string, "=");
		if (R_Type_Type(datum->value) == R_String_Type) {
			R_String_appendCString(string, "'");
			R_String_appendString(string, datum->value);
			R_String_appendCString(string, "';");
		}
		else if (R_Type_Type(datum->value) == R_Dictionary_Type) {
			R_Dictionary_serialize(datum->value, string);
		}
		else if (R_Type_Type(datum->value) == R_ObjectArray_Type) {
			R_String_appendCString(string, "[");
			for (int i=0; i<R_ObjectArray_length(datum->value); i++) {
				R_Dictionary* arrayMember = (R_Dictionary*)R_ObjectArray_pointerAtIndex(datum->value, i);
				R_Dictionary_serialize(arrayMember, string);
			}
			R_String_appendCString(string, "];");
		}
	}

	R_String_appendCString(string, "};");
	return string;
}

R_Dictionary* R_String_objectize(R_Dictionary* self, R_String* string) {
	if (self == NULL || string == NULL) return NULL;
	R_String* thisObjectString = R_Type_New(R_String_Type);
	if (R_String_getEnclosedString(string, '{', '}', thisObjectString) == NULL ) {
		R_Type_Delete(thisObjectString);
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
	if (_input == NULL || nextDatumKey == NULL || nextDatumValue == NULL || _remainder == NULL) return false;
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
	if (input == NULL) return NULL;
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
	if (input == NULL) return input;
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



