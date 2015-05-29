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
#include "R_String.h"

static void R_FUNCTION_ATTRIBUTES R_Dictionary_toJson_writeValue(R_String* buffer, void* value);
R_String* R_FUNCTION_ATTRIBUTES R_Dictionary_toJson(R_Dictionary* self, R_String* buffer) {
  if (R_Type_IsNotOf(self, R_Dictionary) || buffer == NULL || R_String_reset(buffer) == NULL) return NULL;
  R_String_appendCString(buffer, "{");

  R_List* elements = R_Dictionary_listOfPairs(self);
  for (int i=0; i<R_List_size(elements); i++) {
    R_KeyValuePair* element = R_List_pointerAtIndex(elements, i);
    R_String_appendStringAsJson(buffer, R_KeyValuePair_key(element));
    R_String_appendCString(buffer, ":");
    R_Dictionary_toJson_writeValue(buffer, R_KeyValuePair_value(element));
    if (i < R_List_size(elements) - 1) {
      R_String_appendCString(buffer, ",");
    }
  }

  R_String_appendCString(buffer, "}");
  return buffer;
}

static void R_FUNCTION_ATTRIBUTES R_Dictionary_toJson_writeValue(R_String* buffer, void* value) {
  if (R_Type_IsOf(value, R_String)) R_String_appendStringAsJson(buffer, value);
  else if (R_Type_IsOf(value, R_Integer)) R_String_appendInt(buffer, R_Integer_get(value));
  else if (R_Type_IsOf(value, R_Float)) R_String_appendFloat(buffer, R_Float_get(value));
  else if (R_Type_IsOf(value, R_Boolean)) {
    if (R_Boolean_get(value)) R_String_appendCString(buffer, "true");
    else R_String_appendCString(buffer, "false");
  }
  else if (R_Type_IsOf(value, R_Dictionary)) {
    R_String* sub_buffer = R_Type_New(R_String);
    R_String_appendString(buffer, R_Dictionary_toJson(value, sub_buffer));
    R_Type_Delete(sub_buffer);
  }
  else if (R_Type_IsOf(value, R_List)) {
    R_String_appendCString(buffer, "[");
    R_List_each(value, void, element) {
      R_Dictionary_toJson_writeValue(buffer, element);
      if (element != R_List_last(value)) R_String_appendCString(buffer, ",");
    }
    R_String_appendCString(buffer, "]");
  }
  else if (R_Type_IsOf(value, R_Null)) R_String_appendCString(buffer, "null");
  else R_String_appendCString(buffer, "\"Unknown Type\"");
}

static R_String* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_moveQuotedString(R_String* source, R_String* dest);
static void* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_readNumber(R_String* source);
static void* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_readValue(R_String* string);
static R_Dictionary* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_readObject(R_Dictionary* object, R_String* string);
static R_String* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_advanceToNextNonWhitespace(R_String* string);
static R_List* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_readArray(R_String* string);
R_Dictionary* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson(R_Dictionary* self, R_String* buffer) {
  if (self == NULL || buffer == NULL) return NULL;
  R_Dictionary_removeAll(self);
  R_String* string = R_Type_Copy(buffer);
  if (string == NULL) return NULL;
  R_String_trim(string);

  R_Dictionary_fromJson_readObject(self, string);

  R_Type_Delete(string);
  return self;
}

static R_String* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_moveQuotedString(R_String* source, R_String* dest) {
  if (source == NULL || dest == NULL) return NULL;
  if (R_String_first(source) != '"') return NULL;
  R_String_shift(source);
  while (R_String_length(source) > 0) {
    char character = R_String_shift(source);
    if (character == '\\' && R_String_length(source) > 0) {
      char escaped = R_String_shift(source);
      if (escaped == '\\') R_String_appendCString(dest, "\\");
      else if (escaped == '/') R_String_appendCString(dest, "/");
      else if (escaped == 'b') R_String_appendCString(dest, "\b");
      else if (escaped == 'f') R_String_appendCString(dest, "\f");
      else if (escaped == 'n') R_String_appendCString(dest, "\n");
      else if (escaped == 'r') R_String_appendCString(dest, "\r");
      else if (escaped == 't') R_String_appendCString(dest, "\t");
    }
    else if (character == '"') {
      R_String_trim(source);
      return dest;
    }
    else R_String_push(dest, character);
  }
  return NULL;
}

