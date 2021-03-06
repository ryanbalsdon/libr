#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "R_Dictionary.h"
#include "R_MutableString.h"
#include "R_List.h"
#include "R_KeyValuePair.h"

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

void test_write_json_strings(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_MutableString* json = R_Type_New(R_MutableString);

	R_MutableString* string = R_Dictionary_add(dict, "string key 1", R_MutableString);
	R_MutableString_appendCString(string, "string value 1");
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_MutableString_compare(json, "{\"string key 1\":\"string value 1\"}"));

	string = R_Dictionary_add(dict, "string key 2", R_MutableString);
	R_MutableString_appendCString(string, "string value 2");
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_MutableString_compare(json, "{\"string key 1\":\"string value 1\",\"string key 2\":\"string value 2\"}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_write_json_numbers(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_MutableString* json = R_Type_New(R_MutableString);

	R_Integer* integer = R_Dictionary_add(dict, "int", R_Integer);
	R_Integer_set(integer, -42);
	R_Float* floater = R_Dictionary_add(dict, "float", R_Float);
	R_Float_set(floater, 1.4e-12f);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_MutableString_compare(json, "{\"int\":-42,\"float\":1.4e-12}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_write_json_objects(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_MutableString* json = R_Type_New(R_MutableString);

	R_Dictionary* sub = R_Dictionary_add(dict, "upper", R_Dictionary);
	sub = R_Dictionary_add(sub, "lower", R_Dictionary);
	R_Integer* integer = R_Dictionary_add(sub, "int", R_Integer);
	R_Integer_set(integer, 1);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_MutableString_compare(json, "{\"upper\":{\"lower\":{\"int\":1}}}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_write_json_booleans(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_MutableString* json = R_Type_New(R_MutableString);

	R_Boolean* bin = R_Dictionary_add(dict, "pass", R_Boolean);
	R_Boolean_set(bin, true);
	bin = R_Dictionary_add(dict, "fail", R_Boolean);
	R_Boolean_set(bin, false);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_MutableString_compare(json, "{\"pass\":true,\"fail\":false}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_write_json_arrays(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_MutableString* json = R_Type_New(R_MutableString);

	R_List* list = R_Dictionary_add(dict, "array", R_List);
	R_Integer_set(R_List_add(list, R_Integer), 0);
	R_Integer_set(R_List_add(list, R_Integer), 1);
	R_Float_set(R_List_add(list, R_Float), 2.02f);
	R_Integer_set(R_List_add(list, R_Integer), 3);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_MutableString_compare(json, "{\"array\":[0,1,2.02,3]}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_read_json_strings(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_MutableString* json1 = R_MutableString_appendCString(R_Type_New(R_MutableString), "{\"string key 1\":\"string value 1\"}");
	R_MutableString* json2 = R_MutableString_appendCString(R_Type_New(R_MutableString), "{\"string key 1\":\"string value 1\",\"string key 2\":\"string value 2\"}");
	
	assert(R_Dictionary_fromJson(dict, json1) == dict);
	assert(R_Dictionary_get(dict, "string key 1") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "string key 1"), R_MutableString));
	assert(R_MutableString_compare(R_Dictionary_get(dict, "string key 1"), "string value 1"));

	assert(R_Dictionary_fromJson(dict, json2) == dict);
	assert(R_Dictionary_get(dict, "string key 1") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "string key 1"), R_MutableString));
	assert(R_MutableString_compare(R_Dictionary_get(dict, "string key 1"), "string value 1"));
	assert(R_Dictionary_get(dict, "string key 2") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "string key 2"), R_MutableString));
	assert(R_MutableString_compare(R_Dictionary_get(dict, "string key 2"), "string value 2"));

	R_Type_Delete(dict);
	R_Type_Delete(json1);
	R_Type_Delete(json2);
}

