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


struct R_KeyValuePair {
  R_Type* type;
  R_MutableString* key;
  void* value; //May be a string, integer, float, array of values or a dictionary
};
static R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_Constructor(R_KeyValuePair* self);
static R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_Destructor(R_KeyValuePair* self);
static R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_Copier(R_KeyValuePair* self, R_KeyValuePair* new);

static R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_Constructor(R_KeyValuePair* self) {
  self->key = R_Type_New(R_MutableString);
  return self;
}

static R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_Destructor(R_KeyValuePair* self) {
  R_Type_DeleteAndNull(self->key);
  R_Type_DeleteAndNull(self->value);
  return self;
}

static R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_Copier(R_KeyValuePair* self, R_KeyValuePair* new) {
  new->value = R_Type_Copy(self->value);
  if (new->value == NULL) return R_Type_Delete(new), NULL;
  R_MutableString_appendString(new->key, self->key);
  return new;
}

R_Type_Def(R_KeyValuePair, R_KeyValuePair_Constructor, R_KeyValuePair_Destructor, R_KeyValuePair_Copier, NULL);

R_MutableString* R_FUNCTION_ATTRIBUTES R_KeyValuePair_key(R_KeyValuePair* self) {
  if (R_Type_IsNotOf(self, R_KeyValuePair)) return NULL;
  return self->key;
}

void* R_FUNCTION_ATTRIBUTES R_KeyValuePair_value(R_KeyValuePair* self) {
  if (R_Type_IsNotOf(self, R_KeyValuePair)) return NULL;
  return self->value;
}

R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_setKey(R_KeyValuePair* self, const char* key) {
  if (R_Type_IsNotOf(self, R_KeyValuePair)) return NULL;
  R_MutableString_setString(self->key, key);
  return self;
}

R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_setValue(R_KeyValuePair* self, void* value) {
  if (R_Type_IsNotOf(self, R_KeyValuePair)) return NULL;
  R_Type_Delete(self->value);
  self->value = value;
  return self;
}
