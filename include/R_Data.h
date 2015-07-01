#ifndef R_Data_h
#define R_Data_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdint.h>
#include "R_Type.h"

typedef struct R_Data R_Data;

#include "R_MutableString.h"

R_Type_Declare(R_Data);

/*  R_Data_reset
    Empties the array but does not free any memory.
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_reset(R_Data* self);

void R_FUNCTION_ATTRIBUTES R_Data_puts(R_Data* self);

/*  R_Data_setByte
    Empties the array and appends a single byte.
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_setByte(R_Data* self, uint8_t byte);

/*  R_Data_setCArray
    Empties the array and appends the given bytes. The bytes are copied.
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_setCArray(R_Data* self, const uint8_t* bytes, size_t count);

/*  R_Data_setArray
    Empties the array and appends the contents of the given array. The array's bytes are copied.
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_setArray(R_Data* self, const R_Data* array);

/*  R_Data_setHexString
    Empties the array and appends the given string, after converting the string from hex to binary. ie. "0F01" is [15, 1].
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_setHexString(R_Data* self, R_MutableString* hex);

/*  R_Data_setHexCString
    Empties the array and appends the given string, after converting the string from hex to binary. ie. "0F01" is [15, 1].
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_setHexCString(R_Data* self, const char* hex);

/*  R_Data_setBytes
    Empties the array and appends the given list of bytes.
*/
#define R_Data_setBytes(self, ...) R_Data_setCArray(self, (uint8_t[]){__VA_ARGS__}, (sizeof((uint8_t[]){__VA_ARGS__})/sizeof(uint8_t)));

/*  R_Data_setUInt32
    Empties the array and appends the given integer as 4 bytes, network order.
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_setUInt32(R_Data* self, uint32_t value);

/*  R_Data_getUInt32
    Returns the array as an integer, assuming network byte order. Returns 0 on error.
*/
uint32_t R_FUNCTION_ATTRIBUTES R_Data_getUInt32(R_Data* self);

/*  R_Data_appendByte
    Appends the given byte to the end of the array, allocating more memory if required.
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendByte(R_Data* self, uint8_t byte);

/*  R_Data_appendCArray
    Appends the given array to the end of the array, allocating more memory if required. The bytes are copied.
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendCArray(R_Data* self, const uint8_t* bytes, size_t count);

/*  R_Data_appendArray
    Appends the given array to the end of the array, allocating more memory if required. The bytes are copied.
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendArray(R_Data* self, const R_Data* array);

/*  R_Data_appendHexString
    Appends the given string to the end of the array, after converting the string from hex to binary. ie. "0F01" is [15, 1]. More memory is allocated if required.
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendHexString(R_Data* self, R_MutableString* hex);

/*  R_Data_appendHexCString
    Appends the given string to the end of the array, after converting the string from hex to binary. ie. "0F01" is [15, 1]. More memory is allocated if required.
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendHexCString(R_Data* self, const char* hex);

/*  R_Data_appendBytes
    Appends the given list of bytes to the end of the array, allocating more memory if required.
*/
#define R_Data_appendBytes(self, ...) R_Data_appendCArray(self, (uint8_t[]){__VA_ARGS__}, (sizeof((uint8_t[]){__VA_ARGS__})/sizeof(uint8_t)));

/*  R_Data_setUInt32
    Appends the given integer as 4 bytes, network order.
*/
R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendUInt32(R_Data* self, uint32_t value);
R_Data* R_FUNCTION_ATTRIBUTES R_Data_appendUInt32AsBCD(R_Data* self, uint32_t value);

/*  R_Data_size
    Returns the number of bytes currently in the array, not the total amount of allocated memory.
 */
size_t R_FUNCTION_ATTRIBUTES R_Data_size(const R_Data* self);
#define R_Data_length R_Data_size

/*  R_Data_bytes
    Returns a C Array pointer to the byte array. This is not a copy!
 */
const uint8_t* R_FUNCTION_ATTRIBUTES R_Data_bytes(const R_Data* self);

/*  R_Data_byte
    Returns the byte at the given index.
 */
uint8_t R_FUNCTION_ATTRIBUTES R_Data_byte(const R_Data* self, size_t index);

/*  R_Data_first
    Returns the first byte in the array or zero if the array is empty.
 */
uint8_t R_FUNCTION_ATTRIBUTES R_Data_first(const R_Data* self);

/*  R_Data_last
    Returns the last byte in the array or zero if the array is empty.
 */
uint8_t R_FUNCTION_ATTRIBUTES R_Data_last(const R_Data* self);

/*  R_Data_push
    Appends the given byte to the end of the array.
 */
R_Data* R_FUNCTION_ATTRIBUTES R_Data_push(R_Data* self, uint8_t byte);

/*  R_Data_pop
    Removes the last byte from the array and returns it.
 */
uint8_t R_FUNCTION_ATTRIBUTES R_Data_pop(R_Data* self);

/*  R_Data_unshift
    Adds the given byte to the beginning of the array.
 */
R_Data* R_FUNCTION_ATTRIBUTES R_Data_unshift(R_Data* self, uint8_t byte);

/*  R_Data_shift
    Removes the first byte from the array and returns it.
 */
uint8_t R_FUNCTION_ATTRIBUTES R_Data_shift(R_Data* self);

/*  R_Data_moveSubArray
    Removes the given number of bytes from the second param and appends them to the first.
 */
R_Data* R_FUNCTION_ATTRIBUTES R_Data_moveSubArray(R_Data* self, R_Data* array, size_t start, size_t length);

/*  R_Data_compare
    Returns 0 if both arrays are the same length and have identical contents.
 */
int R_FUNCTION_ATTRIBUTES R_Data_compare(const R_Data* self, const R_Data* comparor);

/*  R_Data_isSame
    Returns true if the arrays are equal length and have identical contents.
 */
bool R_FUNCTION_ATTRIBUTES R_Data_isSame(R_Data* self, R_Data* comparor);

/*  R_Data_compare
    Returns 0 if both arrays are the same length and have identical contents.
 */
int R_FUNCTION_ATTRIBUTES R_Data_compareWithCArray(const R_Data* self, const uint8_t* comparor, size_t bytes);


#endif /* R_Data_h */
