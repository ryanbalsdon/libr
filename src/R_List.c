#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "R_OS.h"
#include "R_List.h"


struct R_List {
    R_Type* type;
    void ** array;          //The actual array
    size_t arrayAllocationSize;//How large the internal array is. This is always as-large or larger than ArraySize.
    size_t arraySize;          //How many objects the user has added to the array.
    size_t last_index_of_pointer_at_index; //Optimization for the 'each' operator
};

static R_List* R_FUNCTION_ATTRIBUTES R_List_Constructor(R_List* self);
static R_List* R_FUNCTION_ATTRIBUTES R_List_Destructor(R_List* self);
static R_List* R_FUNCTION_ATTRIBUTES R_List_Copier(R_List* self, R_List* new);
static R_JumpTable_Entry methods[] = {
  R_JumpTable_Entry_Make(R_Stringify, R_List_stringify), 
  R_JumpTable_Entry_NULL
};
R_Type_Def(R_List, R_List_Constructor, R_List_Destructor, R_List_Copier, methods);

static void R_FUNCTION_ATTRIBUTES R_List_increaseAllocationIfRequired(R_List* self);

static R_List* R_FUNCTION_ATTRIBUTES R_List_Constructor(R_List* self) {
    self->array = NULL;
    self->arrayAllocationSize = 0;
    self->arraySize = 0;

    return self;
}

static R_List* R_FUNCTION_ATTRIBUTES R_List_Destructor(R_List* self) {
    R_List_removeAll(self);
    os_free(self->array);

    return self;
}

static R_List* R_FUNCTION_ATTRIBUTES R_List_Copier(R_List* self, R_List* new) {
    return R_List_appendList(new, self);
}

inline size_t R_FUNCTION_ATTRIBUTES R_List_size(R_List* self) {
    return self->arraySize;
}

static void R_FUNCTION_ATTRIBUTES R_List_increaseAllocationIfRequired(R_List* self) { //increase by 1 void**
    if (self->arrayAllocationSize > self->arraySize) return;
    self->arrayAllocationSize += 1;
    self->array = (void**)os_realloc(self->array, self->arrayAllocationSize*sizeof(void*));
    self->array[self->arrayAllocationSize - 1] = NULL;
}

inline void* R_FUNCTION_ATTRIBUTES R_List_pointerAtIndex(R_List* self, size_t index) {
    if (self == NULL || index >= R_List_length(self)) return NULL;
    self->last_index_of_pointer_at_index = index;
    return self->array[index];
}

void* R_FUNCTION_ATTRIBUTES R_List_last(R_List* self) {
    return R_List_pointerAtIndex(self, R_List_size(self)-1);
}

void* R_FUNCTION_ATTRIBUTES R_List_first(R_List* self) {
    return R_List_pointerAtIndex(self, 0);
}

size_t R_FUNCTION_ATTRIBUTES R_List_indexOfPointer(R_List* self, void* pointer) {
    if (self == NULL || pointer == NULL) return -1;
    //last_index_of_pointer_at_index is an optimization for the 'each' operator
    if (pointer == self->array[self->last_index_of_pointer_at_index]) return self->last_index_of_pointer_at_index;
    for (int i=0; i<self->arraySize; i++) {
        if (self->array[i] == pointer)
            return i;
    }
    return -1;
}

void R_FUNCTION_ATTRIBUTES R_List_pop(R_List* self) {
    R_List_removeIndex(self, R_List_size(self)-1);
}

void R_FUNCTION_ATTRIBUTES R_List_shift(R_List* self) {
    R_List_removeIndex(self, 0);
}

void R_FUNCTION_ATTRIBUTES R_List_swap(R_List* self, int indexA, int indexB) {
    if (indexA > self->arraySize || indexB > self->arraySize || indexA < 0 || indexB < 0)
        return;
    void* temp = self->array[indexA];
    self->array[indexA] = self->array[indexB];
    self->array[indexB] = temp;
}

