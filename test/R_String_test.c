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
#include "R_String.h"
#include "R_ByteArray.h"

void test_set_get(void) {
	R_String* string = R_Type_New(R_String);
	assert(R_String_setString(string, "test") == string);
	assert(R_String_length(string) == 4);
	assert(strcmp(R_String_getString(string), "test") == 0);

	R_String_setString(string, "test 2");
	assert(strcmp(R_String_getString(string), "test 2") == 0);

	assert(R_String_isEmpty(string) == false);
	R_String_reset(string);
	assert(R_String_isEmpty(string) == true);

	assert(R_String_setSizedString(string, "0123456789", 4) != NULL);
	assert(strcmp(R_String_getString(string), "0123") == 0);

	R_Type_Delete(string);
}

void test_append_bytes(void) {
	R_String* string = R_Type_New(R_String);
	assert(R_String_setString(string, "test") == string);
	assert(R_String_length(string) == 4);
	R_String_appendCString(string, " of awesomeness");
	assert(R_String_length(string) == 19);
	assert(R_String_character(string, 0) == 't');
	assert(R_String_character(string, 8) == 'a');
	assert(R_String_character(string, 19) == '\0');
	assert(strcmp(R_String_getString(string), "test") != 0);
	assert(strcmp(R_String_getString(string), "test of awesomeness") == 0);
	R_String_appendBytes(string, " of awesomeness", 11);
	assert(strcmp(R_String_getString(string), "test of awesomeness of awesome") == 0);
	R_Type_Delete(string);
}

void test_int_float_string(void) {
	R_String* string = R_Type_New(R_String);
	R_String_appendInt(string, 494857);
	assert(strcmp("494857", R_String_getString(string)) == 0);
	R_String_appendFloat(string, 3.141592653589793238462643383279502884);
	assert(strcmp("4948573.14159", R_String_getString(string)) == 0);
	assert(R_String_compare(string, "4948573.14159"));
	R_String* appendage = R_Type_New(R_String);
	R_String_setString(appendage, "appendage");
	R_String_appendString(string, appendage);
	assert(strcmp("4948573.14159appendage", R_String_getString(string)) == 0);
	assert(R_String_compare(string, "4948573.14159appendage"));
	R_Type_Delete(appendage);
	R_String_reset(string);
	R_String_appendInt(string, 494857);
	assert(R_String_getInt(string) == 494857);
	R_String_reset(string);
	R_String_appendFloat(string, 3.14f);
	assert(R_String_getFloat(string) == 3.14f);
	R_Type_Delete(string);
}

void test_getSubString(void) {
	R_String* string = R_Type_New(R_String);

	R_String_setString(string, "if (killer) then {hide};");
	R_String* substring = R_Type_New(R_String);
	assert(R_String_getSubstring(string, substring, 4, 6) != NULL);
	assert(strcmp(R_String_getString(R_String_getSubstring(string, substring, 4, 6)), "killer") == 0);
	assert(R_String_length(substring) == 6);
	assert(R_String_compare(string, "if (killer) then {hide};"));
	R_Type_Delete(substring);

	substring = R_Type_New(R_String);
	assert(R_String_moveSubstring(string, substring, 4, 6) != NULL);
	assert(R_String_compare(substring, "killer"));
	assert(R_String_compare(string, "if () then {hide};"));
	R_Type_Delete(substring);

	R_Type_Delete(string);
}

void test_setHex(void) {
	R_String* string = R_Type_New(R_String);
	R_ByteArray* testArray = R_Type_New(R_ByteArray);
	R_ByteArray_appendBytes(testArray, 0x01, 0x23, 0x5A, 0xFF);
	R_String_appendArrayAsHex(string, testArray);
	assert(R_String_length(string) == 8);
	assert(strcmp(R_String_getString(string), "01235AFF") == 0);
	R_Type_Delete(testArray);
	R_Type_Delete(string);
}

void test_is_same(void) {
	R_String* stringA = R_Type_New(R_String);
	R_String* stringB = R_Type_New(R_String);
	R_String_setString(stringA, "different");
	R_String_setString(stringB, "sizes");
	assert(R_String_isSame(stringA, stringB) == false);
	assert(R_String_isSame(stringB, stringA) == false);
	assert(R_String_compare(stringA, "different") == true);
	assert(R_String_compare(stringA, "diff") == false);
	R_String_setString(stringA, "same");
	R_String_setString(stringB, "size");
	assert(R_String_isSame(stringA, stringB) == false);
	assert(R_String_isSame(stringB, stringA) == false);
	R_String_setString(stringA, "substring");
	R_String_setString(stringB, "substrin");
	assert(R_String_isSame(stringA, stringB) == false);
	assert(R_String_isSame(stringB, stringA) == false);
	R_String_setString(stringA, "actually equal");
	R_String_setString(stringB, "actually equal");
	assert(R_String_isSame(stringA, stringB) == true);
	assert(R_String_isSame(stringB, stringA) == true);
	R_Type_Delete(stringA);
	R_Type_Delete(stringB);
}

