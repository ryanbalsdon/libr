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
	assert(R_Dictionary_doesKeyExist(dict, "key") == false);
	assert(R_Dictionary_setString(dict, "key", "data") == true);
	assert(R_Dictionary_doesKeyExist(dict, "key") == true);
	const char* data = R_Dictionary_getString(dict, "key");
	assert (data != NULL);
	assert(strcmp(data, "data") == 0);
	assert(R_Dictionary_setString(dict, "key", "data2") == true);
	data = R_Dictionary_getString(dict, "key");
	assert (data != NULL);
	assert(strcmp(data, "data2") == 0);
	R_Type_Delete(dict);
}

void test_strings(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	assert(R_Dictionary_setString(dict, "crazy", "no") == true);
	assert(R_Dictionary_setString(dict, "awesome", "yes") == true);
	assert(strcmp(R_Dictionary_getString(dict, "crazy"), "no") == 0);
	assert(strcmp(R_Dictionary_getString(dict, "awesome"), "yes") == 0);
	R_Type_Delete(dict);
}

void test_ints(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	assert(R_Dictionary_setInt(dict, "integer-test", 42) == true);
	assert(R_Dictionary_setInt(dict, "whatager-test", 3153) == true);
	assert(R_Dictionary_getInt(dict, "integer-test") == 42);
	assert(R_Dictionary_getInt(dict, "whatager-test") == 3153);
	assert(R_Dictionary_setString(dict, "crazy", "no") == true);
	assert(R_Dictionary_setString(dict, "awesome", "yes") == true);
	assert(strcmp(R_Dictionary_getString(dict, "crazy"), "no") == 0);
	assert(strcmp(R_Dictionary_getString(dict, "awesome"), "yes") == 0);
	R_Type_Delete(dict);
}

void test_floats(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	assert(R_Dictionary_setFloat(dict, "float-test", 42.0f) == true);
	assert(R_Dictionary_setFloat(dict, "pi", 3.14159f) == true);
	assert(R_Dictionary_getFloat(dict, "float-test") == 42.0f);
	assert(R_Dictionary_getFloat(dict, "pi") == 3.14159f);
	assert(R_Dictionary_setInt(dict, "integer-test", 42) == true);
	assert(R_Dictionary_setInt(dict, "whatager-test", 3153) == true);
	assert(R_Dictionary_getInt(dict, "integer-test") == 42);
	assert(R_Dictionary_getInt(dict, "whatager-test") == 3153);
	assert(R_Dictionary_setString(dict, "crazy", "no") == true);
	assert(R_Dictionary_setString(dict, "awesome", "yes") == true);
	assert(strcmp(R_Dictionary_getString(dict, "crazy"), "no") == 0);
	assert(strcmp(R_Dictionary_getString(dict, "awesome"), "yes") == 0);
	R_Type_Delete(dict);
}

void test_data_mutability(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	assert(R_Dictionary_setInt(dict, "type-test", 42) == true);
	assert(strcmp(R_Dictionary_getString(dict, "type-test"), "42") == 0);
	assert(R_Dictionary_getFloat(dict, "type-test") == 42.0f);
	assert(R_Dictionary_getInt(dict, "type-test") == 42);
	assert(R_Dictionary_getObject(dict, "type-test") == NULL);
	assert(R_Dictionary_getArraySize(dict, "type-test") == 0);

	assert(R_Dictionary_setString(dict, "type-test", "hello") == true);
	assert(strcmp(R_Dictionary_getString(dict, "type-test"), "hello") == 0);
	assert(R_Dictionary_getFloat(dict, "type-test") == 0.0f);
	assert(R_Dictionary_getInt(dict, "type-test") == 0);
	assert(R_Dictionary_getObject(dict, "type-test") == NULL);
	assert(R_Dictionary_getArraySize(dict, "type-test") == 0);
	R_Type_Delete(dict);
}

void test_sub_objects(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_Dictionary* child = R_Dictionary_setObject(dict, "child");
	assert(child != NULL);
	assert(R_Dictionary_setInt(child, "id", 1) == true);
	assert(R_Dictionary_setString(child, "name", "Gary") == true);
	assert(R_Dictionary_setInt(child, "awesomeness", 10.0f) == true);
	R_Dictionary* grandChild = R_Dictionary_setObject(child, "child");
	assert(grandChild != NULL);
	assert(R_Dictionary_setInt(grandChild, "id", 2) == true);
	assert(R_Dictionary_setString(grandChild, "name", "Grant") == true);
	assert(R_Dictionary_setInt(grandChild, "awesomeness", 8.0f) == true);

	assert(R_Dictionary_getObject(dict, "child") == child);
	assert(R_Dictionary_getObject(dict, "this key does not exist") == NULL);
	assert(R_Dictionary_getObject(child, "child") == grandChild);
	assert(R_Dictionary_getObject(R_Dictionary_getObject(dict, "child"), "child") == grandChild);
	R_Type_Delete(dict);
}

void test_arrays(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_Dictionary* arrayMember1 = R_Dictionary_addToArray(dict, "array");
	assert(arrayMember1 != NULL);
	assert(R_Dictionary_setInt(arrayMember1, "member", 1) == true);
	assert(R_Dictionary_getArraySize(dict, "array") == 1);
	R_Dictionary* arrayMember2 = R_Dictionary_addToArray(dict, "array");
	assert(arrayMember2 != NULL);
	assert(R_Dictionary_setInt(arrayMember2, "member", 2) == true);
	assert(R_Dictionary_getArraySize(dict, "array") == 2);
	assert(arrayMember1 == R_Dictionary_getArrayIndex(dict, "array", 0));
	assert(arrayMember2 == R_Dictionary_getArrayIndex(dict, "array", 1));
	R_Type_Delete(dict);
}

