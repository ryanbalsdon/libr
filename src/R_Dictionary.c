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
#include "R_MutableString.h"


struct R_Dictionary {
	R_Type* type;
	R_List* elements;
};
static R_Dictionary* R_FUNCTION_ATTRIBUTES R_Dictionary_Constructor(R_Dictionary* self);
static R_Dictionary* R_FUNCTION_ATTRIBUTES R_Dictionary_Destructor(R_Dictionary* self);
static R_Dictionary* R_Dictionary_Copier(R_Dictionary* self, R_Dictionary* new);
static void* R_FUNCTION_ATTRIBUTES R_Dictionary_Methods(const R_Face* interface);
R_Type_Def(R_Dictionary, R_Dictionary_Constructor, R_Dictionary_Destructor, R_Dictionary_Copier, R_Dictionary_Methods);

static R_Dictionary* R_FUNCTION_ATTRIBUTES R_Dictionary_Constructor(R_Dictionary* self) {
	self->elements = R_Type_New(R_List);
	return self;
}

static R_Dictionary* R_FUNCTION_ATTRIBUTES R_Dictionary_Destructor(R_Dictionary* self) {
	R_Type_DeleteAndNull(self->elements);
	return self;
}
static R_Dictionary* R_FUNCTION_ATTRIBUTES R_Dictionary_Copier(R_Dictionary* self, R_Dictionary* new) {
	if (R_List_appendList(new->elements, self->elements) == NULL) return R_Type_Delete(new), NULL;
	return new;
}
static void* R_FUNCTION_ATTRIBUTES R_Dictionary_Methods(const R_Face* interface) {
	R_Face_DefJump(R_Puts, R_Dictionary_puts);
	return NULL;
}

R_List* R_FUNCTION_ATTRIBUTES R_Dictionary_listOfPairs(R_Dictionary* self) {
  if (R_Type_IsNotOf(self, R_Dictionary)) return NULL;
  return self->elements;
}

static R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_Dictionary_getElement(R_Dictionary* self, const char* key);

void* R_FUNCTION_ATTRIBUTES R_Dictionary_addObjectOfType(R_Dictionary* self, const char* key, const R_Type* type) {
	if (R_Type_IsNotOf(self, R_Dictionary) || key == NULL || type == NULL) return NULL;
	R_KeyValuePair* element = R_Dictionary_getElement(self, key);
	if (element == NULL) {
		element = R_List_add(self->elements, R_KeyValuePair);
		if (element == NULL) return NULL;
    R_KeyValuePair_setKey(element, key);
	}
  R_KeyValuePair_setValue(element, R_Type_NewObjectOfType(type));
	return R_KeyValuePair_value(element);
}

void* R_FUNCTION_ATTRIBUTES R_Dictionary_addCopy(R_Dictionary* self, const char* key, const void* object) {
	if (R_Type_IsNotOf(self, R_Dictionary) || key == NULL || object == NULL) return NULL;
	R_KeyValuePair* element = R_Dictionary_getElement(self, key);
	if (element == NULL) {
		element = R_List_add(self->elements, R_KeyValuePair);
		if (element == NULL) return NULL;
    R_KeyValuePair_setKey(element, key);
	}
  R_KeyValuePair_setValue(element, R_Type_Copy(object));
  return R_KeyValuePair_value(element);
}

R_Dictionary* R_FUNCTION_ATTRIBUTES R_Dictionary_merge(R_Dictionary* self, R_Dictionary* dictionary_to_copy) {
	if (R_Type_IsNotOf(self, R_Dictionary) || R_Type_IsNotOf(dictionary_to_copy, R_Dictionary)) return NULL;
	R_List_each(dictionary_to_copy->elements, R_KeyValuePair, element) {
		if (R_Dictionary_addCopy(self, R_MutableString_cstring(R_KeyValuePair_key(element)), R_KeyValuePair_value(element)) == NULL) return NULL;
	}
	return self;
}

void* R_FUNCTION_ATTRIBUTES R_Dictionary_transferOwnership(R_Dictionary* self, const char* key, void* object) {
	if (R_Type_IsNotOf(self, R_Dictionary) || key == NULL) return NULL;
	R_KeyValuePair* element = R_Dictionary_getElement(self, key);
	if (element == NULL) {
		element = R_List_add(self->elements, R_KeyValuePair);
		if (element == NULL) return NULL;
    R_KeyValuePair_setKey(element, key);
	}
  R_KeyValuePair_setValue(element, object);
  return R_KeyValuePair_value(element);
}

void R_FUNCTION_ATTRIBUTES R_Dictionary_remove(R_Dictionary* self, const char* key) {
	if (R_Type_IsNotOf(self, R_Dictionary) || key == NULL) return;
	R_List_removePointer(self->elements, R_Dictionary_getElement(self, key));
}

void* R_FUNCTION_ATTRIBUTES R_Dictionary_get(R_Dictionary* self, const char* key) {
	if (R_Type_IsNotOf(self, R_Dictionary) || key == NULL) return NULL;
	R_KeyValuePair* element = R_Dictionary_getElement(self, key);
	if (element == NULL) return NULL;
	return R_KeyValuePair_value(element);
}

void* R_FUNCTION_ATTRIBUTES R_Dictionary_getFromString(R_Dictionary* self, R_MutableString* key) {
	return R_Dictionary_get(self, R_MutableString_cstring(key));
}

static R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_Dictionary_getElement(R_Dictionary* self, const char* key) {
	if (R_Type_IsNotOf(self, R_Dictionary) || key == NULL) return NULL;
	R_List_each(self->elements, R_KeyValuePair, element) {
		if (R_MutableString_compare(R_KeyValuePair_key(element), key)) return element;
	}
	return NULL;
}

void R_FUNCTION_ATTRIBUTES R_Dictionary_removeAll(R_Dictionary* self) {
  if (R_Type_IsNotOf(self, R_Dictionary)) return;
	R_List_removeAll(self->elements);
}
bool R_FUNCTION_ATTRIBUTES R_Dictionary_isPresent(R_Dictionary* self, const char* key) {
	if (R_Dictionary_getElement(self, key) == NULL) return false;
	return true;
}
bool R_FUNCTION_ATTRIBUTES R_Dictionary_isNotPresent(R_Dictionary* self, const char* key) {
	return !R_Dictionary_isPresent(self, key);
}
size_t R_FUNCTION_ATTRIBUTES R_Dictionary_size(R_Dictionary* self) {
  if (R_Type_IsNotOf(self, R_Dictionary)) return 0;
	return R_List_size(self->elements);
}

void R_FUNCTION_ATTRIBUTES R_Dictionary_puts(R_Dictionary* self) {
  if (R_Type_IsNotOf(self, R_Dictionary)) return;
  R_MutableString* buffer = R_Type_New(R_MutableString);
  R_Dictionary_toJson(self, buffer);
  R_MutableString_puts(buffer);
  R_Type_Delete(buffer);
}
