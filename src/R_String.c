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
#include "R_String.h"

struct R_String {
    char* string;           //The allocated buffer
    size_t stringAllocationSize;//How large the internal buffer is. This is always as-large or larger than stringSize.
    size_t stringSize;          //How many objects the user has added to the array.
};

void R_String_increaseAllocationSize(R_String* self, size_t spaceNeeded);
char* R_String_Strcpy(char* dest, const char* source);
char* R_String_Strncpy(char* dest, const char* source, size_t num);
char* R_String_Strcat(char* dest, const char* source);

R_String* R_String_alloc(void) {
	R_String* self = (R_String*)malloc(sizeof(R_String));
    self->string = (char*)malloc(128);
    memset(self->string, '\0', 128);
    self->stringAllocationSize = 128;
    self->stringSize = 0;

    return self;
}
R_String* R_String_reset(R_String* self) {
	self->string = (char*)realloc(self->string, 128);
    memset(self->string, '\0', 128);
    self->stringAllocationSize = 128;
    self->stringSize = 0;

    return self;
}
R_String* R_String_free(R_String* self) {
    free(self->string);
    free(self);

    return NULL;
}

const char* R_String_getString(R_String* self) {
	return self->string;
}

R_String* R_String_appendCString(R_String* self, const char* string) {
	size_t length = strlen(string);
	R_String_increaseAllocationSize(self, self->stringSize+length);
	R_String_Strcat(self->string, string);
	self->stringSize += length;
	return self;
}

void R_String_increaseAllocationSize(R_String* self, size_t spaceNeeded) {
	size_t allocationNeeded = spaceNeeded + 1;
	if (allocationNeeded < self->stringAllocationSize)
		return;

	size_t newSize = (self->stringAllocationSize == 0)?1:self->stringAllocationSize;
	do {newSize*=2;} while (allocationNeeded > newSize);
	self->string = (char*)realloc(self->string, newSize);
	self->stringAllocationSize = newSize;
}

R_String* R_String_setString(R_String* self, const char* string) {
	size_t length = strlen(string);
	R_String_increaseAllocationSize(self, length);
	R_String_Strcpy(self->string, string);
	self->stringSize = length;
	return self;
}

R_String* R_String_setSizedString(R_String* self, const char* string, size_t stringLength) {
	size_t length = strlen(string);
	if (length > stringLength) length = stringLength;
	R_String_increaseAllocationSize(self, length);
	R_String_Strncpy(self->string, string, length);
	self->string[length] = '\0';
	self->stringSize = length;
	return self;
}

R_String* R_String_appendInt(R_String* self, int value) {
	int length = snprintf(NULL, 0, "%d", value);
	R_String_increaseAllocationSize(self, self->stringSize+length);
	self->stringSize += sprintf(self->string+self->stringSize, "%d", value);
	return self;
}

R_String* R_String_appendFloat(R_String* self, float value) {
	int length = snprintf(NULL, 0, "%g", value);
	R_String_increaseAllocationSize(self, self->stringSize+length);
	self->stringSize += sprintf(self->string+self->stringSize, "%g", value);
	return self;
}

R_String* R_String_appendString(R_String* self, R_String* string) {
	return R_String_appendCString(self, R_String_getString(string));
}

int R_String_getInt(R_String* self) {
	int output = 0;
	if (sscanf(self->string, "%d", &output) == 1) return output;
	return 0;
}
float R_String_getFloat(R_String* self) {
	float output = 0.0f;
	if (sscanf(self->string, "%g", &output) == 1) return output;
	return 0.0f;
}

R_String* R_String_getSubstring(R_String* self, int startingIndex, int endingIndex, R_String* output) {
	R_String_reset(output);
	R_String_increaseAllocationSize(output, output->stringSize+(endingIndex-startingIndex)+2);
	R_String_Strncpy(output->string, self->string+startingIndex, endingIndex-startingIndex+1);
	output->string[endingIndex-startingIndex+2] = '\0';
	output->stringSize = endingIndex-startingIndex+2;
	return output;
}

R_String* R_String_getBracedString(R_String* self, char beginningBrace, char finishingBrace, R_String* output) {
	if (R_String_splitBracedString(self, beginningBrace, finishingBrace, NULL, output, NULL, NULL))
		return output;
	return NULL;
}

R_String* R_String_getEnclosedString(R_String* self, char beginningBrace, char finishingBrace, R_String* output) {
	if (R_String_splitBracedString(self, beginningBrace, finishingBrace, NULL, NULL, output, NULL))
		return output;
	return NULL;

}

bool R_String_splitBracedString(R_String* self, char beginningBrace, char finishingBrace, 
	R_String* beforeBraces, R_String* withBraces, R_String* insideBraces, R_String* afterBraces)
{
	R_String* input = R_String_alloc();
	R_String_appendString(input, self);
	if (beforeBraces != NULL) R_String_reset(beforeBraces);
	if (withBraces != NULL) R_String_reset(withBraces);
	if (insideBraces != NULL) R_String_reset(insideBraces);
	if (afterBraces != NULL) R_String_reset(afterBraces);

	int depth = 0;
	for (int i=0; i<strlen(input->string); i++) {
		if (beginningBrace == '\0' || input->string[i] == beginningBrace) {
			if (finishingBrace == '\0') {
				if (beforeBraces != NULL) R_String_setSizedString(beforeBraces, input->string, i);
				if (withBraces != NULL) R_String_getSubstring(input, (beginningBrace != 0)?i:i, strlen(input->string), withBraces);
				if (insideBraces != NULL) R_String_getSubstring(input, (beginningBrace != 0)?i+1:i, strlen(input->string), insideBraces);						
				//if (afterBraces != NULL) R_String_setString(afterBraces, "");
				return true;
			}
			for (int j=i+1; j<strlen(input->string)+1; j++) {
				if (input->string[j] == finishingBrace) {
					if (depth == 0) {
						if (beforeBraces != NULL) R_String_setSizedString(beforeBraces, input->string, i);
						if (withBraces != NULL) R_String_getSubstring(input, (beginningBrace != 0)?i:i, j, withBraces);
						if (insideBraces != NULL) R_String_getSubstring(input, (beginningBrace != 0)?i+1:i, j-1, insideBraces);
						if (afterBraces != NULL) R_String_setString(afterBraces, input->string+j+1);
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

char R_String_findFirstToken(R_String* self, char* tokens) {
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



char* R_String_Strcpy(char* dest, const char* source) {
	
	for (int i=0; ; i++) {
		dest[i] = source[i];
		if (source[i] == '\0')
			return dest;
	}
	return dest;
	
	//return strcpy(dest, source);
}
char* R_String_Strncpy(char* dest, const char* source, size_t num) {
	
	for (int i=0; i<num; i++) {
		dest[i] = source[i];
		if (source[i] == '\0')
			return dest;
	}
	return dest;
	
	//return strncpy(dest, source, num);
}
char* R_String_Strcat(char* dest, const char* source) {
	char* copyStart = dest + strlen(dest);
	return R_String_Strcpy(copyStart, source);
	//return strcat(dest, source);
}

bool R_String_isEmpty(R_String* self) {
	if (self->stringSize == 0) return true;
	return false;
}