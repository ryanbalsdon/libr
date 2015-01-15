#ifndef R_String_h
#define R_String_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdbool.h>
#include <stddef.h>
#include "R_Type.h"

typedef struct R_String R_String;

#include "R_ByteArray.h"

R_Type_Declare(R_String);

/*  R_String_reset
    Empties the string.
 */
R_String* R_String_reset(R_String* self);

/*  R_String_getString
    Returns a buffer to a C String. This buffer is not a copy!
 */
const char* R_String_getString(const R_String* self);

/*  R_String_appendCString
    Appends the given C String to the end of the string.
 */
R_String* R_String_appendCString(R_String* self, const char* string);

/*  R_String_appendString
    Appends the given string.
 */
R_String* R_String_appendString(R_String* self, R_String* string);

/*  R_String_appendBytes
    Appends the given number of characters to the string.
 */
R_String* R_String_appendBytes(R_String* self, const char* bytes, size_t byteCount);

/*  R_String_appendArray
    Appends the given R_ByteArray, assuming it's ASCII.
 */
R_String* R_String_appendArray(R_String* self, const R_ByteArray* array);

/*  R_String_appendArrayAsHex
    Appends the given array as a hex string. ie [0x2a, 0x50] becomes "2A50"
 */
R_String* R_String_appendArrayAsHex(R_String* self, const R_ByteArray* array);

/*  R_String_setString
    Empties the string then appends the given C String.
 */
R_String* R_String_setString(R_String* self, const char* string);

/*  R_String_setSizedString
    Empties the string then appends the given number of bytes of the given C String.
 */
R_String* R_String_setSizedString(R_String* self, const char* string, size_t stringLength);

/*  R_String_length
    Returns the number of characters in the string.
 */
size_t R_String_length(const R_String* self);

/*  R_String_appendInt
    Converts the given integer to a string and appends it.
 */
R_String* R_String_appendInt(R_String* self, int value);

/*  R_String_appendInt
    Converts the string to an integer and returns it.
 */
int R_String_getInt(R_String* self);

/*  R_String_appendFloat 
    Converts the given float to a string and appends it.
 */
R_String* R_String_appendFloat(R_String* self, float value);

/*  R_String_getFloat
    Converts the string to a float and returns it.
 */
float R_String_getFloat(R_String* self);

/*  R_String_getSubstring
    Sets the given output string to the substring in between the given indexes. The characters at the given indexes are included.
 */
R_String* R_String_getSubstring(R_String* self, size_t startingIndex, size_t endingIndex, R_String* output);

/*  R_String_getBracedString
    Sets the given output string to the substring in between the given characters, matching scopes.
 */
R_String* R_String_getBracedString(R_String* self, char beginningBrace, char finishingBrace, R_String* output);

/*  R_String_getEnclosedString
    Sets the given output string to the substring in between the given characters, matching scopes.
 */
R_String* R_String_getEnclosedString(R_String* self, char beginningBrace, char finishingBrace, R_String* output);

/*  R_String_findFirstToken
    Returns the first character found of the given list.
 */
char R_String_findFirstToken(R_String* self, char* tokens);

/*  R_String_splitBracedString
    Splits a string.
 */
bool R_String_splitBracedString(R_String* self, char beginningBrace, char finishingBrace, 
    R_String* beforeBraces, R_String* withBraces, R_String* insideBraces, R_String* afterBraces);

/*  R_String_isEmpty
    Returns true if there are no characters in the string.
 */
bool R_String_isEmpty(R_String* self);

/*  R_String_isSame
    Returns true if the strings are equal length and have identical contents.
 */
bool R_String_isSame(R_String* self, R_String* comparor);

#endif /* R_String_h */
