#ifndef R_MutableString_h
#define R_MutableString_h

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
#include "R_List.h"

typedef struct R_MutableString R_MutableString;

#include "R_MutableData.h"

R_Type_Declare(R_MutableString);

/*  R_MutableString_reset
    Empties the string.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_reset(R_MutableString* self);

/*  R_MutableString_getString
    Returns a buffer to a C String. This buffer is not a copy!
 */
const char* R_FUNCTION_ATTRIBUTES R_MutableString_getString(R_MutableString* self);
#define R_MutableString_cstring R_MutableString_getString

void R_FUNCTION_ATTRIBUTES R_MutableString_puts(R_MutableString* self);

/*  R_MutableString_first
    Returns the first character of the string.
 */
char R_FUNCTION_ATTRIBUTES R_MutableString_first(const R_MutableString* self);

/*  R_MutableString_shift
    Removes the first character from the string.
 */
char R_FUNCTION_ATTRIBUTES R_MutableString_shift(R_MutableString* self);

/*  R_MutableString_first
    Returns the last character of the string.
 */
char R_FUNCTION_ATTRIBUTES R_MutableString_last(const R_MutableString* self);

/*  R_MutableString_pop
    Removes the last character from the string.
 */
char R_FUNCTION_ATTRIBUTES R_MutableString_pop(R_MutableString* self);

/*  R_MutableString_character
    Gets the character at the given index or 0 on error.
 */
char R_FUNCTION_ATTRIBUTES R_MutableString_character(R_MutableString* self, size_t index);

/*  R_MutableString_push
    Apends one character to the end of the string.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_push(R_MutableString* self, char character);

/*  R_MutableString_trim
    Removes whitespace from the beginning and end of the string.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_trim(R_MutableString* self);


/*  R_MutableString_appendCString
    Appends the given C String to the end of the string.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendCString(R_MutableString* self, const char* string);

/*  R_MutableString_appendString
    Appends the given string.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendString(R_MutableString* self, R_MutableString* string);

/*  R_MutableString_appendBytes
    Appends the given number of characters to the string.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendBytes(R_MutableString* self, const char* bytes, size_t byteCount);

/*  R_MutableString_appendArray
    Appends the given R_MutableData, assuming it's ASCII.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendArray(R_MutableString* self, const R_MutableData* array);

/*  R_MutableString_appendArrayAsHex
    Appends the given array as a hex string. ie [0x2a, 0x50] becomes "2A50"
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendArrayAsHex(R_MutableString* self, const R_MutableData* array);

/*  R_MutableString_setString
    Empties the string then appends the given C String.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_setString(R_MutableString* self, const char* string);

/*  R_MutableString_setSizedString
    Empties the string then appends the given number of bytes of the given C String.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_setSizedString(R_MutableString* self, const char* string, size_t stringLength);

/*  R_MutableString_length
    Returns the number of characters in the string.
 */
size_t R_FUNCTION_ATTRIBUTES R_MutableString_length(const R_MutableString* self);
#define R_MutableString_size R_MutableString_length

/*  R_MutableString_appendInt
    Converts the given integer to a string and appends it.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendInt(R_MutableString* self, int value);

/*  R_MutableString_appendInt
    Converts the string to an integer and returns it.
 */
int R_FUNCTION_ATTRIBUTES R_MutableString_getInt(R_MutableString* self);

/*  R_MutableString_appendFloat 
    Converts the given float to a string and appends it.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendFloat(R_MutableString* self, float value);

/*  R_MutableString_getFloat
    Converts the string to a float and returns it.
 */
float R_FUNCTION_ATTRIBUTES R_MutableString_getFloat(R_MutableString* self);

/*  R_MutableString_getSubstring
    Sets the given output string to the substring in between the given indexes. The characters at the given indexes are included.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_getSubstring(R_MutableString* self, R_MutableString* output, size_t startingIndex, size_t length);

/*  R_MutableString_moveSubstring
    Sets the given output string to the substring in between the given indexes. The characters at the given indexes are included.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_moveSubstring(R_MutableString* self, R_MutableString* output, size_t startingIndex, size_t length);

/*  R_MutableString_isEmpty
    Returns true if there are no characters in the string.
 */
bool R_FUNCTION_ATTRIBUTES R_MutableString_isEmpty(R_MutableString* self);

/*  R_MutableString_isSame
    Returns true if the strings are equal length and have identical contents.
 */
bool R_FUNCTION_ATTRIBUTES R_MutableString_isSame(R_MutableString* self, R_MutableString* comparor);

/*  R_MutableString_compare
    Returns true if the output of strcmp would be 0
 */
bool R_FUNCTION_ATTRIBUTES R_MutableString_compare(const R_MutableString* self, const char* comparor);

/*  R_MutableString_appendStringAsJson
    Formats string as a quoted JSON-formatted string value and appends it to self.
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendStringAsJson(R_MutableString* self, R_MutableString* string);

/*  R_MutableString_bytes
    The raw bytes in the string. This array is not null-terminated!
 */
const R_FUNCTION_ATTRIBUTES R_MutableData* R_MutableString_bytes(R_MutableString* self);

/*  R_MutableString_bytes
    Returns the first index of the given substring or -1.
 */
int R_FUNCTION_ATTRIBUTES R_MutableString_find(R_MutableString* self, const char* substring);

/*  R_MutableString_split
    Splits the string into lines (seperated by the given seperator) and adds each line as a string to the output list.
 */
R_List* R_FUNCTION_ATTRIBUTES R_MutableString_split(R_MutableString* self, const char* seperator, R_List* output);

/* R_MutableString_join
   Appends the strings in the given list to self, seperated by the seperator
 */
R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_join(R_MutableString* self, const char* seperator, R_List* input);

R_MutableString* R_FUNCTION_ATTRIBUTES R_MutableString_appendArrayAsBase64(R_MutableString* self, const R_MutableData* array);


#endif /* R_MutableString_h */
