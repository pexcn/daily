#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


#include "common.h"


static int parse_ip_strtol (
   const char* const restrict arg,
   const int base,
   long* const restrict lout
);


bool parse_ip_check_type_valid ( const int type ) {
   switch ( type ) {
      case PARSE_IP_TYPE_IPV4:
      case PARSE_IP_TYPE_IPV6:
      case PARSE_IP_TYPE_BOTH:
         return true;

      default:
         return false;
   }
}


#if 0
int parse_ip_hexchr_to_int ( const char c ) {
   switch ( c ) {
      case '0':
         return 0;

      case '1':
         return 1;

      case '2':
         return 2;

      case '3':
         return 3;

      case '4':
         return 4;

      case '5':
         return 5;

      case '6':
         return 6;

      case '7':
         return 7;

      case '8':
         return 8;

      case '9':
         return 9;

      case 'a':
      case 'A':
         return 10;

      case 'b':
      case 'B':
         return 11;

      case 'c':
      case 'C':
         return 12;

      case 'd':
      case 'D':
         return 13;

      case 'e':
      case 'E':
         return 14;

      case 'f':
      case 'F':
         return 15;

      default:
         return -1;
   }
}
#endif


char* parse_ip_split_prefixlen (
    char* const restrict s,
    const size_t slen,  /* size excluding terminating null char */
    const char** const restrict prefixstr_out
) {
   size_t k;

   *prefixstr_out = NULL;

   if ( (s == NULL) || (*s == '\0') ) { return NULL; }

   /* strrchr searches left-to-right, we can do better. */
   for ( k = slen; k > 0; k-- ) {
      if ( s[k - 1] == '/' ) {
         if ( s[k] == '\0' ) { return NULL; }
         s[k - 1] = '\0';
         *prefixstr_out = (s + k);
         break;
      }
   }

   return s;
}


static int parse_ip_strtol (
   const char* const restrict arg,
   const int base,
   long* const restrict lout
) {
   char* endptr;

   *lout = strtol ( arg, &endptr, base );

   return ( (*endptr == '\0') ? 0 : -1 );
}


int parse_ip_read_octet (
    const char* const restrict arg,
    const int base,
    uint_fast8_t* const restrict bout
) {
   long buf;

   if ( parse_ip_strtol ( arg, base, &buf ) != 0 ) { return -1; }
   if ( (buf < 0) || (buf > 0xff) ) { return -1; }

   *bout = (uint_fast8_t) buf;
   return 0;
}


int parse_ip_read_double_octet (
    const char* const restrict arg,
    const int base,
    uint_fast16_t* const restrict bout
) {
   long buf;

   if ( parse_ip_strtol ( arg, base, &buf ) != 0 ) { return -1; }
   if ( (buf < 0) || (buf > 0xffff) ) { return -1; }

   *bout = (uint_fast16_t) buf;
   return 0;
}
