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
typedef void (*R_Type_Copier)(void* object_input, void* object_output);

/*  R_Type
    Struct used to describe an object.
 */
typedef struct {
  size_t size; //Must be at least sizeof(R_Type*) because the first param of all objects must be a R_Type*.
  R_Type_Constructor ctor; //May be NULL. If not, it's called after alloc succeeds during 'new'.
  R_Type_Destructor dtor; //May be NULL. If not, free is called on its result during 'delete'.
  R_Type_Copier copy; //If set to NULL, 'copy' will always fail. If not NULL, it's called during 'copy' to do deep copying.
} R_Type;

#define R_Type_Def(Type, ctor, dtor, copier) const R_Type* Type ## _Type = &(R_Type){ \
    sizeof(Type), \
    (R_Type_Constructor)ctor, \
    (R_Type_Destructor)dtor, \
    (R_Type_Copier)copier \
  }

/*  R_Type_New
    Allocates a new object of the given type. Allocates and nulls type->size bytes than calls type->ctor, if it isn't null.
 */
void* R_Type_New(const R_Type* type);

/*  R_Type_Delete
    Gives the memory allocated to the given object back to the system. If type->dtor isn't null, free is called on the result
   of it. If type->dtor is null, free is called on the given object.
 */
void R_Type_Delete(void* object);

/*  R_Type_Copy
    Deep-copies the given object. If type->copy is null, this will always fail (and return null). If type->copy isn't null, a new
   object of the same type is allocated, type->ctor is run on it (if it isn't null) and type->copy is run on both.
 */
void* R_Type_Copy(void* object);

/*  R_Type_Type(void* object);
    Returns the type/class of the object. Casts the given object to an R_Type** then dereferences it. Useful as shorthand.
 */
R_Type* R_Type_Type(void* object);

/*  R_Type_BytesAllocated
    Number of bytes currently in-use. Mostly just useful for testing or profiling.
*/
extern size_t R_Type_BytesAllocated;

#endif