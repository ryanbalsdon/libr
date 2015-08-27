#ifndef R_MutableData_h
#define R_MutableData_h

#include <stdint.h>
#include "R_Type.h"

typedef struct R_MutableData R_MutableData;

#include "R_MutableString.h"

R_Type_Declare(R_MutableData);

/*  R_MutableData_reset
    Empties the array but does not free any memory.
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_reset(R_MutableData* self);

size_t R_FUNCTION_ATTRIBUTES R_MutableData_stringify(R_MutableData* self, char* buffer, size_t size);

/*  R_MutableData_setByte
    Empties the array and appends a single byte.
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setByte(R_MutableData* self, uint8_t byte);

/*  R_MutableData_setCArray
    Empties the array and appends the given bytes. The bytes are copied.
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setCArray(R_MutableData* self, const uint8_t* bytes, size_t count);

/*  R_MutableData_setArray
    Empties the array and appends the contents of the given array. The array's bytes are copied.
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setArray(R_MutableData* self, const R_MutableData* array);

/*  R_MutableData_setHexString
    Empties the array and appends the given string, after converting the string from hex to binary. ie. "0F01" is [15, 1].
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setHexString(R_MutableData* self, R_MutableString* hex);

/*  R_MutableData_setHexCString
    Empties the array and appends the given string, after converting the string from hex to binary. ie. "0F01" is [15, 1].
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setHexCString(R_MutableData* self, const char* hex);

/*  R_MutableData_setBytes
    Empties the array and appends the given list of bytes.
*/
#define R_MutableData_setBytes(self, ...) R_MutableData_setCArray(self, (uint8_t[]){__VA_ARGS__}, (sizeof((uint8_t[]){__VA_ARGS__})/sizeof(uint8_t)));

/*  R_MutableData_setUInt32
    Empties the array and appends the given integer as 4 bytes, network order.
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_setUInt32(R_MutableData* self, uint32_t value);

/*  R_MutableData_getUInt32
    Returns the array as an integer, assuming network byte order. Returns 0 on error.
*/
uint32_t R_FUNCTION_ATTRIBUTES R_MutableData_getUInt32(R_MutableData* self);

/*  R_MutableData_appendByte
    Appends the given byte to the end of the array, allocating more memory if required.
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendByte(R_MutableData* self, uint8_t byte);

/*  R_MutableData_appendCArray
    Appends the given array to the end of the array, allocating more memory if required. The bytes are copied.
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendCArray(R_MutableData* self, const uint8_t* bytes, size_t count);

/*  R_MutableData_appendArray
    Appends the given array to the end of the array, allocating more memory if required. The bytes are copied.
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendArray(R_MutableData* self, const R_MutableData* array);

/*  R_MutableData_appendHexString
    Appends the given string to the end of the array, after converting the string from hex to binary. ie. "0F01" is [15, 1]. More memory is allocated if required.
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendHexString(R_MutableData* self, R_MutableString* hex);

/*  R_MutableData_appendHexCString
    Appends the given string to the end of the array, after converting the string from hex to binary. ie. "0F01" is [15, 1]. More memory is allocated if required.
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendHexCString(R_MutableData* self, const char* hex);

/*  R_MutableData_appendBytes
    Appends the given list of bytes to the end of the array, allocating more memory if required.
*/
#define R_MutableData_appendBytes(self, ...) R_MutableData_appendCArray(self, (uint8_t[]){__VA_ARGS__}, (sizeof((uint8_t[]){__VA_ARGS__})/sizeof(uint8_t)));

/*  R_MutableData_setUInt32
    Appends the given integer as 4 bytes, network order.
*/
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendUInt32(R_MutableData* self, uint32_t value);
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_appendUInt32AsBCD(R_MutableData* self, uint32_t value);

/*  R_MutableData_size
    Returns the number of bytes currently in the array, not the total amount of allocated memory.
 */
size_t R_FUNCTION_ATTRIBUTES R_MutableData_size(const R_MutableData* self);
#define R_MutableData_length R_MutableData_size

/*  R_MutableData_bytes
    Returns a C Array pointer to the byte array. This is not a copy!
 */
const uint8_t* R_FUNCTION_ATTRIBUTES R_MutableData_bytes(const R_MutableData* self);

/*  R_MutableData_byte
    Returns the byte at the given index.
 */
uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_byte(const R_MutableData* self, size_t index);

/*  R_MutableData_first
    Returns the first byte in the array or zero if the array is empty.
 */
uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_first(const R_MutableData* self);

/*  R_MutableData_last
    Returns the last byte in the array or zero if the array is empty.
 */
uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_last(const R_MutableData* self);

/*  R_MutableData_push
    Appends the given byte to the end of the array.
 */
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_push(R_MutableData* self, uint8_t byte);

/*  R_MutableData_pop
    Removes the last byte from the array and returns it.
 */
uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_pop(R_MutableData* self);

/*  R_MutableData_unshift
    Adds the given byte to the beginning of the array.
 */
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_unshift(R_MutableData* self, uint8_t byte);

/*  R_MutableData_shift
    Removes the first byte from the array and returns it.
 */
uint8_t R_FUNCTION_ATTRIBUTES R_MutableData_shift(R_MutableData* self);

/*  R_MutableData_moveSubArray
    Removes the given number of bytes from the second param and appends them to the first.
 */
R_MutableData* R_FUNCTION_ATTRIBUTES R_MutableData_moveSubArray(R_MutableData* self, R_MutableData* array, size_t start, size_t length);

/*  R_MutableData_compare
    Returns 0 if both arrays are the same length and have identical contents.
 */
int R_FUNCTION_ATTRIBUTES R_MutableData_compare(const R_MutableData* self, const R_MutableData* comparor);

/*  R_MutableData_isSame
    Returns true if the arrays are equal length and have identical contents.
 */
bool R_FUNCTION_ATTRIBUTES R_MutableData_isSame(R_MutableData* self, R_MutableData* comparor);

/*  R_MutableData_compare
    Returns 0 if both arrays are the same length and have identical contents.
 */
int R_FUNCTION_ATTRIBUTES R_MutableData_compareWithCArray(const R_MutableData* self, const uint8_t* comparor, size_t bytes);

const R_Data* R_FUNCTION_ATTRIBUTES R_MutableData_data(const R_MutableData* self);

#endif /* R_MutableData_h */
