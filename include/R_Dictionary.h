#ifndef R_Dictionary_h
#define R_Dictionary_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdbool.h>
#include "R_Type.h"
#include "R_String.h"

typedef struct R_Dictionary R_Dictionary;
R_Type_Declare(R_Dictionary);

void* R_Dictionary_addObjectOfType(R_Dictionary* self, const char* key, const R_Type* type);
#define R_Dictionary_add(self, key, Type) (Type*)R_Dictionary_addObjectOfType(self, key, R_Type_Object(Type))
void* R_Dictionary_addCopy(R_Dictionary* self, const char* key, const void* object);
void R_Dictionary_remove(R_Dictionary* self, const char* key);
void* R_Dictionary_get(R_Dictionary* self, const char* key);

void R_Dictionary_removeAll(R_Dictionary* self);
bool R_Dictionary_isPresent(R_Dictionary* self, const char* key);
bool R_Dictionary_isNotPresent(R_Dictionary* self, const char* key);

size_t R_Dictionary_size(R_Dictionary* self);

R_String* R_Dictionary_toJson(R_Dictionary* self, R_String* buffer);
R_Dictionary* R_Dictionary_fromJson(R_Dictionary* self, R_String* buffer);

#endif /* R_Dictionary_h */
