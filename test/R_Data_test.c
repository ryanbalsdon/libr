#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "R_MutableData.h"
#include "R_MutableString.h"

void test_allocation(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	R_Type_Delete(array);
	assert(R_Type_BytesAllocated == 0);
}

void test_appendByte(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	assert(R_MutableData_size(array) == 0);
	R_MutableData_appendByte(array, 0xA5);
	assert(R_MutableData_size(array) == 1);
	assert(*R_MutableData_bytes(array) == 0xA5);
	R_MutableData_appendByte(array, 0xFF);
	assert(R_MutableData_size(array) == 2);
	assert(R_MutableData_bytes(array)[1] == 0xFF);
	assert(R_MutableData_first(array) == 0xA5);
	assert(R_MutableData_last(array) == 0xFF);
	R_Type_Delete(array);
}

void test_appendCArray(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	assert(R_MutableData_size(array) == 0);
	uint8_t bytes[] = {0x00, 0x01, 0x02, 0x03};
	R_MutableData_appendCArray(array, bytes, sizeof(bytes));
	assert(R_MutableData_size(array) == sizeof(bytes));
	assert(os_memcmp(bytes, R_MutableData_bytes(array), sizeof(bytes)) == 0);
	R_MutableData_appendByte(array, 0xA5);
	assert(R_MutableData_size(array) == sizeof(bytes)+1);
	assert(R_MutableData_bytes(array)[sizeof(bytes)] == 0xA5);
	R_Type_Delete(array);
}

void test_appendArray(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	R_MutableData* array2 = R_Type_New(R_MutableData);
	uint8_t bytes[] = {0x00, 0x01, 0x02, 0x03};
	assert(R_MutableData_size(array) == 0);
	assert(R_MutableData_size(array2) == 0);
	R_MutableData_appendCArray(array, bytes, sizeof(bytes));
	assert(R_MutableData_size(array) == sizeof(bytes));
	R_MutableData_appendArray(array2, array);
	assert(R_MutableData_size(array2) == sizeof(bytes));
	assert(os_memcmp(bytes, R_MutableData_bytes(array2), sizeof(bytes)) == 0);
	R_Type_Delete(array);
	R_Type_Delete(array2);
}

void test_push_pop(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	R_MutableData_appendByte(array, 0x01);
	R_MutableData_appendByte(array, 0x02);
	assert(R_MutableData_size(array) == 2);
	R_MutableData_push(array, 0x03);
	assert(R_MutableData_size(array) == 3);
	assert(R_MutableData_bytes(array)[0] == 0x01);
	assert(R_MutableData_bytes(array)[1] == 0x02);
	assert(R_MutableData_bytes(array)[2] == 0x03);
	R_MutableData_unshift(array, 0x00);
	assert(R_MutableData_size(array) == 4);
	assert(R_MutableData_bytes(array)[0] == 0x00);
	assert(R_MutableData_bytes(array)[1] == 0x01);
	assert(R_MutableData_bytes(array)[2] == 0x02);
	assert(R_MutableData_bytes(array)[3] == 0x03);
	assert(R_MutableData_byte(array, 0) == 0x00);
	assert(R_MutableData_byte(array, 1) == 0x01);
	assert(R_MutableData_byte(array, 2) == 0x02);
	assert(R_MutableData_byte(array, 3) == 0x03);
	assert(R_MutableData_pop(array) == 0x03);
	assert(R_MutableData_size(array) == 3);
	assert(R_MutableData_shift(array) == 0x00);
	assert(R_MutableData_size(array) == 2);
	assert(R_MutableData_pop(array) == 0x02);
	assert(R_MutableData_size(array) == 1);
	assert(R_MutableData_shift(array) == 0x01);
	assert(R_MutableData_size(array) == 0);
	R_Type_Delete(array);
}