void* R_FUNCTION_ATTRIBUTES R_List_addObjectOfType(R_List* self, const R_Type* type) {
    if (type == NULL || self == NULL) return NULL;
    R_List_increaseAllocationIfRequired(self);

    void* newPointer = R_Type_NewObjectOfType(type);
    if (newPointer == NULL) return NULL;

    self->array[self->arraySize] = newPointer;
    self->arraySize++;
    
    return newPointer;
}

void* R_FUNCTION_ATTRIBUTES R_List_appendList(R_List* self, R_List* list) {
  if (R_Type_IsNotOf(self, R_List) || R_Type_IsNotOf(list, R_List)) return NULL;
  int original_size = R_List_size(list); //Buffer because self and list may be the same object
  for (int i=0; i<original_size; i++) {
    void* object = R_List_pointerAtIndex(list, i);
    if (R_List_addCopy(self, object) == NULL) return NULL;
  }
  return self;
}

void* R_FUNCTION_ATTRIBUTES R_List_transferList(R_List* self, R_List* list) {
  if (R_Type_IsNotOf(self, R_List) || R_Type_IsNotOf(list, R_List)) return NULL;
  for (int i=R_List_size(list)-1; i>=0; i--) {
    void* object = R_List_pointerAtIndex(list, i);
    if (R_List_transferOwnership(self, object) == NULL) return NULL;
  }
  return self;

}

void* R_FUNCTION_ATTRIBUTES R_List_transferOwnership(R_List* self, void* object) {
    if (self == NULL || object == NULL) return NULL;
    R_List_increaseAllocationIfRequired(self);

    self->array[self->arraySize] = object;
    self->arraySize++;
    
    return object;
}

void* R_FUNCTION_ATTRIBUTES R_List_addCopy(R_List* self, const void* object) {
    if (object == NULL || self == NULL) return NULL;
    R_List_increaseAllocationIfRequired(self);

    void* copy = R_Type_Copy(object);
    if (copy == NULL) return NULL;

    self->array[self->arraySize] = copy;
    self->arraySize++;
    
    return copy;
}

void R_FUNCTION_ATTRIBUTES R_List_removeIndex(R_List* self, size_t index) {
    if (self == NULL || index>=self->arraySize) return;

    void* pointerToDelete = R_List_pointerAtIndex(self, index);
    R_Type_Delete(pointerToDelete);

    for (size_t i=index; i<self->arraySize-1; i++) {
        self->array[i] = self->array[i+1];
    }
    self->arraySize--;
    self->array[self->arraySize] = 0;
}
void R_FUNCTION_ATTRIBUTES R_List_removePointer(R_List* self, void* pointer) {
    if (self == NULL || pointer == NULL) return;

    int i;
    for (i=0; i<self->arraySize; i++) if (self->array[i] == pointer) break;
    if (i >= self->arraySize) return; //No pointer found
    
    R_List_removeIndex(self, i);
    
    //Recurse incase there are more copies
    R_List_removePointer(self, pointer);
}
void R_FUNCTION_ATTRIBUTES R_List_removeAll(R_List* self) {
    if (self == NULL) return;
    for (int i=0; i<self->arraySize; i++) R_Type_Delete(self->array[i]);
    self->arraySize = 0;
}

size_t R_FUNCTION_ATTRIBUTES R_List_stringify(R_List* self, char* buffer, size_t size) {
  if (R_Type_IsNotOf(self, R_List)) return 0;
  char* buffer_head = buffer;
  size_t this_size = os_snprintf(buffer, size, "List of %zu:\n", R_List_size(self));
  if (this_size >= size) return this_size;
  size -= this_size;
  buffer += this_size;

  R_List_each(self, void, item) {
    this_size = 0;
    if (R_Type_hasMethod(item, R_Stringify)) {
      this_size = R_Stringify(item, buffer, size);
    } else {
      this_size = snprintf(buffer, size, "Unknown Type");
    }
    if (this_size >= size) return buffer - buffer_head;
    size -= this_size;
    buffer += this_size;

    this_size = snprintf(buffer, size, "\n");
    if (this_size >= size) return buffer - buffer_head;
    size -= this_size;
    buffer += this_size;
  }

  return buffer - buffer_head;
}