static R_Dictionary* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_readObject(R_Dictionary* object, R_String* string) {
  if (R_String_first(string) != '{') return NULL;
  R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
  while (R_String_length(string) > 0) {
    if (R_String_first(string) == '}') break;
    //find the key as a quoted string
    R_String* key = R_Type_New(R_String);
    if (R_Dictionary_fromJson_moveQuotedString(string, key) == NULL) return R_Type_Delete(key), NULL;
    //ignore the separator
    if (R_String_first(string) != ':') return R_Type_Delete(key), NULL;
    R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
    //read value
    void* value = R_Dictionary_fromJson_readValue(string);
    //add kay/value to dictionary
    if (value == NULL || R_Dictionary_transferOwnership(object, R_String_cstring(key), value) == NULL) return R_Type_Delete(key), NULL;
    //cleanup
    R_Type_Delete(key);
    //decide whether there are more objects to read
    R_String_trim(string);
    if (R_String_first(string) == ',') {
      R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
      continue;
    }
    else if (R_String_first(string) == '}') break;
    else return NULL;
  }

  if (R_String_first(string) != '}') return NULL;
  R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
  return object;
}

static R_String* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_advanceToNextNonWhitespace(R_String* string) {
  R_String_shift(string);
  R_String_trim(string);
  return string;
}

static void* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_readValue(R_String* string) {
  if (R_String_first(string) == '"') {//value is a string
    R_String* value = R_Type_New(R_String);
    if (R_Dictionary_fromJson_moveQuotedString(string, value) == NULL) return R_Type_Delete(value), NULL;
    return value;
  }
  else if ((R_String_first(string) >= '0' && R_String_first(string) <= '9') || R_String_first(string) == '-') {//value is a number
    return R_Dictionary_fromJson_readNumber(string);
  }
  else if (R_String_first(string) == 't' || R_String_first(string) == 'f') {//value is a boolean (true)
    const char* characters = R_String_cstring(string);
    if ((char*)os_strstr(characters, "true") == characters) {
      R_String_getSubstring(string, string, 4, 0);
      return R_Boolean_set(R_Type_New(R_Boolean), true);
    }
    else if ((char*)os_strstr(characters, "false") == characters) {
      R_String_getSubstring(string, string, 5, 0);
      return R_Boolean_set(R_Type_New(R_Boolean), false);
    }
  }
  else if (R_String_first(string) == 'n') {//value is a null
    const char* characters = R_String_cstring(string);
    if ((char*)os_strstr(characters, "null") == characters) {
      R_String_getSubstring(string, string, 4, 0);
      return R_Type_New(R_Null);
    }
  }
  else if (R_String_first(string) == '{') {
    R_Dictionary* child = R_Type_New(R_Dictionary);
    if (R_Dictionary_fromJson_readObject(child, string) == NULL) {
      R_Type_Delete(child);
    }
    else return child;
  }
  else if (R_String_first(string) == '[') {
    return R_Dictionary_fromJson_readArray(string);
  }
  return NULL;
}

static R_List* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_readArray(R_String* string) {
  if (R_String_first(string) != '[') return NULL;
  R_List* array = R_Type_New(R_List);
  R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
  while (R_String_length(string) > 0) {
    if (R_String_first(string) == ']') {
      R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
      return array;
    }
    void* value = R_Dictionary_fromJson_readValue(string);
    if (value == NULL || R_List_transferOwnership(array, value) == NULL) return R_Type_Delete(array), NULL;
    R_String_trim(string);
    if (R_String_first(string) == ',') {
      R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
      continue;
    }
    else if (R_String_first(string) == ']') {
      R_Dictionary_fromJson_advanceToNextNonWhitespace(string);
      return array;
    }
    else return R_Type_Delete(array), NULL;
  }
  return R_Type_Delete(array), NULL;
}

static void* R_FUNCTION_ATTRIBUTES R_Dictionary_fromJson_readNumber(R_String* source) {
  if (source == NULL) return NULL;
  R_String* value = R_Type_New(R_String);
  bool isFloat = false;
  bool isExponent = false;
  while (R_String_length(source) > 0) {
    if (R_String_first(source) >= '0' && R_String_first(source) <= '9') {
      R_String_push(value, R_String_shift(source));
      continue;
    }
    else if (R_String_first(source) == '.') {
      R_String_push(value, R_String_shift(source));
      isFloat = true;
      continue;
    }
    else if (R_String_first(source) == 'e' || R_String_first(source) == 'E') {
      R_String_push(value, R_String_shift(source));
      isExponent = true;
      isFloat = true;
      continue;
    }
    else if (isExponent && (R_String_first(source) == '+' || R_String_first(source) == '-')) {
      R_String_push(value, R_String_shift(source));
      continue;
    }
    else if (R_String_length(value) == 0 && R_String_first(source) == '-') {
      R_String_push(value, R_String_shift(source));
      continue;
    }
    else break;
  }

  if (isFloat) {
    float floater = R_String_getFloat(value);
    R_Type_Delete(value);
    R_Float* number = R_Type_New(R_Float);
    R_Float_set(number, floater);
    return number;
  }
  else { //is integer
    int integer = R_String_getInt(value);
    R_Type_Delete(value);
    R_Integer* number = R_Type_New(R_Integer);
    R_Integer_set(number, integer);
    return number;
  }
  return R_Type_Delete(value), NULL;
}