void test_serialization_1(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_Dictionary_setInt(dict, "integer 1", 42);
	R_String* string = R_Type_New(R_String);
	assert(R_Dictionary_serialize(dict, string) != NULL);
	assert(strcmp(R_String_getString(string), "{integer 1='42';};") == 0);
	R_String_reset(string);
	R_Dictionary_setInt(dict, "integer 2", 1);
	assert(R_Dictionary_serialize(dict, string) != NULL);
	assert(strcmp(R_String_getString(string), "{integer 1='42';integer 2='1';};") == 0);
	R_String_reset(string);
	R_Dictionary_setFloat(dict, "float 1", 80.12f);
	assert(R_Dictionary_serialize(dict, string) != NULL);
	assert(strcmp(R_String_getString(string), "{integer 1='42';integer 2='1';float 1='80.12';};") == 0);
	R_String_reset(string);
	R_Dictionary_setString(dict, "string 1", "testors");
	assert(R_Dictionary_serialize(dict, string) != NULL);
	assert(strcmp(R_String_getString(string), "{integer 1='42';integer 2='1';float 1='80.12';string 1='testors';};") == 0);
	R_String_reset(string);
	R_Dictionary* child = R_Dictionary_setObject(dict, "object");
	R_Dictionary_setInt(child, "id", 1);
	assert(R_Dictionary_serialize(dict, string) != NULL);
	assert(strcmp(R_String_getString(string), "{integer 1='42';integer 2='1';float 1='80.12';string 1='testors';object={id='1';};};") == 0);
	R_String_reset(string);
	R_Dictionary* arrayMember = R_Dictionary_addToArray(dict, "array");
	R_Dictionary_setInt(arrayMember, "i", 0);
	arrayMember = R_Dictionary_addToArray(dict, "array");
	R_Dictionary_setInt(arrayMember, "i", 1);
	R_Dictionary_setString(arrayMember, "final", "thoughts");
	assert(R_Dictionary_serialize(dict, string) != NULL);
	assert(strcmp(R_String_getString(string), "{integer 1='42';integer 2='1';float 1='80.12';string 1='testors';object={id='1';};array=[{i='0';};{i='1';final='thoughts';};];};") == 0);
	R_Type_Delete(dict);
	R_Type_Delete(string);
}

void test_serialization_2(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* string = R_Type_New(R_String);
	R_String_setString(string, "{number='14.1';};");
	assert(R_String_objectize(dict, string) != NULL);
	assert(R_Dictionary_getFloat(dict, "number") == 14.1f);
	assert(strcmp(R_Dictionary_getString(dict, "number"), "14.1") == 0);
	R_String_setString(string, "{number 1='14.1';number 2='2';};");
	assert(R_String_objectize(dict, string) != NULL);
	assert(R_Dictionary_getFloat(dict, "number 1") == 14.1f);
	assert(R_Dictionary_getInt(dict, "number 2") == 2);
	R_String_setString(string, "{lumber='14.5';child={number 2='3';stringified='yes';};};");
	assert(R_String_objectize(dict, string) != NULL);
	assert(strcmp(R_Dictionary_getString(dict, "lumber"), "14.5") == 0);
	R_Dictionary* child = R_Dictionary_getObject(dict, "child");
	assert(child != NULL);
	assert(R_Dictionary_getInt(child, "number 2") == 3);
	assert(strcmp(R_Dictionary_getString(child, "stringified"), "yes") == 0);
	R_String_setString(string, "{flumber='14.8';child1={grandchild={id='53';};}; child 2={grandchild={id='54';};};};");
	assert(R_String_objectize(dict, string) != NULL);
	assert(strcmp(R_Dictionary_getString(dict, "flumber"), "14.8") == 0);
	child = R_Dictionary_getObject(dict, "child1");
	assert(child != NULL);
	R_Dictionary* grandChild = R_Dictionary_getObject(child, "grandchild");
	assert(grandChild != NULL);
	assert(R_Dictionary_getInt(grandChild, "id") == 53);
	child = R_Dictionary_getObject(dict, "child 2");
	assert(child != NULL);
	grandChild = R_Dictionary_getObject(child, "grandchild");
	assert(grandChild != NULL);
	assert(R_Dictionary_getInt(grandChild, "id") == 54);
	R_String_setString(string, "{flubber='14.9';children=[{rd='1';};{rd='2';};];};");
	assert(R_String_objectize(dict, string) != NULL);
	assert(strcmp(R_Dictionary_getString(dict, "flubber"), "14.9") == 0);
	assert(R_Dictionary_getArraySize(dict, "children") == 2);
	assert(R_Dictionary_getArrayIndex(dict, "children", 0) != NULL);
	assert(R_Dictionary_getInt(R_Dictionary_getArrayIndex(dict, "children", 0), "rd") == 1);
	assert(R_Dictionary_getArrayIndex(dict, "children", 1) != NULL);
	assert(R_Dictionary_getInt(R_Dictionary_getArrayIndex(dict, "children", 1), "rd") == 2);
	R_Type_Delete(dict);
	R_Type_Delete(string);
}

int main(void) {
	assert(R_Type_BytesAllocated == 0);
	test_allocation();
	test_key_creation();
	test_strings();
	test_ints();
	test_floats();
	test_data_mutability();
	test_sub_objects();
	test_arrays();
	test_serialization_1();
	test_serialization_2();
	assert(R_Type_BytesAllocated == 0);
	printf("Pass\n");
}
