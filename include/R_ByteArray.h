#ifndef R_ByteArray_h
#define R_ByteArray_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

typedef struct R_ByteArray R_ByteArray;

R_ByteArray* R_ByteArray_alloc(void);
R_ByteArray* R_ByteArray_reset(R_ByteArray* self);
R_ByteArray* R_ByteArray_free(R_ByteArray* self);

R_ByteArray* R_ByteArray_appendByte(R_ByteArray* self, uint8_t byte);
R_ByteArray* R_ByteArray_appendCArray(R_ByteArray* self, const uint8_t* bytes, size_t count);
R_ByteArray* R_ByteArray_appendArray(R_ByteArray* self, R_ByteArray* array);

size_t R_ByteArray_size(R_ByteArray* self);
const uint8_t* R_ByteArray_bytes(R_ByteArray* self);

void    R_ByteArray_push(R_ByteArray* self, uint8_t byte);
uint8_t R_ByteArray_pop(R_ByteArray* self);
void    R_ByteArray_shift(R_ByteArray* self, uint8_t byte);
uint8_t R_ByteArray_unshift(R_ByteArray* self);

size_t R_ByteArray_appendSubArray(R_ByteArray* self, R_ByteArray* array, size_t start, size_t end);

#endif /* R_ByteArray_h */