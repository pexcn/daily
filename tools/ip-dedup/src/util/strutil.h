#ifndef _HAVE_UTIL_STRUTIL_H_
#define _HAVE_UTIL_STRUTIL_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/** @return true if c is a whitespace character, else false */
bool is_whitespace ( char c );

/**
 * Removes leading whitespace by advancing the str begin pointer
 * and trailing whitespace by replacing it with null chars.
 *
 * @return pointer to new beginning of the string
 * */
char* str_strip ( char* restrict str, size_t* const restrict len );


#endif  /* _HAVE_UTIL_STRUTIL_H_ */
