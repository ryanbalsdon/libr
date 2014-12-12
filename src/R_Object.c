/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdlib.h>
#include "R_Object.h"

size_t R_Object_BytesAllocated = 0;

void* R_Object_New(const R_Object* type) {
  if (type->size < sizeof(R_Object*)) return NULL; //If they were equal, this object would be useless. No good reason to limit that though...
  void* new_object = calloc(1, type->size);
  if (new_object != NULL) *(const R_Object**)new_object = type;
  if (new_object != NULL) R_Object_BytesAllocated += type->size;
  if (new_object != NULL && type->ctor != NULL) type->ctor(new_object);
  return new_object; 
}

void R_Object_Delete(void* object) {
  if (object == NULL) return;
  R_Object* type = *(R_Object**)object; //First element of every object must be an R_Object*
  R_Object_BytesAllocated -= type->size;
  if (type->dtor != NULL) free(type->dtor(object));
  else free(object);
}

void* R_Object_Copy(void* object) {
  if (object == NULL) return NULL;
  R_Object* type = *(R_Object**)object; //First element of every object must be an R_Object*
  if (type->copy == NULL) return NULL;

  void* new_object = R_Object_New(type);
  if (new_object != NULL) type->copy(object, new_object);
  return new_object;
}