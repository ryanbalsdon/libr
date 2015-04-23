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
#include "R_String.h"
#include "R_List.h"

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
	assert(R_Dictionary_size(dict) == 1);
	R_Dictionary_add(dict, "key2", R_Integer);
	assert(R_Dictionary_size(dict) == 2);

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

void test_copy(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);

	R_Integer_set(R_Dictionary_add(dict, "integer", R_Integer), 12);
	R_Float_set(R_Dictionary_add(dict, "float", R_Float), 3.14f);

	R_Dictionary* copy = R_Type_Copy(dict);
	R_Type_Delete(dict);

	R_Integer* integer = R_Dictionary_get(copy, "integer");
	assert(integer != NULL);
	assert(R_Type_IsOf(integer, R_Integer));
	assert(R_Integer_get(integer) == 12);

	R_Float* floater = R_Dictionary_get(copy, "float");
	assert(floater != NULL);
	assert(R_Type_IsOf(floater, R_Float));
	assert(R_Float_get(floater) == 3.14f);

	R_Type_Delete(copy);
}

void test_merge(void) {
	R_Dictionary* dict_a = R_Type_New(R_Dictionary);
	R_Dictionary* dict_b = R_Type_New(R_Dictionary);
	R_Integer_set(R_Dictionary_add(dict_a, "integer", R_Integer), 12);
	R_Float_set(R_Dictionary_add(dict_b, "float", R_Float), 3.14f);

	assert(R_Dictionary_merge(dict_a, dict_b) == dict_a);

	assert(R_Type_IsOf(R_Dictionary_get(dict_a, "integer"), R_Integer));
	assert(R_Integer_get(R_Dictionary_get(dict_a, "integer")) == 12);
	assert(R_Type_IsOf(R_Dictionary_get(dict_a, "float"), R_Float));
	assert(R_Float_get(R_Dictionary_get(dict_a, "float")) == 3.14f);

	assert(R_Dictionary_get(dict_b, "integer") == NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict_b, "float"), R_Float));
	assert(R_Float_get(R_Dictionary_get(dict_b, "float")) == 3.14f);

	R_Type_Delete(dict_a);
	R_Type_Delete(dict_b);
}

void test_value_iterator(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_Integer* integer = R_Dictionary_add(dict, "first", R_Integer);
	R_Integer_set(integer, 0);
	integer = R_Dictionary_add(dict, "second", R_Integer);
	R_Integer_set(integer, 2);
	integer = R_Dictionary_add(dict, "third", R_Integer);
	R_Integer_set(integer, 1);

	bool integer0_found = false;
	bool integer1_found = false;
	bool integer2_found = false;

	R_Functor* iterator = R_Dictionary_ValueIterator(R_Type_New(R_Functor), dict);
	R_Integer* element = NULL;
	while ((element = R_Functor_call(iterator)) != NULL) {
		assert(R_Type_IsOf(element, R_Integer));
		if (R_Integer_get(element) == 0) integer0_found = true;
		if (R_Integer_get(element) == 1) integer1_found = true;
		if (R_Integer_get(element) == 2) integer2_found = true;
	}

	assert(integer0_found);
	assert(integer1_found);
	assert(integer2_found);

	R_Type_Delete(iterator);
	R_Type_Delete(dict);
}

void test_key_iterator(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_Integer* integer = R_Dictionary_add(dict, "first", R_Integer);
	R_Integer_set(integer, 0);
	integer = R_Dictionary_add(dict, "second", R_Integer);
	R_Integer_set(integer, 2);
	integer = R_Dictionary_add(dict, "third", R_Integer);
	R_Integer_set(integer, 1);

	bool integer0_found = false;
	bool integer1_found = false;
	bool integer2_found = false;
	R_Functor* iterator = R_Dictionary_KeyIterator(R_Type_New(R_Functor), dict);
	R_String* key = NULL;
	while ((key = R_Functor_call(iterator)) != NULL) {
		assert(R_Type_IsOf(key, R_String));
		if (R_String_compare(key, "first")) integer0_found = true;
		if (R_String_compare(key, "second")) integer1_found = true;
		if (R_String_compare(key, "third")) integer2_found = true;
	}

	assert(integer0_found);
	assert(integer1_found);
	assert(integer2_found);

	R_Type_Delete(iterator);
	R_Type_Delete(dict);
}

