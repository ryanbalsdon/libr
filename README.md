# R Library
 This library is a collection of small utilities for C. The focus of these modules is on rapid-prototyping and ease-of-use. The idea being that, once bottlenecks in a project are found, these generic classes can be replaced with more traditional, higher-performance, non-object-oriented C code.

# Contributing
 There are many examples in the test/ folder of how to use this library. The tests can be run with `make test` or `make valgrind` if Valgrind is installed.

# Namespacing Rules
 Because Namespacing doesn't exist in C, some general rules are used to achieve it:
- Namespaces and classes are CamelCase, starting with a capital.
- Namespaces and classes are separated with an underscore.
- Class methods are CamelCase, starting with a capital.
- Instance methods are camelCase, starting a lowercase. Instance methods always take an object as their first argument.
- Local or instance values are snake_case, starting with a lowercase.

## Namespacing Examples
- `R_Type_New`: `New is a class method of the `Type` class in the `R` namespace.
- `R_Integer_get`: `get` is an instance method of the `Integer` class in the `R` namespace. It's first argument is an `R_Integer*`.
- `r_type`: This is a local variable who's name doesn't conflict with any namespaces.

# R_Type
 R_Type is an OO (Object-Oriented) framework for C. All the other utilities in libr build off this framework. 

 There are three steps to creating a new class with this framework.

## Header Definition
 Use the `R_Type_Declare` macro to declare that your struct is a class.
```
typedef struct NewClass NewClass;
R_Type_Declare(NewClass);
```

## Implementation
 Use the `R_Type_Define` macro to define a new class using your struct.
 ```
 R_Type_Define(NewClass, NULL);
 ```

 The struct _must_ have an R_Type (or void) pointer as its first element.
```
struct NewClass {
  R_Type* isa;
  //Other elements of the class can follow
  int some_integer;
  float floating_point_value;
  char* etc;
}
```

## Allocation
 Objects are allocated using the `R_Type_New` function and deallocated with `R_Type_Delete`.
```
NewClass* instance = R_Type_New(NewClass);
assert(R_Type_IsOf(instance, NewClass));
R_Type_Delete(instance);
```

## Constructors and Destructors
 Constructor and Destructor methods can be added to the class in the `R_Type_Define` call.
```
NewClass* NewClass_Constructor(NewClass* self) {return self;}
NewClass* NewClass_Destructor(NewClass* self) {return self;}
 R_Type_Define(NewClass, .ctor = NewClass_Constructor, .dtor = NewClass_Destructor);
```

## Deep Copying
 If your class contains other allocated object, create a custom copy method. Otherwise, use the builtin shallow copy.
```
R_Type_Define(NewClass, .copy = R_Type_shallowCopy);
//or...
NewClass* NewClass_Copier(NewClass* old, NewClass* new) {old->value = new->value; return new;}
 R_Type_Define(NewClass, .copy = NewClass_Copier);
```

## Duck Typing / Dynamic method calls / Interfaces
 Use a method table to inplement an interface/selector in your class.
```
size_t NewClass_stringify(R_Float* self, char* buffer, size_t size) {
  int bytes_written = os_snprintf(buffer, size, "NewClass, in string form");
  return bytes_written < size ? bytes_written : size;
}
static R_JumpTable_Entry NewClass_methods[] = {
  R_JumpTable_Entry_Make(R_Stringify, NewClass_stringify), 
  R_JumpTable_Entry_NULL
};
R_Type_Define(NewClass, .interfaces = NewClass_methods);
```

# R_Type Builtins
 There are wrapper classes built with R_Type for the standard C types. These are meant to be immutable types but their struct definitions are left public. This is to make optimizations that use the lower-level types possible.

 The builtins are:
- R_Type_Integer
- R_Type_Float
- R_Type_Unsigned
- R_Type_Boolean
- R_Type_Null
- R_Type_Data
- R_Type_String

# R_MutableData
 This is a dynamic-sized byte array. It will increase its own allocation as needed.

 An example of its usage:
 ```
 R_MutableData* bytes = R_Type_New(R_MutableData);
 R_MutableData_appendBytes(bytes, 0x01, 0x42, 0xFF, 0xa5);
 assert(R_MutaData_byte(bytes, 1) == 0x42);
 R_Type_Delete(bytes);
 ```

 It comes with many helper methods to append and extract different data types like:
- R_MutableData_appendByte
- R_MutableData_appendArray
- R_MutableData_appendHexString
- R_MutableData_pop
- R_MutableData_shift
- etc...

# R_MutableString
 This is a dynamic-sized string that is built off of R_MutableData and is used similarly.
 ```
 R_MutableString* string = R_MutableString_setString(R_Type_New(R_MutableString), "Hello");
 R_MutableString_push(string, ' ');
 R_MutableString_appendCString(string, "World");
 assert(strcmp(R_MutableString_getString(string), "Hello World") == 0);
 R_Type_Delete(string);
 ```

# R_List
 This is a list of R_Type instances. The instance memory is managed by the list. The list can allocate a new instance or being given an existing instance to manage.
```
R_List* strings = R_Type_New(R_List);
R_MutableString* first_string = R_List_add(strings, R_MutableString);
R_MutableString* second_string = R_Type_New(R_MutableString);
R_List_transferOwnership(strings, second_string);
assert(R_List_size(strings) == 2);
R_Type_Delete(strings); //This will deallocate both first_string and second_string!
```

 The list can be looped over with an `each` method.
 ```
  R_List_each(strings, R_MutableString, string) {
    assert(R_Type_IsOf(string, R_MutableString));
  }

  R_List_each(strings, void, unknown_type) {
    if (R_Type_IsOf(unknown_type, R_MutableString)) printf("This is a mutable string!\n");
    if (R_Type_IsOf(unknown_type, R_Integer)) printf("This is an integer!\n");
    if (R_Type_IsOf(unknown_type, R_Boolean)) printf("This is a boolean!\n");
  }
 ```

# R_Dictionary
 This is a key-value dictionary, implemented as an R_List of R_KeyValuePair instances. It supports JSON parsing, manual creation and each loops.
```
  R_Dictionary* dictionary = R_Type_New(R_Dictionary);
  R_Integer_set(R_Dictionary_add(dictionary, "key1", R_Integer), 1);
  R_Integer_set(R_Dictionary_add(dictionary, "key2", R_Integer), 2);
  assert(R_Dictionary_size(dictionary) == 2);
  R_Type_Delete(dictionary);
```

 Each loop returns an R_KeyValuePair instance which has key and value getters.
```
  R_Dictionary_each(dictionary, element) {
    if (R_MutableString_compare(R_KeyValuePair_key(element), "key1")) {
      printf("Found 'key1': ");
      R_Puts(R_KeyValuePair_value(element));
    }
  }
```
