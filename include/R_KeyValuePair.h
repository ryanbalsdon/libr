#ifndef R_KeyValuePair_h
#define R_KeyValuePair_h

#include <stdbool.h>
#include "R_Type.h"
#include "R_MutableString.h"

typedef struct R_KeyValuePair R_KeyValuePair;
R_Type_Declare(R_KeyValuePair);

R_MutableString* R_FUNCTION_ATTRIBUTES R_KeyValuePair_key(R_KeyValuePair* pair);
void* R_FUNCTION_ATTRIBUTES R_KeyValuePair_value(R_KeyValuePair* pair);
R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_setKey(R_KeyValuePair* pair, const char* key);
R_KeyValuePair* R_FUNCTION_ATTRIBUTES R_KeyValuePair_setValue(R_KeyValuePair* pair, void* value);

#endif /* R_KeyValuePair_h */