void test_sexy_iterator(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_Integer* integer = R_Dictionary_add(dict, "first", R_Integer);
	R_Integer_set(integer, 0);
	integer = R_Dictionary_add(dict, "second", R_Integer);
	R_Integer_set(integer, 2);
	integer = R_Dictionary_add(dict, "third", R_Integer);
	R_Integer_set(integer, 1);

	bool integer0_found = false;
	bool integer1_found = false;
	bool integer2_found = false;
	R_Functor* iterator = R_Dictionary_KeyIterator(R_Type_New(R_Functor), dict);
	R_Functor_iterate(iterator, R_String, key) {
		assert(R_Type_IsOf(key, R_String));
		if (R_String_compare(key, "first")) integer0_found = true;
		else if (R_String_compare(key, "second")) integer1_found = true;
		else if (R_String_compare(key, "third")) integer2_found = true;
		else assert(NULL);
	}

	assert(integer0_found);
	assert(integer1_found);
	assert(integer2_found);

	R_Type_Delete(iterator);
	R_Type_Delete(dict);
}

void test_write_json_strings(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_Type_New(R_String);

	R_String* string = R_Dictionary_add(dict, "string key 1", R_String);
	R_String_appendCString(string, "string value 1");
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_String_compare(json, "{\"string key 1\":\"string value 1\"}"));

	string = R_Dictionary_add(dict, "string key 2", R_String);
	R_String_appendCString(string, "string value 2");
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_String_compare(json, "{\"string key 1\":\"string value 1\",\"string key 2\":\"string value 2\"}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_write_json_numbers(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_Type_New(R_String);

	R_Integer* integer = R_Dictionary_add(dict, "int", R_Integer);
	R_Integer_set(integer, -42);
	R_Float* floater = R_Dictionary_add(dict, "float", R_Float);
	R_Float_set(floater, 1.4e-12f);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_String_compare(json, "{\"int\":-42,\"float\":1.4e-12}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_write_json_objects(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_Type_New(R_String);

	R_Dictionary* sub = R_Dictionary_add(dict, "upper", R_Dictionary);
	sub = R_Dictionary_add(sub, "lower", R_Dictionary);
	R_Integer* integer = R_Dictionary_add(sub, "int", R_Integer);
	R_Integer_set(integer, 1);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_String_compare(json, "{\"upper\":{\"lower\":{\"int\":1}}}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_write_json_booleans(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_Type_New(R_String);

	R_Boolean* bin = R_Dictionary_add(dict, "pass", R_Boolean);
	R_Boolean_set(bin, true);
	bin = R_Dictionary_add(dict, "fail", R_Boolean);
	R_Boolean_set(bin, false);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_String_compare(json, "{\"pass\":true,\"fail\":false}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_write_json_arrays(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_Type_New(R_String);

	R_List* list = R_Dictionary_add(dict, "array", R_List);
	R_Integer_set(R_List_add(list, R_Integer), 0);
	R_Integer_set(R_List_add(list, R_Integer), 1);
	R_Float_set(R_List_add(list, R_Float), 2.02f);
	R_Integer_set(R_List_add(list, R_Integer), 3);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_String_compare(json, "{\"array\":[0,1,2.02,3]}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_read_json_strings(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json1 = R_String_appendCString(R_Type_New(R_String), "{\"string key 1\":\"string value 1\"}");
	R_String* json2 = R_String_appendCString(R_Type_New(R_String), "{\"string key 1\":\"string value 1\",\"string key 2\":\"string value 2\"}");
	
	assert(R_Dictionary_fromJson(dict, json1) == dict);
	assert(R_Dictionary_get(dict, "string key 1") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "string key 1"), R_String));
	assert(R_String_compare(R_Dictionary_get(dict, "string key 1"), "string value 1"));

	assert(R_Dictionary_fromJson(dict, json2) == dict);
	assert(R_Dictionary_get(dict, "string key 1") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "string key 1"), R_String));
	assert(R_String_compare(R_Dictionary_get(dict, "string key 1"), "string value 1"));
	assert(R_Dictionary_get(dict, "string key 2") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "string key 2"), R_String));
	assert(R_String_compare(R_Dictionary_get(dict, "string key 2"), "string value 2"));

	R_Type_Delete(dict);
	R_Type_Delete(json1);
	R_Type_Delete(json2);
}

void test_read_json_numbers(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_String_appendCString(R_Type_New(R_String), "{\"int\":-42,\"float\":1.4e-12}");

	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "int") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "int"), R_Integer));
	R_Integer* integer = R_Dictionary_get(dict, "int");
	assert(R_Integer_get(integer) == -42);
	assert(R_Dictionary_get(dict, "float") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "float"), R_Float));
	R_Float* floater = R_Dictionary_get(dict, "float");
	assert(R_Float_get(floater) == 1.4e-12f);

	R_Type_Delete(dict);
	R_Type_Delete(json);
}

