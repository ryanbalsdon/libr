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
#include "R_String.h"


void R_FUNCTION_ATTRIBUTES R_String_puts(R_String* self) {
  os_printf("%.*s\n", (int)R_String_length(self), R_ByteArray_bytes(R_String_bytes(self)));
}

R_String* R_FUNCTION_ATTRIBUTES R_String_appendCString(R_String* self, const char* string) {
  return R_String_appendBytes(self, string, os_strlen(string));
}

R_String* R_FUNCTION_ATTRIBUTES R_String_setString(R_String* self, const char* string) {
  R_String_reset(self);
  return R_String_appendCString(self, string);
}

R_String* R_FUNCTION_ATTRIBUTES R_String_setSizedString(R_String* self, const char* string, size_t stringLength) {
  if (string == NULL) return NULL;
  R_String_reset(self);
  size_t length = os_strlen(string);
  length = (length > stringLength) ? stringLength : length;
  return R_String_appendBytes(self, string, length);
}

R_String* R_FUNCTION_ATTRIBUTES R_String_appendInt(R_String* self, int value) {
  char characters[os_snprintf(NULL, 0, "%d", value)];
  os_sprintf(characters, "%d", value);
  return R_String_appendCString(self, characters);
}

R_String* R_FUNCTION_ATTRIBUTES R_String_appendFloat(R_String* self, float value) {
  char characters[os_snprintf(NULL, 0, "%g", value)];
  os_sprintf(characters, "%g", value);
  return R_String_appendCString(self, characters);
}

int R_FUNCTION_ATTRIBUTES R_String_getInt(R_String* self) {
  if (R_Type_IsNotOf(self, R_String)) return 0;
  int output = 0;
  if (os_sscanf(R_String_getString(self), "%d", &output) == 1) return output;
  return 0;
}
float R_FUNCTION_ATTRIBUTES R_String_getFloat(R_String* self) {
  if (R_Type_IsNotOf(self, R_String)) return 0.0f;
  float output = 0.0f;
  if (os_sscanf(R_String_getString(self), "%g", &output) == 1) return output;
  return 0.0f;
}

bool R_FUNCTION_ATTRIBUTES R_String_isEmpty(R_String* self) {
  if (R_String_length(self) == 0) return true;
  return false;
}

R_String* R_FUNCTION_ATTRIBUTES R_String_appendStringAsJson(R_String* self, R_String* string) {
  R_String_appendCString(self, "\"");

  for (int i=0; i<R_String_length(string); i++) {
    char character = R_String_getString(string)[i];
    if (character == '"')  {R_String_appendCString(self, "\\\""); continue;}
    if (character == '\\') {R_String_appendCString(self, "\\\\"); continue;}
    if (character == '/')  {R_String_appendCString(self, "\\/");  continue;}
    if (character == '\b') {R_String_appendCString(self, "\\b");  continue;}
    if (character == '\f') {R_String_appendCString(self, "\\f");  continue;}
    if (character == '\n') {R_String_appendCString(self, "\\n");  continue;}
    if (character == '\r') {R_String_appendCString(self, "\\r");  continue;}
    if (character == '\t') {R_String_appendCString(self, "\\t");  continue;}
    if (character < 0x1f) continue;
    R_String_appendBytes(self, &character, 1);
  }

  R_String_appendCString(self, "\"");
  return self;
}

int R_FUNCTION_ATTRIBUTES R_String_find(R_String* self, const char* substring) {
  if (R_Type_IsNotOf(self, R_String) || substring == NULL) return -1;
  const char* string = R_String_cstring(self);
  char* result = os_strstr(string, substring);
  if (result == NULL) return -1;
  return (int)(result - string);
}

static bool R_FUNCTION_ATTRIBUTES R_String_trim_isWhiteSpace(char character);
R_String* R_String_trim(R_String* self) {
  while(R_String_length(self) > 0 && R_String_trim_isWhiteSpace(R_String_first(self))) R_String_shift(self);
  while(R_String_length(self) > 0 && R_String_trim_isWhiteSpace(R_String_last(self))) R_String_pop(self);
  return self;
}

