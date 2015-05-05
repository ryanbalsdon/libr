#ifndef R_KeyValuePair_h
#define R_KeyValuePair_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdbool.h>
#include "R_Type.h"
#include "R_String.h"

typedef struct R_KeyValuePair R_KeyValuePair;
R_Type_Declare(R_KeyValuePair);

R_String* R_FUNCTION_ATTRIBUTES R_KeyValuePair_key(R_KeyValuePair* pair);
void* R_FUNCTION_ATTRIBUTES R_KeyValuePair_value(R_KeyValuePair* pair);
R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_setKey(R_KeyValuePair* pair, const char* key);
R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_setValue(R_KeyValuePair* pair, void* value);

#endif /* R_KeyValuePair_h */
