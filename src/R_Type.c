/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdlib.h>
#include "R_Type.h"

size_t R_Type_BytesAllocated = 0;

void* R_Type_New(const R_Type* type) {
  if (type->size < sizeof(R_Type*)) return NULL; //If they were equal, this object would be useless. No good reason to limit that though...
  void* new_object = calloc(1, type->size);
  if (new_object != NULL) *(const R_Type**)new_object = type;
  if (new_object != NULL) R_Type_BytesAllocated += type->size;
  if (new_object != NULL && type->ctor != NULL) type->ctor(new_object);
  return new_object; 
}

void R_Type_Delete(void* object) {
  if (object == NULL) return;
  R_Type* type = *(R_Type**)object; //First element of every object must be an R_Type*
  R_Type_BytesAllocated -= type->size;
  if (type->dtor != NULL) free(type->dtor(object));
  else free(object);
}

void* R_Type_Copy(void* object) {
  if (object == NULL) return NULL;
  R_Type* type = *(R_Type**)object; //First element of every object must be an R_Type*
  if (type->copy == NULL) return NULL;

  void* new_object = R_Type_New(type);
  if (new_object != NULL) type->copy(object, new_object);
  return new_object;
}

R_Type* R_Type_Type(void* object) {
  return *(R_Type**)object; //First element of every object must be an R_Type*
}
