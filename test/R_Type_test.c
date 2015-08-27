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
Testor* Testor_Copier(Testor* testor, Testor* new_testor) {
  Testor_Copier_Called++;
  new_testor->test = testor->test;
  return new_testor;
}

R_Type_Def(Testor, NULL, NULL, NULL, NULL);
R_Type_Def(ConstructorTestor, Testor_Constructor, NULL, NULL, NULL);
R_Type_Def(DestructorTestor, NULL, Testor_Destructor, NULL, NULL);
R_Type_Def(CopierTestor, NULL, NULL, Testor_Copier, NULL);
R_Type_Def(FullTestor, Testor_Constructor, Testor_Destructor, Testor_Copier, NULL);

void test_simple(void) {
  assert(R_Type_Object(Testor)->size == sizeof(Testor));
  assert(R_Type_Object(Testor)->ctor == NULL);
  assert(R_Type_Object(Testor)->dtor == NULL);
  assert(R_Type_Object(Testor)->copy == NULL);

  Testor* testor = R_Type_New(Testor);
  assert(testor != NULL);
  assert(testor->type == R_Type_Object(Testor));
  assert(R_Type_IsOf(testor, Testor));
  assert(!R_Type_IsOf(testor, FullTestor));
  assert(R_Type_IsNotOf(testor, FullTestor));
  assert(R_Type_BytesAllocated == sizeof(Testor));
  R_Type_Delete(testor);
  assert(R_Type_BytesAllocated == 0);
}

void test_constructor(void) {
  assert(R_Type_Object(ConstructorTestor)->size == sizeof(Testor));
  assert(R_Type_Object(ConstructorTestor)->ctor == (R_Type_Constructor)Testor_Constructor);
  assert(R_Type_Object(ConstructorTestor)->dtor == NULL);
  assert(R_Type_Object(ConstructorTestor)->copy == NULL);

  assert(Testor_Constructor_Called == 0);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);

  assert(Testor_Constructor_Called == 0);
  Testor* testor = R_Type_New(ConstructorTestor);
  assert(testor != NULL);
  assert(testor->type == R_Type_Object(ConstructorTestor));
  assert(R_Type_IsOf(testor, ConstructorTestor));
  assert(R_Type_IsNotOf(testor, Testor));
  assert(Testor_Constructor_Called == 1);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);
  assert(R_Type_BytesAllocated == sizeof(Testor));
  R_Type_Delete(testor);
  assert(R_Type_BytesAllocated == 0);

  Testor_Constructor_Called = 0;
}

void test_destructor(void) {
  assert(R_Type_Object(DestructorTestor)->size == sizeof(Testor));
  assert(R_Type_Object(DestructorTestor)->ctor == NULL);
  assert(R_Type_Object(DestructorTestor)->dtor == (R_Type_Destructor)Testor_Destructor);
  assert(R_Type_Object(DestructorTestor)->copy == NULL);

  assert(Testor_Constructor_Called == 0);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);

  Testor* testor = R_Type_New(DestructorTestor);
  assert(testor != NULL);
  assert(testor->type == R_Type_Object(DestructorTestor));
  assert(R_Type_IsOf(testor, DestructorTestor));
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
  assert(R_Type_Object(CopierTestor)->size == sizeof(Testor));
  assert(R_Type_Object(CopierTestor)->ctor == NULL);
  assert(R_Type_Object(CopierTestor)->dtor == NULL);
  assert(R_Type_Object(CopierTestor)->copy == (R_Type_Copier)Testor_Copier);

  assert(Testor_Constructor_Called == 0);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);

  Testor* testor = R_Type_New(CopierTestor);
  assert(testor != NULL);
  assert(testor->type == R_Type_Object(CopierTestor));
  assert(R_Type_IsOf(testor, CopierTestor));
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
  assert(R_Type_Object(FullTestor)->size == sizeof(Testor));
  assert(R_Type_Object(FullTestor)->ctor == (R_Type_Constructor)Testor_Constructor);
  assert(R_Type_Object(FullTestor)->dtor == (R_Type_Destructor)Testor_Destructor);
  assert(R_Type_Object(FullTestor)->copy == (R_Type_Copier)Testor_Copier);

  assert(Testor_Constructor_Called == 0);
  assert(Testor_Destructor_Called == 0);
  assert(Testor_Copier_Called == 0);

  Testor* testor = R_Type_New(FullTestor);
  assert(testor != NULL);
  assert(testor->type == R_Type_Object(FullTestor));
  assert(R_Type_IsOf(testor, FullTestor));
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

bool BadConstructorTestor_Constructor_Called = false;
bool BadConstructorTestor_Destructor_Called = false;
typedef struct Testor BadConstructorTestor;
BadConstructorTestor* Bad_Constructor(BadConstructorTestor* self) {
  BadConstructorTestor_Constructor_Called = true;
  if (1 /* there is an error */)
    return NULL;
}
BadConstructorTestor* Bad_Constructor_Destructor(BadConstructorTestor* self) {
  BadConstructorTestor_Destructor_Called = true;
  //Generic clean-up...
  return self;
}
R_Type_Def(BadConstructorTestor, Bad_Constructor, Bad_Constructor_Destructor, NULL, NULL);

void test_bad_constructor(void) {
  BadConstructorTestor* testor = R_Type_New(BadConstructorTestor);
  assert(testor == NULL);
  assert(BadConstructorTestor_Constructor_Called);
  assert(BadConstructorTestor_Destructor_Called);
}

int main(void) {
  test_simple();
  test_constructor();
  test_destructor();
  test_copier();
  test_full();
  test_bad_constructor();

  assert(R_Type_BytesAllocated == 0);
  printf("Pass\n");

  return 0;
}