void test_json_formatting(void) {
	R_String* test1 = R_String_appendCString(R_Type_New(R_String), "no controls");
	R_String* test2 = R_String_appendCString(R_Type_New(R_String), "all controls: \" \\ / \b \f \n \r \t");

	R_String* string = R_Type_New(R_String);
	assert(R_String_appendStringAsJson(string, test1) == string);
	assert(R_String_compare(string, "\"no controls\""));

	R_String_reset(string);
	assert(R_String_appendStringAsJson(string, test2) == string);
	assert(R_String_compare(string, "\"all controls: \\\" \\\\ \\/ \\b \\f \\n \\r \\t\""));
	
	R_Type_Delete(string);
	R_Type_Delete(test1);
	R_Type_Delete(test2);
}

void test_shift(void) {
	R_String* string = R_String_appendCString(R_Type_New(R_String), "012345");
	assert(R_String_length(string) == 6);
	assert(R_String_first(string) == '0');
	assert(R_String_shift(string) == '0');
	assert(R_String_length(string) == 5);
	assert(R_String_first(string) == '1');
	assert(R_String_shift(string) == '1');
	assert(R_String_length(string) == 4);
	assert(R_String_first(string) == '2');
	assert(R_String_shift(string) == '2');
	assert(R_String_length(string) == 3);
	assert(R_String_last(string) == '5');
	assert(R_String_pop(string)  == '5');
	assert(R_String_length(string) == 2);
	assert(R_String_last(string) == '4');
	assert(R_String_pop(string)  == '4');
	assert(R_String_length(string) == 1);
	assert(R_String_push(string, '6') == string);
	assert(R_String_length(string) == 2);
	assert(R_String_last(string) == '6');
	R_Type_Delete(string);
}

void test_trim(void) {
	R_String* string = R_String_appendCString(R_Type_New(R_String), " \t hello world\r\n ");
	assert(R_String_trim(string) == string);
	assert(R_String_compare(string, "hello world"));
	R_Type_Delete(string);
}

void test_split(void) {
	R_String* string = R_String_appendCString(R_Type_New(R_String), "qweHEREasdHEREzxcHEREHERE");
	assert(R_String_find(string, "HERE") == 3);
	R_List* split = R_Type_New(R_List);
	assert(R_String_split(string, "HERE", split) == split);
	assert(R_List_length(split) == 4);

	assert(R_Type_IsOf(R_List_pointerAtIndex(split, 0), R_String));
	assert(R_String_compare(R_List_pointerAtIndex(split, 0), "qwe"));

	assert(R_Type_IsOf(R_List_pointerAtIndex(split, 1), R_String));
	assert(R_String_compare(R_List_pointerAtIndex(split, 1), "asd"));

	assert(R_Type_IsOf(R_List_pointerAtIndex(split, 2), R_String));
	assert(R_String_compare(R_List_pointerAtIndex(split, 2), "zxc"));

	assert(R_Type_IsOf(R_List_pointerAtIndex(split, 3), R_String));
	assert(R_String_compare(R_List_pointerAtIndex(split, 3), ""));

	R_String* joined = R_Type_New(R_String);
	assert(R_String_join(joined, "!", split) == joined);
	assert(R_String_compare(joined, "qwe!asd!zxc!"));

	R_Type_Delete(joined);
	R_Type_Delete(split);
	R_Type_Delete(string);
}

void test_base64(void) {
	R_String* ascii = R_String_setString(R_Type_New(R_String), "20bb4f55f37cb435ea793ecb1012083a:1d4c592428c12368fdbfbbc6e35887ae");
	R_String* base64 = R_Type_New(R_String);
	assert(R_String_appendArrayAsBase64(base64, R_String_bytes(ascii)) == base64);
	assert(R_String_compare(base64, "MjBiYjRmNTVmMzdjYjQzNWVhNzkzZWNiMTAxMjA4M2E6MWQ0YzU5MjQyOGMxMjM2OGZkYmZiYmM2ZTM1ODg3YWU="));

	R_String_setString(ascii, "Hello World");
	R_String_reset(base64);
	assert(R_String_appendArrayAsBase64(base64, R_String_bytes(ascii)) == base64);
	assert(R_String_compare(base64, "SGVsbG8gV29ybGQ="));

	R_Type_Delete(base64);
	R_Type_Delete(ascii);
}

void test_puts(void) {
 	R_String* pass = R_String_setString(R_Type_New(R_String), "Puts Test String");
	R_Puts(pass);
	R_Type_Delete(pass);
}

int main(void) {
	assert(R_Type_BytesAllocated == 0);
	test_set_get();
	test_append_bytes();
	test_int_float_string();
	test_getSubString();
	test_setHex();
	test_json_formatting();
	test_shift();
	test_trim();
	test_split();
	test_base64();
	test_puts();

	assert(R_Type_BytesAllocated == 0);
	printf("Pass\n");
}
