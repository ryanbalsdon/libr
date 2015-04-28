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
#include "R_Face.h"
#include "R_Type.h"

R_Face_Def(R_Puts);
R_Face_DeclareCaller(R_Puts, void*, void* object) {R_Face_DefCaller(R_Puts, NULL, object);}
void R_Puts(void* object) {R_Type_Call(object, R_Puts, object);}

R_Face_Def(R_Equals);
R_Face_DeclareCaller(R_Equals, int, void* object_a, void* object_b) {R_Face_DefCaller(R_Equals, false, object_a, object_b);}
int R_Equals(void* object_a, void* object_b) {
  if (R_Type_Of(object_a) != R_Type_Of(object_b)) return false;
  return R_Type_Call(object_a, R_Equals, object_a, object_b);
}