static bool R_FUNCTION_ATTRIBUTES R_String_trim_isWhiteSpace(char character) {
  switch (character) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      return true;
  }
  return false;
}

R_List* R_FUNCTION_ATTRIBUTES R_String_split(R_String* self, const char* seperator, R_List* output) {
  if (R_Type_IsNotOf(self, R_String) || R_Type_IsNotOf(output, R_List)) return NULL;
  if (R_String_length(self) == 0) return NULL;
  if (seperator == NULL) seperator = "\n";

  R_String* copy = R_Type_Copy(self);

  while (R_String_length(copy) > 0) {
    R_String* this_splice = R_List_add(output, R_String);
    int splice_length = R_String_find(copy, seperator);
    if (splice_length < 0) {
      R_String_appendString(this_splice, copy);
      R_Type_Delete(copy);
      return output;
    }
    if (splice_length > 0) R_String_moveSubstring(copy, this_splice, 0, splice_length);
    for (int i=0; i<os_strlen(seperator); i++) R_String_shift(copy);
  }

  R_Type_Delete(copy);
  return output;
}

R_String* R_FUNCTION_ATTRIBUTES R_String_join(R_String* self, const char* seperator, R_List* input) {
  if (R_Type_IsNotOf(self, R_String) || R_Type_IsNotOf(input, R_List)) return NULL;
  if (seperator == NULL) seperator = "";
  R_List_each(input, R_String, string) {
    if (R_Type_IsNotOf(string, R_String)) continue;
    R_String_appendString(self, string);
    if (string != R_List_last(input)) R_String_appendCString(self, seperator);
  }
  return self;
}

static uint8_t R_FUNCTION_ATTRIBUTES R_String_appendArrayAsBase64_base64FromIndex(uint8_t index);
R_String* R_FUNCTION_ATTRIBUTES R_String_appendArrayAsBase64(R_String* self, const R_ByteArray* array) {
  if (R_Type_IsNotOf(self, R_String) || R_Type_IsNotOf(array, R_ByteArray)) return NULL;
  for(int i=0; i<R_ByteArray_size(array); i+=3) {
    uint8_t byte1 = ((R_ByteArray_byte(array,   i) & 0xFC) >> 2);
    uint8_t byte2 = ((R_ByteArray_byte(array,   i) & 0x03) << 4) + ((R_ByteArray_byte(array, i+1) & 0xF0) >> 4);
    uint8_t byte3 = ((R_ByteArray_byte(array, i+1) & 0x0F) << 2) + ((R_ByteArray_byte(array, i+2) & 0xC0) >> 6);
    uint8_t byte4 = (R_ByteArray_byte(array, i+2) & 0x3F);
    if (R_String_push(self, R_String_appendArrayAsBase64_base64FromIndex(byte1)) == NULL) return NULL;
    if (R_String_push(self, R_String_appendArrayAsBase64_base64FromIndex(byte2)) == NULL) return NULL;
    if (R_ByteArray_length(array) > i+1) {
      if (R_String_push(self, R_String_appendArrayAsBase64_base64FromIndex(byte3)) == NULL) return NULL;
    }
    else {
      if (R_String_push(self, '=') == NULL) return NULL;
    }
    if (R_ByteArray_length(array) > i+2) {
      if (R_String_push(self, R_String_appendArrayAsBase64_base64FromIndex(byte4)) == NULL) return NULL;
    }
    else {
      if (R_String_push(self, '=') == NULL) return NULL;
    }
  }
  return self;
}

static uint8_t R_FUNCTION_ATTRIBUTES R_String_appendArrayAsBase64_base64FromIndex(uint8_t index) {
  if (index <= 25) return 'A'+index;
  if (index <= 51) return 'a'+(index-26);
  if (index <= 61) return '0'+(index-52);
  if (index == 62) return '+';
  if (index == 63) return '/';
  return '=';
}
