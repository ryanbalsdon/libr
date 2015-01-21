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
