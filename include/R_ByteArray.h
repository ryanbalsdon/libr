#ifndef R_ByteArray_h
#define R_ByteArray_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdint.h>
#include "R_Type.h"

typedef struct R_ByteArray R_ByteArray;

#include "R_String.h"

R_Type_Declare(R_ByteArray);

R_ByteArray* R_ByteArray_reset(R_ByteArray* self);

R_ByteArray* R_ByteArray_appendByte(R_ByteArray* self, uint8_t byte);
R_ByteArray* R_ByteArray_appendCArray(R_ByteArray* self, const uint8_t* bytes, size_t count);
R_ByteArray* R_ByteArray_appendArray(R_ByteArray* self, R_ByteArray* array);
R_ByteArray* R_ByteArray_appendHexString(R_ByteArray* self, R_String* hex);

size_t R_ByteArray_size(const R_ByteArray* self);
const uint8_t* R_ByteArray_bytes(const R_ByteArray* self);
uint8_t R_ByteArray_byte(const R_ByteArray* self, size_t index);

void    R_ByteArray_push(R_ByteArray* self, uint8_t byte);
uint8_t R_ByteArray_pop(R_ByteArray* self);
void    R_ByteArray_shift(R_ByteArray* self, uint8_t byte);
uint8_t R_ByteArray_unshift(R_ByteArray* self);

size_t R_ByteArray_moveSubArray(R_ByteArray* self, R_ByteArray* array, size_t start, size_t end);

#define R_ByteArray_appendBytes(self, ...) R_ByteArray_appendCArray(self, (uint8_t[]){__VA_ARGS__}, (sizeof((uint8_t[]){__VA_ARGS__})/sizeof(uint8_t)));

int R_ByteArray_compare(R_ByteArray* self, R_ByteArray* comparor);

#endif /* R_ByteArray_h */
