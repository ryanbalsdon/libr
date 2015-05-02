#ifndef R_OS_h
#define R_OS_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#ifdef ESP8266
  #define R_FUNCTION_ATTRIBUTES __attribute__((section(".irom0.text")))
#else
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
  #define os_snprintf snprintf
  #define os_sprintf sprintf
  #define os_memcpy memcpy

  #define R_FUNCTION_ATTRIBUTES
#endif
 
#endif /* R_OS_h */
