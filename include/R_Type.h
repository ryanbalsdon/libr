#ifndef R_Type_h
#define R_Type_h

#include "R_OS.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "R_JumpTable.h"

/*  R_Type_Constructor
    Function Pointer for an R_Type constructor. Input is an allocated space of memory. Output
   is an initialized object.
*/
typedef void* (*R_Type_Constructor)(void* object);

/*  R_Type_Allocator
    Function Pointer for a memory allocator. Input is the size of the object to allocate. Output
   is a pointer to a chunk of zero-initialized memory or NULL on error.
*/
typedef void* (*R_Type_Allocator)(size_t size);

/*  R_Type_Destructor
    Function Pointer for an R_Type destructor. Input is an initialized object. free() is called
   on the output. This is used to clean up internally-allocated memory of the object and return the
   cleaned object.
*/
typedef void* (*R_Type_Destructor)(void* object);

/*  R_Type_Copier
    Function Pointer for an R_Type copier. Used to deep-copy an object. object_input is the
   object to copy. object_output is a newly allocated and inited object of the same type.
*/
typedef void* (*R_Type_Copier)(const void* object_input, void* object_output);

/*  R_Type
    Struct used to describe an object.
 */
typedef struct {
  size_t size; //Must be at least sizeof(R_Type*) because the first param of all objects must be a R_Type*.
  R_Type_Allocator alloc; //May be NULL. If not, zalloc is used.
  R_Type_Constructor ctor; //May be NULL. If not, it's called after alloc succeeds during 'new'.
  R_Type_Destructor dtor; //May be NULL. If not, free is called on its result during 'delete'.
  R_Type_Copier copy; //If set to NULL, 'copy' will always fail. If not NULL, it's called during 'copy' to do deep copying.
  R_JumpTable* interfaces; //A jump table to define implemented interfaces.
} R_Type;

#define R_Type_Object(Type) R_Type__ ## Type

#define R_Type_Def(Type, ctor, dtor, copier, jump_table) const R_Type* R_Type_Object(Type) = &(R_Type){ \
    sizeof(Type), \
    NULL, \
    (R_Type_Constructor)ctor, \
    (R_Type_Destructor)dtor, \
    (R_Type_Copier)copier, \
    jump_table \
  }

#define R_Type_Define(Type, ...) const R_Type* R_Type_Object(Type) = &(R_Type){ .size = sizeof(Type), __VA_ARGS__ }

#define R_Type_Declare(Type) extern const R_Type* R_Type_Object(Type)

/*  R_Type_New
    Allocates a new object of the given type. Allocates and nulls type->size bytes than calls type->ctor, if it isn't null.
 */
void* R_FUNCTION_ATTRIBUTES R_Type_NewObjectOfType(const R_Type* type);
#define R_Type_New(Type) (Type*)R_Type_NewObjectOfType(R_Type_Object(Type))

/*  R_Type_Delete
    Gives the memory allocated to the given object back to the system. If type->dtor isn't null, free is called on the result
   of it. If type->dtor is null, free is called on the given object.
 */
void R_FUNCTION_ATTRIBUTES R_Type_Delete(void* object);

/*  R_Type_DeleteAndNull
    Convenience macro that runs R_Type_Delete against the object then sets the object to NULL.
 */
#define R_Type_DeleteAndNull(object) do{R_Type_Delete(object);object=NULL;}while(0)

/*  R_Type_Copy
    Deep-copies the given object. If type->copy is null, this will always fail (and return null). If type->copy isn't null, a new
   object of the same type is allocated, type->ctor is run on it (if it isn't null) and type->copy is run on both.
 */
void* R_FUNCTION_ATTRIBUTES R_Type_Copy(const void* object);

/*  R_Type_Type(void* object);
    Returns whether the object is the given type/class. Casts the given object to an R_Type** then dereferences and compares it. Useful as shorthand.
 */
int R_FUNCTION_ATTRIBUTES R_Type_IsObjectOfType(const void* object, const R_Type* type);
#define R_Type_IsOf(object, Type) R_Type_IsObjectOfType(object, R_Type_Object(Type))
int R_FUNCTION_ATTRIBUTES R_Type_IsObjectNotOfType(const void* object, const R_Type* type);
#define R_Type_IsNotOf(object, Type) R_Type_IsObjectNotOfType(object, R_Type_Object(Type))
#define R_Type_Of(object) ((object==NULL)?NULL:(*(R_Type**)object))

#define R_Type_call(object, method, ...) R_JumpTable_call((R_Type_Of(object)?R_Type_Of(object)->interfaces:NULL), method, __VA_ARGS__)
#define R_Type_hasMethod(object, method) R_JumpTable_hasEntry((R_Type_Of(object)?R_Type_Of(object)->interfaces:NULL), method)
#define R_Type_hasNoMethod(object, method) (!R_Type_hasMethod(object, method))

/*  R_Type_BytesAllocated
    Number of bytes currently in-use. Mostly just useful for testing or profiling.
 */
extern size_t R_Type_BytesAllocated;

void R_Puts(void* object);
#define R_Stringify(object, buffer, size) R_Type_call(object, R_Stringify, object, buffer, size)
R_JumpTable_DeclareKey(R_Stringify);
R_JumpTable_DeclareFunction(R_Stringify, size_t, void*, char*, size_t);

#define R_Equals(object1, object2) R_Type_call(object1, R_Equals, object1, object2)
R_JumpTable_DeclareKey(R_Equals);
R_JumpTable_DeclareFunction(R_Equals, bool, void*, void*);


#include "R_Type_Builtins.h"

#endif