void test_MoveSubArray(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	R_MutableData* array2 = R_Type_New(R_MutableData);
	uint8_t bytes[] = {0x00, 0x01, 0x02, 0x03};
	R_MutableData_appendCArray(array, bytes, sizeof(bytes));
	assert(R_MutableData_size(array)  == 4);
	assert(R_MutableData_size(array2) == 0);
	assert(R_MutableData_moveSubArray(array2, array, 0, 4) == array2);
	assert(R_MutableData_size(array)  == 0);
	assert(R_MutableData_size(array2) == 4);
	assert(R_MutableData_bytes(array2)[0] == 0x00);
	assert(R_MutableData_bytes(array2)[1] == 0x01);
	assert(R_MutableData_bytes(array2)[2] == 0x02);
	assert(R_MutableData_bytes(array2)[3] == 0x03);
	R_MutableData_appendCArray(array, bytes, sizeof(bytes));
	assert(R_MutableData_moveSubArray(array2, array, 1, 2) == array2);
	assert(R_MutableData_size(array2) == 6);
	assert(R_MutableData_bytes(array2)[0] == 0x00);
	assert(R_MutableData_bytes(array2)[1] == 0x01);
	assert(R_MutableData_bytes(array2)[2] == 0x02);
	assert(R_MutableData_bytes(array2)[3] == 0x03);
	assert(R_MutableData_bytes(array2)[4] == 0x01);
	assert(R_MutableData_bytes(array2)[5] == 0x02);
	assert(R_MutableData_size(array) == 2);
	assert(R_MutableData_bytes(array)[0] == 0x00);
	assert(R_MutableData_bytes(array)[1] == 0x03);
	R_Type_Delete(array);
	R_Type_Delete(array2);
}

void test_read_byte(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	R_MutableData_appendBytes(array, 0x10, 0x20, 0x30);
	assert(R_MutableData_size(array) == 3);
	assert(R_MutableData_byte(array,0) == 0x10);
	assert(R_MutableData_byte(array,1) == 0x20);
	assert(R_MutableData_byte(array,2) == 0x30);
	R_Type_Delete(array);
}

void test_compare(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	R_MutableData* array2 = R_Type_New(R_MutableData);
	R_MutableData_appendBytes(array,  0x10, 0x20, 0x30);
	R_MutableData_appendBytes(array2, 0x10, 0x20, 0x30);
	assert(R_MutableData_compare(array, array2) == 0);
	assert(R_Equals(array, array2) == true);
	R_MutableData_reset(array);
	R_MutableData_reset(array2);
	R_MutableData_appendBytes(array,  0x10, 0x20);
	R_MutableData_appendBytes(array2, 0x10, 0x20, 0x30);
	assert(R_MutableData_compare(array, array2) != 0);
	assert(R_Equals(array, array2) == false);
	R_MutableData_reset(array);
	R_MutableData_reset(array2);
	R_MutableData_appendBytes(array,  0x10, 0x20, 0x30, 0x40);
	R_MutableData_appendBytes(array2, 0x10, 0x20, 0x30);
	assert(R_MutableData_compare(array, array2) != 0);
	assert(R_Equals(array, array2) == false);
	R_MutableData_reset(array);
	R_MutableData_reset(array2);
	R_MutableData_appendBytes(array,  0x10, 0x20, 0x31, 0x40);
	R_MutableData_appendBytes(array2, 0x10, 0x20, 0x30, 0x40);
	assert(R_MutableData_compare(array, array2) != 0);
	assert(R_Equals(array, array2) == false);
	R_Type_Delete(array);
	R_Type_Delete(array2);
}

void test_appendHexString(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	R_MutableData* array2 = R_Type_New(R_MutableData);

	R_MutableData_appendBytes(array,  0x10, 0xFA, 0xB5, 0x09);
	R_MutableString* string = R_Type_New(R_MutableString);
	R_MutableString_appendCString(string, "10fAB509");
	R_MutableData_appendHexString(array2, string);
	assert(R_MutableData_size(array2) == 4);
	assert(R_MutableData_compare(array, array2) == 0);
	R_MutableData_setHexString(array2, string);
	assert(R_MutableData_size(array2) == 4);
	assert(R_MutableData_compare(array, array2) == 0);
	R_Type_Delete(string);

	R_MutableData_reset(array2);
	R_MutableData_appendHexCString(array2, "10fAB509");
	assert(R_MutableData_size(array2) == 4);
	assert(R_MutableData_compare(array, array2) == 0);
	R_MutableData_setHexCString(array2, "10fAB509");
	assert(R_MutableData_size(array2) == 4);
	assert(R_MutableData_compare(array, array2) == 0);

	R_MutableData_setHexCString(array2, "c");
	assert(R_MutableData_size(array2) == 1);
	assert(R_MutableData_first(array2) == 0x0C);
	assert(R_MutableData_getUInt32(array2) == 0x0C);

	R_MutableData_setHexCString(array2, "aec");
	assert(R_MutableData_size(array2) == 2);
	assert(R_MutableData_byte(array2, 0) == 0x0A);
	assert(R_MutableData_byte(array2, 1) == 0xEC);
	assert(R_MutableData_getUInt32(array2) == 0x0AEC);

	R_Type_Delete(array);
	R_Type_Delete(array2);
}

