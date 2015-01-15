#ifndef R_Dictionary_h
#define R_Dictionary_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdbool.h>
#include "R_String.h"


typedef struct R_Dictionary R_Dictionary;
R_Type_Declare(R_Dictionary);

/*  R_Dictionary_doesKeyExist
    Returns true if the dictionary contains the given key. This only searches the top level keys of the dictionary.
 */
bool R_Dictionary_doesKeyExist(R_Dictionary* self, const char* key);

/*  R_Dictionary_setString
    Sets the value of the given key to the given string. Returns true if successful.
 */
bool R_Dictionary_setString(R_Dictionary* self, const char* key, const char* data);

/*  R_Dictionary_getString
    Returns the value of the given key or NULL if it doesn't exist. Returns NULL if the key is an array or dictionary.
 */
const char* R_Dictionary_getString(R_Dictionary* self, const char* key);

/*  R_Dictionary_setInt
    Converts the given integer to a string and sets the value of the given key to it. Returns true if successful.
 */
bool R_Dictionary_setInt(R_Dictionary* self, const char* key, int data);

/*  R_Dictionary_getInt
    If the key exists and is not an array or dictionary, the value is converted to an integer and returned.
 */
int R_Dictionary_getInt(R_Dictionary* self, const char* key);

/*  R_Dictionary_setFloat
    Converts the given float to a string and sets the value of the given key to it. Returns true if successful.
 */
bool R_Dictionary_setFloat(R_Dictionary* self, const char* key, float data);

/*  R_Dictionary_getFloat
    If the key exists and is not an array or dictionary, the value is converted to a float and returned.
 */
float R_Dictionary_getFloat(R_Dictionary* self, const char* key);

/*  R_Dictionary_setObject
    Creates a new Dictionary assigned to the given key and returns it.
 */
R_Dictionary* R_Dictionary_setObject(R_Dictionary* self, const char* key);

/*  R_Dictionary_getObject
    If the key exists and is a dictionary, it is returned. Otherwise, NULL is returned.
 */
R_Dictionary* R_Dictionary_getObject(R_Dictionary* self, const char* key);

/*  R_Dictionary_addToArray
    Appends a new dictionary to the array at the given key and returns it. Creates the array if it doesn't exist. Returns NULL on error.
 */
R_Dictionary* R_Dictionary_addToArray(R_Dictionary* self, const char* key);

/*  R_Dictionary_getArraySize
    Returns the size of the array at the given key, if it exists. Otherwise, zero.
 */
int R_Dictionary_getArraySize(R_Dictionary* self, const char* key);

/*  R_Dictionary_getArrayIndex
    Returns the dictionary at the given index of the array at the given key.
 */
R_Dictionary* R_Dictionary_getArrayIndex(R_Dictionary* self, const char* key, unsigned int index);

/*  R_Dictionary_serialize
    Converts the given dictionary into a string and appends it to the given R_String instance.
 */
R_String* R_Dictionary_serialize(R_Dictionary* self, R_String* stringToAppendTo);

/*  R_String_objectize
    Converts the given string 
 */
R_Dictionary* R_String_objectize(R_Dictionary* self, R_String* string);

#endif /* R_Dictionary_h */
