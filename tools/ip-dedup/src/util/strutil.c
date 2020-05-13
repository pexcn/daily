#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "strutil.h"

bool is_whitespace ( char c ) {
   switch ( c ) {
      case ' ':
      case '\t':
      case '\n':
      case '\v':
      case '\f':
         return true;

      default:
         return false;
   }
}


char* str_strip ( char* restrict str, size_t* const restrict len ) {
   while ( ((*len) > 0) && (is_whitespace(*str)) ) {
      str++;
      (*len)--;
   }

   while ( ((*len) > 0) && (is_whitespace(str[(*len) - 1])) ) {
      str[(*len) - 1] = '\0';
      (*len)--;
   }

   return str;
}
