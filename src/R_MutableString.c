/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "R_MutableString.h"

struct R_MutableString {
    char* string;           //The allocated buffer
    int stringAllocationSize;//How large the internal buffer is. This is always as-large or larger than stringSize.
    int stringSize;          //How many objects the user has added to the array.
};

void R_MutableString_increaseAllocationSize(R_MutableString* self, int spaceNeeded);
char* R_MutableString_Strcpy(char* dest, const char* source);
char* R_MutableString_Strncpy(char* dest, const char* source, int num);
char* R_MutableString_Strcat(char* dest, const char* source);

R_MutableString* R_MutableString_alloc(void) {
	R_MutableString* self = (R_MutableString*)malloc(sizeof(R_MutableString));
    self->string = (char*)malloc(128);
    memset(self->string, '\0', 128);
    self->stringAllocationSize = 128;
    self->stringSize = 0;

    return self;
}
R_MutableString* R_MutableString_reset(R_MutableString* self) {
	self->string = (char*)realloc(self->string, 128);
    memset(self->string, '\0', 128);
    self->stringAllocationSize = 128;
    self->stringSize = 0;

    return self;
}
R_MutableString* R_MutableString_free(R_MutableString* self) {
    free(self->string);
    free(self);

    return NULL;
}

const char* R_MutableString_getString(R_MutableString* self) {
	return self->string;
}

R_MutableString* R_MutableString_appendString(R_MutableString* self, const char* string) {
	int length = strlen(string);
	R_MutableString_increaseAllocationSize(self, self->stringSize+length);
	R_MutableString_Strcat(self->string, string);
	self->stringSize += length;
	return self;
}

void R_MutableString_increaseAllocationSize(R_MutableString* self, int spaceNeeded) {
	int allocationNeeded = spaceNeeded + 1;
	if (allocationNeeded < self->stringAllocationSize)
		return;

	int newSize = (self->stringAllocationSize == 0)?1:self->stringAllocationSize;
	do {newSize*=2;} while (allocationNeeded > newSize);
	self->string = (char*)realloc(self->string, newSize);
	self->stringAllocationSize = newSize;
}

R_MutableString* R_MutableString_setString(R_MutableString* self, const char* string) {
	int length = strlen(string);
	R_MutableString_increaseAllocationSize(self, length);
	R_MutableString_Strcpy(self->string, string);
	self->stringSize = length;
	return self;
}

R_MutableString* R_MutableString_setSizedString(R_MutableString* self, const char* string, int stringLength) {
	int length = strlen(string);
	if (length > stringLength) length = stringLength;
	R_MutableString_increaseAllocationSize(self, length);
	R_MutableString_Strncpy(self->string, string, length);
	self->string[length] = '\0';
	self->stringSize = length;
	return self;
}

R_MutableString* R_MutableString_appendInt(R_MutableString* self, int value) {
	int length = snprintf(NULL, 0, "%d", value);
	R_MutableString_increaseAllocationSize(self, self->stringSize+length);
	self->stringSize += sprintf(self->string+self->stringSize, "%d", value);
	return self;
}

R_MutableString* R_MutableString_appendFloat(R_MutableString* self, float value) {
	int length = snprintf(NULL, 0, "%g", value);
	R_MutableString_increaseAllocationSize(self, self->stringSize+length);
	self->stringSize += sprintf(self->string+self->stringSize, "%g", value);
	return self;
}

R_MutableString* R_MutableString_appendMutableString(R_MutableString* self, R_MutableString* string) {
	return R_MutableString_appendString(self, R_MutableString_getString(string));
}

int R_MutableString_getInt(R_MutableString* self) {
	int output = 0;
	if (sscanf(self->string, "%d", &output) == 1) return output;
	return 0;
}
float R_MutableString_getFloat(R_MutableString* self) {
	float output = 0.0f;
	if (sscanf(self->string, "%g", &output) == 1) return output;
	return 0.0f;
}