void test_copy(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	uint8_t bytes[] = {0x00, 0x01, 0x02, 0x03};
	R_MutableData_appendCArray(array, bytes, sizeof(bytes));
	R_MutableData* array2 = R_Type_Copy(array);
	assert(array2 != NULL);
	assert(R_MutableData_size(array2) == sizeof(bytes));
	assert(os_memcmp(bytes, R_MutableData_bytes(array2), sizeof(bytes)) == 0);
	R_Type_Delete(array);
	R_Type_Delete(array2);
}

void test_append_integers(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	R_MutableData* comparor = R_MutableData_appendBytes(R_Type_New(R_MutableData), 0x01);
	assert(R_MutableData_appendUInt32(array, 0x01) == array);
	assert(R_MutableData_compare(array, comparor) == 0);
	assert(R_MutableData_getUInt32(array) == 0x01);

	R_MutableData_setBytes(comparor, 0x20, 0x01);
	assert(R_MutableData_setUInt32(array, 0x2001) == array);
	assert(R_MutableData_compare(array, comparor) == 0);
	assert(R_MutableData_getUInt32(array) == 0x2001);

	R_MutableData_setBytes(comparor, 0x20, 0x00, 0x01);
	assert(R_MutableData_setUInt32(array, 0x200001) == array);
	assert(R_MutableData_compare(array, comparor) == 0);
	assert(R_MutableData_getUInt32(array) == 0x200001);

	R_MutableData_setBytes(comparor, 0xFF, 0x20, 0x00, 0x01);
	assert(R_MutableData_setUInt32(array, 0xFF200001) == array);
	assert(R_MutableData_compare(array, comparor) == 0);
	assert(R_MutableData_getUInt32(array) == 0xFF200001);

	R_Type_Delete(array);
	R_Type_Delete(comparor);
}

void test_bcd(void) {
	R_MutableData* array = R_Type_New(R_MutableData);
	assert(R_MutableData_appendUInt32AsBCD(array, 0) == array);
	assert(R_MutableData_size(array) == 1);
	assert(R_MutableData_byte(array,0) == 0x00);

	R_MutableData_reset(array);
	assert(R_MutableData_appendUInt32AsBCD(array, 5) == array);
	assert(R_MutableData_size(array) == 1);
	assert(R_MutableData_byte(array,0) == 0x05);

	R_MutableData_reset(array);
	assert(R_MutableData_appendUInt32AsBCD(array, 54) == array);
	assert(R_MutableData_size(array) == 1);
	assert(R_MutableData_byte(array,0) == 0x54);

	R_MutableData_reset(array);
	assert(R_MutableData_appendUInt32AsBCD(array, 1234) == array);
	assert(R_MutableData_size(array) == 2);
	assert(R_MutableData_byte(array,0) == 0x12);
	assert(R_MutableData_byte(array,1) == 0x34);

	R_MutableData_reset(array);
	assert(R_MutableData_appendUInt32AsBCD(array, 1234567) == array);
	assert(R_MutableData_size(array) == 4);
	assert(R_MutableData_byte(array,0) == 0x01);
	assert(R_MutableData_byte(array,1) == 0x23);
	assert(R_MutableData_byte(array,2) == 0x45);
	assert(R_MutableData_byte(array,3) == 0x67);

	R_Type_Delete(array);
}

void test_puts(void) {
	R_MutableData* test = R_MutableData_setBytes(R_Type_New(R_MutableData), 0x01, 0x20, 0x02, 0x01);
	char buffer[25];
	assert(R_Stringify(test, buffer, 25) == 24);
	assert(strcmp(buffer, "{0x01, 0x20, 0x02, 0x01}") == 0);
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
