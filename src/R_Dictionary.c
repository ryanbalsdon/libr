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
static R_Dictionary_Element* R_Dictionary_Element_Constructor(R_Dictionary_Element* self);
static R_Dictionary_Element* R_Dictionary_Element_Destructor(R_Dictionary_Element* self);
static R_Dictionary_Element* R_Dictionary_Element_Copier(R_Dictionary_Element* self, R_Dictionary_Element* new);

static R_Dictionary_Element* R_Dictionary_Element_Constructor(R_Dictionary_Element* self) {
	self->key = R_Type_New(R_String);
	return self;
}

static R_Dictionary_Element* R_Dictionary_Element_Destructor(R_Dictionary_Element* self) {
	R_Type_DeleteAndNull(self->key);
	R_Type_DeleteAndNull(self->value);
	return self;
}

static R_Dictionary_Element* R_Dictionary_Element_Copier(R_Dictionary_Element* self, R_Dictionary_Element* new) {
	new->value = R_Type_Copy(self->value);
	if (new->value == NULL) return R_Type_Delete(new), NULL;
	R_String_appendString(new->key, self->key);
	return new;
}

R_Type_Def(R_Dictionary_Element, R_Dictionary_Element_Constructor, R_Dictionary_Element_Destructor, R_Dictionary_Element_Copier);

struct R_Dictionary {
	R_Type* type;
	R_List* elements;
};
static R_Dictionary* R_Dictionary_Constructor(R_Dictionary* self);
static R_Dictionary* R_Dictionary_Destructor(R_Dictionary* self);
static R_Dictionary* R_Dictionary_Copier(R_Dictionary* self, R_Dictionary* new);

static R_Dictionary* R_Dictionary_Constructor(R_Dictionary* self) {
	self->elements = R_Type_New(R_List);
	return self;
}

static R_Dictionary* R_Dictionary_Destructor(R_Dictionary* self) {
	R_Type_DeleteAndNull(self->elements);
	return self;
}
static R_Dictionary* R_Dictionary_Copier(R_Dictionary* self, R_Dictionary* new) {
	if (R_List_appendList(new->elements, self->elements) == NULL) return R_Type_Delete(new), NULL;
	return new;
}
R_Type_Def(R_Dictionary, R_Dictionary_Constructor, R_Dictionary_Destructor, R_Dictionary_Copier);

static R_Dictionary_Element* R_Dictionary_getElement(R_Dictionary* self, const char* key);

