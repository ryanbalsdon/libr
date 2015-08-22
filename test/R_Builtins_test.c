/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "R_Type.h"


void test_integer(void) {
  R_Integer* value = R_Type_New(R_Integer);
  assert(R_Integer_get(value) == 0);
  R_Integer_set(value, 42);
  assert(R_Integer_get(value) == 42);
  char buffer[20];
  assert(R_Stringify(value, buffer, 20) == 2);
  assert(strcmp(buffer, "42") == 0);
  R_Integer* value_copy = R_Type_Copy(value);
  assert(R_Integer_get(value_copy) == 42);
  R_Type_Delete(value);
  assert(R_Integer_get(value_copy) == 42);
  R_Type_Delete(value_copy);
}

void test_float(void) {
  R_Float* value = R_Type_New(R_Float);
  assert(R_Float_get(value) == 0.0f);
  R_Float_set(value, 4.2f);
  assert(R_Float_get(value) == 4.2f);

  char buffer[20];
  assert(R_Stringify(value, buffer, 20) == 3);
  assert(strcmp(buffer, "4.2") == 0);

  R_Float* value_copy = R_Type_Copy(value);
  assert(R_Float_get(value_copy) == 4.2f);
  R_Type_Delete(value);
  assert(R_Float_get(value_copy) == 4.2f);
  R_Type_Delete(value_copy);
}

void test_unsigned(void) {
  R_Unsigned* value = R_Type_New(R_Unsigned);
  assert(R_Unsigned_get(value) == 0);
  R_Unsigned_set(value, -1);
  assert(R_Unsigned_get(value) > 0);
  R_Unsigned* value_copy = R_Type_Copy(value);
  assert(R_Unsigned_get(value_copy) == R_Unsigned_get(value));

  R_Unsigned_set(value, 2);
  char buffer[20];
  assert(R_Stringify(value, buffer, 20) == 1);
  assert(strcmp(buffer, "2") == 0);

  R_Type_Delete(value);
  R_Type_Delete(value_copy);
}

void test_boolean(void) {
  R_Boolean* value = R_Type_New(R_Boolean);
  assert(R_Boolean_get(value) == false);
  R_Boolean_set(value, true);
  assert(R_Boolean_get(value) == true);
  char buffer[20];
  assert(R_Stringify(value, buffer, 20) == 4);
  assert(strcmp(buffer, "true") == 0);
  R_Boolean* value_copy = R_Type_Copy(value);
  assert(R_Boolean_get(value_copy) == R_Boolean_get(value));
  R_Type_Delete(value);
  R_Type_Delete(value_copy);
}

void test_data(void) {
  uint8_t test_data[] = {0xFF, 0x00, 0x02, 0xC3};
  size_t test_size = sizeof(test_data);

  R_Data* empty = R_Type_New(R_Data);
  assert(R_Data_size(empty) == 0);
  assert(R_Data_bytes(empty) == NULL);
  R_Type_Delete(empty);

  R_Data* zeros = R_Data_New(NULL, 20);
  assert(R_Data_size(zeros) == 20);
  assert(R_Data_bytes(zeros) != NULL);
  for (int i=0; i<20; i++) assert(R_Data_bytes(zeros)[i] == 0x00);
  R_Type_Delete(zeros);

  R_Data* test = R_Data_New(test_data, test_size);
  assert(R_Data_size(test) == test_size);
  assert(R_Data_bytes(test) != NULL);
  assert(memcmp(R_Data_bytes(test), test_data, test_size) == 0);

  R_Data* copy = R_Type_Copy(test);
  R_Type_Delete(test);
  assert(R_Data_size(copy) == test_size);
  assert(R_Data_bytes(copy) != NULL);
  assert(memcmp(R_Data_bytes(copy), test_data, test_size) == 0);

  char buffer[25];
  assert(R_Stringify(copy, buffer, 25) == 24);
  assert(strcmp(buffer, "{0xFF, 0x00, 0x02, 0xC3}") == 0);
  R_Type_Delete(copy);
}

void test_string(void) {
  char test_string[] = "test";
  R_String* new = R_String_New(test_string);
  assert(new != NULL);
  assert(R_String_get(new) != NULL);
  assert(strcmp(R_String_get(new), test_string) == 0);
  char buffer[5];
  assert(R_Stringify(new, buffer, 5) == 4);
  assert(strcmp(buffer, "test") == 0);
  R_String* copy = R_Type_Copy(new);
  assert(copy);
  R_Type_Delete(new);
  assert(strcmp(R_String_get(copy), test_string) == 0);
  R_Type_Delete(copy);
}

void test_null(void) {
  R_Null* null = R_Type_New(R_Null);
  R_Type_Delete(null);
}

void test_stringify(void) {
  R_Integer* value = R_Type_New(R_Integer);
  assert(R_Integer_get(value) == 0);
  R_Integer_set(value, 24135);
  char buffer_large[20];
  char buffer_short[3];
  char buffer_perfect[6];

  assert(R_Stringify(value, buffer_large, 20) == 5);
  assert(R_Stringify(value, buffer_short, 3) == 3);
  assert(R_Stringify(value, buffer_perfect, 6) == 5);

  assert(strcmp(buffer_large, "24135") == 0);
  assert(strcmp(buffer_short, "24") == 0);
  assert(strcmp(buffer_perfect, "24135") == 0);

  R_Type_Delete(value);
}

int main(void) {
  test_stringify();
  test_integer();
  test_float();
  test_unsigned();
  test_boolean();
  test_data();
  test_string();
  test_null();

  assert(R_Type_BytesAllocated == 0);
  printf("Pass\n");

  return 0;
}