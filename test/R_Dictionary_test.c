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

int main(void) {
	R_Dictionary* whit = R_Dictionary_alloc();
	R_Dictionary_reset(whit);
	whit = R_Dictionary_free(whit);
	assert(whit == NULL);

	//test key creation
	whit = R_Dictionary_alloc();
	assert(R_Dictionary_doesKeyExist(whit, "key") == false);
	assert(R_Dictionary_setString(whit, "key", "data") == true);
	assert(R_Dictionary_doesKeyExist(whit, "key") == true);
	const char* data = R_Dictionary_getString(whit, "key");
	assert (data != NULL);
	assert(strcmp(data, "data") == 0);
	assert(R_Dictionary_setString(whit, "key", "data2") == true);
	data = R_Dictionary_getString(whit, "key");
	assert (data != NULL);
	assert(strcmp(data, "data2") == 0);	

	//test strings
	R_Dictionary_reset(whit);
	assert(R_Dictionary_setString(whit, "crazy", "no") == true);
	assert(R_Dictionary_setString(whit, "awesome", "yes") == true);
	assert(strcmp(R_Dictionary_getString(whit, "crazy"), "no") == 0);
	assert(strcmp(R_Dictionary_getString(whit, "awesome"), "yes") == 0);

	//test ints
	assert(R_Dictionary_setInt(whit, "integer-test", 42) == true);
	assert(R_Dictionary_setInt(whit, "whatager-test", 3153) == true);
	assert(R_Dictionary_getInt(whit, "integer-test") == 42);
	assert(R_Dictionary_getInt(whit, "whatager-test") == 3153);
	assert(strcmp(R_Dictionary_getString(whit, "crazy"), "no") == 0);
	assert(strcmp(R_Dictionary_getString(whit, "awesome"), "yes") == 0);

	//test floats
	assert(R_Dictionary_setFloat(whit, "float-test", 42.0f) == true);
	assert(R_Dictionary_setFloat(whit, "pi", 3.14159f) == true);
	assert(R_Dictionary_getFloat(whit, "float-test") == 42.0f);
	assert(R_Dictionary_getFloat(whit, "pi") == 3.14159f);
	assert(R_Dictionary_getInt(whit, "integer-test") == 42);
	assert(R_Dictionary_getInt(whit, "whatager-test") == 3153);
	assert(strcmp(R_Dictionary_getString(whit, "crazy"), "no") == 0);
	assert(strcmp(R_Dictionary_getString(whit, "awesome"), "yes") == 0);

	//test data-type mutability
	assert(R_Dictionary_setInt(whit, "type-test", 42) == true);
	assert(strcmp(R_Dictionary_getString(whit, "type-test"), "42") == 0);
	assert(R_Dictionary_getFloat(whit, "type-test") == 42.0f);
	assert(R_Dictionary_getInt(whit, "type-test") == 42);
	assert(R_Dictionary_getObject(whit, "type-test") == NULL);
	assert(R_Dictionary_getArraySize(whit, "type-test") == 0);

	assert(R_Dictionary_setString(whit, "type-test", "hello") == true);
	assert(strcmp(R_Dictionary_getString(whit, "type-test"), "hello") == 0);
	assert(R_Dictionary_getFloat(whit, "type-test") == 0.0f);
	assert(R_Dictionary_getInt(whit, "type-test") == 0);
	assert(R_Dictionary_getObject(whit, "type-test") == NULL);
	assert(R_Dictionary_getArraySize(whit, "type-test") == 0);

	//sub-objects
	R_Dictionary* child = R_Dictionary_setObject(whit, "child");
	assert(child != NULL);
	assert(R_Dictionary_setInt(child, "id", 1) == true);
	assert(R_Dictionary_setString(child, "name", "Gary") == true);
	assert(R_Dictionary_setInt(child, "awesomeness", 10.0f) == true);
	R_Dictionary* grandChild = R_Dictionary_setObject(child, "child");
	assert(grandChild != NULL);
	assert(R_Dictionary_setInt(grandChild, "id", 2) == true);
	assert(R_Dictionary_setString(grandChild, "name", "Grant") == true);
	assert(R_Dictionary_setInt(grandChild, "awesomeness", 8.0f) == true);

	assert(R_Dictionary_getObject(whit, "child") == child);
	assert(R_Dictionary_getObject(whit, "this key does not exist") == NULL);
	assert(R_Dictionary_getObject(child, "child") == grandChild);
	assert(R_Dictionary_getObject(R_Dictionary_getObject(whit, "child"), "child") == grandChild);

	//arrays
	R_Dictionary* arrayMember1 = R_Dictionary_addToArray(whit, "array");
	assert(arrayMember1 != NULL);
	assert(R_Dictionary_setInt(arrayMember1, "member", 1) == true);
	assert(R_Dictionary_getArraySize(whit, "array") == 1);
	R_Dictionary* arrayMember2 = R_Dictionary_addToArray(whit, "array");
	assert(arrayMember2 != NULL);
	assert(R_Dictionary_setInt(arrayMember2, "member", 2) == true);
	assert(R_Dictionary_getArraySize(whit, "array") == 2);
	assert(arrayMember1 == R_Dictionary_getArrayIndex(whit, "array", 0));
	assert(arrayMember2 == R_Dictionary_getArrayIndex(whit, "array", 1));

	R_Dictionary_reset(whit);
	R_Dictionary_setInt(whit, "integer 1", 42);
	R_MutableString* string = R_MutableString_alloc();
	assert(R_Dictionary_serialize(whit, string) != NULL);
	assert(strcmp(R_MutableString_getString(string), "{integer 1='42';};") == 0);
	R_MutableString_reset(string);
	R_Dictionary_setInt(whit, "integer 2", 1);
	assert(R_Dictionary_serialize(whit, string) != NULL);
	assert(strcmp(R_MutableString_getString(string), "{integer 1='42';integer 2='1';};") == 0);
	R_MutableString_reset(string);
	R_Dictionary_setFloat(whit, "float 1", 80.12f);
	assert(R_Dictionary_serialize(whit, string) != NULL);
	assert(strcmp(R_MutableString_getString(string), "{integer 1='42';integer 2='1';float 1='80.12';};") == 0);
	R_MutableString_reset(string);
	R_Dictionary_setString(whit, "string 1", "testors");
	assert(R_Dictionary_serialize(whit, string) != NULL);
	assert(strcmp(R_MutableString_getString(string), "{integer 1='42';integer 2='1';float 1='80.12';string 1='testors';};") == 0);
	R_MutableString_reset(string);
	child = R_Dictionary_setObject(whit, "object");
	R_Dictionary_setInt(child, "id", 1);
	assert(R_Dictionary_serialize(whit, string) != NULL);
	assert(strcmp(R_MutableString_getString(string), "{integer 1='42';integer 2='1';float 1='80.12';string 1='testors';object={id='1';};};") == 0);
	R_MutableString_reset(string);
	R_Dictionary* arrayMember = R_Dictionary_addToArray(whit, "array");
	R_Dictionary_setInt(arrayMember, "i", 0);
	arrayMember = R_Dictionary_addToArray(whit, "array");
	R_Dictionary_setInt(arrayMember, "i", 1);
	R_Dictionary_setString(arrayMember, "final", "thoughts");
	assert(R_Dictionary_serialize(whit, string) != NULL);
	assert(strcmp(R_MutableString_getString(string), "{integer 1='42';integer 2='1';float 1='80.12';string 1='testors';object={id='1';};array=[{i='0';};{i='1';final='thoughts';};];};") == 0);

	R_Dictionary_reset(whit);
	R_MutableString_setString(string, "{number='14.1';};");
	assert(R_MutableString_objectize(whit, string) != NULL);
	assert(R_Dictionary_getFloat(whit, "number") == 14.1f);
	assert(strcmp(R_Dictionary_getString(whit, "number"), "14.1") == 0);
	R_MutableString_setString(string, "{number 1='14.1';number 2='2';};");
	assert(R_MutableString_objectize(whit, string) != NULL);
	assert(R_Dictionary_getFloat(whit, "number 1") == 14.1f);
	assert(R_Dictionary_getInt(whit, "number 2") == 2);
	R_MutableString_setString(string, "{lumber='14.5';child={number 2='3';stringified='yes';};};");
	assert(R_MutableString_objectize(whit, string) != NULL);
	assert(strcmp(R_Dictionary_getString(whit, "lumber"), "14.5") == 0);
	child = R_Dictionary_getObject(whit, "child");
	assert(child != NULL);
	assert(R_Dictionary_getInt(child, "number 2") == 3);
	assert(strcmp(R_Dictionary_getString(child, "stringified"), "yes") == 0);
	R_MutableString_setString(string, "{flumber='14.8';child1={grandchild={id='53';};}; child 2={grandchild={id='54';};};};");
	assert(R_MutableString_objectize(whit, string) != NULL);
	assert(strcmp(R_Dictionary_getString(whit, "flumber"), "14.8") == 0);
	child = R_Dictionary_getObject(whit, "child1");
	assert(child != NULL);
	grandChild = R_Dictionary_getObject(child, "grandchild");
	assert(grandChild != NULL);
	assert(R_Dictionary_getInt(grandChild, "id") == 53);
	child = R_Dictionary_getObject(whit, "child 2");
	assert(child != NULL);
	grandChild = R_Dictionary_getObject(child, "grandchild");
	assert(grandChild != NULL);
	assert(R_Dictionary_getInt(grandChild, "id") == 54);
	R_MutableString_setString(string, "{flubber='14.9';children=[{rd='1';};{rd='2';};];};");
	assert(R_MutableString_objectize(whit, string) != NULL);
	assert(strcmp(R_Dictionary_getString(whit, "flubber"), "14.9") == 0);
	assert(R_Dictionary_getArraySize(whit, "children") == 2);
	assert(R_Dictionary_getArrayIndex(whit, "children", 0) != NULL);
	assert(R_Dictionary_getInt(R_Dictionary_getArrayIndex(whit, "children", 0), "rd") == 1);
	assert(R_Dictionary_getArrayIndex(whit, "children", 1) != NULL);
	assert(R_Dictionary_getInt(R_Dictionary_getArrayIndex(whit, "children", 1), "rd") == 2);




	R_Dictionary_free(whit);
	printf("Pass\n");
}