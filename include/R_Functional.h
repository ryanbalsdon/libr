#ifndef R_Functional_h
#define R_Functional_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include "R_Type.h"

typedef void (*R_Functor_Function)(void* state);
typedef struct {
	R_Type* type;
	void* state; //Must be an instance of an R_Type object because R_Type_Delete will be called against it
	R_Functor_Function function;
} R_Functor;
R_Type_Declare(R_Functor);

void* R_Functor_call(R_Functor*); //Returns the functor's state object

#define R_ObjectArray_each(array, TYPE, NAME) for(TYPE* NAME=NULL; (NAME=(TYPE*)R_ObjectArray_pointerAtIndex(array, R_ObjectArray_indexOfPointer(array, NAME)+1));)

#endif /*R_Functional_h*/