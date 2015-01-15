# R Library
 This library is a collection of useful, free, single-file libraries for C. The focus of these modules is on ease-of-use, clarity and stability. It will be slower and consume more memory than custom or larger implementations.

## Why Single-File?
 Single-File libraries are easier to deal with! They're small enough that they can be included into a project as source which brings two main benefits: they're much easier to use when debugging and we don't have to worry about low-level formats of pre-built libraries.

## Why Free?
 Putting these libs into the public domain makes them even easier to use and usefulness is the main goal of the project.

## Instructions and Documentation
 There are some examples in the test/ folder and a sample makefile in the project's root. The tests can be run with 'make test'.

### R_Type
 R_Type is an Object/Class implementation based on a simplified form of 'OOC' (http://www.cs.rit.edu/~ats/books/ooc.pdf). A 'Type' object is created that stores the object's size, constructor, destructor, etc. that's used to alloc and free objects.

 R_Type_Declare is used to create forward declarations for classes and R_Type_Def is used to define a class as an instance of an R_Type. R_Type_Def is given a struct (first parameter is assumed to be an R_Type*) and optional constructors and destructors.

 A simple example containing only an Integer instance variable would look like this:
```
typedef struct {R_Type* type; int Integer;} R_Integer;
R_Type_Def(R_Integer, NULL, NULL, R_Type_shallowCopy); //Constructor and Destructor are not needed so are set to NULL.

R_Integer* instance = R_Type_New(R_Integer); //Allocates memory from the heap and runs the constructor, if available.
R_Type_Delete(instance); // Free memory back to the heap, after running the destructor.
```

### R_String
 R_String is a higher-level string class built using R_Type. It adds some auto-allocation, hex-conversion and safe self-copying features to regular C Strings.
```
 R_String* string = R_String_setString(R_Type_New(R_String), "Hello ");
 R_String_appendCString(string, "World!");
 printf("%s\n", R_String_getString(string)); //prints "Hello World!"
 R_Type_Delete(string);
```

### R_ByteArray
 R_ByteArray is a higher-level byte-array (array of type uint8_t) built using R_Type. It adds some auto-allocation and conversion features to regular byte arrays.
```
 R_ByteArray* array = R_TypeNew(R_ByteArray);
 R_ByteArray_appendBytes(array, 0x01, 0x02, 0x03);
 R_ByteArray_appendBytes(array, 0x40, 0x50, 0x60);
 for (int i=0; i<R_ByteArray_size(array);i++) printf("%02X"); //prints "010203405060"
 R_Type_Delete(array);
```

### R_ObjectArray
 R_ObjectArray is a collection of objects allocated using the R_Type methods, stored internally as an array of pointers.
```
 R_ObjectArray* list = R_Type_New(R_ObjectArray);
 R_Integer* integer = R_ObjectArray_add(list, R_Integer); integer->Integer = 42;
 integer = R_ObjectArray_add(list, R_Integer); number->Integer = 2;
 R_ObjectArray_each(list, R_Integer, number) {printf("%d ", number->Integer);} //prints "42 2"
 R_Type_Delete(list); //Also deletes both R_Integer elements
```

### R_Dictionary
 R_Dictionary is a Dictionary/Hash object built using R_Type, R_String and R_ObjectArray. It supports sub-object, array and string data types with some helper functions to convert other data types into strings. It was built primary for (de)serializing data as json but doesn't currently follow the json spec.
 