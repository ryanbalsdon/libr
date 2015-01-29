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
#include "R_List.h"
#include "R_String.h"



typedef struct {
	R_Type* type;
	R_String* key;
	void* value; //May be a string, integer, float, array of values or a dictionary
} R_Dictionary_Element;
R_Dictionary_Element* R_Dictionary_Element_Constructor(R_Dictionary_Element* self);
R_Dictionary_Element* R_Dictionary_Element_Destructor(R_Dictionary_Element* self);

R_Dictionary_Element* R_Dictionary_Element_Constructor(R_Dictionary_Element* self) {
	self->key = R_Type_New(R_String);
	return self;
}

R_Dictionary_Element* R_Dictionary_Element_Destructor(R_Dictionary_Element* self) {
	R_Type_Delete(self->key);
	R_Type_Delete(self->value);
	return self;
}
R_Type_Def(R_Dictionary_Element, R_Dictionary_Element_Constructor, R_Dictionary_Element_Destructor, NULL);

struct R_Dictionary {
	R_Type* type;
	R_List* elements;
};
R_Dictionary* R_Dictionary_Constructor(R_Dictionary* self);
R_Dictionary* R_Dictionary_Destructor(R_Dictionary* self);

R_Dictionary* R_Dictionary_Constructor(R_Dictionary* self) {
	self->elements = R_Type_New(R_List);
	return self;
}

R_Dictionary* R_Dictionary_Destructor(R_Dictionary* self) {
	R_Type_Delete(self->elements);
	return self;
}
R_Type_Def(R_Dictionary, R_Dictionary_Constructor, R_Dictionary_Destructor, NULL);

R_Dictionary_Element* R_Dictionary_getElement(R_Dictionary* self, const char* key);

void* R_Dictionary_addObjectOfType(R_Dictionary* self, const char* key, const R_Type* type) {
	if (self == NULL || key == NULL || type == NULL) return NULL;
	R_Dictionary_Element* element = R_Dictionary_getElement(self, key);
	if (element == NULL) {
		element = R_List_add(self->elements, R_Dictionary_Element);
		if (element == NULL) return NULL;
		if (R_String_setString(element->key, key) == NULL) return NULL;
	}
	if (element->value != NULL) R_Type_Delete(element->value);
	element->value = R_Type_NewObjectOfType(type);
	return element->value;
}
void R_Dictionary_remove(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return;
	R_List_removePointer(self->elements, R_Dictionary_getElement(self, key));
}

void* R_Dictionary_get(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return NULL;
	R_Dictionary_Element* element = R_Dictionary_getElement(self, key);
	if (element == NULL) return NULL;
	return element->value;
}

R_Dictionary_Element* R_Dictionary_getElement(R_Dictionary* self, const char* key) {
	if (self == NULL || key == NULL) return NULL;
	R_List_each(self->elements, R_Dictionary_Element, element) {
		if (R_String_compare(element->key, key)) return element;
	}
	return NULL;
}

void R_Dictionary_removeAll(R_Dictionary* self) {
	if (self == NULL) return;
	R_List_removeAll(self->elements);
}
bool R_Dictionary_isPresent(R_Dictionary* self, const char* key) {
	if (R_Dictionary_getElement(self, key) == NULL) return false;
	return true;
}
bool R_Dictionary_isNotPresent(R_Dictionary* self, const char* key) {
	return !R_Dictionary_isPresent(self, key);
}
size_t R_Dictionary_size(R_Dictionary* self) {
	if (self == NULL) return 0;
	return R_List_size(self->elements);
}

void R_Dictionary_toJson_writeValue(R_String* buffer, void* value);
R_String* R_Dictionary_toJson(R_Dictionary* self, R_String* buffer) {
	if (self == NULL || R_Dictionary_size(self) == 0 || buffer == NULL || R_String_reset(buffer) == NULL) return NULL;
	R_String_appendCString(buffer, "{");

	R_List_each(self->elements, R_Dictionary_Element, element) {
		R_String_appendStringAsJson(buffer, element->key);
		R_String_appendCString(buffer, ":");
		R_Dictionary_toJson_writeValue(buffer, element->value);
		if (element != R_List_last(self->elements)) R_String_appendCString(buffer, ",");
	}

	R_String_appendCString(buffer, "}");
	return buffer;
}

