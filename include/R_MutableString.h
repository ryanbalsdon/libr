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


typedef struct R_MutableString R_MutableString;

R_MutableString* R_MutableString_alloc(void);
R_MutableString* R_MutableString_reset(R_MutableString* self);
R_MutableString* R_MutableString_free(R_MutableString* self);

const char* R_MutableString_getString(R_MutableString* self);
R_MutableString* R_MutableString_appendString(R_MutableString* self, const char* string);
R_MutableString* R_MutableString_appendMutableString(R_MutableString* self, R_MutableString* string);
R_MutableString* R_MutableString_setString(R_MutableString* self, const char* string);
R_MutableString* R_MutableString_setSizedString(R_MutableString* self, const char* string, int stringLength);


R_MutableString* R_MutableString_appendInt(R_MutableString* self, int value);
int R_MutableString_getInt(R_MutableString* self);
R_MutableString* R_MutableString_appendFloat(R_MutableString* self, float value);
float R_MutableString_getFloat(R_MutableString* self);

R_MutableString* R_MutableString_getSubstring(R_MutableString* self, int startingIndex, int endingIndex, R_MutableString* output);
R_MutableString* R_MutableString_getBracedString(R_MutableString* self, char beginningBrace, char finishingBrace, R_MutableString* output);
R_MutableString* R_MutableString_getEnclosedString(R_MutableString* self, char beginningBrace, char finishingBrace, R_MutableString* output);
char R_MutableString_findFirstToken(R_MutableString* self, char* tokens);

bool R_MutableString_splitBracedString(R_MutableString* self, char beginningBrace, char finishingBrace, 
	R_MutableString* beforeBraces, R_MutableString* withBraces, R_MutableString* insideBraces, R_MutableString* afterBraces);

#endif /* R_MutableString_h */
