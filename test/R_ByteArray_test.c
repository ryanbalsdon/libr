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
#include "R_ByteArray.h"
#include "R_String.h"

void test_allocation(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	R_Type_Delete(array);
	assert(R_Type_BytesAllocated == 0);
}

void test_appendByte(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	assert(R_ByteArray_size(array) == 0);
	R_ByteArray_appendByte(array, 0xA5);
	assert(R_ByteArray_size(array) == 1);
	assert(*R_ByteArray_bytes(array) == 0xA5);
	R_ByteArray_appendByte(array, 0xFF);
	assert(R_ByteArray_size(array) == 2);
	assert(R_ByteArray_bytes(array)[1] == 0xFF);
	assert(R_ByteArray_first(array) == 0xA5);
	assert(R_ByteArray_last(array) == 0xFF);
	R_Type_Delete(array);
}

void test_appendCArray(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	assert(R_ByteArray_size(array) == 0);
	uint8_t bytes[] = {0x00, 0x01, 0x02, 0x03};
	R_ByteArray_appendCArray(array, bytes, sizeof(bytes));
	assert(R_ByteArray_size(array) == sizeof(bytes));
	assert(memcmp(bytes, R_ByteArray_bytes(array), sizeof(bytes)) == 0);
	R_ByteArray_appendByte(array, 0xA5);
	assert(R_ByteArray_size(array) == sizeof(bytes)+1);
	assert(R_ByteArray_bytes(array)[sizeof(bytes)] == 0xA5);
	R_Type_Delete(array);
}

void test_appendArray(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	R_ByteArray* array2 = R_Type_New(R_ByteArray);
	uint8_t bytes[] = {0x00, 0x01, 0x02, 0x03};
	assert(R_ByteArray_size(array) == 0);
	assert(R_ByteArray_size(array2) == 0);
	R_ByteArray_appendCArray(array, bytes, sizeof(bytes));
	assert(R_ByteArray_size(array) == sizeof(bytes));
	R_ByteArray_appendArray(array2, array);
	assert(R_ByteArray_size(array2) == sizeof(bytes));
	assert(memcmp(bytes, R_ByteArray_bytes(array2), sizeof(bytes)) == 0);
	R_Type_Delete(array);
	R_Type_Delete(array2);
}

void test_push_pop(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	R_ByteArray_appendByte(array, 0x01);
	R_ByteArray_appendByte(array, 0x02);
	assert(R_ByteArray_size(array) == 2);
	R_ByteArray_push(array, 0x03);
	assert(R_ByteArray_size(array) == 3);
	assert(R_ByteArray_bytes(array)[0] == 0x01);
	assert(R_ByteArray_bytes(array)[1] == 0x02);
	assert(R_ByteArray_bytes(array)[2] == 0x03);
	R_ByteArray_unshift(array, 0x00);
	assert(R_ByteArray_size(array) == 4);
	assert(R_ByteArray_bytes(array)[0] == 0x00);
	assert(R_ByteArray_bytes(array)[1] == 0x01);
	assert(R_ByteArray_bytes(array)[2] == 0x02);
	assert(R_ByteArray_bytes(array)[3] == 0x03);
	assert(R_ByteArray_byte(array, 0) == 0x00);
	assert(R_ByteArray_byte(array, 1) == 0x01);
	assert(R_ByteArray_byte(array, 2) == 0x02);
	assert(R_ByteArray_byte(array, 3) == 0x03);
	assert(R_ByteArray_pop(array) == 0x03);
	assert(R_ByteArray_size(array) == 3);
	assert(R_ByteArray_shift(array) == 0x00);
	assert(R_ByteArray_size(array) == 2);
	assert(R_ByteArray_pop(array) == 0x02);
	assert(R_ByteArray_size(array) == 1);
	assert(R_ByteArray_shift(array) == 0x01);
	assert(R_ByteArray_size(array) == 0);
	R_Type_Delete(array);
}

