#ifndef R_Face_h
#define R_Face_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

typedef struct {
  char* description;
} R_Face;

typedef void* (*R_Face_JumpTable)(const R_Face* interface);

#define R_Face_Def(name) const R_Face R_Face__##name = {#name}
#define R_Face_Named(name) &R_Face__##name
#define R_Face_DeclareName(name) extern const R_Face R_Face__##name
#define R_Face_DeclareFunction(name, return_type, ...) typedef return_type (*R_Face_Function__##name)(__VA_ARGS__)
#define R_Face_DeclareCaller(name, return_type, ...) return_type R_Face_Caller__##name(void* f, __VA_ARGS__)
#define R_Face_Call(table, name, ...) R_Face_Caller__##name (table ? table(R_Face_Named(name)) : NULL, __VA_ARGS__)
#define R_Face_Declare(name, return_type, ...) \
  R_Face_DeclareName(name); \
  R_Face_DeclareFunction(name, return_type, __VA_ARGS__); \
  R_Face_DeclareCaller(name, return_type, __VA_ARGS__);
#define R_Face_DefJump(name, method) if (interface == R_Face_Named(name)) return method
#define R_Face_DefCaller(name, default_return_value, ...) return (f ? ((R_Face_Function__##name)f)(__VA_ARGS__) : default_return_value)

R_Face_Declare(R_Puts, void*, void* object)
void R_Puts(void* object);

R_Face_Declare(R_Equals, int, void* object_a, void* object_b)
int R_Equals(void* object_a, void* object_b);

#endif /* R_Face_h */