void test_read_json_numbers(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_MutableString* json = R_MutableString_appendCString(R_Type_New(R_MutableString), "{\"int\":-42,\"float\":1.4e-12}");

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
	R_MutableString* json = R_MutableString_appendCString(R_Type_New(R_MutableString), "{\"pass\":true,\"fail\":false}");

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
	R_MutableString* json = R_MutableString_appendCString(R_Type_New(R_MutableString), "{\"upper\":{\"lower\":{\"int\":1}}}");

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
	R_MutableString* json = R_MutableString_appendCString(R_Type_New(R_MutableString), "{\"array\":[0,1,2.02,3]}");

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
	R_MutableString* json = R_MutableString_appendCString(R_Type_New(R_MutableString), "{\"this\":null}");

	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "this") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "this"), R_Null));

	R_MutableString_reset(json);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_MutableString_compare(json, "{\"this\":null}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_empty_array(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_MutableString* json = R_MutableString_appendCString(R_Type_New(R_MutableString), "{\"array\":[]}");
	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "array") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "array"), R_List));
	R_List* array = R_Dictionary_get(dict, "array");
	assert(R_List_size(array) == 0);

	R_MutableString_reset(json);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_MutableString_compare(json, "{\"array\":[]}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_array_with_one_object(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_MutableString* json = R_MutableString_appendCString(R_Type_New(R_MutableString), "{\"some_product\":[{\"price\":\"26.61\",\"tax\":0.13,\"name\":\"bugs\"}]}");
	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "some_product") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "some_product"), R_List));
	R_List* array = R_Dictionary_get(dict, "some_product");
	assert(R_List_size(array) == 1);

	R_MutableString_reset(json);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_MutableString_compare(json, "{\"some_product\":[{\"price\":\"26.61\",\"tax\":0.13,\"name\":\"bugs\"}]}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_empty_object(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_MutableString* json = R_MutableString_appendCString(R_Type_New(R_MutableString), "{\"object\":{}}");
	assert(R_Dictionary_fromJson(dict, json) == dict);
	assert(R_Dictionary_get(dict, "object") != NULL);
	assert(R_Type_IsOf(R_Dictionary_get(dict, "object"), R_Dictionary));

	R_MutableString_reset(json);
	assert(R_Dictionary_toJson(dict, json) == json);
	assert(R_MutableString_compare(json, "{\"object\":{}}"));

	R_Type_Delete(json);
	R_Type_Delete(dict);
}

void test_puts(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);

	R_MutableString* string = R_Dictionary_add(dict, "hello", R_MutableString);
	R_MutableString_appendCString(string, "world");

	char buffer[18];
	assert(R_Stringify(dict, buffer, 18) == 17);
	assert(strcmp(buffer, "{\"hello\":\"world\"}") == 0);

	R_Type_Delete(dict);
}

void test_foreach(void) {
	R_Dictionary* dict = R_Type_New(R_Dictionary);
	R_Integer_set(R_Dictionary_add(dict, "key1", R_Integer), 1);
	R_Integer_set(R_Dictionary_add(dict, "key2", R_Integer), 2);
	assert(R_Dictionary_size(dict) == 2);

  bool key1_found = false;
  bool key2_found = false;

  R_Dictionary_each(dict, pair) {
    assert(R_Type_IsOf(pair, R_KeyValuePair));
    assert(R_Type_IsOf(R_KeyValuePair_key(pair), R_MutableString));
    assert(R_Type_IsOf(R_KeyValuePair_value(pair), R_Integer));
    if (R_MutableString_compare(R_KeyValuePair_key(pair), "key1")) {
      key1_found = true;
      assert(R_Integer_get(R_KeyValuePair_value(pair)) == 1);
    }
    if (R_MutableString_compare(R_KeyValuePair_key(pair), "key2")) {
      key2_found = true;
      assert(R_Integer_get(R_KeyValuePair_value(pair)) == 2);
    }
  }
  assert(key1_found);
  assert(key2_found);
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
	test_foreach();
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
