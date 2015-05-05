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
#include "R_ByteArray.h"

void R_FUNCTION_ATTRIBUTES R_ByteArray_puts(R_ByteArray* self) {
  if (R_Type_IsNotOf(self, R_ByteArray)) return;
  os_printf("{");
  for (int i=0; i<R_ByteArray_size(self); i++) {
    os_printf("0x%02X", R_ByteArray_byte(self, i));
    if (i < R_ByteArray_size(self)-1) os_printf(", ");
  }
  os_printf("}\n");
}

uint8_t R_FUNCTION_ATTRIBUTES R_ByteArray_first(const R_ByteArray* self) {
  return R_ByteArray_byte(self, 0);
}

uint8_t R_FUNCTION_ATTRIBUTES R_ByteArray_last(const R_ByteArray* self) {
  return R_ByteArray_byte(self, R_ByteArray_size(self) - 1);
}

bool R_FUNCTION_ATTRIBUTES R_ByteArray_isSame(R_ByteArray* self, R_ByteArray* comparor) {
  if (R_ByteArray_compare(self, comparor) != 0) return false;
  return true;
}

static uint8_t R_FUNCTION_ATTRIBUTES hex_to_byte(char hex1, char hex2);
static uint8_t R_FUNCTION_ATTRIBUTES hex_to_nibble(char hex);
R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_appendHexCString(R_ByteArray* self, const char* hex) {
  if (R_Type_IsNotOf(self, R_ByteArray) || hex == NULL) return NULL;
  size_t length = os_strlen(hex);
  int odd_offset = length%2;
  if (odd_offset) R_ByteArray_appendByte(self, hex_to_byte('0', hex[0]));
  for (int i=odd_offset; i<length; i+=2) {
    R_ByteArray_appendByte(self, hex_to_byte(hex[i], hex[i+1]));
  }
  return self;
}
R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_appendHexString(R_ByteArray* self, R_String* hex) {
  if (R_Type_IsNotOf(self, R_ByteArray) || R_Type_IsNotOf(hex, R_String)) return NULL;
  int odd_offset = R_String_length(hex)%2;
  if (odd_offset) R_ByteArray_appendByte(self, hex_to_byte('0', R_String_getString(hex)[0]));
  for (int i=odd_offset; i<R_String_length(hex); i+=2) {
    R_ByteArray_appendByte(self, hex_to_byte(R_String_getString(hex)[i], R_String_getString(hex)[i+1]));
  }
  return self;
}
static uint8_t R_FUNCTION_ATTRIBUTES hex_to_byte(char hex1, char hex2) {
  return (hex_to_nibble(hex1) << 4) + hex_to_nibble(hex2);
}
static uint8_t R_FUNCTION_ATTRIBUTES hex_to_nibble(char hex) {
  if (hex >= '0' && hex <= '9')
    return 0x0 + (uint8_t)hex - (uint8_t)'0';
  else if (hex >= 'a' && hex <= 'f')
    return 0xa + (uint8_t)hex - (uint8_t)'a';
  else if (hex >= 'A' && hex <= 'F')
    return 0xA + (uint8_t)hex - (uint8_t)'A';
  else return 0x0;
}

R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_setByte(R_ByteArray* self, uint8_t byte) {
  R_ByteArray_reset(self);
  return R_ByteArray_appendByte(self, byte);
}
R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_setCArray(R_ByteArray* self, const uint8_t* bytes, size_t count) {
  R_ByteArray_reset(self);
  return R_ByteArray_appendCArray(self, bytes, count);
}
R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_setArray(R_ByteArray* self, const R_ByteArray* array) {
  R_ByteArray_reset(self);
  return R_ByteArray_appendArray(self, array);
}
R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_setHexString(R_ByteArray* self, R_String* hex) {
  R_ByteArray_reset(self);
  return R_ByteArray_appendHexString(self, hex);
}
R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_setHexCString(R_ByteArray* self, const char* hex) {
  R_ByteArray_reset(self);
  return R_ByteArray_appendHexCString(self, hex);
}
R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_setUInt32(R_ByteArray* self, uint32_t value) {
  R_ByteArray_reset(self);
  return R_ByteArray_appendUInt32(self, value);
}

R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_appendUInt32(R_ByteArray* self, uint32_t value) {
  if ((value & 0xFF000000) != 0) R_ByteArray_appendByte(self, (value & 0xFF000000) >> 24);
  if ((value & 0xFFFF0000) != 0) R_ByteArray_appendByte(self, (value & 0x00FF0000) >> 16);
  if ((value & 0xFFFFFF00) != 0) R_ByteArray_appendByte(self, (value & 0x0000FF00) >>  8);
  if ((value & 0xFFFFFFFF) != 0) R_ByteArray_appendByte(self, (value & 0x000000FF) >>  0);
  return self;
}

uint32_t R_FUNCTION_ATTRIBUTES R_ByteArray_getUInt32(R_ByteArray* self) {
  uint32_t integer = (uint32_t)R_ByteArray_byte(self, 0);
  if (R_ByteArray_size(self) > 1) integer = R_ByteArray_byte(self, 1) + (integer << 8);
  if (R_ByteArray_size(self) > 2) integer = R_ByteArray_byte(self, 2) + (integer << 8);
  if (R_ByteArray_size(self) > 3) integer = R_ByteArray_byte(self, 3) + (integer << 8);
  return integer;
}

R_ByteArray* R_FUNCTION_ATTRIBUTES R_ByteArray_appendUInt32AsBCD(R_ByteArray* self, uint32_t value) {
  //Construct the array first in reverse, then append it to self
  R_ByteArray* reversed = R_Type_New(R_ByteArray);
  if (self == NULL || reversed == NULL) return NULL;
  do {
    uint8_t right_digit = value % 10;
    value /= 10;
    uint8_t left_digit = value % 10;
    value /= 10;
    R_ByteArray_appendByte(reversed, (left_digit << 4) | right_digit);
  } while (value > 0);

  for (int i=(int)R_ByteArray_size(reversed)-1; i>=0; i--) {
    R_ByteArray_appendByte(self, R_ByteArray_byte(reversed, i));
  }

  R_Type_Delete(reversed);
  return self;
}
