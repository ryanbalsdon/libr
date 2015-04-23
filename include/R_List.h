#ifndef R_List_h
#define R_List_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include "R_Type.h"
#include "R_Functor.h"

R_Type_Declare(R_List);
typedef struct R_List R_List;

/*  R_List_addObjectOfType
    Allocates an instance of the given type, appends it to the list and returns it.
 */
void* R_List_addObjectOfType(R_List* self, const R_Type* type);

/*  R_List_transferOwnership
    Adds the pointer to the list without copying the object. This is dangerous. The object must be on the heap.
 */
void* R_List_transferOwnership(R_List* self, void* object);

/*  R_List_add
    Allocates an instance of the given type, appends it to the list and returns it.
 */
#define R_List_add(self, Type) (Type*)R_List_addObjectOfType(self, R_Type_Object(Type))

/*  R_List_addCopy
    Copies the given object, appends it to the list and returns it. If the object hasn't defined 
    an R_Type_Copier, this will fail.
 */
void* R_List_addCopy(R_List* self, const void* object);

/*  R_List_appendList
    Appends the given list by adding a copy of every element. Returns NULL on the first failed copy.
 */
void* R_List_appendList(R_List* self, R_List* list);

/*  R_List_removeIndex
    Removes the object at the given index and destroys it.
 */
void R_List_removeIndex(R_List* self, size_t index);

/*  R_List_removePointer
    Removes the given object and destroys it.
 */
void R_List_removePointer(R_List* self, void* pointer);

/*  R_List_removeAll
    Empties the list and destroys all objects.
 */
void R_List_removeAll(R_List* self);

/*  R_List_pointerAtIndex
    Returns the object at the given index or NULL.
 */
void* R_List_pointerAtIndex(R_List* self, size_t index);

/*  R_List_lastPointer
    Returns the last object in the list or NULL.
 */
void* R_List_last(R_List* self);

/*  R_List_lastPointer
 Returns the last object in the list or NULL.
 */
void* R_List_first(R_List* self);

/*  R_List_indexOfPointer
    Finds the given object in the list and returns the index of it. Returns -1 if the object isn't found.
 */
size_t R_List_indexOfPointer(R_List* self, void* pointer);

/*  R_List_size
    Returns the number of objects in the list.
 */
size_t R_List_size(R_List* self);
#define  R_List_length R_List_size

/*  R_List_pop
    Removes the last object in the list and destroys it.
 */
void R_List_pop(R_List* self);

/*  R_List_shift
    Removes the first object in the list and destroys it.
 */
void R_List_shift(R_List* self);

/*  R_List_swap
    Swaps the locations of the two objects at the given indexes.
 */
void R_List_swap(R_List* self, int indexA, int indexB);

/*  R_List_Iterator
    Initializes a functor as an iterator/enumerator for the list. Each call returns the next element.
 */
R_Functor* R_List_Iterator(R_Functor* functor, R_List* list);

/*  R_List_each
    Sets up a loop to iterate over the list.
 */
#define R_List_each(array, TYPE, NAME) for(TYPE* NAME=NULL; (NAME=(TYPE*)R_List_pointerAtIndex(array, R_List_indexOfPointer(array, NAME)+1));)

void R_List_puts(R_List* self);

#endif /* R_List_h */
