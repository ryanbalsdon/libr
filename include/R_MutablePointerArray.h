#ifndef R_MutablePointerArray_h
#define R_MutablePointerArray_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */



typedef void* (*R_MutablePointerArray_Allocator)(void);
typedef void* (*R_MutablePointerArray_Deallocator)(void*);

typedef struct R_MutablePointerArray R_MutablePointerArray;

R_MutablePointerArray* R_MutablePointerArray_alloc(void);
R_MutablePointerArray* R_MutablePointerArray_reset(R_MutablePointerArray* self);
R_MutablePointerArray* R_MutablePointerArray_free(R_MutablePointerArray* self);

//allocates memory and adds a pointer to it to the array. Returns the new pointer.
void* R_MutablePointerArray_addPointer(R_MutablePointerArray* self, size_t size);
//removes the pointer from the array and deallocates associated memory.
void R_MutablePointerArray_removeIndex(R_MutablePointerArray* self, int index);
void R_MutablePointerArray_removePointer(R_MutablePointerArray* self, void* pointer);
void R_MutablePointerArray_removeAll(R_MutablePointerArray* self);

//allocates memory and adds a pointer to it to the array. Returns the new pointer.
void* R_MutablePointerArray_addManagedPointer(R_MutablePointerArray* self, R_MutablePointerArray_Allocator allocator);
//removes the pointer from the array and deallocates associated memory.
void R_MutablePointerArray_removeManagedIndex(R_MutablePointerArray* self, R_MutablePointerArray_Deallocator deallocator, int index);
void R_MutablePointerArray_removeManagedPointer(R_MutablePointerArray* self, R_MutablePointerArray_Deallocator deallocator, void* pointer);
void R_MutablePointerArray_removeManagedAll(R_MutablePointerArray* self, R_MutablePointerArray_Deallocator deallocator);

void* R_MutablePointerArray_pointerAtIndex(R_MutablePointerArray* self, int index);
void* R_MutablePointerArray_lastPointer(R_MutablePointerArray* self);
int R_MutablePointerArray_indexOfPointer(R_MutablePointerArray* self, void* pointer);

//Returns the length of the array
int R_MutablePointerArray_getSize(R_MutablePointerArray* self);
int R_MutablePointerArray_length(R_MutablePointerArray* self);

void R_MutablePointerArray_pop(R_MutablePointerArray* self);
void R_MutablePointerArray_swap(R_MutablePointerArray* self, int indexA, int indexB);



#endif /* R_MutablePointerArray_h */
