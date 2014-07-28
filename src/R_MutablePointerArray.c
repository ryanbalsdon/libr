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
#include "R_MutablePointerArray.h"

struct R_MutablePointerArray {
    void ** array;           //The actual array
    int arrayAllocationSize;//How large the internal array is. This is always as-large or larger than ArraySize.
    int arraySize;          //How many objects the user has added to the array.
};

static void R_MutablePointerArray_increaseAllocation(R_MutablePointerArray* self);

R_MutablePointerArray* R_MutablePointerArray_alloc(void) {
    R_MutablePointerArray* self = (R_MutablePointerArray*)malloc(sizeof(R_MutablePointerArray));
    self->array = (void**)malloc(128*sizeof(void*));
    memset(self->array, 0, 128*sizeof(void*));
    self->arrayAllocationSize = 128;
    self->arraySize = 0;

    return self;
}
R_MutablePointerArray* R_MutablePointerArray_reset(R_MutablePointerArray* self) {
    self->array = (void**)realloc(self->array, 128*sizeof(void*));
    memset(self->array, 0, 128*sizeof(void*));
    self->arrayAllocationSize = 128;
    self->arraySize = 0;

    return self;
}
R_MutablePointerArray* R_MutablePointerArray_free(R_MutablePointerArray* self) {
    R_MutablePointerArray_removeAll(self);
    free(self->array);
    free(self);

    return NULL;
}

inline int R_MutablePointerArray_getSize(R_MutablePointerArray* self) {
    return self->arraySize;
}

inline int R_MutablePointerArray_length(R_MutablePointerArray* self) {
    return self->arraySize;
}


static void R_MutablePointerArray_increaseAllocation(R_MutablePointerArray* self) {
    //double allocation
    void ** newArray = (void**)malloc(2*self->arrayAllocationSize*sizeof(void*));
    for (int i=0; i<self->arrayAllocationSize; i++) {
        newArray[i] = self->array[i];
    }
    free(self->array);
    self->array = newArray;
    self->arrayAllocationSize *= 2;
}

void* R_MutablePointerArray_addManagedPointer(R_MutablePointerArray* self, R_MutablePointerArray_Allocator allocator) {
    if (self->arrayAllocationSize <= self->arraySize)
        R_MutablePointerArray_increaseAllocation(self);

    void* newPointer;
    newPointer = allocator();
    
    self->array[self->arraySize] = newPointer;
    self->arraySize++;
    
    return newPointer;
}
void R_MutablePointerArray_removeManagedIndex(R_MutablePointerArray* self, R_MutablePointerArray_Deallocator deallocator, int index) {
    void* pointerToDelete = R_MutablePointerArray_pointerAtIndex(self, index);
    deallocator(pointerToDelete);

    for (int i=index; i<self->arraySize-1; i++) {
        self->array[i] = self->array[i+1];
    }
    self->arraySize--;
    self->array[self->arraySize] = 0;
}
void R_MutablePointerArray_removeManagedPointer(R_MutablePointerArray* self, R_MutablePointerArray_Deallocator deallocator, void* pointer) {
    int i;
    for (i=0; i<self->arraySize; i++) {
        if (self->array[i] == pointer)
            break;
    }
    if (i >= self->arraySize) 
        return; //No pointer found
    
    R_MutablePointerArray_removeManagedIndex(self, deallocator, i);
    
    //Recurse incase there are more copies
    R_MutablePointerArray_removeManagedPointer(self, deallocator, pointer);
}
void R_MutablePointerArray_removeManagedAll(R_MutablePointerArray* self, R_MutablePointerArray_Deallocator deallocator) {
    while (R_MutablePointerArray_getSize(self) > 0) {
        R_MutablePointerArray_removeManagedIndex(self, deallocator, 0);
    }
}

void* R_MutablePointerArray_addPointer(R_MutablePointerArray* self, size_t size) {
    if (self->arrayAllocationSize <= self->arraySize)
        R_MutablePointerArray_increaseAllocation(self);

    void* newPointer = malloc(size);
    
    self->array[self->arraySize] = newPointer;
    self->arraySize++;
    
    return newPointer;
}
void R_MutablePointerArray_removeIndex(R_MutablePointerArray* self, int index) {
    void* pointerToDelete = R_MutablePointerArray_pointerAtIndex(self, index);
    free(pointerToDelete);

    for (int i=index; i<self->arraySize-1; i++) {
        self->array[i] = self->array[i+1];
    }
    self->arraySize--;
    self->array[self->arraySize] = 0;
}
void R_MutablePointerArray_removePointer(R_MutablePointerArray* self, void* pointer) {
    int i;
    for (i=0; i<self->arraySize; i++) {
        if (self->array[i] == pointer)
            break;
    }
    if (i >= self->arraySize) 
        return; //No pointer found
    
    R_MutablePointerArray_removeIndex(self, i);
    
    //Recurse incase there are more copies
    R_MutablePointerArray_removePointer(self, pointer);
}
void R_MutablePointerArray_removeAll(R_MutablePointerArray* self) {
    while (R_MutablePointerArray_getSize(self) > 0) {
        R_MutablePointerArray_pop(self);
    }
}

inline void* R_MutablePointerArray_pointerAtIndex(R_MutablePointerArray* self, int index) {
    if (index < 0 || index >= R_MutablePointerArray_length(self))
        return NULL;
    return self->array[index];
}

void* R_MutablePointerArray_lastPointer(R_MutablePointerArray* self) {
    return R_MutablePointerArray_pointerAtIndex(self, R_MutablePointerArray_getSize(self)-1);
}

int R_MutablePointerArray_indexOfPointer(R_MutablePointerArray* self, void* pointer) {
    for (int i=0; i<self->arraySize; i++) {
        if (self->array[i] == pointer)
            return i;
    }
    return -1;
}

void R_MutablePointerArray_pop(R_MutablePointerArray* self) {
    R_MutablePointerArray_removeIndex(self, R_MutablePointerArray_getSize(self)-1);
}

void R_MutablePointerArray_swap(R_MutablePointerArray* self, int indexA, int indexB) {
    if (indexA > self->arraySize || indexB > self->arraySize || indexA < 0 || indexB < 0)
        return;
    void* temp = self->array[indexA];
    self->array[indexA] = self->array[indexB];
    self->array[indexB] = temp;
}



