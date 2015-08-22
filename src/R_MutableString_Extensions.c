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
#include "R_MutableString.h"


size_t R_FUNCTION_ATTRIBUTES R_MutableString_stringify(R_MutableString* self, char* buffer, size_t size) {
  if (R_Type_IsNotOf(self, R_MutableString)) return 0;
  return os_snprintf(buffer, size, "%.*s", (int)R_MutableString_length(self), R_MutableData_bytes(R_MutableString_bytes(self)));
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendCString(R_MutableString* self, const char* string) {
  return R_MutableString_appendBytes(self, string, os_strlen(string));
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_setString(R_MutableString* self, const char* string) {
  R_MutableString_reset(self);
  return R_MutableString_appendCString(self, string);
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_setSizedString(R_MutableString* self, const char* string, size_t stringLength) {
  if (string == NULL) return NULL;
  R_MutableString_reset(self);
  size_t length = os_strlen(string);
  length = (length > stringLength) ? stringLength : length;
  return R_MutableString_appendBytes(self, string, length);
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendInt(R_MutableString* self, int value) {
#ifdef ESP8266
  char characters[10]; //because max value of int32 is 213748364
#else
  char characters[os_snprintf(NULL, 0, "%d", value)];
#endif
  os_sprintf(characters, "%d", value);
  return R_MutableString_appendCString(self, characters);
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendFloat(R_MutableString* self, float value) {
#ifdef ESP8266
  char characters[14]; //because most digits I've seen from %g is 4.94066e-324
#else
  char characters[os_snprintf(NULL, 0, "%g", value)];
#endif
  os_sprintf(characters, "%g", value);
  return R_MutableString_appendCString(self, characters);
}

int R_FUNCTION_ATTRIBUTES R_MutableString_getInt(R_MutableString* self) {
  if (R_Type_IsNotOf(self, R_MutableString)) return 0;
  int output = 0;
#ifdef ESP8266
  return os_atoi(R_MutableString_getString(self));
#else
  if (os_sscanf(R_MutableString_getString(self), "%d", &output) == 1) return output;
#endif
  return 0;
}
float R_FUNCTION_ATTRIBUTES R_MutableString_getFloat(R_MutableString* self) {
  if (R_Type_IsNotOf(self, R_MutableString)) return 0.0f;
  float output = 0.0f;
#ifdef ESP8266
  return os_atof(R_MutableString_getString(self));
#endif
  if (os_sscanf(R_MutableString_getString(self), "%g", &output) == 1) return output;
  return 0.0f;
}

bool R_FUNCTION_ATTRIBUTES R_MutableString_isEmpty(R_MutableString* self) {
  if (R_MutableString_length(self) == 0) return true;
  return false;
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendStringAsJson(R_MutableString* self, R_MutableString* string) {
  R_MutableString_appendCString(self, "\"");

  for (int i=0; i<R_MutableString_length(string); i++) {
    char character = R_MutableString_getString(string)[i];
    if (character == '"')  {R_MutableString_appendCString(self, "\\\""); continue;}
    if (character == '\\') {R_MutableString_appendCString(self, "\\\\"); continue;}
    if (character == '/')  {R_MutableString_appendCString(self, "\\/");  continue;}
    if (character == '\b') {R_MutableString_appendCString(self, "\\b");  continue;}
    if (character == '\f') {R_MutableString_appendCString(self, "\\f");  continue;}
    if (character == '\n') {R_MutableString_appendCString(self, "\\n");  continue;}
    if (character == '\r') {R_MutableString_appendCString(self, "\\r");  continue;}
    if (character == '\t') {R_MutableString_appendCString(self, "\\t");  continue;}
    if (character < 0x1f) continue;
    R_MutableString_appendBytes(self, &character, 1);
  }

  R_MutableString_appendCString(self, "\"");
  return self;
}

int R_FUNCTION_ATTRIBUTES R_MutableString_find(R_MutableString* self, const char* substring) {
  if (R_Type_IsNotOf(self, R_MutableString) || substring == NULL) return -1;
  const char* string = R_MutableString_cstring(self);
  char* result = (char*)os_strstr(string, substring);
  if (result == NULL) return -1;
  return (int)(result - string);
}

static bool R_FUNCTION_ATTRIBUTES R_MutableString_trim_isWhiteSpace(char character);
R_MutableString* R_MutableString_trim(R_MutableString* self) {
  while(R_MutableString_length(self) > 0 && R_MutableString_trim_isWhiteSpace(R_MutableString_first(self))) R_MutableString_shift(self);
  while(R_MutableString_length(self) > 0 && R_MutableString_trim_isWhiteSpace(R_MutableString_last(self))) R_MutableString_pop(self);
  return self;
}

static bool R_FUNCTION_ATTRIBUTES R_MutableString_trim_isWhiteSpace(char character) {
  switch (character) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      return true;
  }
  return false;
}

R_List* R_FUNCTION_ATTRIBUTES R_MutableString_split(R_MutableString* self, const char* seperator, R_List* output) {
  if (R_Type_IsNotOf(self, R_MutableString) || R_Type_IsNotOf(output, R_List)) return NULL;
  if (R_MutableString_length(self) == 0) return NULL;
  if (seperator == NULL) seperator = "\n";

  R_MutableString* copy = R_Type_Copy(self);

  while (R_MutableString_length(copy) > 0) {
    R_MutableString* this_splice = R_List_add(output, R_MutableString);
    int splice_length = R_MutableString_find(copy, seperator);
    if (splice_length < 0) {
      R_MutableString_appendString(this_splice, copy);
      R_Type_Delete(copy);
      return output;
    }
    if (splice_length > 0) R_MutableString_moveSubstring(copy, this_splice, 0, splice_length);
    for (int i=0; i<os_strlen(seperator); i++) R_MutableString_shift(copy);
  }

  R_Type_Delete(copy);
  return output;
}

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_join(R_MutableString* self, const char* seperator, R_List* input) {
  if (R_Type_IsNotOf(self, R_MutableString) || R_Type_IsNotOf(input, R_List)) return NULL;
  if (seperator == NULL) seperator = "";
  R_List_each(input, R_MutableString, string) {
    if (R_Type_IsNotOf(string, R_MutableString)) continue;
    R_MutableString_appendString(self, string);
    if (string != R_List_last(input)) R_MutableString_appendCString(self, seperator);
  }
  return self;
}

static uint8_t R_FUNCTION_ATTRIBUTES R_MutableString_appendArrayAsBase64_base64FromIndex(uint8_t index);
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendArrayAsBase64(R_MutableString* self, const R_MutableData* array) {
  if (R_Type_IsNotOf(self, R_MutableString) || R_Type_IsNotOf(array, R_MutableData)) return NULL;
  for(int i=0; i<R_MutableData_size(array); i+=3) {
    uint8_t byte1 = ((R_MutableData_byte(array,   i) & 0xFC) >> 2);
    uint8_t byte2 = ((R_MutableData_byte(array,   i) & 0x03) << 4) + ((R_MutableData_byte(array, i+1) & 0xF0) >> 4);
    uint8_t byte3 = ((R_MutableData_byte(array, i+1) & 0x0F) << 2) + ((R_MutableData_byte(array, i+2) & 0xC0) >> 6);
    uint8_t byte4 = (R_MutableData_byte(array, i+2) & 0x3F);
    if (R_MutableString_push(self, R_MutableString_appendArrayAsBase64_base64FromIndex(byte1)) == NULL) return NULL;
    if (R_MutableString_push(self, R_MutableString_appendArrayAsBase64_base64FromIndex(byte2)) == NULL) return NULL;
    if (R_MutableData_length(array) > i+1) {
      if (R_MutableString_push(self, R_MutableString_appendArrayAsBase64_base64FromIndex(byte3)) == NULL) return NULL;
    }
    else {
      if (R_MutableString_push(self, '=') == NULL) return NULL;
    }
    if (R_MutableData_length(array) > i+2) {
      if (R_MutableString_push(self, R_MutableString_appendArrayAsBase64_base64FromIndex(byte4)) == NULL) return NULL;
    }
    else {
      if (R_MutableString_push(self, '=') == NULL) return NULL;
    }
  }
  return self;
}

static uint8_t R_FUNCTION_ATTRIBUTES R_MutableString_appendArrayAsBase64_base64FromIndex(uint8_t index) {
  if (index <= 25) return 'A'+index;
  if (index <= 51) return 'a'+(index-26);
  if (index <= 61) return '0'+(index-52);
  if (index == 62) return '+';
  if (index == 63) return '/';
  return '=';
}