void test_read_json_booleans(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_String_appendCString(R_Type_New(R_String), "{\"pass\":true,\"fail\":false}");

	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "pass") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "pass"), R_Boolean));
	R_Boolean* boolean = R_Dictionary_get(dict, "pass");
	assert(R_Boolean_get(boolean) == true);
	assert(R_Dictionary_get(dict, "fail") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "fail"), R_Boolean));
	boolean = R_Dictionary_get(dict, "fail");
	assert(R_Boolean_get(boolean) == false);

	R_Type_Delete(dict);
	R_Type_Delete(json);
}

void test_read_json_objects(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_String_appendCString(R_Type_New(R_String), "{\"upper\":{\"lower\":{\"int\":1}}}");

	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "upper") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "upper"), R_Dictionary));
	R_Dictionary* upper = R_Dictionary_get(dict, "upper");

	assert(R_Dictionary_get(upper, "lower") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(upper, "lower"), R_Dictionary));
	R_Dictionary* lower = R_Dictionary_get(upper, "lower");

	assert(R_Dictionary_get(lower, "int") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(lower, "int"), R_Integer));
	R_Integer* integer = R_Dictionary_get(lower, "int");
	assert(R_Integer_get(integer) == 1);

	R_Type_Delete(dict);
	R_Type_Delete(json);
}

void test_read_json_arrays(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_String_appendCString(R_Type_New(R_String), "{\"array\":[0,1,2.02,3]}");

	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "array") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "array"), R_List));
	R_List* array = R_Dictionary_get(dict, "array");
	assert(R_List_size(array) == 4);

	assert(R_Type_IsOf(R_List_pointerAtIndex(array,0),R_Integer));
	R_Integer* integer = R_List_pointerAtIndex(array,0);
	assert(R_Integer_get(integer) == 0);

	assert(R_Type_IsOf(R_List_pointerAtIndex(array,1),R_Integer));
	integer = R_List_pointerAtIndex(array,1);
	assert(R_Integer_get(integer) == 1);

	assert(R_Type_IsOf(R_List_pointerAtIndex(array,2),R_Float));
	R_Float* floater = R_List_pointerAtIndex(array,2);
	assert(R_Float_get(floater) == 2.02f);

	assert(R_Type_IsOf(R_List_pointerAtIndex(array,3),R_Integer));
	integer = R_List_pointerAtIndex(array,3);
	assert(R_Integer_get(integer) == 3);

	R_Type_Delete(dict);
	R_Type_Delete(json);
}

void test_json_nulls(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_String_appendCString(R_Type_New(R_String), "{\"this\":null}");

	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "this") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "this"), R_Null));

	R_String_reset(json);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_String_compare(json, "{\"this\":null}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_empty_array(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_String_appendCString(R_Type_New(R_String), "{\"array\":[]}");
	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "array") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "array"), R_List));
	R_List* array = R_Dictionary_get(dict, "array");
	assert(R_List_size(array) == 0);

	R_String_reset(json);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_String_compare(json, "{\"array\":[]}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_array_with_one_object(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_String_appendCString(R_Type_New(R_String), "{\"some_product\":[{\"price\":\"26.61\",\"tax\":0.13,\"name\":\"bugs\"}]}");
	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "some_product") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "some_product"), R_List));
	R_List* array = R_Dictionary_get(dict, "some_product");
	assert(R_List_size(array) == 1);

	R_String_reset(json);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_String_compare(json, "{\"some_product\":[{\"price\":\"26.61\",\"tax\":0.13,\"name\":\"bugs\"}]}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_empty_object(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_String* json = R_String_appendCString(R_Type_New(R_String), "{\"object\":{}}");
	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "object") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "object"), R_Dictionary));

	R_String_reset(json);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_String_compare(json, "{\"object\":{}}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_puts(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);

	R_String* string = R_Dictionary_add(dict, "hello", R_String);
	R_String_appendCString(string, "world");

	R_Puts(dict);

	R_Type_Delete(dict);
}

int main(void) {
	assert(R_Type_BytesAllocated == 0);
	test_allocation();
	test_key_creation();
	test_copy();
	test_merge();
	test_integers();
	test_mixed();
	test_value_iterator();
	test_key_iterator();
	test_sexy_iterator();
	test_write_json_strings();
	test_write_json_numbers();
	test_write_json_objects();
	test_write_json_booleans();
	test_write_json_arrays();
	test_read_json_strings();
	test_read_json_numbers();
	test_read_json_booleans();
	test_read_json_objects();
	test_read_json_arrays();
	test_json_nulls();
	test_empty_array();
	test_array_with_one_object();
	test_empty_object();
	test_puts();

	assert(R_Type_BytesAllocated == 0);
	printf("Pass\n");
}
