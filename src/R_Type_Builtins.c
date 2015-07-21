/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdlib.h>
#include <stdio.h>
#include "R_Type.h"


/* R_Integer */
static void* R_FUNCTION_ATTRIBUTES R_Integer_Methods(const R_Face* interface);
R_Type_Define(R_Integer, .copy = R_Type_shallowCopy, .interfaces = R_Integer_Methods);
R_Integer* R_FUNCTION_ATTRIBUTES R_Integer_set(R_Integer* self, int value) {
  if (self == NULL) return NULL;
  self->value = value; 
  return self;
}
int R_FUNCTION_ATTRIBUTES R_Integer_get(R_Integer* self) {
  return self->value;
}
static void R_FUNCTION_ATTRIBUTES R_Integer_puts(R_Integer* self) {
  if (R_Type_IsNotOf(self, R_Integer)) return;
  os_printf("%d\n", self->value);
}
static void* R_FUNCTION_ATTRIBUTES R_Integer_Methods(const R_Face* interface) {
  R_Face_DefJump(R_Puts, R_Integer_puts);
  return NULL;
}


/* R_Float */
static void* R_FUNCTION_ATTRIBUTES R_Float_Methods(const R_Face* interface);
R_Type_Define(R_Float, .copy = R_Type_shallowCopy, .interfaces = R_Float_Methods);
R_Float* R_FUNCTION_ATTRIBUTES R_Float_set(R_Float* self, float value) {
  if (self == NULL) return NULL;
  self->value = value; 
  return self;
}
float R_FUNCTION_ATTRIBUTES R_Float_get(R_Float* self) {
  return self->value;
}
static void R_FUNCTION_ATTRIBUTES R_Float_puts(R_Float* self) {
  if (R_Type_IsNotOf(self, R_Float)) return;
  os_printf("%g\n", self->value);
}
static void* R_FUNCTION_ATTRIBUTES R_Float_Methods(const R_Face* interface) {
  R_Face_DefJump(R_Puts, R_Float_puts);
  return NULL;
}


/* R_Unsigned */
static void* R_FUNCTION_ATTRIBUTES R_Unsigned_Methods(const R_Face* interface);
R_Type_Define(R_Unsigned, .copy = R_Type_shallowCopy, .interfaces = R_Unsigned_Methods);
R_Unsigned* R_FUNCTION_ATTRIBUTES R_Unsigned_set(R_Unsigned* self, unsigned int value) {
  if (self == NULL) return NULL;
  self->value = value; 
  return self;
}
unsigned int R_FUNCTION_ATTRIBUTES R_Unsigned_get(R_Unsigned* self) {
  return self->value;
}
static void R_FUNCTION_ATTRIBUTES R_Unsigned_puts(R_Unsigned* self) {
  if (R_Type_IsNotOf(self, R_Unsigned)) return;
  os_printf("%u\n", self->value);
}
static void* R_FUNCTION_ATTRIBUTES R_Unsigned_Methods(const R_Face* interface) {
  R_Face_DefJump(R_Puts, R_Unsigned_puts);
  return NULL;
}


/* R_Boolean */
static void* R_FUNCTION_ATTRIBUTES R_Boolean_Methods(const R_Face* interface);
R_Type_Define(R_Boolean, .copy = R_Type_shallowCopy, .interfaces = R_Boolean_Methods);
R_Boolean* R_FUNCTION_ATTRIBUTES R_Boolean_set(R_Boolean* self, bool value) {
  if (self == NULL || (value != 0 && value != 1)) return NULL;
  self->value = value; 
  return self;
}
bool R_FUNCTION_ATTRIBUTES R_Boolean_get(R_Boolean* self) {
  return self->value;
}
static void R_FUNCTION_ATTRIBUTES R_Boolean_puts(R_Boolean* self) {
  if (R_Type_IsNotOf(self, R_Boolean)) return;
  os_printf("%s\n", self->value ? "true" : "false");
}
static void* R_FUNCTION_ATTRIBUTES R_Boolean_Methods(const R_Face* interface) {
  R_Face_DefJump(R_Puts, R_Boolean_puts);
  return NULL;
}


/* R_Null */
R_Type_Define(R_Null, .copy = R_Type_shallowCopy);


/* R_Data */
R_Data* R_FUNCTION_ATTRIBUTES R_Data_Destructor(R_Data* self);
R_Type_Define(R_Data, .dtor = (R_Type_Destructor)R_Data_Destructor);
R_Data* R_FUNCTION_ATTRIBUTES R_Data_Destructor(R_Data* self) {
  if (self->is_owner && self->bytes) {
    os_free(self->bytes);
    R_Type_BytesAllocated -= self->size;
  }
  self->size = 0;
  self->bytes = NULL;
  return self;
}
R_Data* R_FUNCTION_ATTRIBUTES R_Data_New(size_t size) {
  R_Data* self = R_Type_New(R_Data);
  if (self == NULL) return NULL;
  self->is_owner = true;
  self->bytes = os_zalloc(size);
  R_Type_BytesAllocated += size;
  if (self->bytes == NULL) return R_Data_Destructor(self), NULL;
  self->size = size;
  return self;
}
R_Data* R_FUNCTION_ATTRIBUTES R_Data_set(R_Data* self, uint8_t* bytes, size_t size) {
  if (R_Type_IsNotOf(self, R_Data)) return NULL;
  R_Data_Destructor(self);
  self->is_owner = true;
  self->bytes = os_zalloc(size);
  R_Type_BytesAllocated += size;
  if (self->bytes == NULL) return R_Data_Destructor(self), NULL;
  os_memcpy(self->bytes, bytes, size);
  self->size = size;
  return self;
}
uint8_t* R_FUNCTION_ATTRIBUTES R_Data_bytes(R_Data* self) {
  if (R_Type_IsNotOf(self, R_Data)) return NULL;
  return self->bytes;
}
size_t R_FUNCTION_ATTRIBUTES R_Data_size(R_Data* self) {
  if (R_Type_IsNotOf(self, R_Data)) return 0;
  return self->size;
}


/* R_String */
R_String* R_FUNCTION_ATTRIBUTES R_String_Destructor(R_String* self);
R_Type_Define(R_String, .dtor = (R_Type_Destructor)R_String_Destructor);
R_String* R_FUNCTION_ATTRIBUTES R_String_Destructor(R_String* self) {
  if (self->is_owner && self->string) {
    size_t size = strlen(self->string);
    R_Type_BytesAllocated -= size+1;
    os_free(self->string);
  }
  self->string = NULL;
  return self;
}
R_String* R_FUNCTION_ATTRIBUTES R_String_New(char* string) {
  R_String* self = R_Type_New(R_String);
  if (self == NULL) return NULL;
  size_t size = strlen(string);
  self->is_owner = true;
  self->string = os_zalloc(size+1);
  R_Type_BytesAllocated += size+1;
  os_strcpy(self->string, string);
  return self;
}
R_String* R_FUNCTION_ATTRIBUTES R_String_set(R_String* self, char* string) {
  if (R_Type_IsNotOf(self, R_String)) return NULL;
  R_String_Destructor(self);
  size_t size = strlen(string);
  self->is_owner = true;
  self->string = os_zalloc(size+1);
  R_Type_BytesAllocated += size+1;
  os_strcpy(self->string, string);
  return self;
}
char* R_FUNCTION_ATTRIBUTES R_String_get(R_String* self) {
  if (R_Type_IsNotOf(self, R_String)) return NULL;
  return self->string;
}


