#ifndef R_Type_h
#define R_Type_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stddef.h>


/*  R_Type_Constructor
    Function Pointer for an R_Type constructor. Input is an allocated space of memory. Output
   is an initialized object.
*/
typedef void* (*R_Type_Constructor)(void* object);

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
typedef void (*R_Type_Copier)(const void* object_input, void* object_output);

/*  R_Type
    Struct used to describe an object.
 */
typedef struct {
  size_t size; //Must be at least sizeof(R_Type*) because the first param of all objects must be a R_Type*.
  R_Type_Constructor ctor; //May be NULL. If not, it's called after alloc succeeds during 'new'.
  R_Type_Destructor dtor; //May be NULL. If not, free is called on its result during 'delete'.
  R_Type_Copier copy; //If set to NULL, 'copy' will always fail. If not NULL, it's called during 'copy' to do deep copying.
} R_Type;

#define R_Type_Object(Type) R_Type__ ## Type

#define R_Type_Def(Type, ctor, dtor, copier) const R_Type* R_Type_Object(Type) = &(R_Type){ \
    sizeof(Type), \
    (R_Type_Constructor)ctor, \
    (R_Type_Destructor)dtor, \
    (R_Type_Copier)copier \
  }

#define R_Type_Declare(Type) extern const R_Type* R_Type_Object(Type)


/*  R_Type_New
    Allocates a new object of the given type. Allocates and nulls type->size bytes than calls type->ctor, if it isn't null.
 */
void* R_Type_NewObjectOfType(const R_Type* type);
#define R_Type_New(Type) (Type*)R_Type_NewObjectOfType(R_Type_Object(Type))

/*  R_Type_Delete
    Gives the memory allocated to the given object back to the system. If type->dtor isn't null, free is called on the result
   of it. If type->dtor is null, free is called on the given object.
 */
void R_Type_Delete(void* object);

/*  R_Type_DeleteAndNull
    Convenience macro that runs R_Type_Delete against the object then sets the object to NULL.
 */
#define R_Type_DeleteAndNull(object) do{R_Type_Delete(object);object=NULL;}while(0)

/*  R_Type_Copy
    Deep-copies the given object. If type->copy is null, this will always fail (and return null). If type->copy isn't null, a new
   object of the same type is allocated, type->ctor is run on it (if it isn't null) and type->copy is run on both.
 */
void* R_Type_Copy(const void* object);

/*  R_Type_Type(void* object);
    Returns whether the object is the given type/class. Casts the given object to an R_Type** then dereferences and compares it. Useful as shorthand.
 */
int R_Type_IsObjectOfType(const void* object, const R_Type* type);
#define R_Type_IsOf(object, Type) R_Type_IsObjectOfType(object, R_Type_Object(Type))
int R_Type_IsObjectNotOfType(const void* object, const R_Type* type);
#define R_Type_IsNotOf(object, Type) R_Type_IsObjectNotOfType(object, R_Type_Object(Type))


/*  R_Type_BytesAllocated
    Number of bytes currently in-use. Mostly just useful for testing or profiling.
 */
extern size_t R_Type_BytesAllocated;

/* R_Type_shallowCopy
   A basic R_Type_Copier for objects without any internally-allocated memory.
 */
void R_Type_shallowCopy(const void* object_input, void* object_output);

/* R_Integer
   A basic integer type.
 */
typedef struct {R_Type* type; int Integer;} R_Integer;
R_Type_Declare(R_Integer);

/* R_Float
   A basic floating point type.
 */
typedef struct {R_Type* type; float Float;} R_Float;
R_Type_Declare(R_Float);

/* R_Unsigned
   A basic unsigned integer type.
 */
typedef struct {R_Type* type; unsigned int Integer;} R_Unsigned;
R_Type_Declare(R_Unsigned);

#endif
