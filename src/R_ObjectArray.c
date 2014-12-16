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
    R_Type* type;
    void ** array;          //The actual array
    int arrayAllocationSize;//How large the internal array is. This is always as-large or larger than ArraySize.
    int arraySize;          //How many objects the user has added to the array.
};

static R_ObjectArray* R_ObjectArray_Constructor(R_ObjectArray* self);
static R_ObjectArray* R_ObjectArray_Destructor(R_ObjectArray* self);
//static void R_ObjectArray_Copier(R_ObjectArray* self, R_ObjectArray* new);
R_Type_Def(R_ObjectArray, R_ObjectArray_Constructor, R_ObjectArray_Destructor, NULL);

static void R_ObjectArray_increaseAllocationIfRequired(R_ObjectArray* self);

R_ObjectArray* R_ObjectArray_Constructor(R_ObjectArray* self) {
    self->array = (void**)malloc(128*sizeof(void*));
    memset(self->array, 0, 128*sizeof(void*));
    self->arrayAllocationSize = 128;
    self->arraySize = 0;

    return self;
}

R_ObjectArray* R_ObjectArray_Destructor(R_ObjectArray* self) {
    R_ObjectArray_removeAll(self);
    free(self->array);

    return self;
}

inline int R_ObjectArray_getSize(R_ObjectArray* self) {
    return self->arraySize;
}

inline int R_ObjectArray_length(R_ObjectArray* self) {
    return self->arraySize;
}


static void R_ObjectArray_increaseAllocationIfRequired(R_ObjectArray* self) {
    if (self->arrayAllocationSize > self->arraySize) return;
    //double allocation
    void ** newArray = (void**)malloc(2*self->arrayAllocationSize*sizeof(void*));
    for (int i=0; i<self->arrayAllocationSize; i++) {
        newArray[i] = self->array[i];
    }
    free(self->array);
    self->array = newArray;
    self->arrayAllocationSize *= 2;
}

inline void* R_ObjectArray_pointerAtIndex(R_ObjectArray* self, unsigned int index) {
    if (index >= R_ObjectArray_length(self))
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

void* R_ObjectArray_addObject(R_ObjectArray* self, const R_Type* type) {
    if (type == NULL || self == NULL) return NULL;
    R_ObjectArray_increaseAllocationIfRequired(self);

    void* newPointer = R_Type_New(type);
    if (newPointer == NULL) return NULL;

    self->array[self->arraySize] = newPointer;
    self->arraySize++;
    
    return newPointer;
}

void R_ObjectArray_removeIndex(R_ObjectArray* self, unsigned int index) {
    if (self == NULL || index>=self->arraySize) return;

    void* pointerToDelete = R_ObjectArray_pointerAtIndex(self, index);
    R_Type_Delete(pointerToDelete);

    for (int i=index; i<self->arraySize-1; i++) {
        self->array[i] = self->array[i+1];
    }
    self->arraySize--;
    self->array[self->arraySize] = 0;
}
void R_ObjectArray_removePointer(R_ObjectArray* self, void* pointer) {
    if (self == NULL || pointer == NULL) return;

    int i;
    for (i=0; i<self->arraySize; i++) if (self->array[i] == pointer) break;
    if (i >= self->arraySize) return; //No pointer found
    
    R_ObjectArray_removeIndex(self, i);
    
    //Recurse incase there are more copies
    R_ObjectArray_removePointer(self, pointer);
}
void R_ObjectArray_removeAll(R_ObjectArray* self) {
    if (self == NULL) return;
    for (int i=0; i<self->arraySize; i++) R_Type_Delete(self->array[i]);
    self->arraySize = 0;
}