R_MutableString* R_MutableString_getSubstring(R_MutableString* self, int startingIndex, int endingIndex, R_MutableString* output) {
	R_MutableString_reset(output);
	R_MutableString_increaseAllocationSize(output, output->stringSize+(endingIndex-startingIndex)+2);
	R_MutableString_Strncpy(output->string, self->string+startingIndex, endingIndex-startingIndex+1);
	output->string[endingIndex-startingIndex+2] = '\0';
	output->stringSize = endingIndex-startingIndex+2;
	return output;
}

R_MutableString* R_MutableString_getBracedString(R_MutableString* self, char beginningBrace, char finishingBrace, R_MutableString* output) {
	if (R_MutableString_splitBracedString(self, beginningBrace, finishingBrace, NULL, output, NULL, NULL))
		return output;
	return NULL;
}

R_MutableString* R_MutableString_getEnclosedString(R_MutableString* self, char beginningBrace, char finishingBrace, R_MutableString* output) {
	if (R_MutableString_splitBracedString(self, beginningBrace, finishingBrace, NULL, NULL, output, NULL))
		return output;
	return NULL;

}

bool R_MutableString_splitBracedString(R_MutableString* self, char beginningBrace, char finishingBrace, 
	R_MutableString* beforeBraces, R_MutableString* withBraces, R_MutableString* insideBraces, R_MutableString* afterBraces)
{
	R_MutableString* input = R_MutableString_alloc();
	R_MutableString_appendMutableString(input, self);
	if (beforeBraces != NULL) R_MutableString_reset(beforeBraces);
	if (withBraces != NULL) R_MutableString_reset(withBraces);
	if (insideBraces != NULL) R_MutableString_reset(insideBraces);
	if (afterBraces != NULL) R_MutableString_reset(afterBraces);

	int depth = 0;
	for (int i=0; i<strlen(input->string); i++) {
		if (beginningBrace == '\0' || input->string[i] == beginningBrace) {
			if (finishingBrace == '\0') {
				if (beforeBraces != NULL) R_MutableString_setSizedString(beforeBraces, input->string, i);
				if (withBraces != NULL) R_MutableString_getSubstring(input, (beginningBrace != 0)?i:i, strlen(input->string), withBraces);
				if (insideBraces != NULL) R_MutableString_getSubstring(input, (beginningBrace != 0)?i+1:i, strlen(input->string), insideBraces);						
				//if (afterBraces != NULL) R_MutableString_setString(afterBraces, "");
				return true;
			}
			for (int j=i+1; j<strlen(input->string)+1; j++) {
				if (input->string[j] == finishingBrace) {
					if (depth == 0) {
						if (beforeBraces != NULL) R_MutableString_setSizedString(beforeBraces, input->string, i);
						if (withBraces != NULL) R_MutableString_getSubstring(input, (beginningBrace != 0)?i:i, j, withBraces);
						if (insideBraces != NULL) R_MutableString_getSubstring(input, (beginningBrace != 0)?i+1:i, j-1, insideBraces);
						if (afterBraces != NULL) R_MutableString_setString(afterBraces, input->string+j+1);
						return true;
					}
					else {
						depth--;
					}
				}
				else if (input->string[j] == beginningBrace) {
					depth++;
				}
			}
		}
	}

	return false;

}

char R_MutableString_findFirstToken(R_MutableString* self, char* tokens) {
	if (tokens == NULL) return '\0';
	for (int i=0; i<strlen(self->string); i++) {
		for (int j=0; j<strlen(tokens); j++) {
			if (self->string[i] == tokens[j]) {
				return tokens[j];
			}
		}
	}
	return '\0';
}



char* R_MutableString_Strcpy(char* dest, const char* source) {
	
	for (int i=0; ; i++) {
		dest[i] = source[i];
		if (source[i] == '\0')
			return dest;
	}
	return dest;
	
	//return strcpy(dest, source);
}
char* R_MutableString_Strncpy(char* dest, const char* source, int num) {
	
	for (int i=0; i<num; i++) {
		dest[i] = source[i];
		if (source[i] == '\0')
			return dest;
	}
	return dest;
	
	//return strncpy(dest, source, num);
}
char* R_MutableString_Strcat(char* dest, const char* source) {
	char* copyStart = dest + strlen(dest);
	return R_MutableString_Strcpy(copyStart, source);
	//return strcat(dest, source);
}