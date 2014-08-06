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

typedef struct R_String R_String;

R_String* R_String_alloc(void);
R_String* R_String_reset(R_String* self);
R_String* R_String_free(R_String* self);

const char* R_String_getString(R_String* self);
R_String* R_String_appendCString(R_String* self, const char* string);
R_String* R_String_appendString(R_String* self, R_String* string);
R_String* R_String_setString(R_String* self, const char* string);
R_String* R_String_setSizedString(R_String* self, const char* string, size_t stringLength);


R_String* R_String_appendInt(R_String* self, int value);
int R_String_getInt(R_String* self);
R_String* R_String_appendFloat(R_String* self, float value);
float R_String_getFloat(R_String* self);

R_String* R_String_getSubstring(R_String* self, size_t startingIndex, size_t endingIndex, R_String* output);
R_String* R_String_getBracedString(R_String* self, char beginningBrace, char finishingBrace, R_String* output);
R_String* R_String_getEnclosedString(R_String* self, char beginningBrace, char finishingBrace, R_String* output);
char R_String_findFirstToken(R_String* self, char* tokens);

bool R_String_splitBracedString(R_String* self, char beginningBrace, char finishingBrace, 
	R_String* beforeBraces, R_String* withBraces, R_String* insideBraces, R_String* afterBraces);

#endif /* R_String_h */
