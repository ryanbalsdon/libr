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

int main(void) {
	R_ByteArray* array = NULL;
	array = R_ByteArray_alloc();
	array = R_ByteArray_free(array);
	assert(array == NULL);

	array = R_ByteArray_alloc();
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
	R_ByteArray* array2 = R_ByteArray_alloc();
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
	assert(R_ByteArray_pop(array) == 0x03);
	assert(R_ByteArray_size(array) == 3);
	assert(R_ByteArray_unshift(array) == 0x00);
	assert(R_ByteArray_size(array) == 2);
	assert(R_ByteArray_pop(array) == 0x02);
	assert(R_ByteArray_size(array) == 1);
	assert(R_ByteArray_unshift(array) == 0x01);
	assert(R_ByteArray_size(array) == 0);


	array = R_ByteArray_free(array);
	array2 = R_ByteArray_free(array2);
	printf("Pass\n");
}