void* R_Dictionary_addObjectOfType(R_Dictionary* self, const char* key, const R_Type* type) {
	if (R_Type_IsNotOf(self, R_Dictionary) || key == NULL || type == NULL) return NULL;
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

void* R_Dictionary_addCopy(R_Dictionary* self, const char* key, const void* object) {
	if (R_Type_IsNotOf(self, R_Dictionary) || key == NULL || object == NULL) return NULL;
	R_Dictionary_Element* element = R_Dictionary_getElement(self, key);
	if (element == NULL) {
		element = R_List_add(self->elements, R_Dictionary_Element);
		if (element == NULL) return NULL;
		if (R_String_setString(element->key, key) == NULL) return NULL;
	}
	if (element->value != NULL) R_Type_Delete(element->value);
	element->value = R_Type_Copy(object);
	return element->value;
}

R_Dictionary* R_Dictionary_merge(R_Dictionary* self, R_Dictionary* dictionary_to_copy) {
	if (R_Type_IsNotOf(self, R_Dictionary) || R_Type_IsNotOf(dictionary_to_copy, R_Dictionary)) return NULL;
	R_List_each(dictionary_to_copy->elements, R_Dictionary_Element, element) {
		if (R_Dictionary_addCopy(self, R_String_cstring(element->key), element->value) == NULL) return NULL;
	}
	return self;
}

void* R_Dictionary_transferOwnership(R_Dictionary* self, const char* key, void* object) {
	if (R_Type_IsNotOf(self, R_Dictionary) || key == NULL) return NULL;
	R_Dictionary_Element* element = R_Dictionary_getElement(self, key);
	if (element == NULL) {
		element = R_List_add(self->elements, R_Dictionary_Element);
		if (element == NULL) return NULL;
		if (R_String_setString(element->key, key) == NULL) return NULL;
	}
	if (element->value != NULL) R_Type_Delete(element->value);
	element->value = object;
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

void* R_Dictionary_getFromString(R_Dictionary* self, R_String* key) {
	return R_Dictionary_get(self, R_String_cstring(key));
}

static R_Dictionary_Element* R_Dictionary_getElement(R_Dictionary* self, const char* key) {
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

static void R_Dictionary_toJson_writeValue(R_String* buffer, void* value);
R_String* R_Dictionary_toJson(R_Dictionary* self, R_String* buffer) {
	if (R_Type_IsNotOf(self, R_Dictionary) || buffer == NULL || R_String_reset(buffer) == NULL) return NULL;
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

static void R_Dictionary_toJson_writeValue(R_String* buffer, void* value) {
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
	else if (R_Type_IsOf(value, R_Null)) R_String_appendCString(buffer, "null");
	else R_String_appendCString(buffer, "\"Unknown Type\"");
}

static R_String* R_Dictionary_fromJson_moveQuotedString(R_String* source, R_String* dest);
static void* R_Dictionary_fromJson_readNumber(R_String* source);
static void* R_Dictionary_fromJson_readValue(R_String* string);
static R_Dictionary* R_Dictionary_fromJson_readObject(R_Dictionary* object, R_String* string);
static R_String* R_Dictionary_fromJson_advanceToNextNonWhitespace(R_String* string);
static R_List* R_Dictionary_fromJson_readArray(R_String* string);
R_Dictionary* R_Dictionary_fromJson(R_Dictionary* self, R_String* buffer) {
	if (self == NULL || buffer == NULL) return NULL;
	R_Dictionary_removeAll(self);
	R_String* string = R_Type_Copy(buffer);
	if (string == NULL) return NULL;
	R_String_trim(string);

	R_Dictionary_fromJson_readObject(self, string);

	R_Type_Delete(string);
	return self;
}

static R_String* R_Dictionary_fromJson_moveQuotedString(R_String* source, R_String* dest) {
	if (source == NULL || dest == NULL) return NULL;
	if (R_String_first(source) != '"') return NULL;
	R_String_shift(source);
	while (R_String_length(source) > 0) {
		char character = R_String_shift(source);
		if (character == '\\' && R_String_length(source) > 0) {
			char escaped = R_String_shift(source);
			if (escaped == '\\') R_String_appendCString(dest, "\\");
			else if (escaped == '/') R_String_appendCString(dest, "/");
			else if (escaped == 'b') R_String_appendCString(dest, "\b");
			else if (escaped == 'f') R_String_appendCString(dest, "\f");
			else if (escaped == 'n') R_String_appendCString(dest, "\n");
			else if (escaped == 'r') R_String_appendCString(dest, "\r");
			else if (escaped == 't') R_String_appendCString(dest, "\t");
		}
		else if (character == '"') {
			R_String_trim(source);
			return dest;
		}
		else R_String_push(dest, character);
	}
	return NULL;
}

static R_Dictionary* R_Dictionary_fromJson_readObject(R_Dictionary* object, R_String* string) {
	if (R_String_first(string) != '{') return NULL;
	R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
	while (R_String_length(string) > 0) {
		if (R_String_first(string) == '}') break;
		//find the key as a quoted string
		R_String* key = R_Type_New(R_String);
		if (R_Dictionary_fromJson_moveQuotedString(string, key) == NULL) return R_Type_Delete(key), NULL;
		//ignore the separator
		if (R_String_first(string) != ':') return R_Type_Delete(key), NULL;
		R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
		//read value
		void* value = R_Dictionary_fromJson_readValue(string);
		//add kay/value to dictionary
		if (value == NULL || R_Dictionary_transferOwnership(object, R_String_cstring(key), value) == NULL) return R_Type_Delete(key), NULL;
		//cleanup
		R_Type_Delete(key);
		//decide whether there are more objects to read
		R_String_trim(string);
		if (R_String_first(string) == ',') {
			R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
			continue;
		}
		else if (R_String_first(string) == '}') break;
		else return NULL;
	}

	if (R_String_first(string) != '}') return NULL;
	R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
	return object;
}

static R_String* R_Dictionary_fromJson_advanceToNextNonWhitespace(R_String* string) {
	R_String_shift(string);
	R_String_trim(string);
	return string;
}

static void* R_Dictionary_fromJson_readValue(R_String* string) {
	if (R_String_first(string) == '"') {//value is a string
		R_String* value = R_Type_New(R_String);
		if (R_Dictionary_fromJson_moveQuotedString(string, value) == NULL) return R_Type_Delete(value), NULL;
		return value;
	}
	else if ((R_String_first(string) >= '0' && R_String_first(string) <= '9') || R_String_first(string) == '-') {//value is a number
		return R_Dictionary_fromJson_readNumber(string);
	}
	else if (R_String_first(string) == 't' || R_String_first(string) == 'f') {//value is a boolean (true)
		const char* characters = R_String_cstring(string);
		if (strstr(characters, "true") == characters) {
			R_String_getSubstring(string, string, 4, 0);
			return R_Boolean_set(R_Type_New(R_Boolean), true);
		}
		else if (strstr(characters, "false") == characters) {
			R_String_getSubstring(string, string, 5, 0);
			return R_Boolean_set(R_Type_New(R_Boolean), false);
		}
	}
	else if (R_String_first(string) == 'n') {//value is a null
		const char* characters = R_String_cstring(string);
		if (strstr(characters, "null") == characters) {
			R_String_getSubstring(string, string, 4, 0);
			return R_Type_New(R_Null);
		}
	}
	else if (R_String_first(string) == '{') {
		R_Dictionary* child = R_Type_New(R_Dictionary);
		if (R_Dictionary_fromJson_readObject(child, string) == NULL) {
			R_Type_Delete(child);
		}
		else return child;
	}
	else if (R_String_first(string) == '[') {
		return R_Dictionary_fromJson_readArray(string);
	}
	return NULL;
}

static R_List* R_Dictionary_fromJson_readArray(R_String* string) {
	if (R_String_first(string) != '[') return NULL;
	R_List* array = R_Type_New(R_List);
	R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
	while (R_String_length(string) > 0) {
		if (R_String_first(string) == ']') {
			R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
			return array;
		}
		void* value = R_Dictionary_fromJson_readValue(string);
		if (value == NULL || R_List_transferOwnership(array, value) == NULL) return R_Type_Delete(array), NULL;
		R_String_trim(string);
		if (R_String_first(string) == ',') {
			R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
			continue;
		}
		else if (R_String_first(string) == ']') {
			R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
			return array;
		}
		else return R_Type_Delete(array), NULL;
	}
	return R_Type_Delete(array), NULL;
}

static void* R_Dictionary_fromJson_readNumber(R_String* source) {
	if (source == NULL) return NULL;
	R_String* value = R_Type_New(R_String);
	bool isFloat = false;
	bool isExponent = false;
	while (R_String_length(source) > 0) {
		if (R_String_first(source) >= '0' && R_String_first(source) <= '9') {
			R_String_push(value, R_String_shift(source));
			continue;
		}
		else if (R_String_first(source) == '.') {
			R_String_push(value, R_String_shift(source));
			isFloat = true;
			continue;
		}
		else if (R_String_first(source) == 'e' || R_String_first(source) == 'E') {
			R_String_push(value, R_String_shift(source));
			isExponent = true;
			isFloat = true;
			continue;
		}
		else if (isExponent && (R_String_first(source) == '+' || R_String_first(source) == '-')) {
			R_String_push(value, R_String_shift(source));
			continue;
		}
		else if (R_String_length(value) == 0 && R_String_first(source) == '-') {
			R_String_push(value, R_String_shift(source));
			continue;
		}
		else break;
	}

	if (isFloat) {
		float floater = R_String_getFloat(value);
		R_Type_Delete(value);
		R_Float* number = R_Type_New(R_Float);
		R_Float_set(number, floater);
		return number;
	}
	else { //is integer
		int integer = R_String_getInt(value);
		R_Type_Delete(value);
		R_Integer* number = R_Type_New(R_Integer);
		R_Integer_set(number, integer);
		return number;
	}
	return R_Type_Delete(value), NULL;
}


typedef struct {
    R_Type* type;
    size_t previous_index;
    R_Dictionary* dictionary; //This is not a copy. Do not call R_Type_Delete on it!
} R_Dictionary_Iterator_State;
R_Type_Def(R_Dictionary_Iterator_State, NULL, NULL, NULL);

static void* R_Dictionary_valueIterator(R_Dictionary_Iterator_State* state) {
    if (state->previous_index == 0) return NULL;
    R_Dictionary_Element* element = R_List_pointerAtIndex(state->dictionary->elements, --state->previous_index);
    if (element == NULL) return NULL;
    return element->value;
}

R_Functor* R_Dictionary_ValueIterator(R_Functor* functor, R_Dictionary* dictionary) {
    R_Dictionary_Iterator_State* state = R_Type_New(R_Dictionary_Iterator_State);
    state->dictionary = dictionary;
    state->previous_index = R_List_size(dictionary->elements);
    functor->state = state;
    functor->function = (R_Functor_Function)R_Dictionary_valueIterator;
    return functor;
}

static void* R_Dictionary_keyIterator(R_Dictionary_Iterator_State* state) {
    if (state->previous_index == 0) return NULL;
    R_Dictionary_Element* element = R_List_pointerAtIndex(state->dictionary->elements, --state->previous_index);
    if (element == NULL) return NULL;
    return element->key;
}

R_Functor* R_Dictionary_KeyIterator(R_Functor* functor, R_Dictionary* dictionary) {
    R_Dictionary_Iterator_State* state = R_Type_New(R_Dictionary_Iterator_State);
    state->dictionary = dictionary;
    state->previous_index = R_List_size(dictionary->elements);
    functor->state = state;
    functor->function = (R_Functor_Function)R_Dictionary_keyIterator;
    return functor;
}
