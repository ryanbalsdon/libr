#ifndef R_OS_h
#define R_OS_h

#include <string.h>

void* os_realloc_alt(void* old_ptr, size_t new_size);
int os_atoi_alt(const char* string);
double os_atof_alt(const char* string);

#ifdef ESP8266
  #include "mem.h"
  #include "osapi.h"
  #define os_realloc os_realloc_alt
  #define os_atoi os_atoi_alt
  #define os_atof os_atof_alt
  #define R_FUNCTION_ATTRIBUTES __attribute__((section(".irom0.text")))
  typedef int pthread_mutex_t;
  #define pthread_mutex_lock(mutex_pointer) do {while(*(mutex_pointer)); (*(mutex_pointer))++; } while(0)
  #define pthread_mutex_unlock(mutex_pointer) do {(*(mutex_pointer))--; } while(0)
  #define pthread_mutex_init(mutex_pointer, options) do {*(mutex_pointer)=0; } while(0)
  #define pthread_mutex_destroy
#else
  #include <string.h>
  #include <stdio.h>
  #define os_calloc calloc
  #define os_zalloc(size) os_calloc(1,size)
  #define os_free free
  #define os_malloc malloc
  #define os_memcmp memcmp
  #define os_printf printf
  #define os_realloc realloc
  #define os_sscanf sscanf
  #define os_strlen strlen
  #define os_strstr strstr
  #define os_strcpy strcpy
  #define os_snprintf snprintf
  #define os_sprintf sprintf
  #define os_memcpy memcpy

  #define R_FUNCTION_ATTRIBUTES
#endif
 
#endif /* R_OS_h */
