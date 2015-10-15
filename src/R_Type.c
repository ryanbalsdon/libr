#include <stdlib.h>
#include <string.h>
#include "R_Type.h"

size_t R_Type_BytesAllocated = 0;

void* R_FUNCTION_ATTRIBUTES R_Type_NewObjectOfType(const R_Type* type) {
  if (type->size < sizeof(R_Type*)) return NULL; //If they were equal, this object would be useless. No good reason to limit that though...
  void* new_object = type->alloc ? type->alloc(type->size) : (void*)os_zalloc(type->size);
  if (new_object == NULL) return NULL;
  *(const R_Type**)new_object = type;
  R_Type_BytesAllocated += type->size;
  if (type->ctor != NULL && type->ctor(new_object) == NULL) {
    //Constructor has failed
    if (type->dtor != NULL) type->dtor(new_object);
    os_free(new_object);
    R_Type_BytesAllocated -= type->size;
    return NULL;
  }
  return new_object;
}

void R_FUNCTION_ATTRIBUTES R_Type_Delete(void* object) {
  if (object == NULL) return;
  R_Type* type = *(R_Type**)object; //First element of every object must be an R_Type*
  R_Type_BytesAllocated -= type->size;
  if (type->dtor != NULL) os_free(type->dtor(object));
  else os_free(object);
}

void* R_FUNCTION_ATTRIBUTES R_Type_Copy(const void* object) {
  if (object == NULL) return NULL;
  R_Type* type = *(R_Type**)object; //First element of every object must be an R_Type*
  if (type->copy == NULL) return NULL;

  void* new_object = R_Type_NewObjectOfType(type);
  if (new_object != NULL) return type->copy(object, new_object);
  return NULL;
}

int R_FUNCTION_ATTRIBUTES R_Type_IsObjectOfType(const void* object, const R_Type* type) {
  if (object == NULL || type == NULL) return 0;
  R_Type* type_of_object = *(R_Type**)object; //First element of every object must be an R_Type*
  if (type_of_object != type) return 0;
  return 1;
}

int R_FUNCTION_ATTRIBUTES R_Type_IsObjectNotOfType(const void* object, const R_Type* type) {
  return !R_Type_IsObjectOfType(object, type);
}

void* R_FUNCTION_ATTRIBUTES R_Type_shallowCopy(const void* object_input, void* object_output) {
  if (object_input == NULL || object_output == NULL) return NULL;
  R_Type* input_type  = *(R_Type**)object_input;
  R_Type* output_type = *(R_Type**)object_output;
  if (input_type->size != output_type->size) return NULL;
  os_memcpy(object_output, object_input, input_type->size);
  return object_output;
}

void R_Puts(void* object) {
  if (R_Type_hasNoMethod(object, R_Stringify)) return;
  char* buffer = (char*)os_zalloc(2048);
  if (buffer) {
    size_t bytes = R_Type_call(object, R_Stringify, object, buffer, 2048);
    os_printf("%.*s\n", (int)bytes, buffer);
  }
  os_free(buffer);
}

R_JumpTable_DefineKey(R_Stringify);
R_JumpTable_DefineKey(R_Equals);
