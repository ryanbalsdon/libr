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
#include "R_KeyValuePair.h"

void test_stuff(void);

int main(void) {
  test_stuff();
  assert(R_Type_BytesAllocated == 0);
  printf("PASS\n");
}

void test_stuff(void) {
  R_KeyValuePair* pair = R_Type_New(R_KeyValuePair);
  R_Integer* test_value = R_Type_New(R_Integer);
  R_Integer_set(test_value, 2);

  R_KeyValuePair_setKey(pair, "key");
  R_KeyValuePair_setValue(pair, test_value);

  assert(R_KeyValuePair_value(pair) == test_value);
  assert(R_MutableString_compare(R_KeyValuePair_key(pair), "key"));

  R_KeyValuePair* copy = R_Type_Copy(pair);
  assert(R_KeyValuePair_value(copy) != test_value);
  assert(R_Type_IsOf(R_KeyValuePair_value(copy), R_Integer));
  assert(R_Integer_get(R_KeyValuePair_value(copy)) == 2);
  assert(R_MutableString_compare(R_KeyValuePair_key(copy), "key"));

  R_Type_Delete(copy);
  R_Type_Delete(pair);
}
