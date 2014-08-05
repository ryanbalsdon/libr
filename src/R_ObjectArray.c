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
#include "R_ObjectArray.h"

struct R_ObjectArray {
    void ** array;           //The actual array
    int arrayAllocationSize;//How large the internal array is. This is always as-large or larger than ArraySize.
    int arraySize;          //How many objects the user has added to the array.
};

static void R_ObjectArray_increaseAllocation(R_ObjectArray* self);

R_ObjectArray* R_ObjectArray_alloc(void) {
    R_ObjectArray* self = (R_ObjectArray*)malloc(sizeof(R_ObjectArray));
    self->array = (void**)malloc(128*sizeof(void*));
    memset(self->array, 0, 128*sizeof(void*));
    self->arrayAllocationSize = 128;
    self->arraySize = 0;

    return self;
}
R_ObjectArray* R_ObjectArray_reset(R_ObjectArray* self) {
    self->array = (void**)realloc(self->array, 128*sizeof(void*));
    memset(self->array, 0, 128*sizeof(void*));
    self->arrayAllocationSize = 128;
    self->arraySize = 0;

    return self;
}
R_ObjectArray* R_ObjectArray_free(R_ObjectArray* self) {
    R_ObjectArray_removeAll(self);
    free(self->array);
    free(self);

    return NULL;
}

inline int R_ObjectArray_getSize(R_ObjectArray* self) {
    return self->arraySize;
}

inline int R_ObjectArray_length(R_ObjectArray* self) {
    return self->arraySize;
}


static void R_ObjectArray_increaseAllocation(R_ObjectArray* self) {
    //double allocation
    void ** newArray = (void**)malloc(2*self->arrayAllocationSize*sizeof(void*));
    for (int i=0; i<self->arrayAllocationSize; i++) {
        newArray[i] = self->array[i];
    }
    free(self->array);
    self->array = newArray;
    self->arrayAllocationSize *= 2;
}

void* R_ObjectArray_addManagedPointer(R_ObjectArray* self, R_ObjectArray_Allocator allocator) {
    if (self->arrayAllocationSize <= self->arraySize)
        R_ObjectArray_increaseAllocation(self);

    void* newPointer;
    newPointer = allocator();
    
    self->array[self->arraySize] = newPointer;
    self->arraySize++;
    
    return newPointer;
}
void R_ObjectArray_removeManagedIndex(R_ObjectArray* self, R_ObjectArray_Deallocator deallocator, int index) {
    void* pointerToDelete = R_ObjectArray_pointerAtIndex(self, index);
    deallocator(pointerToDelete);

    for (int i=index; i<self->arraySize-1; i++) {
        self->array[i] = self->array[i+1];
    }
    self->arraySize--;
    self->array[self->arraySize] = 0;
}
void R_ObjectArray_removeManagedPointer(R_ObjectArray* self, R_ObjectArray_Deallocator deallocator, void* pointer) {
    int i;
    for (i=0; i<self->arraySize; i++) {
        if (self->array[i] == pointer)
            break;
    }
    if (i >= self->arraySize) 
        return; //No pointer found
    
    R_ObjectArray_removeManagedIndex(self, deallocator, i);
    
    //Recurse incase there are more copies
    R_ObjectArray_removeManagedPointer(self, deallocator, pointer);
}
void R_ObjectArray_removeManagedAll(R_ObjectArray* self, R_ObjectArray_Deallocator deallocator) {
    while (R_ObjectArray_getSize(self) > 0) {
        R_ObjectArray_removeManagedIndex(self, deallocator, 0);
    }
}

void* R_ObjectArray_addPointer(R_ObjectArray* self, size_t size) {
    if (self->arrayAllocationSize <= self->arraySize)
        R_ObjectArray_increaseAllocation(self);

    void* newPointer = malloc(size);
    memset(newPointer, 0, size);
    
    self->array[self->arraySize] = newPointer;
    self->arraySize++;
    
    return newPointer;
}
void R_ObjectArray_removeIndex(R_ObjectArray* self, int index) {
    void* pointerToDelete = R_ObjectArray_pointerAtIndex(self, index);
    free(pointerToDelete);

    for (int i=index; i<self->arraySize-1; i++) {
        self->array[i] = self->array[i+1];
    }
    self->arraySize--;
    self->array[self->arraySize] = 0;
}
void R_ObjectArray_removePointer(R_ObjectArray* self, void* pointer) {
    int i;
    for (i=0; i<self->arraySize; i++) {
        if (self->array[i] == pointer)
            break;
    }
    if (i >= self->arraySize) 
        return; //No pointer found
    
    R_ObjectArray_removeIndex(self, i);
    
    //Recurse incase there are more copies
    R_ObjectArray_removePointer(self, pointer);
}
void R_ObjectArray_removeAll(R_ObjectArray* self) {
    while (R_ObjectArray_getSize(self) > 0) {
        R_ObjectArray_pop(self);
    }
}

inline void* R_ObjectArray_pointerAtIndex(R_ObjectArray* self, int index) {
    if (index < 0 || index >= R_ObjectArray_length(self))
        return NULL;
    return self->array[index];
}

void* R_ObjectArray_lastPointer(R_ObjectArray* self) {
    return R_ObjectArray_pointerAtIndex(self, R_ObjectArray_getSize(self)-1);
}

int R_ObjectArray_indexOfPointer(R_ObjectArray* self, void* pointer) {
    for (int i=0; i<self->arraySize; i++) {
        if (self->array[i] == pointer)
            return i;
    }
    return -1;
}

void R_ObjectArray_pop(R_ObjectArray* self) {
    R_ObjectArray_removeIndex(self, R_ObjectArray_getSize(self)-1);
}

void R_ObjectArray_swap(R_ObjectArray* self, int indexA, int indexB) {
    if (indexA > self->arraySize || indexB > self->arraySize || indexA < 0 || indexB < 0)
        return;
    void* temp = self->array[indexA];
    self->array[indexA] = self->array[indexB];
    self->array[indexB] = temp;
}



