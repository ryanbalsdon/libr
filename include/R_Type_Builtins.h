#ifndef R_Type_Builtins_h
#define R_Type_Builtins_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

/* R_Type_shallowCopy
   A basic R_Type_Copier for objects without any internally-allocated memory.
 */
void* R_FUNCTION_ATTRIBUTES R_Type_shallowCopy(const void* object_input, void* object_output);

/* R_Integer
   A basic integer type.
 */
typedef struct {
  R_Type* type; 
  int value;
} R_Integer;
R_Type_Declare(R_Integer);
R_Integer* R_FUNCTION_ATTRIBUTES R_Integer_set(R_Integer* self, int value);
int R_FUNCTION_ATTRIBUTES R_Integer_get(R_Integer* self);

/* R_Float
   A basic floating point type.
 */
typedef struct {
  R_Type* type; 
  float value;
} R_Float;
R_Type_Declare(R_Float);
R_Float* R_FUNCTION_ATTRIBUTES R_Float_set(R_Float* self, float value);
float R_FUNCTION_ATTRIBUTES R_Float_get(R_Float* self);

/* R_Unsigned
   A basic unsigned integer type.
 */
typedef struct {
  R_Type* type; 
  unsigned int value;
} R_Unsigned;
R_Type_Declare(R_Unsigned);
R_Unsigned* R_FUNCTION_ATTRIBUTES R_Unsigned_set(R_Unsigned* self, unsigned int value);
unsigned int R_FUNCTION_ATTRIBUTES R_Unsigned_get(R_Unsigned* self);

/* R_Boolean
   A basic boolean type.
 */
typedef struct {
  R_Type* type; 
  bool value;
} R_Boolean;
R_Type_Declare(R_Boolean);
R_Boolean* R_FUNCTION_ATTRIBUTES R_Boolean_set(R_Boolean* self, bool value);
bool R_FUNCTION_ATTRIBUTES R_Boolean_get(R_Boolean* self);

/* R_Null
   A useless type.
 */
typedef struct {
  R_Type* type;
} R_Null;
R_Type_Declare(R_Null);

/* R_Data
   An array of bytes.
 */
typedef struct {
  R_Type* type; 
  bool is_owner; //Low (default) means destructor will call free on bytes. High means destructor will not call free on bytes.
  size_t size; 
  uint8_t* bytes;
} R_Data;
R_Type_Declare(R_Data);
R_Data* R_FUNCTION_ATTRIBUTES R_Data_New(size_t size);
R_Data* R_FUNCTION_ATTRIBUTES R_Data_set(R_Data* self, uint8_t* bytes, size_t size);
uint8_t* R_FUNCTION_ATTRIBUTES R_Data_bytes(R_Data* self);
size_t R_FUNCTION_ATTRIBUTES R_Data_size(R_Data* self);

/* R_String
   A NULL-terminated array of chars.
 */
typedef struct {
  R_Type* type; 
  bool is_owner; //Low (default) means destructor will call free on bytes. High means destructor will not call free on bytes.
  char* string;
} R_String;
R_Type_Declare(R_String);
R_String* R_FUNCTION_ATTRIBUTES R_String_New(char* string);
R_String* R_FUNCTION_ATTRIBUTES R_String_set(R_String* self, char* string);
char* R_FUNCTION_ATTRIBUTES R_String_get(R_String* self);

#endif /* R_Type_Builtins_h */