void R_Dictionary_toJson_writeValue(R_String* buffer, void* value) {
	if (R_Type_IsOf(value, R_String)) R_String_appendStringAsJson(buffer, value);
	else if (R_Type_IsOf(value, R_Integer)) R_String_appendInt(buffer, R_Integer_get(value));
	else if (R_Type_IsOf(value, R_Float)) R_String_appendFloat(buffer, R_Float_get(value));
	else if (R_Type_IsOf(value, R_Boolean)) {
		if (R_Boolean_get(value)) R_String_appendCString(buffer, "true");
		else R_String_appendCString(buffer, "false");
	}
	else if (R_Type_IsOf(value, R_Dictionary)) {
		R_String* sub_buffer = R_Type_New(R_String);
		R_String_appendString(buffer, R_Dictionary_toJson(value, sub_buffer));
		R_Type_Delete(sub_buffer);
	}
	else if (R_Type_IsOf(value, R_List)) {
		R_String_appendCString(buffer, "[");
		R_List_each(value, void, element) {
			R_Dictionary_toJson_writeValue(buffer, element);
			if (element != R_List_last(value)) R_String_appendCString(buffer, ",");
		}
		R_String_appendCString(buffer, "]");
	}
	else R_String_appendCString(buffer, "\"Unknown Type\"");
}

R_String* R_Dictionary_fromJson_moveQuotedString(R_String* source, R_String* dest);
R_Dictionary* R_Dictionary_fromJson(R_Dictionary* self, R_String* buffer) {
	if (self == NULL || buffer == NULL) return NULL;
	R_String* string = R_Type_Copy(buffer);
	if (string == NULL) return NULL;
	R_String_trim(string);
	if (R_String_first(string) != '{') {
		R_Type_delete(string);
		return NULL;
	}
	R_String_shift(string);
	R_String_trim(string);
	while (1) {
		R_String* key = R_Type_New(R_String);
		if (R_Dictionary_fromJson_moveQuotedString(string, key) == NULL) {
			R_Type_Delete(key);
			R_Type_Delete(string);
			return NULL;
		}
		R_String_trim(string);
		if (R_String_first(string) != ':') {
			R_Type_Delete(key);
			R_Type_Delete(string);
			return NULL;
		}
		R_String_shift(string);
		R_String_trim(string);
		if (R_String_first(string) == '"') {//value is a string
			R_String* value = R_Type_New(R_String);
			if (R_Dictionary_fromJson_moveQuotedString(string, value) == NULL) {
				R_Type_Delete(value);
				R_Type_Delete(key);
				R_Type_Delete(string);
				return NULL;
			}
		}
	}

	return R_Dictionary_fromJson_readObject(self, string);
}

R_String* R_Dictionary_fromJson_moveQuotedString(R_String* source, R_String* dest) {
	if (source == NULL || dest == NULL) return NULL;
	if (R_String_first(string) != '"') return NULL;
	R_String_shift(source);
	while (R_String_length(source) > 0) {
		char character = R_String_shift(source);
		if (character == '\\' && R_String_length(source) > 0) {
			char escaped = R_String_shift(string);
			if (escaped == '\\') R_String_appendCString(dest, "\\");
			else if (escaped == '/') R_String_appendCString(dest, "/");
			else if (escaped == 'b') R_String_appendCString(dest, "\b");
			else if (escaped == 'f') R_String_appendCString(dest, "\f");
			else if (escaped == 'n') R_String_appendCString(dest, "\n");
			else if (escaped == 'r') R_String_appendCString(dest, "\r");
			else if (escaped == 't') R_String_appendCString(dest, "\t");
		}
		else if (character == '"') return dest;
		else R_String_appendBytes(dest, &character, 1);
	}
	return NULL;
}




