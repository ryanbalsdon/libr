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
#include "R_Functor.h"

static R_Functor* R_Functor_Destructor(R_Functor* self);
static void R_Functor_Copier(R_Functor* self, R_Functor* new);
R_Type_Def(R_Functor, NULL, R_Functor_Destructor, R_Functor_Copier);

static R_Functor* R_Functor_Destructor(R_Functor* self) {
	R_Type_Delete(self->state);
	return self;
}

static void R_Functor_Copier(R_Functor* self, R_Functor* new) {
	new->function = self->function;
	new->state = R_Type_Copy(self->state);
}

void* R_Functor_call(R_Functor* self) {
	if (self == NULL || self->function == NULL) return NULL;
	self->function(self->state);
	return self->state;
}
