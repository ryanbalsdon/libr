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
#include "R_MutableString.h"


typedef struct R_Dictionary R_Dictionary;

R_Dictionary* R_Dictionary_alloc(void);
R_Dictionary* R_Dictionary_reset(R_Dictionary* self);
R_Dictionary* R_Dictionary_free(R_Dictionary* self);

bool R_Dictionary_doesKeyExist(R_Dictionary* self, const char* key);

bool R_Dictionary_setString(R_Dictionary* self, const char* key, const char* data);
const char* R_Dictionary_getString(R_Dictionary* self, const char* key);
bool R_Dictionary_setInt(R_Dictionary* self, const char* key, int data);
int R_Dictionary_getInt(R_Dictionary* self, const char* key);
bool R_Dictionary_setFloat(R_Dictionary* self, const char* key, float data);
float R_Dictionary_getFloat(R_Dictionary* self, const char* key);

R_Dictionary* R_Dictionary_setObject(R_Dictionary* self, const char* key);
R_Dictionary* R_Dictionary_getObject(R_Dictionary* self, const char* key);

R_Dictionary* R_Dictionary_addToArray(R_Dictionary* self, const char* key);
int R_Dictionary_getArraySize(R_Dictionary* self, const char* key);
R_Dictionary* R_Dictionary_getArrayIndex(R_Dictionary* self, const char* key, int index);

R_MutableString* R_Dictionary_serialize(R_Dictionary* self, R_MutableString* stringToAppendTo);
R_Dictionary* R_MutableString_objectize(R_Dictionary* self, R_MutableString* string);

#endif /* R_Dictionary_h */
