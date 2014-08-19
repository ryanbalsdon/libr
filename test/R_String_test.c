/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "R_String.h"



int main(void) {
	R_String* string = R_String_alloc();
	string = R_String_free(string);
	assert(string == NULL);

	string = R_String_alloc();
	assert(R_String_setString(string, "test") == string);
	assert(strcmp(R_String_getString(string), "test") == 0);

	R_String_appendCString(string, " of awesomeness");
	assert(strcmp(R_String_getString(string), "test") != 0);
	assert(strcmp(R_String_getString(string), "test of awesomeness") == 0);
	R_String_appendBytes(string, " of awesomeness", 11);
	assert(strcmp(R_String_getString(string), "test of awesomeness of awesome") == 0);

	R_String_setString(string, "test 2");
	assert(strcmp(R_String_getString(string), "test 2") == 0);

	R_String_reset(string);
	R_String_appendInt(string, 494857);
	assert(strcmp("494857", R_String_getString(string)) == 0);
	R_String_appendFloat(string, 3.141592653589793238462643383279502884);
	assert(strcmp("4948573.14159", R_String_getString(string)) == 0);

	R_String* appendage = R_String_alloc();
	R_String_setString(appendage, "appendage");
	R_String_appendString(string, appendage);
	assert(strcmp("4948573.14159appendage", R_String_getString(string)) == 0);

	R_String_reset(string);
	R_String_appendInt(string, 494857);
	assert(R_String_getInt(string) == 494857);
	R_String_reset(string);
	R_String_appendFloat(string, 3.14f);
	assert(R_String_getFloat(string) == 3.14f);

	R_String_setString(string, "if (killer) then {hide};");
	R_String* substring = R_String_alloc();
	assert(R_String_getSubstring(string, 4, 9, substring) != NULL);
	assert(strcmp(R_String_getString(R_String_getSubstring(string, 4, 9, substring)), "killer") == 0);
	assert(R_String_getBracedString(string, '{', '}', substring) != NULL);
	assert(strcmp(R_String_getString(R_String_getBracedString(string, '{', '}', substring)), "{hide}") == 0);

	R_String_setString(string, "hi{r{t}y{fg{h}vb}}qwe");
	assert(R_String_getBracedString(string, '{', '}', substring) != NULL);
	assert(strcmp(R_String_getString(R_String_getBracedString(string, '{', '}', substring)), "{r{t}y{fg{h}vb}}") == 0);
	assert(R_String_getEnclosedString(string, '{', '}', substring) != NULL);
	assert(strcmp(R_String_getString(R_String_getEnclosedString(string, '{', '}', substring)), "r{t}y{fg{h}vb}") == 0);
	R_String* beforeBraces = R_String_alloc();
	R_String* withBraces = R_String_alloc();
	R_String* insideBraces = R_String_alloc();
	R_String* afterBraces = R_String_alloc();
	assert(R_String_splitBracedString(string, '{', '}', beforeBraces, withBraces, insideBraces, afterBraces) == true);
	assert(strcmp(R_String_getString(beforeBraces), "hi") == 0);
	assert(strcmp(R_String_getString(withBraces), "{r{t}y{fg{h}vb}}") == 0);
	assert(strcmp(R_String_getString(insideBraces), "r{t}y{fg{h}vb}") == 0);
	assert(strcmp(R_String_getString(afterBraces), "qwe") == 0);


	R_String_setString(string, "command;not");
	assert(R_String_getBracedString(string, '\0', ';', substring) != NULL);
	assert(strcmp(R_String_getString(R_String_getBracedString(string, 0, ';', substring)), "command;") == 0);
	assert(R_String_getBracedString(string, ';', '\0', substring) != NULL);
	assert(strcmp(R_String_getString(R_String_getBracedString(string, ';', 0, substring)), ";not") == 0);

	R_String_setString(string, "a%%sdf#ht");
	assert(R_String_findFirstToken(string, "%%fh") == '%');
	assert(R_String_findFirstToken(string, "#fh") == 'f');
	assert(R_String_findFirstToken(string, "s#d") == 's');

	assert(R_String_setSizedString(string, "0123456789", 4) != NULL);
	assert(strcmp(R_String_getString(string), "0123") == 0);

	R_String_setString(string, "0123456789");
	R_String_setString(string, R_String_getString(string)+2);
	assert(strcmp(R_String_getString(string), "23456789") == 0);

	assert(R_String_isEmpty(string) == false);
	R_String_reset(string);
	assert(R_String_isEmpty(string) == true);


	printf("Pass\n");
}