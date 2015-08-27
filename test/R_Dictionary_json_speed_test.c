#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "R_Dictionary.h"
#include "R_MutableString.h"
#include "R_List.h"

 int main(void) {
#include "R_Dictionary_large_json.data"
  R_Dictionary* dict = R_Type_New(R_Dictionary);

  clock_t begin = clock();
  assert(R_Dictionary_fromJson(dict, large_json) == dict);
  clock_t end = clock();

  R_List* orders = R_Dictionary_get(dict, "orders");
  assert(orders != NULL);
  assert(R_Type_IsOf(orders, R_List));
  assert(R_List_size(orders) == 31);
  R_Type_Delete(large_json);
  R_Type_Delete(dict);

  assert(R_Type_BytesAllocated == 0);
  printf("Pass: %lu ticks\n", end-begin);
  return 0;
 }