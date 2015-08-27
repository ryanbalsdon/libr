#include <stdlib.h>
#include <stdio.h>
#include "R_OS.h"

void* R_FUNCTION_ATTRIBUTES os_realloc_alt(void* old_ptr, size_t new_size) {
  void* new_ptr = (void*)os_malloc(new_size);
  os_memcpy(new_ptr, old_ptr, new_size); //this should be old_size but we don't have access to it
  os_free(old_ptr);
  return new_ptr;
}

int R_FUNCTION_ATTRIBUTES os_atoi_alt(const char* string) {
  int index = 0;
  int sign = +1;
  int output = 0;
  char this_char = string[index];
  if (this_char == '-') sign = -1;
  if (this_char == '-' || this_char == '+') this_char = string[++index];
  while (this_char >= '0' && this_char <= '9') {
    output = (output*10) + (this_char-'0');
    this_char = string[++index];
  }
  output *= sign;
  return output;
}

double R_FUNCTION_ATTRIBUTES os_atof_alt(const char *s)
{
  double output = 0.0;
  int mantissa = 0;
  char this_char;
  int sign = +1;
  if (*s == '-') {
    sign = -1;
    s++;
  }
  else if (*s == '+') s++; 
  while ((this_char = *s++) != '\0' && (this_char >= '0' && this_char <= '9')) {
    output = (output*10.0) + (this_char-'0');
  }
  if (this_char == '.') {
    double fraction = 0.1;
    while ((this_char = *s++) != '\0' && (this_char >= '0' && this_char <= '9')) {
      output += fraction * (this_char - '0');
      fraction *= 0.1;
    }
  }
  if (this_char == 'e' || this_char == 'E') {
    int exponent_sign = +1;
    int exponent = 0;
    this_char = *s++;
    if (this_char == '+') this_char = *s++;
    else if (this_char == '-') {
      this_char = *s++;
      exponent_sign = -1;
    }
    while (this_char >= '0' && this_char <= '9') {
      exponent = exponent*10 + (this_char - '0');
      this_char = *s++;
    }
    mantissa += exponent*exponent_sign;
  }
  while (mantissa > 0) {
    output *= 10.0;
    mantissa--;
  }
  while (mantissa < 0) {
    output *= 0.1;
    mantissa++;
  }
  output *= sign;
  return output;
}

