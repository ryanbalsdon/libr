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
  if (new_object != NULL) type->copy(object, new_object);
  return new_object;
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
