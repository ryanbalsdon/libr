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
#include "R_Type.h"
#include "R_String.h"
#include "R_Functor.h"
 

typedef struct R_Dictionary R_Dictionary;
R_Type_Declare(R_Dictionary);

/*  R_Dictionary_addObjectOfType
    Allocates a new object with the given R_Type definition and adds it to the dictionary.
 */
void* R_Dictionary_addObjectOfType(R_Dictionary* self, const char* key, const R_Type* type);

/*  R_Dictionary_add
    Allocates a new object with the given R_Type type name and adds it to the dictionary.
 */
#define R_Dictionary_add(self, key, Type) (Type*)R_Dictionary_addObjectOfType(self, key, R_Type_Object(Type))

/*  R_Dictionary_addCopy
    Copies the given object and adds it to the dictionary. If the object doesn't have an R_Type_Copier, this will fail.
 */
void* R_Dictionary_addCopy(R_Dictionary* self, const char* key, const void* object);

/*  R_Dictionary_merge
    Copies the given dictionary into self.
 */
R_Dictionary* R_Dictionary_merge(R_Dictionary* self, R_Dictionary* dictionary_to_copy);


/*  R_Dictionary_transferOwnership
    Adds the pointer to the dictionary without copying the object. This is dangerous. The object must be on the heap.
 */
void* R_Dictionary_transferOwnership(R_Dictionary* self, const char* key, void* object);

/*  R_Dictionary_remove
    Removes the object with the given key.
 */
void R_Dictionary_remove(R_Dictionary* self, const char* key);

/*  R_Dictionary_get
    Fetches the object with the given key. Returns NULL if it doesn't exist.
 */
void* R_Dictionary_get(R_Dictionary* self, const char* key);

/*  R_Dictionary_getFromString
    Fetches the object with the given key. Returns NULL if it doesn't exist.
 */
void* R_Dictionary_getFromString(R_Dictionary* self, R_String* key);

/*  R_Dictionary_removeAll
    Removes all objects from the dictionary.
 */
void R_Dictionary_removeAll(R_Dictionary* self);

/*  R_Dictionary_isPresent
    Returns true if the key exists in the dictionary or false if it isn't.
 */
bool R_Dictionary_isPresent(R_Dictionary* self, const char* key);
#define R_Dictionary_hasKey R_Dictionary_isPresent

/*  R_Dictionary_isNotPresent
    Returns false if the key exists in the dictionary or true if it isn't.
 */
bool R_Dictionary_isNotPresent(R_Dictionary* self, const char* key);

/*  R_Dictionary_ValueIterator
    Initializes a functor that can be used to iterate over the dictionary keys.
 */
R_Functor* R_Dictionary_KeyIterator(R_Functor* functor, R_Dictionary* dictionary);

/*  R_Dictionary_ValueIterator
    Initializes a functor that can be used to iterate over the dictionary values.
 */
R_Functor* R_Dictionary_ValueIterator(R_Functor* functor, R_Dictionary* dictionary);

/*  R_Dictionary_size
    Returns the number of keys in the dictionary.
 */
size_t R_Dictionary_size(R_Dictionary* self);

/*  R_Dictionary_toJson
    Writes the dictionary to the given string, as json. 
 */
R_String* R_Dictionary_toJson(R_Dictionary* self, R_String* buffer);

/*  R_Dictionary_toJson
    Initializes the dictionary with the given json string.
 */
R_Dictionary* R_Dictionary_fromJson(R_Dictionary* self, R_String* buffer);

void R_Dictionary_puts(R_Dictionary* self);

#endif /* R_Dictionary_h */