void test_MoveSubArray(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	R_ByteArray* array2 = R_Type_New(R_ByteArray);
	uint8_t bytes[] = {0x00, 0x01, 0x02, 0x03};
	R_ByteArray_appendCArray(array, bytes, sizeof(bytes));
	assert(R_ByteArray_size(array)  == 4);
	assert(R_ByteArray_size(array2) == 0);
	assert(R_ByteArray_moveSubArray(array2, array, 0, 4) == array2);
	assert(R_ByteArray_size(array)  == 0);
	assert(R_ByteArray_size(array2) == 4);
	assert(R_ByteArray_bytes(array2)[0] == 0x00);
	assert(R_ByteArray_bytes(array2)[1] == 0x01);
	assert(R_ByteArray_bytes(array2)[2] == 0x02);
	assert(R_ByteArray_bytes(array2)[3] == 0x03);
	R_ByteArray_appendCArray(array, bytes, sizeof(bytes));
	assert(R_ByteArray_moveSubArray(array2, array, 1, 2) == array2);
	assert(R_ByteArray_size(array2) == 6);
	assert(R_ByteArray_bytes(array2)[0] == 0x00);
	assert(R_ByteArray_bytes(array2)[1] == 0x01);
	assert(R_ByteArray_bytes(array2)[2] == 0x02);
	assert(R_ByteArray_bytes(array2)[3] == 0x03);
	assert(R_ByteArray_bytes(array2)[4] == 0x01);
	assert(R_ByteArray_bytes(array2)[5] == 0x02);
	assert(R_ByteArray_size(array) == 2);
	assert(R_ByteArray_bytes(array)[0] == 0x00);
	assert(R_ByteArray_bytes(array)[1] == 0x03);
	R_Type_Delete(array);
	R_Type_Delete(array2);
}

void test_read_byte(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	R_ByteArray_appendBytes(array, 0x10, 0x20, 0x30);
	assert(R_ByteArray_size(array) == 3);
	assert(R_ByteArray_byte(array,0) == 0x10);
	assert(R_ByteArray_byte(array,1) == 0x20);
	assert(R_ByteArray_byte(array,2) == 0x30);
	R_Type_Delete(array);
}

void test_compare(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	R_ByteArray* array2 = R_Type_New(R_ByteArray);
	R_ByteArray_appendBytes(array,  0x10, 0x20, 0x30);
	R_ByteArray_appendBytes(array2, 0x10, 0x20, 0x30);
	assert(R_ByteArray_compare(array, array2) == 0);
	assert(R_Equals(array, array2) == true);
	R_ByteArray_reset(array);
	R_ByteArray_reset(array2);
	R_ByteArray_appendBytes(array,  0x10, 0x20);
	R_ByteArray_appendBytes(array2, 0x10, 0x20, 0x30);
	assert(R_ByteArray_compare(array, array2) != 0);
	assert(R_Equals(array, array2) == false);
	R_ByteArray_reset(array);
	R_ByteArray_reset(array2);
	R_ByteArray_appendBytes(array,  0x10, 0x20, 0x30, 0x40);
	R_ByteArray_appendBytes(array2, 0x10, 0x20, 0x30);
	assert(R_ByteArray_compare(array, array2) != 0);
	assert(R_Equals(array, array2) == false);
	R_ByteArray_reset(array);
	R_ByteArray_reset(array2);
	R_ByteArray_appendBytes(array,  0x10, 0x20, 0x31, 0x40);
	R_ByteArray_appendBytes(array2, 0x10, 0x20, 0x30, 0x40);
	assert(R_ByteArray_compare(array, array2) != 0);
	assert(R_Equals(array, array2) == false);
	R_Type_Delete(array);
	R_Type_Delete(array2);
}

void test_appendHexString(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	R_ByteArray* array2 = R_Type_New(R_ByteArray);

	R_ByteArray_appendBytes(array,  0x10, 0xFA, 0xB5, 0x09);
	R_String* string = R_Type_New(R_String);
	R_String_appendCString(string, "10fAB509");
	R_ByteArray_appendHexString(array2, string);
	assert(R_ByteArray_size(array2) == 4);
	assert(R_ByteArray_compare(array, array2) == 0);
	R_ByteArray_setHexString(array2, string);
	assert(R_ByteArray_size(array2) == 4);
	assert(R_ByteArray_compare(array, array2) == 0);
	R_Type_Delete(string);

	R_ByteArray_reset(array2);
	R_ByteArray_appendHexCString(array2, "10fAB509");
	assert(R_ByteArray_size(array2) == 4);
	assert(R_ByteArray_compare(array, array2) == 0);
	R_ByteArray_setHexCString(array2, "10fAB509");
	assert(R_ByteArray_size(array2) == 4);
	assert(R_ByteArray_compare(array, array2) == 0);

	R_ByteArray_setHexCString(array2, "c");
	assert(R_ByteArray_size(array2) == 1);
	assert(R_ByteArray_first(array2) == 0x0C);
	assert(R_ByteArray_getUInt32(array2) == 0x0C);

	R_ByteArray_setHexCString(array2, "aec");
	assert(R_ByteArray_size(array2) == 2);
	assert(R_ByteArray_byte(array2, 0) == 0x0A);
	assert(R_ByteArray_byte(array2, 1) == 0xEC);
	assert(R_ByteArray_getUInt32(array2) == 0x0AEC);

	R_Type_Delete(array);
	R_Type_Delete(array2);
}

