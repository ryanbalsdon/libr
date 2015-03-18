/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdlib.h>
#include <string.h>
#include "R_Type.h"

size_t R_Type_BytesAllocated = 0;

void* R_Type_NewObjectOfType(const R_Type* type) {
  if (type->size < sizeof(R_Type*)) return NULL; //If they were equal, this object would be useless. No good reason to limit that though...
  void* new_object = calloc(1, type->size);
  if (new_object != NULL) *(const R_Type**)new_object = type;
  if (new_object != NULL) R_Type_BytesAllocated += type->size;
  if (new_object != NULL && type->ctor != NULL && type->ctor(new_object) == NULL) {
    free(new_object);
    new_object = NULL;
  }
  return new_object;
}

void R_Type_Delete(void* object) {
  if (object == NULL) return;
  R_Type* type = *(R_Type**)object; //First element of every object must be an R_Type*
  R_Type_BytesAllocated -= type->size;
  if (type->dtor != NULL) free(type->dtor(object));
  else free(object);
}

void* R_Type_Copy(const void* object) {
  if (object == NULL) return NULL;
  R_Type* type = *(R_Type**)object; //First element of every object must be an R_Type*
  if (type->copy == NULL) return NULL;

  void* new_object = R_Type_NewObjectOfType(type);
  if (new_object != NULL) return type->copy(object, new_object);
  return NULL;
}

int R_Type_IsObjectOfType(const void* object, const R_Type* type) {
  if (object == NULL || type == NULL) return 0;
  R_Type* type_of_object = *(R_Type**)object; //First element of every object must be an R_Type*
  if (type_of_object != type) return 0;
  return 1;
}

int R_Type_IsObjectNotOfType(const void* object, const R_Type* type) {
  return !R_Type_IsObjectOfType(object, type);
}

void R_Type_shallowCopy(const void* object_input, void* object_output) {
  if (object_input == NULL || object_output == NULL) return;
  R_Type* input_type  = *(R_Type**)object_input;
  R_Type* output_type = *(R_Type**)object_output;
  if (input_type->size != output_type->size) return;
  memcpy(object_output, object_input, input_type->size);
}

struct R_Integer {R_Type* type; int Integer;};
R_Type_Def(R_Integer, NULL, NULL, R_Type_shallowCopy);
R_Integer* R_Integer_set(R_Integer* self, int value) {
  if (self == NULL) return NULL;
  self->Integer = value; 
  return self;
}
int R_Integer_get(R_Integer* self) {
  return self->Integer;
}

struct R_Float {R_Type* type; float Float;};
R_Type_Def(R_Float, NULL, NULL, R_Type_shallowCopy);
R_Float* R_Float_set(R_Float* self, float value) {
  if (self == NULL) return NULL;
  self->Float = value; 
  return self;
}
float R_Float_get(R_Float* self) {
  return self->Float;
}

struct R_Unsigned {R_Type* type; unsigned int Integer;};
R_Type_Def(R_Unsigned, NULL, NULL, R_Type_shallowCopy);
R_Unsigned* R_Unsigned_set(R_Unsigned* self, unsigned int value) {
  if (self == NULL) return NULL;
  self->Integer = value; 
  return self;
}
unsigned int R_Unsigned_get(R_Unsigned* self) {
  return self->Integer;
}

struct R_Boolean {R_Type* type; bool Boolean;};
R_Type_Def(R_Boolean, NULL, NULL, R_Type_shallowCopy);
R_Boolean* R_Boolean_set(R_Boolean* self, bool value) {
  if (self == NULL || (value != 0 && value != 1)) return NULL;
  self->Boolean = value; 
  return self;
}
bool R_Boolean_get(R_Boolean* self) {
  return self->Boolean;
}

struct R_Null {R_Type* type;};
R_Type_Def(R_Null, NULL, NULL, R_Type_shallowCopy);

