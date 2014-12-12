#ifndef R_Object_h
#define R_Object_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stddef.h>


/*  R_Object_Constructor
    Function Pointer for an R_Object constructor. Input is an allocated space of memory. Output
   is an initialized object.
*/
typedef void* (*R_Object_Constructor)(void* object);

/*  R_Object_Destructor
    Function Pointer for an R_Object destructor. Input is an initialized object. free() is called
   on the output. This is used to clean up internally-allocated memory of the object and return the
   cleaned object.
*/
typedef void* (*R_Object_Destructor)(void* object);

/*  R_Object_Copier
    Function Pointer for an R_Object copier. Used to deep-copy an object. object_input is the
   object to copy. object_output is a newly allocated and inited object of the same type.
*/
typedef void (*R_Object_Copier)(void* object_input, void* object_output);

/*  R_Object
    Struct used to describe an object.
 */
typedef struct {
  size_t size; //Must be at least sizeof(R_Object*) because the first param of all objects must be a R_Object*.
  R_Object_Constructor ctor; //May be NULL. If not, it's called after alloc succeeds during 'new'.
  R_Object_Destructor dtor; //May be NULL. If not, free is called on its result during 'delete'.
  R_Object_Copier copy; //If set to NULL, 'copy' will always fail. If not NULL, it's called during 'copy' to do deep copying.
} R_Object;

#define R_Object_Typedef(Type, ctor, dtor, copier) static const R_Object* Type ## _Type = &(R_Object){ \
    sizeof(Type), \
    (R_Object_Constructor)ctor, \
    (R_Object_Destructor)dtor, \
    (R_Object_Copier)copier \
  }

/*  R_Object_New
    Allocates a new object of the given type. Allocates and nulls type->size bytes than calls type->ctor, if it isn't null.
 */
void* R_Object_New(const R_Object* type);

/*  R_Object_Delete
    Gives the memory allocated to the given object back to the system. If type->dtor isn't null, free is called on the result
   of it. If type->dtor is null, free is called on the given object.
 */
void R_Object_Delete(void* object);

/*  R_Object_Copy
    Deep-copies the given object. If type->copy is null, this will always fail (and return null). If type->copy isn't null, a new
   object of the same type is allocated, type->ctor is run on it (if it isn't null) and type->copy is run on both.
 */
void* R_Object_Copy(void* object);

/*  R_Object_BytesAllocated
    Number of bytes currently in-use. Mostly just useful for testing or profiling.
*/
extern size_t R_Object_BytesAllocated;

#endif