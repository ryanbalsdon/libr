#ifndef R_ObjectArray_h
#define R_ObjectArray_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */



typedef void* (*R_ObjectArray_Allocator)(void);
typedef void* (*R_ObjectArray_Deallocator)(void*);

typedef struct R_ObjectArray R_ObjectArray;

R_ObjectArray* R_ObjectArray_alloc(void);
R_ObjectArray* R_ObjectArray_reset(R_ObjectArray* self);
R_ObjectArray* R_ObjectArray_free(R_ObjectArray* self);

//allocates memory and adds a pointer to it to the array. Returns the new pointer.
void* R_ObjectArray_addPointer(R_ObjectArray* self, size_t size);
//removes the pointer from the array and deallocates associated memory.
void R_ObjectArray_removeIndex(R_ObjectArray* self, int index);
void R_ObjectArray_removePointer(R_ObjectArray* self, void* pointer);
void R_ObjectArray_removeAll(R_ObjectArray* self);

//allocates memory and adds a pointer to it to the array. Returns the new pointer.
void* R_ObjectArray_addManagedPointer(R_ObjectArray* self, R_ObjectArray_Allocator allocator);
//removes the pointer from the array and deallocates associated memory.
void R_ObjectArray_removeManagedIndex(R_ObjectArray* self, R_ObjectArray_Deallocator deallocator, int index);
void R_ObjectArray_removeManagedPointer(R_ObjectArray* self, R_ObjectArray_Deallocator deallocator, void* pointer);
void R_ObjectArray_removeManagedAll(R_ObjectArray* self, R_ObjectArray_Deallocator deallocator);

void* R_ObjectArray_pointerAtIndex(R_ObjectArray* self, int index);
void* R_ObjectArray_lastPointer(R_ObjectArray* self);
int R_ObjectArray_indexOfPointer(R_ObjectArray* self, void* pointer);

//Returns the length of the array
int R_ObjectArray_getSize(R_ObjectArray* self);
int R_ObjectArray_length(R_ObjectArray* self);

void R_ObjectArray_pop(R_ObjectArray* self);
void R_ObjectArray_swap(R_ObjectArray* self, int indexA, int indexB);



#endif /* R_ObjectArray_h */
