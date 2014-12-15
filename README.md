# R Library
 This library is a collection of useful, free, single-file libraries for C. The focus of these modules is on ease-of-use, clarity and stability. It will be slower and consume more memory than custom or larger implementations.

## Why Single-File?
 Single-File libraries are easier to deal with! They're small enough that they can be included into a project as source which brings two main benefits: they're much easier to use when debugging and we don't have to worry about low-level formats of pre-built libraries.

## Why Free?
 Putting these libs into the public domain makes them even easier to use and usefulness is the main goal of the project.

## Instructions and Documentation
 There are some examples in the test/ folder and a smaple makefile in the project's root. The tests can be run with 'make test'.

### R_Type
 R_Type is an Object/Class implementation based on a simplified form of 'OOC' (http://www.cs.rit.edu/~ats/books/ooc.pdf). A 'Type' object is created that stores the object's size, constructor, destructor, etc. that's used to alloc and free objects.

### R_String
 R_String is a higher-level string class built using R_Type. It adds some auto-allocation, hex-conversion and safe self-copying features to regular C Strings.

### R_ByteArray
 R_ByteArray is a higher-level byte-array (array of type uint8_t) built using R_Type. It adds some auto-allocation and conversion features to regular byte arrays.

### R_ObjectArray
 R_ObjectArray is a collection of objects allocated using the R_Type methods, stored internally as an array of pointers.

### R_Dictionary
 R_Dictionary is a Dictionary/Hash object built using R_Type, R_String and R_ObjectArray. It supports sub-object, array and string data types with some helper functions to convert other data types into strings. It was built primary for (de)serializing data as json but doesn't currently follow the json spec.