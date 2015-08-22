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
#include "R_MutableData.h"

size_t R_FUNCTION_ATTRIBUTES R_MutableData_stringify(R_MutableData* self, char* buffer, size_t size) {
  if (R_Type_IsNotOf(self, R_MutableData)) return 0;
  char* buffer_head = buffer;
  for (int i=0; i<R_MutableData_size(self); i++) {
    size_t this_size = 0;
    if (i==0) this_size = os_snprintf(buffer, size, "{0x%02X, ", R_MutableData_byte(self, i));
    else if (i==R_MutableData_size(self)-1) this_size = os_snprintf(buffer, size, "0x%02X}", R_MutableData_byte(self, i));
    else this_size = os_snprintf(buffer, size, "0x%02X, ", R_MutableData_byte(self, i));
    if (this_size >= size) return buffer - buffer_head;
    size -= this_size;
    buffer += this_size;
  }
  return buffer - buffer_head;
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendArray(R_MutableData* self, const R_MutableData* array) {
  if (R_Type_IsNotOf(self, R_MutableData) || R_Type_IsNotOf(array, R_MutableData)) return NULL;
  return R_MutableData_appendCArray(self, R_MutableData_bytes(array), R_MutableData_size(array));
}

uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_first(const R_MutableData* self) {
  return R_MutableData_byte(self, 0);
}

uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_last(const R_MutableData* self) {
  return R_MutableData_byte(self, R_MutableData_size(self) - 1);
}

bool R_FUNCTION_ATTRIBUTES R_MutableData_isSame(R_MutableData* self, R_MutableData* comparor) {
  if (R_MutableData_compare(self, comparor) != 0) return false;
  return true;
}

static uint8_t R_FUNCTION_ATTRIBUTES hex_to_byte(char hex1, char hex2);
static uint8_t R_FUNCTION_ATTRIBUTES hex_to_nibble(char hex);
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendHexCString(R_MutableData* self, const char* hex) {
  if (R_Type_IsNotOf(self, R_MutableData) || hex == NULL) return NULL;
  size_t length = os_strlen(hex);
  int odd_offset = length%2;
  if (odd_offset) R_MutableData_appendByte(self, hex_to_byte('0', hex[0]));
  for (int i=odd_offset; i<length; i+=2) {
    R_MutableData_appendByte(self, hex_to_byte(hex[i], hex[i+1]));
  }
  return self;
}
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendHexString(R_MutableData* self, R_MutableString* hex) {
  if (R_Type_IsNotOf(self, R_MutableData) || R_Type_IsNotOf(hex, R_MutableString)) return NULL;
  int odd_offset = R_MutableString_length(hex)%2;
  if (odd_offset) R_MutableData_appendByte(self, hex_to_byte('0', R_MutableString_getString(hex)[0]));
  for (int i=odd_offset; i<R_MutableString_length(hex); i+=2) {
    R_MutableData_appendByte(self, hex_to_byte(R_MutableString_getString(hex)[i], R_MutableString_getString(hex)[i+1]));
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

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setByte(R_MutableData* self, uint8_t byte) {
  R_MutableData_reset(self);
  return R_MutableData_appendByte(self, byte);
}
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setCArray(R_MutableData* self, const uint8_t* bytes, size_t count) {
  R_MutableData_reset(self);
  return R_MutableData_appendCArray(self, bytes, count);
}
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setArray(R_MutableData* self, const R_MutableData* array) {
  R_MutableData_reset(self);
  return R_MutableData_appendArray(self, array);
}
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setHexString(R_MutableData* self, R_MutableString* hex) {
  R_MutableData_reset(self);
  return R_MutableData_appendHexString(self, hex);
}
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setHexCString(R_MutableData* self, const char* hex) {
  R_MutableData_reset(self);
  return R_MutableData_appendHexCString(self, hex);
}
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setUInt32(R_MutableData* self, uint32_t value) {
  R_MutableData_reset(self);
  return R_MutableData_appendUInt32(self, value);
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendUInt32(R_MutableData* self, uint32_t value) {
  if ((value & 0xFF000000) != 0) R_MutableData_appendByte(self, (value & 0xFF000000) >> 24);
  if ((value & 0xFFFF0000) != 0) R_MutableData_appendByte(self, (value & 0x00FF0000) >> 16);
  if ((value & 0xFFFFFF00) != 0) R_MutableData_appendByte(self, (value & 0x0000FF00) >>  8);
  if ((value & 0xFFFFFFFF) != 0) R_MutableData_appendByte(self, (value & 0x000000FF) >>  0);
  return self;
}

uint32_t R_FUNCTION_ATTRIBUTES R_MutableData_getUInt32(R_MutableData* self) {
  uint32_t integer = (uint32_t)R_MutableData_byte(self, 0);
  if (R_MutableData_size(self) > 1) integer = R_MutableData_byte(self, 1) + (integer << 8);
  if (R_MutableData_size(self) > 2) integer = R_MutableData_byte(self, 2) + (integer << 8);
  if (R_MutableData_size(self) > 3) integer = R_MutableData_byte(self, 3) + (integer << 8);
  return integer;
}

R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendUInt32AsBCD(R_MutableData* self, uint32_t value) {
  //Construct the array first in reverse, then append it to self
  R_MutableData* reversed = R_Type_New(R_MutableData);
  if (self == NULL || reversed == NULL) return NULL;
  do {
    uint8_t right_digit = value % 10;
    value /= 10;
    uint8_t left_digit = value % 10;
    value /= 10;
    R_MutableData_appendByte(reversed, (left_digit << 4) | right_digit);
  } while (value > 0);

  for (int i=(int)R_MutableData_size(reversed)-1; i>=0; i--) {
    R_MutableData_appendByte(self, R_MutableData_byte(reversed, i));
  }

  R_Type_Delete(reversed);
  return self;
}

int R_FUNCTION_ATTRIBUTES R_MutableData_compare(const R_MutableData* self, const R_MutableData* comparor) {
  return R_MutableData_compareWithCArray(self, R_MutableData_bytes(comparor), R_MutableData_size(comparor));
}

int R_FUNCTION_ATTRIBUTES R_MutableData_compareWithCArray(const R_MutableData* self, const uint8_t* comparor, size_t bytes) {
  if (R_Type_IsNotOf(self, R_MutableData) || comparor == NULL) return -1;
  if (R_MutableData_size(self) < bytes) return -1;
  if (R_MutableData_size(self) > bytes) return -1;
  return os_memcmp(R_MutableData_bytes(self), comparor, bytes);
}
