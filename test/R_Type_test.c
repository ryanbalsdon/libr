/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "R_Type.h"

struct Testor {
  R_Type* type;
  long test; //just to double-check that it's being nulled
};

typedef struct Testor Testor;
typedef struct Testor ConstructorTestor;
typedef struct Testor DestructorTestor;
typedef struct Testor CopierTestor;
typedef struct Testor FullTestor;

int Testor_Constructor_Called = 0;
Testor* Testor_Constructor(Testor* testor) {
  Testor_Constructor_Called++;
  testor->test = 42;
  return testor;
}

int Testor_Destructor_Called = 0;
Testor* Testor_Destructor(Testor* testor) {
  Testor_Destructor_Called++;
  return testor;
}

int Testor_Copier_Called = 0;
void Testor_Copier(Testor* testor, Testor* new_testor) {
  Testor_Copier_Called++;
  new_testor->test = testor->test;
}

R_Type_Def(Testor, NULL, NULL, NULL);
R_Type_Def(ConstructorTestor, Testor_Constructor, NULL, NULL);
R_Type_Def(DestructorTestor, NULL, Testor_Destructor, NULL);
R_Type_Def(CopierTestor, NULL, NULL, Testor_Copier);
R_Type_Def(FullTestor, Testor_Constructor, Testor_Destructor, Testor_Copier);

void test_simple(void) {
  assert(Testor_Type->size == sizeof(Testor));
  assert(Testor_Type->ctor == NULL);
  assert(Testor_Type->dtor == NULL);
  assert(Testor_Type->copy == NULL);

  Testor* testor = R_Type_New(Testor_Type);
  assert(testor != NULL);
  assert(testor->type == Testor_Type);
  assert(R_Type_BytesAllocated == sizeof(Testor));
  R_Type_Delete(testor);
  assert(R_Type_BytesAllocated == 0);
}

void test_constructor(void) {
  assert(ConstructorTestor_Type->size == sizeof(Testor));
  assert(ConstructorTestor_Type->ctor == (R_Type_Constructor)Testor_Constructor);
  assert(ConstructorTestor_Type->dtor == NULL);
  assert(ConstructorTestor_Type->copy == NULL);

  assert(Testor_Constructor_Called == 0);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);

  assert(Testor_Constructor_Called == 0);
  Testor* testor = R_Type_New(ConstructorTestor_Type);
  assert(testor != NULL);
  assert(testor->type == ConstructorTestor_Type);
  assert(Testor_Constructor_Called == 1);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);
  assert(R_Type_BytesAllocated == sizeof(Testor));
  R_Type_Delete(testor);
  assert(R_Type_BytesAllocated == 0);

  Testor_Constructor_Called = 0;
}

void test_destructor(void) {
  assert(DestructorTestor_Type->size == sizeof(Testor));
  assert(DestructorTestor_Type->ctor == NULL);
  assert(DestructorTestor_Type->dtor == (R_Type_Destructor)Testor_Destructor);
  assert(DestructorTestor_Type->copy == NULL);

  assert(Testor_Constructor_Called == 0);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);

  Testor* testor = R_Type_New(DestructorTestor_Type);
  assert(testor != NULL);
  assert(testor->type == DestructorTestor_Type);
  assert(Testor_Constructor_Called == 0);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);
  assert(R_Type_BytesAllocated == sizeof(Testor));
  R_Type_Delete(testor);
  assert(Testor_Destructor_Called == 1);
  assert(R_Type_BytesAllocated == 0);

  Testor_Destructor_Called = 0;
}

void test_copier(void) {
  assert(CopierTestor_Type->size == sizeof(Testor));
  assert(CopierTestor_Type->ctor == NULL);
  assert(CopierTestor_Type->dtor == NULL);
  assert(CopierTestor_Type->copy == (R_Type_Copier)Testor_Copier);

  assert(Testor_Constructor_Called == 0);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);

  Testor* testor = R_Type_New(CopierTestor_Type);
  assert(testor != NULL);
  assert(testor->type == CopierTestor_Type);
  assert(R_Type_BytesAllocated == sizeof(Testor));

  testor->test = 1001;
  Testor* testor_copy = R_Type_Copy(testor);
  assert(R_Type_BytesAllocated == 2*sizeof(Testor));
  assert(Testor_Constructor_Called == 0);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 1);
  assert(testor_copy->test == 1001);

  R_Type_Delete(testor);
  R_Type_Delete(testor_copy);
  assert(R_Type_BytesAllocated == 0);

  Testor_Copier_Called = 0;
}

void test_full(void) {
  assert(FullTestor_Type->size == sizeof(Testor));
  assert(FullTestor_Type->ctor == (R_Type_Constructor)Testor_Constructor);
  assert(FullTestor_Type->dtor == (R_Type_Destructor)Testor_Destructor);
  assert(FullTestor_Type->copy == (R_Type_Copier)Testor_Copier);

  assert(Testor_Constructor_Called == 0);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);

  Testor* testor = R_Type_New(FullTestor_Type);
  assert(testor != NULL);
  assert(testor->type == FullTestor_Type);
  assert(Testor_Constructor_Called == 1);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);
  assert(R_Type_BytesAllocated == sizeof(Testor));
  R_Type_Delete(testor);
  assert(Testor_Constructor_Called == 1);
  assert(Testor_Destructor_Called == 1);
  assert(Testor_Copier_Called == 0);
  assert(R_Type_BytesAllocated == 0);

  Testor_Constructor_Called = 0;
  Testor_Destructor_Called = 0;
}

int main(void) {
  test_simple();
  test_constructor();
  test_destructor();
  test_copier();
  test_full();

  printf("Pass\n");
}