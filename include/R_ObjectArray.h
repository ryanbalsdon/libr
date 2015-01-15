#ifndef R_ObjectArray_h
#define R_ObjectArray_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include "R_Type.h"

R_Type_Declare(R_ObjectArray);
typedef struct R_ObjectArray R_ObjectArray;

/*  R_ObjectArray_addObjectOfType
    Allocates an instance of the given type, appends it to the list and returns it.
 */
void* R_ObjectArray_addObjectOfType(R_ObjectArray* self, const R_Type* type);

/*  R_ObjectArray_add
    Allocates an instance of the given type, appends it to the list and returns it.
 */
#define R_ObjectArray_add(self, Type) (Type*)R_ObjectArray_addObjectOfType(self, R_Type_Object(Type))

/*  R_ObjectArray_removeIndex
    Removes the object at the given index and destroys it.
 */
void R_ObjectArray_removeIndex(R_ObjectArray* self, unsigned int index);

/*  R_ObjectArray_removePointer
    Removes the given object and destroys it.
 */
void R_ObjectArray_removePointer(R_ObjectArray* self, void* pointer);

/*  R_ObjectArray_removeAll
    Empties the list and destroys all objects.
 */
void R_ObjectArray_removeAll(R_ObjectArray* self);

/*  R_ObjectArray_pointerAtIndex
    Returns the object at the given index or NULL.
 */
void* R_ObjectArray_pointerAtIndex(R_ObjectArray* self, unsigned int index);

/*  R_ObjectArray_lastPointer
    Returns the last object in the list or NULL.
 */
void* R_ObjectArray_lastPointer(R_ObjectArray* self);

/*  R_ObjectArray_indexOfPointer
    Finds the given object in the list and returns the index of it. Returns -1 if the object isn't found.
 */
int R_ObjectArray_indexOfPointer(R_ObjectArray* self, void* pointer);

/*  R_ObjectArray_size
    Returns the number of objects in the list.
 */
int R_ObjectArray_size(R_ObjectArray* self);
#define  R_ObjectArray_length R_ObjectArray_size

/*  R_ObjectArray_pop
    Removes the last object in the list and destroys it.
 */
void R_ObjectArray_pop(R_ObjectArray* self);

/*  R_ObjectArray_swap
    Swaps the locations of the two objects at the given indexes.
 */
void R_ObjectArray_swap(R_ObjectArray* self, int indexA, int indexB);

/*  R_ObjectArray_swap
    Sets up a loop to iterate over the list.
 */
#define R_ObjectArray_each(array, TYPE, NAME) for(TYPE* NAME=NULL; (NAME=(TYPE*)R_ObjectArray_pointerAtIndex(array, R_ObjectArray_indexOfPointer(array, NAME)+1));)

#endif /* R_ObjectArray_h */
