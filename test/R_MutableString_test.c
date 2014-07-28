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
#include "R_MutableString.h"



int main(void) {
	R_MutableString* string = R_MutableString_alloc();
	string = R_MutableString_free(string);
	assert(string == NULL);

	string = R_MutableString_alloc();
	assert(R_MutableString_setString(string, "test") == string);
	assert(strcmp(R_MutableString_getString(string), "test") == 0);

	R_MutableString_appendString(string, " of awesomeness");
	assert(strcmp(R_MutableString_getString(string), "test") != 0);
	assert(strcmp(R_MutableString_getString(string), "test of awesomeness") == 0);

	R_MutableString_setString(string, "test 2");
	assert(strcmp(R_MutableString_getString(string), "test 2") == 0);

	R_MutableString_reset(string);
	R_MutableString_appendInt(string, 494857);
	assert(strcmp("494857", R_MutableString_getString(string)) == 0);
	R_MutableString_appendFloat(string, 3.141592653589793238462643383279502884);
	assert(strcmp("4948573.14159", R_MutableString_getString(string)) == 0);

	R_MutableString* appendage = R_MutableString_alloc();
	R_MutableString_setString(appendage, "appendage");
	R_MutableString_appendMutableString(string, appendage);
	assert(strcmp("4948573.14159appendage", R_MutableString_getString(string)) == 0);

	R_MutableString_reset(string);
	R_MutableString_appendInt(string, 494857);
	assert(R_MutableString_getInt(string) == 494857);
	R_MutableString_reset(string);
	R_MutableString_appendFloat(string, 3.14f);
	assert(R_MutableString_getFloat(string) == 3.14f);

	R_MutableString_setString(string, "if (killer) then {hide};");
	R_MutableString* substring = R_MutableString_alloc();
	assert(R_MutableString_getSubstring(string, 4, 9, substring) != NULL);
	assert(strcmp(R_MutableString_getString(R_MutableString_getSubstring(string, 4, 9, substring)), "killer") == 0);
	assert(R_MutableString_getBracedString(string, '{', '}', substring) != NULL);
	assert(strcmp(R_MutableString_getString(R_MutableString_getBracedString(string, '{', '}', substring)), "{hide}") == 0);

	R_MutableString_setString(string, "hi{r{t}y{fg{h}vb}}qwe");
	assert(R_MutableString_getBracedString(string, '{', '}', substring) != NULL);
	assert(strcmp(R_MutableString_getString(R_MutableString_getBracedString(string, '{', '}', substring)), "{r{t}y{fg{h}vb}}") == 0);
	assert(R_MutableString_getEnclosedString(string, '{', '}', substring) != NULL);
	assert(strcmp(R_MutableString_getString(R_MutableString_getEnclosedString(string, '{', '}', substring)), "r{t}y{fg{h}vb}") == 0);
	R_MutableString* beforeBraces = R_MutableString_alloc();
	R_MutableString* withBraces = R_MutableString_alloc();
	R_MutableString* insideBraces = R_MutableString_alloc();
	R_MutableString* afterBraces = R_MutableString_alloc();
	assert(R_MutableString_splitBracedString(string, '{', '}', beforeBraces, withBraces, insideBraces, afterBraces) == true);
	assert(strcmp(R_MutableString_getString(beforeBraces), "hi") == 0);
	assert(strcmp(R_MutableString_getString(withBraces), "{r{t}y{fg{h}vb}}") == 0);
	assert(strcmp(R_MutableString_getString(insideBraces), "r{t}y{fg{h}vb}") == 0);
	assert(strcmp(R_MutableString_getString(afterBraces), "qwe") == 0);


	R_MutableString_setString(string, "command;not");
	assert(R_MutableString_getBracedString(string, '\0', ';', substring) != NULL);
	assert(strcmp(R_MutableString_getString(R_MutableString_getBracedString(string, 0, ';', substring)), "command;") == 0);
	assert(R_MutableString_getBracedString(string, ';', '\0', substring) != NULL);
	assert(strcmp(R_MutableString_getString(R_MutableString_getBracedString(string, ';', 0, substring)), ";not") == 0);

	R_MutableString_setString(string, "a%%sdf#ht");
	assert(R_MutableString_findFirstToken(string, "%%fh") == '%');
	assert(R_MutableString_findFirstToken(string, "#fh") == 'f');
	assert(R_MutableString_findFirstToken(string, "s#d") == 's');

	assert(R_MutableString_setSizedString(string, "0123456789", 4) != NULL);
	assert(strcmp(R_MutableString_getString(string), "0123") == 0);

	R_MutableString_setString(string, "0123456789");
	R_MutableString_setString(string, R_MutableString_getString(string)+2);
	assert(strcmp(R_MutableString_getString(string), "23456789") == 0);



	printf("Pass\n");
}