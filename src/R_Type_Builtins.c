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
R_Integer* R_FUNCTION_ATTRIBUTES R_Integer_set(R_Integer* self, int value) {
  if (self == NULL) return NULL;
  self->value = value; 
  return self;
}
int R_FUNCTION_ATTRIBUTES R_Integer_get(R_Integer* self) {
  return self->value;
}
static size_t R_FUNCTION_ATTRIBUTES R_Integer_stringify(R_Integer* self, char* buffer, size_t size) {
  if (R_Type_IsNotOf(self, R_Integer)) return 0;
  int bytes_written = os_snprintf(buffer, size, "%d", self->value);
  return bytes_written < size ? bytes_written : size;
}
static R_JumpTable_Entry R_Integer_methods[] = {
  R_JumpTable_Entry_Make(R_Stringify, R_Integer_stringify), 
  R_JumpTable_Entry_NULL
};
R_Type_Define(R_Integer, .copy = R_Type_shallowCopy, .interfaces = R_Integer_methods);



/* R_Float */
R_Float* R_FUNCTION_ATTRIBUTES R_Float_set(R_Float* self, float value) {
  if (self == NULL) return NULL;
  self->value = value; 
  return self;
}
float R_FUNCTION_ATTRIBUTES R_Float_get(R_Float* self) {
  return self->value;
}
static size_t R_FUNCTION_ATTRIBUTES R_Float_stringify(R_Float* self, char* buffer, size_t size) {
  if (R_Type_IsNotOf(self, R_Float)) return 0;
  int bytes_written = os_snprintf(buffer, size, "%g", self->value);
  return bytes_written < size ? bytes_written : size;
}
static R_JumpTable_Entry R_Float_methods[] = {
  R_JumpTable_Entry_Make(R_Stringify, R_Float_stringify), 
  R_JumpTable_Entry_NULL
};
R_Type_Define(R_Float, .copy = R_Type_shallowCopy, .interfaces = R_Float_methods);



/* R_Unsigned */
R_Unsigned* R_FUNCTION_ATTRIBUTES R_Unsigned_set(R_Unsigned* self, unsigned int value) {
  if (self == NULL) return NULL;
  self->value = value; 
  return self;
}
unsigned int R_FUNCTION_ATTRIBUTES R_Unsigned_get(R_Unsigned* self) {
  return self->value;
}
static size_t R_FUNCTION_ATTRIBUTES R_Unsigned_stringify(R_Unsigned* self, char* buffer, size_t size) {
  if (R_Type_IsNotOf(self, R_Unsigned)) return 0;
  int bytes_written = os_snprintf(buffer, size, "%u", self->value);
  return bytes_written < size ? bytes_written : size;
}
static R_JumpTable_Entry R_Unsigned_methods[] = {
  R_JumpTable_Entry_Make(R_Stringify, R_Unsigned_stringify),
  R_JumpTable_Entry_NULL
};
R_Type_Define(R_Unsigned, .copy = R_Type_shallowCopy, .interfaces = R_Unsigned_methods);



/* R_Boolean */
R_Boolean* R_FUNCTION_ATTRIBUTES R_Boolean_set(R_Boolean* self, bool value) {
  if (self == NULL || (value != 0 && value != 1)) return NULL;
  self->value = value; 
  return self;
}
bool R_FUNCTION_ATTRIBUTES R_Boolean_get(R_Boolean* self) {
  return self->value;
}
static size_t R_FUNCTION_ATTRIBUTES R_Boolean_stringify(R_Boolean* self, char* buffer, size_t size) {
  if (R_Type_IsNotOf(self, R_Boolean)) return 0;
  int bytes_written = os_snprintf(buffer, size, "%s", self->value ? "true" : "false");
  return bytes_written < size ? bytes_written : size;
}
static R_JumpTable_Entry R_Boolean_methods[] = {
  R_JumpTable_Entry_Make(R_Stringify, R_Boolean_stringify),
  R_JumpTable_Entry_NULL
};
R_Type_Define(R_Boolean, .copy = R_Type_shallowCopy, .interfaces = R_Boolean_methods);



