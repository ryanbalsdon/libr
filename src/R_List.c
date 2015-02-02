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
#include "R_List.h"


struct R_List {
    R_Type* type;
    void ** array;          //The actual array
    size_t arrayAllocationSize;//How large the internal array is. This is always as-large or larger than ArraySize.
    size_t arraySize;          //How many objects the user has added to the array.
    size_t last_index_of_pointer_at_index; //Optimization for the 'each' operator
};

static R_List* R_List_Constructor(R_List* self);
static R_List* R_List_Destructor(R_List* self);
//static void R_List_Copier(R_List* self, R_List* new);
R_Type_Def(R_List, R_List_Constructor, R_List_Destructor, NULL);

static void R_List_increaseAllocationIfRequired(R_List* self);

static R_List* R_List_Constructor(R_List* self) {
    self->array = NULL;
    self->arrayAllocationSize = 0;
    self->arraySize = 0;

    return self;
}

static R_List* R_List_Destructor(R_List* self) {
    R_List_removeAll(self);
    free(self->array);

    return self;
}

inline size_t R_List_size(R_List* self) {
    return self->arraySize;
}

static void R_List_increaseAllocationIfRequired(R_List* self) { //increase by 1 void**
    if (self->arrayAllocationSize > self->arraySize) return;
    self->arrayAllocationSize += 1;
    self->array = (void**)realloc(self->array, self->arrayAllocationSize*sizeof(void*));
    self->array[self->arrayAllocationSize - 1] = NULL;
}

inline void* R_List_pointerAtIndex(R_List* self, size_t index) {
    if (self == NULL || index >= R_List_length(self)) return NULL;
    self->last_index_of_pointer_at_index = index;
    return self->array[index];
}

void* R_List_last(R_List* self) {
    return R_List_pointerAtIndex(self, R_List_size(self)-1);
}

void* R_List_first(R_List* self) {
	return R_List_pointerAtIndex(self, 0);
}

size_t R_List_indexOfPointer(R_List* self, void* pointer) {
    if (self == NULL || pointer == NULL) return -1;
    //last_index_of_pointer_at_index is an optimization for the 'each' operator
    if (pointer == self->array[self->last_index_of_pointer_at_index]) return self->last_index_of_pointer_at_index;
    for (int i=0; i<self->arraySize; i++) {
        if (self->array[i] == pointer)
            return i;
    }
    return -1;
}

void R_List_pop(R_List* self) {
    R_List_removeIndex(self, R_List_size(self)-1);
}

void R_List_swap(R_List* self, int indexA, int indexB) {
    if (indexA > self->arraySize || indexB > self->arraySize || indexA < 0 || indexB < 0)
        return;
    void* temp = self->array[indexA];
    self->array[indexA] = self->array[indexB];
    self->array[indexB] = temp;
}

void* R_List_addObjectOfType(R_List* self, const R_Type* type) {
    if (type == NULL || self == NULL) return NULL;
    R_List_increaseAllocationIfRequired(self);

    void* newPointer = R_Type_NewObjectOfType(type);
    if (newPointer == NULL) return NULL;

    self->array[self->arraySize] = newPointer;
    self->arraySize++;
    
    return newPointer;
}

void* R_List_transferOwnership(R_List* self, void* object) {
    if (self == NULL || object == NULL) return NULL;
    R_List_increaseAllocationIfRequired(self);

    self->array[self->arraySize] = object;
    self->arraySize++;
    
    return object;
}

void* R_List_addCopy(R_List* self, const void* object) {
    if (object == NULL || self == NULL) return NULL;
    R_List_increaseAllocationIfRequired(self);

    void* copy = R_Type_Copy(object);
    if (copy == NULL) return NULL;

    self->array[self->arraySize] = copy;
    self->arraySize++;
    
    return copy;
}

void R_List_removeIndex(R_List* self, size_t index) {
    if (self == NULL || index>=self->arraySize) return;

    void* pointerToDelete = R_List_pointerAtIndex(self, index);
    R_Type_Delete(pointerToDelete);

    for (size_t i=index; i<self->arraySize-1; i++) {
        self->array[i] = self->array[i+1];
    }
    self->arraySize--;
    self->array[self->arraySize] = 0;
}
void R_List_removePointer(R_List* self, void* pointer) {
    if (self == NULL || pointer == NULL) return;

    int i;
    for (i=0; i<self->arraySize; i++) if (self->array[i] == pointer) break;
    if (i >= self->arraySize) return; //No pointer found
    
    R_List_removeIndex(self, i);
    
    //Recurse incase there are more copies
    R_List_removePointer(self, pointer);
}
void R_List_removeAll(R_List* self) {
    if (self == NULL) return;
    for (int i=0; i<self->arraySize; i++) R_Type_Delete(self->array[i]);
    self->arraySize = 0;
}

typedef struct {
    R_Type* type;
    unsigned int previous_index;
    R_List* list; //This is not a copy. Do not call R_Type_Delete on it!
} R_List_Iterator_State;
R_Type_Def(R_List_Iterator_State, NULL, NULL, NULL);

static void* R_List_iterator(R_List_Iterator_State* state) {
    if (state->previous_index == 0) return NULL;
    return R_List_pointerAtIndex(state->list, --state->previous_index);
}

R_Functor* R_List_Iterator(R_Functor* functor, R_List* list) {
    if (functor == NULL || list == NULL) return NULL;
    R_Type_Delete(functor->state);
    R_List_Iterator_State* state = R_Type_New(R_List_Iterator_State);
    state->list = list;
    state->previous_index = R_List_size(list);
    functor->state = state;
    functor->function = (R_Functor_Function)R_List_iterator;
    return functor;
}
