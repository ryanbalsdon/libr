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
#include "R_Data.h"

void R_FUNCTION_ATTRIBUTES R_Data_puts(R_Data* self) {
  if (R_Type_IsNotOf(self, R_Data)) return;
  os_printf("{");
  for (int i=0; i<R_Data_size(self); i++) {
    os_printf("0x%02X", R_Data_byte(self, i));
    if (i < R_Data_size(self)-1) os_printf(", ");
  }
  os_printf("}\n");
}

uint8_t R_FUNCTION_ATTRIBUTES R_Data_first(const R_Data* self) {
  return R_Data_byte(self, 0);
}

uint8_t R_FUNCTION_ATTRIBUTES R_Data_last(const R_Data* self) {
  return R_Data_byte(self, R_Data_size(self) - 1);
}

bool R_FUNCTION_ATTRIBUTES R_Data_isSame(R_Data* self, R_Data* comparor) {
  if (R_Data_compare(self, comparor) != 0) return false;
  return true;
}

static uint8_t R_FUNCTION_ATTRIBUTES hex_to_byte(char hex1, char hex2);
static uint8_t R_FUNCTION_ATTRIBUTES hex_to_nibble(char hex);
R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendHexCString(R_Data* self, const char* hex) {
  if (R_Type_IsNotOf(self, R_Data) || hex == NULL) return NULL;
  size_t length = os_strlen(hex);
  int odd_offset = length%2;
  if (odd_offset) R_Data_appendByte(self, hex_to_byte('0', hex[0]));
  for (int i=odd_offset; i<length; i+=2) {
    R_Data_appendByte(self, hex_to_byte(hex[i], hex[i+1]));
  }
  return self;
}
R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendHexString(R_Data* self, R_MutableString* hex) {
  if (R_Type_IsNotOf(self, R_Data) || R_Type_IsNotOf(hex, R_MutableString)) return NULL;
  int odd_offset = R_MutableString_length(hex)%2;
  if (odd_offset) R_Data_appendByte(self, hex_to_byte('0', R_MutableString_getString(hex)[0]));
  for (int i=odd_offset; i<R_MutableString_length(hex); i+=2) {
    R_Data_appendByte(self, hex_to_byte(R_MutableString_getString(hex)[i], R_MutableString_getString(hex)[i+1]));
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

R_Data* R_FUNCTION_ATTRIBUTES R_Data_setByte(R_Data* self, uint8_t byte) {
  R_Data_reset(self);
  return R_Data_appendByte(self, byte);
}
R_Data* R_FUNCTION_ATTRIBUTES R_Data_setCArray(R_Data* self, const uint8_t* bytes, size_t count) {
  R_Data_reset(self);
  return R_Data_appendCArray(self, bytes, count);
}
R_Data* R_FUNCTION_ATTRIBUTES R_Data_setArray(R_Data* self, const R_Data* array) {
  R_Data_reset(self);
  return R_Data_appendArray(self, array);
}
R_Data* R_FUNCTION_ATTRIBUTES R_Data_setHexString(R_Data* self, R_MutableString* hex) {
  R_Data_reset(self);
  return R_Data_appendHexString(self, hex);
}
R_Data* R_FUNCTION_ATTRIBUTES R_Data_setHexCString(R_Data* self, const char* hex) {
  R_Data_reset(self);
  return R_Data_appendHexCString(self, hex);
}
R_Data* R_FUNCTION_ATTRIBUTES R_Data_setUInt32(R_Data* self, uint32_t value) {
  R_Data_reset(self);
  return R_Data_appendUInt32(self, value);
}

R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendUInt32(R_Data* self, uint32_t value) {
  if ((value & 0xFF000000) != 0) R_Data_appendByte(self, (value & 0xFF000000) >> 24);
  if ((value & 0xFFFF0000) != 0) R_Data_appendByte(self, (value & 0x00FF0000) >> 16);
  if ((value & 0xFFFFFF00) != 0) R_Data_appendByte(self, (value & 0x0000FF00) >>  8);
  if ((value & 0xFFFFFFFF) != 0) R_Data_appendByte(self, (value & 0x000000FF) >>  0);
  return self;
}

uint32_t R_FUNCTION_ATTRIBUTES R_Data_getUInt32(R_Data* self) {
  uint32_t integer = (uint32_t)R_Data_byte(self, 0);
  if (R_Data_size(self) > 1) integer = R_Data_byte(self, 1) + (integer << 8);
  if (R_Data_size(self) > 2) integer = R_Data_byte(self, 2) + (integer << 8);
  if (R_Data_size(self) > 3) integer = R_Data_byte(self, 3) + (integer << 8);
  return integer;
}

R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendUInt32AsBCD(R_Data* self, uint32_t value) {
  //Construct the array first in reverse, then append it to self
  R_Data* reversed = R_Type_New(R_Data);
  if (self == NULL || reversed == NULL) return NULL;
  do {
    uint8_t right_digit = value % 10;
    value /= 10;
    uint8_t left_digit = value % 10;
    value /= 10;
    R_Data_appendByte(reversed, (left_digit << 4) | right_digit);
  } while (value > 0);

  for (int i=(int)R_Data_size(reversed)-1; i>=0; i--) {
    R_Data_appendByte(self, R_Data_byte(reversed, i));
  }

  R_Type_Delete(reversed);
  return self;
}