/* R_Null */
R_Type_Define(R_Null, .copy = R_Type_shallowCopy);


/* R_Data */
R_Data* R_FUNCTION_ATTRIBUTES R_Data_Destructor(R_Data* self) {
  if (self->bytes) {
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
  self->bytes = os_zalloc(size);
  R_Type_BytesAllocated += size;
  if (self->bytes == NULL) return R_Data_Destructor(self), NULL;
  self->size = size;
  return self;
}
R_Data* R_FUNCTION_ATTRIBUTES R_Data_set(R_Data* self, uint8_t* bytes, size_t size) {
  if (R_Type_IsNotOf(self, R_Data)) return NULL;
  R_Data_Destructor(self);
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
static R_Data* R_Data_Copier(const R_Data* object_input, R_Data* object_output) {
  return R_Data_set(object_output, object_input->bytes, object_input->size);
}
size_t R_FUNCTION_ATTRIBUTES R_Data_stringify(R_Data* self, char* buffer, size_t size) {
  if (R_Type_IsNotOf(self, R_Data)) return 0;
  char* buffer_head = buffer;
  for (int i=0; i<self->size; i++) {
    size_t this_size = 0;
    if (i==0) this_size = os_snprintf(buffer, size, "{0x%02X, ", self->bytes[i]);
    else if (i==self->size-1) this_size = os_snprintf(buffer, size, "0x%02X}", self->bytes[i]);
    else this_size = os_snprintf(buffer, size, "0x%02X, ", self->bytes[i]);
    if (this_size >= size) return buffer - buffer_head;
    size -= this_size;
    buffer += this_size;
  }
  return buffer - buffer_head;
}
static R_JumpTable_Entry R_Data_methods[] = {
  R_JumpTable_Entry_Make(R_Stringify, R_Data_stringify),
  R_JumpTable_Entry_NULL
};
R_Type_Define(R_Data, .dtor = (R_Type_Destructor)R_Data_Destructor, .copy = (R_Type_Copier)R_Data_Copier, .interfaces = R_Data_methods);


/* R_String */
static R_String* R_FUNCTION_ATTRIBUTES R_String_Destructor(R_String* self) {
  if (self->string) {
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
  self->string = os_zalloc(size+1);
  R_Type_BytesAllocated += size+1;
  os_strcpy(self->string, string);
  return self;
}
R_String* R_FUNCTION_ATTRIBUTES R_String_set(R_String* self, char* string) {
  if (R_Type_IsNotOf(self, R_String) || string == NULL) return NULL;
  R_String_Destructor(self);
  size_t size = strlen(string);
  self->string = os_zalloc(size+1);
  R_Type_BytesAllocated += size+1;
  os_strcpy(self->string, string);
  return self;
}
char* R_FUNCTION_ATTRIBUTES R_String_get(R_String* self) {
  if (R_Type_IsNotOf(self, R_String)) return NULL;
  return self->string;
}
static R_String* R_String_Copier(const R_String* object_input, R_String* object_output) {
  return R_String_set(object_output, object_input->string);
}
static size_t R_FUNCTION_ATTRIBUTES R_String_stringify(R_String* self, char* buffer, size_t size) {
  if (R_Type_IsNotOf(self, R_String)) return 0;
  int bytes_written = os_snprintf(buffer, size, "%s", self->string);
  return bytes_written < size ? bytes_written : size;
}
static R_JumpTable_Entry R_String_methods[] = {
  R_JumpTable_Entry_Make(R_Stringify, R_String_stringify),
  R_JumpTable_Entry_NULL
};
R_Type_Define(R_String, .dtor = (R_Type_Destructor)R_String_Destructor, .copy = (R_Type_Copier)R_String_Copier, .interfaces = R_String_methods);


