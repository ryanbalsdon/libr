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

int main(void) {
	R_ByteArray* array = R_Object_New(R_ByteArray_Type);
	R_Object_Delete(array);
	assert(R_Object_BytesAllocated == 0);

	array = R_Object_New(R_ByteArray_Type);
	assert(R_ByteArray_size(array) == 0);
	R_ByteArray_appendByte(array, 0xA5);
	assert(R_ByteArray_size(array) == 1);
	assert(*R_ByteArray_bytes(array) == 0xA5);
	R_ByteArray_appendByte(array, 0xFF);
	assert(R_ByteArray_size(array) == 2);
	assert(R_ByteArray_bytes(array)[1] == 0xFF);

	R_ByteArray_reset(array);
	assert(R_ByteArray_size(array) == 0);
	uint8_t bytes[] = {0x00, 0x01, 0x02, 0x03};
	R_ByteArray_appendCArray(array, bytes, sizeof(bytes));
	assert(R_ByteArray_size(array) == sizeof(bytes));
	assert(memcmp(bytes, R_ByteArray_bytes(array), sizeof(bytes)) == 0);
	R_ByteArray_appendByte(array, 0xA5);
	assert(R_ByteArray_size(array) == sizeof(bytes)+1);
	assert(R_ByteArray_bytes(array)[sizeof(bytes)] == 0xA5);

	array = R_ByteArray_reset(array);
	R_ByteArray* array2 = R_Object_New(R_ByteArray_Type);
	assert(R_ByteArray_size(array) == 0);
	assert(R_ByteArray_size(array2) == 0);
	R_ByteArray_appendCArray(array, bytes, sizeof(bytes));
	assert(R_ByteArray_size(array) == sizeof(bytes));
	R_ByteArray_appendArray(array2, array);
	assert(R_ByteArray_size(array2) == sizeof(bytes));
	assert(memcmp(bytes, R_ByteArray_bytes(array2), sizeof(bytes)) == 0);

	array = R_ByteArray_reset(array);
	R_ByteArray_appendByte(array, 0x01);
	R_ByteArray_appendByte(array, 0x02);
	assert(R_ByteArray_size(array) == 2);
	R_ByteArray_push(array, 0x03);
	assert(R_ByteArray_size(array) == 3);
	assert(R_ByteArray_bytes(array)[0] == 0x01);
	assert(R_ByteArray_bytes(array)[1] == 0x02);
	assert(R_ByteArray_bytes(array)[2] == 0x03);
	R_ByteArray_shift(array, 0x00);
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
	assert(R_ByteArray_unshift(array) == 0x00);
	assert(R_ByteArray_size(array) == 2);
	assert(R_ByteArray_pop(array) == 0x02);
	assert(R_ByteArray_size(array) == 1);
	assert(R_ByteArray_unshift(array) == 0x01);
	assert(R_ByteArray_size(array) == 0);

	R_ByteArray_reset(array);
	R_ByteArray_reset(array2);
	R_ByteArray_appendCArray(array, bytes, sizeof(bytes));
	assert(R_ByteArray_size(array)  == 4);
	assert(R_ByteArray_size(array2) == 0);
	assert(R_ByteArray_moveSubArray(array2, array, 0, 4) == 4);
	assert(R_ByteArray_size(array)  == 0);
	assert(R_ByteArray_size(array2) == 4);
	assert(R_ByteArray_bytes(array2)[0] == 0x00);
	assert(R_ByteArray_bytes(array2)[1] == 0x01);
	assert(R_ByteArray_bytes(array2)[2] == 0x02);
	assert(R_ByteArray_bytes(array2)[3] == 0x03);
	R_ByteArray_appendCArray(array, bytes, sizeof(bytes));
	assert(R_ByteArray_moveSubArray(array2, array, 1, 3) == 2);
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

	R_ByteArray_reset(array);
	R_ByteArray_appendBytes(array, 0x10, 0x20, 0x30);
	assert(R_ByteArray_size(array) == 3);
	assert(R_ByteArray_byte(array,0) == 0x10);
	assert(R_ByteArray_byte(array,1) == 0x20);
	assert(R_ByteArray_byte(array,2) == 0x30);

	R_ByteArray_reset(array);
	R_ByteArray_reset(array2);
	R_ByteArray_appendBytes(array,  0x10, 0x20, 0x30);
	R_ByteArray_appendBytes(array2, 0x10, 0x20, 0x30);
	assert(R_ByteArray_compare(array, array2) == 0);
	R_ByteArray_reset(array);
	R_ByteArray_reset(array2);
	R_ByteArray_appendBytes(array,  0x10, 0x20);
	R_ByteArray_appendBytes(array2, 0x10, 0x20, 0x30);
	assert(R_ByteArray_compare(array, array2) != 0);
	R_ByteArray_reset(array);
	R_ByteArray_reset(array2);
	R_ByteArray_appendBytes(array,  0x10, 0x20, 0x30, 0x40);
	R_ByteArray_appendBytes(array2, 0x10, 0x20, 0x30);
	assert(R_ByteArray_compare(array, array2) != 0);
	R_ByteArray_reset(array);
	R_ByteArray_reset(array2);
	R_ByteArray_appendBytes(array,  0x10, 0x20, 0x31, 0x40);
	R_ByteArray_appendBytes(array2, 0x10, 0x20, 0x30, 0x40);
	assert(R_ByteArray_compare(array, array2) != 0);

	R_ByteArray_reset(array);
	R_ByteArray_reset(array2);
	R_ByteArray_appendBytes(array,  0x10, 0xFA, 0xB5, 0x09);
	R_String* string = R_Object_New(R_String_Type);
	R_String_appendCString(string, "10fAB509");
	R_ByteArray_appendHexString(array2, string);
	assert(R_ByteArray_size(array2) == 4);
	assert(R_ByteArray_compare(array, array2) == 0);
	R_Object_Delete(string);

	R_ByteArray_reset(array);
	R_Object_Delete(array2);
	R_ByteArray_appendCArray(array, bytes, sizeof(bytes));
	array2 = R_Object_Copy(array);
	assert(array2 != NULL);
	assert(R_ByteArray_size(array2) == sizeof(bytes));
	assert(memcmp(bytes, R_ByteArray_bytes(array2), sizeof(bytes)) == 0);

	R_Object_Delete(array);
	R_Object_Delete(array2);
	assert(R_Object_BytesAllocated == 0);
	printf("Pass\n");
}