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
#include "R_Events.h"
#include "R_String.h"


char* test_simple_key1 = "test_simple key 1";
char* test_simple_key2 = "test_simple key 2";
char* test_simple_key3 = "test_simple key 3";
bool test_simple_hasCalledback_key1 = false;
bool test_simple_hasCalledback_key2 = false;
bool test_simple_hasCalledback_key3 = false;
void test_simple_callback(void* target, const char* event_key, void* payload) {
	if (strcmp(event_key, test_simple_key1) == 0) {
		assert(target == payload);
		assert(R_Type_IsOf(payload, R_Integer));
		assert(R_Integer_get(payload) == 1);
		test_simple_hasCalledback_key1 = true;
	}
	else if (strcmp(event_key, test_simple_key2) == 0) {
		assert(target == payload);
		assert(R_Type_IsOf(payload, R_Integer));
		assert(R_Integer_get(payload) == 2);
		test_simple_hasCalledback_key2 = true;
	}
	else if (strcmp(event_key, test_simple_key3) == 0) {
		assert(target == payload);
		assert(R_Type_IsOf(payload, R_Integer));
		assert(R_Integer_get(payload) == 3);
		test_simple_hasCalledback_key3 = true;
	}
}
void test_simple(void) {
	R_Events* events = R_Type_New(R_Events);
	R_Integer* target_1 = R_Integer_set(R_Type_New(R_Integer), 1);
	R_Integer* target_2 = R_Integer_set(R_Type_New(R_Integer), 2);
	R_Integer* target_3 = R_Integer_set(R_Type_New(R_Integer), 3);

	assert(R_Events_IsRegistered(events, test_simple_key1, target_1, test_simple_callback) == false);
	assert(R_Events_IsRegistered(events, test_simple_key2, target_2, test_simple_callback) == false);
	assert(R_Events_IsRegistered(events, test_simple_key3, target_3, test_simple_callback) == false);
	assert(R_Events_Register(events, test_simple_key1, target_1, test_simple_callback) == events);
	assert(R_Events_IsRegistered(events, test_simple_key1, target_1, test_simple_callback) == true);
	assert(R_Events_Register(events, test_simple_key2, target_2, test_simple_callback) == events);
	assert(R_Events_IsRegistered(events, test_simple_key2, target_2, test_simple_callback) == true);
	assert(R_Events_Register(events, test_simple_key3, target_3, test_simple_callback) == events);
	assert(R_Events_IsRegistered(events, test_simple_key3, target_3, test_simple_callback) == true);

	assert(R_Events_Notify(events, test_simple_key1, target_1) == events);
	assert(test_simple_hasCalledback_key1);
	assert(R_Events_Notify(events, test_simple_key2, target_2) == events);
	assert(test_simple_hasCalledback_key2);
	assert(R_Events_Notify(events, test_simple_key3, target_3) == events);
	assert(test_simple_hasCalledback_key3);


	assert(R_Events_Remove(events, test_simple_key1, target_1, test_simple_callback) == events);
	assert(R_Events_IsRegistered(events, test_simple_key1, target_1, test_simple_callback) == false);
	assert(R_Events_IsRegistered(events, test_simple_key2, target_2, test_simple_callback) == true);
	assert(R_Events_IsRegistered(events, test_simple_key3, target_3, test_simple_callback) == true);

	assert(R_Events_RemoveTarget(events, target_2) == events);
	assert(R_Events_IsRegistered(events, test_simple_key1, target_1, test_simple_callback) == false);
	assert(R_Events_IsRegistered(events, test_simple_key2, target_2, test_simple_callback) == false);
	assert(R_Events_IsRegistered(events, test_simple_key3, target_3, test_simple_callback) == true);


	R_Type_Delete(events);
	R_Type_Delete(target_1);
	R_Type_Delete(target_2);
	R_Type_Delete(target_3);
}

char* test_multiples_key = "mega key";
bool test_multiples_hasCalledback_1 = false;
bool test_multiples_hasCalledback_2 = false;
void test_multiples_callback_1(void* target, const char* event_key, void* payload) {
	assert(target == NULL);
	assert(strcmp(event_key, test_multiples_key) == 0);
	assert(payload == NULL);
	test_multiples_hasCalledback_1 = true;
}
void test_multiples_callback_2(void* target, const char* event_key, void* payload) {
	assert(target == NULL);
	assert(strcmp(event_key, test_multiples_key) == 0);
	assert(payload == NULL);
	test_multiples_hasCalledback_2 = true;
}
void test_mulitples(void) {
	R_Events* events = R_Type_New(R_Events);
	assert(R_Events_Register(events, test_multiples_key, NULL, test_multiples_callback_1) == events);
	assert(R_Events_Register(events, test_multiples_key, NULL, test_multiples_callback_2) == events);

	assert(R_Events_Notify(events, test_multiples_key, NULL) == events);
	assert(test_multiples_hasCalledback_1);
	assert(test_multiples_hasCalledback_2);

	R_Type_Delete(events);
}

int main(void) {
	test_simple();
	test_mulitples();

	assert(R_Type_BytesAllocated == 0);
	printf("Pass\n");
}
