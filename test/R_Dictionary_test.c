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
#include "R_Dictionary.h"


void test_allocation(void) {
	assert(R_Type_BytesAllocated == 0);
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_Type_Delete(dict);
	assert(R_Type_BytesAllocated == 0);
}
extern const R_Type* R_Dictionary_Datum_Type;

void test_key_creation(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_Dictionary_add(dict, "key1", R_Integer);
	R_Dictionary_add(dict, "key2", R_Integer);

	assert(R_Dictionary_isPresent(dict, "key1") == true);
	assert(R_Dictionary_isNotPresent(dict, "key1") == false);

	assert(R_Dictionary_isPresent(dict, "key2") == true);
	assert(R_Dictionary_isNotPresent(dict, "key2") == false);

	assert(R_Dictionary_isPresent(dict, "key3") == false);
	assert(R_Dictionary_isNotPresent(dict, "key3") == true);

	R_Type_Delete(dict);
}

void test_integers(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_Integer* integer = R_Dictionary_add(dict, "integer", R_Integer);
	assert(integer != NULL);
	assert(R_Type_IsOf(integer, R_Integer));
	assert(R_Dictionary_get(dict, "integer") == integer);
	R_Integer_set(integer, 1);
	integer = R_Dictionary_add(dict, "answer", R_Integer);
	R_Integer_set(integer, 42);
	assert(R_Dictionary_get(dict, "integer") != integer);
	assert(R_Dictionary_get(dict, "answer") == integer);
	assert(R_Integer_get(R_Dictionary_get(dict, "integer")) == 1);
	assert(R_Integer_get(R_Dictionary_get(dict, "answer")) == 42);

	R_Type_Delete(dict);
}

void test_mixed(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);

	R_Integer* integer = R_Dictionary_add(dict, "integer", R_Integer);
	assert(integer != NULL);
	assert(R_Type_IsOf(integer, R_Integer));
	R_Float* floater = R_Dictionary_add(dict, "float", R_Float);
	assert(floater != NULL);
	assert(R_Type_IsOf(floater, R_Float));
	R_Float_set(floater, 3.14f);
	assert(R_Float_get(R_Dictionary_get(dict, "float")) == 3.14f);

	assert(R_Type_IsNotOf(R_Dictionary_get(dict, "float"), R_Integer));
	assert(R_Type_IsOf(R_Dictionary_get(dict, "float"), R_Float));
	assert(R_Type_IsNotOf(R_Dictionary_get(dict, "integer"), R_Float));
	assert(R_Type_IsOf(R_Dictionary_get(dict, "integer"), R_Integer));

	R_Type_Delete(dict);
}

int main(void) {
	assert(R_Type_BytesAllocated == 0);
	test_allocation();
	test_key_creation();
	test_integers();
	test_mixed();

	assert(R_Type_BytesAllocated == 0);
	printf("Pass\n");
}
