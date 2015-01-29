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

typedef struct Testor BadConstructorTestor;
BadConstructorTestor* Bad_Constructor(BadConstructorTestor* testor) {
  //Assume some setup goes wrong. Clean it up then:
  return NULL;
}
R_Type_Def(BadConstructorTestor, Bad_Constructor, NULL, NULL);

void test_bad_constructor(void) {
  BadConstructorTestor* testor = R_Type_New(BadConstructorTestor);
  assert(testor == NULL);
}

void test_integer(void) {
  R_Integer* value = R_Type_New(R_Integer);
  assert(R_Integer_get(value) == 0);
  R_Integer_set(value, 42);
  assert(R_Integer_get(value) == 42);
  R_Integer* value_copy = R_Type_Copy(value);
  assert(R_Integer_get(value_copy) == 42);
  R_Type_Delete(value);
  assert(R_Integer_get(value_copy) == 42);
  R_Type_Delete(value_copy);
}

void test_float(void) {
  R_Float* value = R_Type_New(R_Float);
  assert(R_Float_get(value) == 0.0f);
  R_Float_set(value, 4.2f);
  assert(R_Float_get(value) == 4.2f);
  R_Float* value_copy = R_Type_Copy(value);
  assert(R_Float_get(value_copy) == 4.2f);
  R_Type_Delete(value);
  assert(R_Float_get(value_copy) == 4.2f);
  R_Type_Delete(value_copy);
}

void test_unsigned(void) {
  R_Unsigned* value = R_Type_New(R_Unsigned);
  assert(R_Unsigned_get(value) == 0);
  R_Unsigned_set(value, -1);
  assert(R_Unsigned_get(value) > 0);
  R_Unsigned* value_copy = R_Type_Copy(value);
  assert(R_Unsigned_get(value_copy) == R_Unsigned_get(value));
  R_Type_Delete(value);
  R_Type_Delete(value_copy);
}

int main(void) {
  test_simple();
  test_constructor();
  test_destructor();
  test_copier();
  test_full();
  test_bad_constructor();
  test_integer();
  test_float();

  printf("Pass\n");
}
