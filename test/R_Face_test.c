/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "R_Face.h"
#include "R_Type.h"

//References to a selector. These would normally be in a header file
R_Face_Declare(puts, void* self)
R_Face_Declare(init, void* self)
R_Face_Declare(change, void* self, int new)
R_Face_Declare(not_implemented, void* self)

//Definition of the selector. Guts don't matter, we just need a unique pointer
R_Face_Def(puts);
R_Face_Def(init);
R_Face_Def(change);
R_Face_Def(not_implemented);

//Callers are needed because doing tail-call recursion in the jump table is difficult
R_Face_DeclareCaller(puts, void* self) {R_Face_DefCaller(puts, self);}
R_Face_DeclareCaller(init, void* self) {R_Face_DefCaller(init, self);}
R_Face_DeclareCaller(change, void* self, int new) {R_Face_DefCaller(change, self, new);}
R_Face_DeclareCaller(not_implemented, void* self) {R_Face_DefCaller(not_implemented, self);}


typedef struct {
  R_Type* type;
  int pass_fail;
} AwesomeClass;
void* AwesomeClass_jumpTable(const R_Face* interface); //takes an interface/selector/method-name and returns the implementation
R_Type_Def(AwesomeClass, NULL, NULL, NULL, AwesomeClass_jumpTable);

void* AwesomeClass_puts(AwesomeClass* self) {
  if (self->pass_fail == 1) printf("Pass\n");
  else printf("Fail\n");
  self->pass_fail = 0;
  return self;
}

void* AwesomeClass_constructor(AwesomeClass* self) {
  self->pass_fail = 2;
  return self;
}

void* AwesomeClass_whatever(AwesomeClass* self, int new) {
  self->pass_fail = new;
  return self;
}

void* AwesomeClass_jumpTable(const R_Face* interface) {
  //This is the list of methods/interfaces that the class/object implements
  R_Face_DefJump(puts, AwesomeClass_puts);
  R_Face_DefJump(init, AwesomeClass_constructor);
  R_Face_DefJump(change, AwesomeClass_whatever);
  return NULL; /* use super's jump table? */
}

int main(void) {
  AwesomeClass* self = R_Type_New(AwesomeClass);
  assert(R_Type_Call(self, not_implemented, self) == NULL);
  assert(R_Type_Call(self, init, self) == self);
  assert(self->pass_fail == 2);
  assert(R_Type_Call(self, change, self, 1) == self);
  assert(self->pass_fail == 1);
  assert(R_Type_Call(self, puts, self) == self);
  assert(self->pass_fail == 0);
}