void test_copy(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	uint8_t bytes[] = {0x00, 0x01, 0x02, 0x03};
	R_ByteArray_appendCArray(array, bytes, sizeof(bytes));
	R_ByteArray* array2 = R_Type_Copy(array);
	assert(array2 != NULL);
	assert(R_ByteArray_size(array2) == sizeof(bytes));
	assert(memcmp(bytes, R_ByteArray_bytes(array2), sizeof(bytes)) == 0);
	R_Type_Delete(array);
	R_Type_Delete(array2);
}

void test_append_integers(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	R_ByteArray* comparor = R_ByteArray_appendBytes(R_Type_New(R_ByteArray), 0x01);
	assert(R_ByteArray_appendUInt32(array, 0x01) == array);
	assert(R_ByteArray_compare(array, comparor) == 0);
	assert(R_ByteArray_getUInt32(array) == 0x01);

	R_ByteArray_setBytes(comparor, 0x20, 0x01);
	assert(R_ByteArray_setUInt32(array, 0x2001) == array);
	assert(R_ByteArray_compare(array, comparor) == 0);
	assert(R_ByteArray_getUInt32(array) == 0x2001);

	R_ByteArray_setBytes(comparor, 0x20, 0x00, 0x01);
	assert(R_ByteArray_setUInt32(array, 0x200001) == array);
	assert(R_ByteArray_compare(array, comparor) == 0);
	assert(R_ByteArray_getUInt32(array) == 0x200001);

	R_ByteArray_setBytes(comparor, 0xFF, 0x20, 0x00, 0x01);
	assert(R_ByteArray_setUInt32(array, 0xFF200001) == array);
	assert(R_ByteArray_compare(array, comparor) == 0);
	assert(R_ByteArray_getUInt32(array) == 0xFF200001);

	R_Type_Delete(array);
	R_Type_Delete(comparor);
}

void test_bcd(void) {
	R_ByteArray* array = R_Type_New(R_ByteArray);
	assert(R_ByteArray_appendUInt32AsBCD(array, 0) == array);
	assert(R_ByteArray_size(array) == 1);
	assert(R_ByteArray_byte(array,0) == 0x00);

	R_ByteArray_reset(array);
	assert(R_ByteArray_appendUInt32AsBCD(array, 5) == array);
	assert(R_ByteArray_size(array) == 1);
	assert(R_ByteArray_byte(array,0) == 0x05);

	R_ByteArray_reset(array);
	assert(R_ByteArray_appendUInt32AsBCD(array, 54) == array);
	assert(R_ByteArray_size(array) == 1);
	assert(R_ByteArray_byte(array,0) == 0x54);

	R_ByteArray_reset(array);
	assert(R_ByteArray_appendUInt32AsBCD(array, 1234) == array);
	assert(R_ByteArray_size(array) == 2);
	assert(R_ByteArray_byte(array,0) == 0x12);
	assert(R_ByteArray_byte(array,1) == 0x34);

	R_ByteArray_reset(array);
	assert(R_ByteArray_appendUInt32AsBCD(array, 1234567) == array);
	assert(R_ByteArray_size(array) == 4);
	assert(R_ByteArray_byte(array,0) == 0x01);
	assert(R_ByteArray_byte(array,1) == 0x23);
	assert(R_ByteArray_byte(array,2) == 0x45);
	assert(R_ByteArray_byte(array,3) == 0x67);

	R_Type_Delete(array);
}

void test_puts(void) {
	R_ByteArray* test = R_ByteArray_setBytes(R_Type_New(R_ByteArray), 0x01, 0x20, 0x02, 0x01);
	R_Puts(test);
	R_Type_Delete(test);
}

int main(void) {
	assert(R_Type_BytesAllocated == 0);

	test_allocation();
	test_appendByte();
	test_appendCArray();
	test_appendArray();
	test_push_pop();
	test_MoveSubArray();
	test_read_byte();
	test_compare();
	test_appendHexString();
	test_copy();
	test_append_integers();
	test_bcd();
	test_puts();

	assert(R_Type_BytesAllocated == 0);
	printf("Pass\n");
}
