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

/*  R_ByteArray_reset
    Empties the array but does not free any memory.
*/
R_ByteArray* R_ByteArray_reset(R_ByteArray* self);

/*  R_ByteArray_setByte
    Empties the array and appends a single byte.
*/
R_ByteArray* R_ByteArray_setByte(R_ByteArray* self, uint8_t byte);

/*  R_ByteArray_setCArray
    Empties the array and appends the given bytes. The bytes are copied.
*/
R_ByteArray* R_ByteArray_setCArray(R_ByteArray* self, const uint8_t* bytes, size_t count);

/*  R_ByteArray_setArray
    Empties the array and appends the contents of the given array. The array's bytes are copied.
*/
R_ByteArray* R_ByteArray_setArray(R_ByteArray* self, const R_ByteArray* array);

/*  R_ByteArray_setHexString
    Empties the array and appends the given string, after converting the string from hex to binary. ie. "0F01" is [15, 1].
*/
R_ByteArray* R_ByteArray_setHexString(R_ByteArray* self, const R_String* hex);

/*  R_ByteArray_setHexCString
    Empties the array and appends the given string, after converting the string from hex to binary. ie. "0F01" is [15, 1].
*/
R_ByteArray* R_ByteArray_setHexCString(R_ByteArray* self, const char* hex);

/*  R_ByteArray_setBytes
    Empties the array and appends the given list of bytes.
*/
#define R_ByteArray_setBytes(self, ...) R_ByteArray_setCArray(self, (uint8_t[]){__VA_ARGS__}, (sizeof((uint8_t[]){__VA_ARGS__})/sizeof(uint8_t)));

/*  R_ByteArray_setUInt32
    Empties the array and appends the given integer as 4 bytes, network order.
*/
R_ByteArray* R_ByteArray_setUInt32(R_ByteArray* self, uint32_t value);

/*  R_ByteArray_appendByte
    Appends the given byte to the end of the array, allocating more memory if required.
*/
R_ByteArray* R_ByteArray_appendByte(R_ByteArray* self, uint8_t byte);

/*  R_ByteArray_appendCArray
    Appends the given array to the end of the array, allocating more memory if required. The bytes are copied.
*/
R_ByteArray* R_ByteArray_appendCArray(R_ByteArray* self, const uint8_t* bytes, size_t count);

/*  R_ByteArray_appendArray
    Appends the given array to the end of the array, allocating more memory if required. The bytes are copied.
*/
R_ByteArray* R_ByteArray_appendArray(R_ByteArray* self, const R_ByteArray* array);

/*  R_ByteArray_appendHexString
    Appends the given string to the end of the array, after converting the string from hex to binary. ie. "0F01" is [15, 1]. More memory is allocated if required.
*/
R_ByteArray* R_ByteArray_appendHexString(R_ByteArray* self, const R_String* hex);

/*  R_ByteArray_appendHexCString
    Appends the given string to the end of the array, after converting the string from hex to binary. ie. "0F01" is [15, 1]. More memory is allocated if required.
*/
R_ByteArray* R_ByteArray_appendHexCString(R_ByteArray* self, const char* hex);

/*  R_ByteArray_appendBytes
    Appends the given list of bytes to the end of the array, allocating more memory if required.
*/
#define R_ByteArray_appendBytes(self, ...) R_ByteArray_appendCArray(self, (uint8_t[]){__VA_ARGS__}, (sizeof((uint8_t[]){__VA_ARGS__})/sizeof(uint8_t)));

/*  R_ByteArray_setUInt32
    Appends the given integer as 4 bytes, network order.
*/
R_ByteArray* R_ByteArray_appendUInt32(R_ByteArray* self, uint32_t value);
R_ByteArray* R_ByteArray_appendUInt32AsBCD(R_ByteArray* self, uint32_t value);

/*  R_ByteArray_size
    Returns the number of bytes currently in the array, not the total amount of allocated memory.
 */
size_t R_ByteArray_size(const R_ByteArray* self);

/*  R_ByteArray_bytes
    Returns a C Array pointer to the byte array. This is not a copy!
 */
const uint8_t* R_ByteArray_bytes(const R_ByteArray* self);

/*  R_ByteArray_byte
    Returns the byte at the given index.
 */
uint8_t R_ByteArray_byte(const R_ByteArray* self, size_t index);

/*  R_ByteArray_first
    Returns the first byte in the array or zero if the array is empty.
 */
uint8_t R_ByteArray_first(const R_ByteArray* self);

/*  R_ByteArray_last
    Returns the last byte in the array or zero if the array is empty.
 */
uint8_t R_ByteArray_last(const R_ByteArray* self);

/*  R_ByteArray_push
    Appends the given byte to the end of the array.
 */
void    R_ByteArray_push(R_ByteArray* self, uint8_t byte);

/*  R_ByteArray_pop
    Removes the last byte from the array and returns it.
 */
uint8_t R_ByteArray_pop(R_ByteArray* self);

/*  R_ByteArray_shift
    Adds the given byte to the beginning of the array.
 */
void    R_ByteArray_shift(R_ByteArray* self, uint8_t byte);

/*  R_ByteArray_push
    Removes the first byte from the array and returns it.
 */
uint8_t R_ByteArray_unshift(R_ByteArray* self);

/*  R_ByteArray_moveSubArray
    Removes the given number of bytes from the second param and appends them to the first.
 */
size_t R_ByteArray_moveSubArray(R_ByteArray* self, R_ByteArray* array, size_t start, size_t end);

/*  R_ByteArray_compare
    Returns 0 if both arrays are the same length and have identical contents.
 */
int R_ByteArray_compare(R_ByteArray* self, const R_ByteArray* comparor);

#endif /* R_ByteArray_h */
