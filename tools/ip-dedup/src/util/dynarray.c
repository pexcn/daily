/*
 * Copyright (c) 2019 André Erdmann <dywi@mailerd.de>
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

#include "dynarray.h"

#define x_free(_ptr)  \
   do { \
      if ( (_ptr) != NULL ) { free((_ptr)); (_ptr) = NULL; } \
   } while (0)


static void _dynarray_free_item_ptr_use_destructor (
   const struct dynarray* const p_darr,
   void** const item_ptr
);

static void _dynarray_free_item_ptr (
   const struct dynarray* const p_darr,
   void** const item_ptr
);


static void _dynarray_free_item_ptr_use_destructor (
   const struct dynarray* const p_darr,
   void** const item_ptr
) {
   /* NULL does not need to be freed */
   if ( *item_ptr == NULL ) { return; }

   p_darr->item_destructor ( item_ptr );
   /* if destructor did not care
    * (i.e. set item to NULL after freeing/releasing it),
    * then we do it here.
    *
    * IOW, it is *critical* that destructors set
    * *item_ptr = NULL after freeing the item.
    */
   x_free ( *item_ptr );
}


static void _dynarray_free_item_ptr (
   const struct dynarray* const p_darr,
   void** const item_ptr
) {
   if ( p_darr->item_destructor != NULL ) {
      _dynarray_free_item_ptr_use_destructor ( p_darr, item_ptr );
   } else {
      x_free ( *item_ptr );
   }
}


int dynarray_resize (
   struct dynarray* const p_darr, const size_t want_len
) {
   size_t new_size;
   void** p_new;

   if ( p_darr->arr == NULL ) {
      errno = EINVAL;
      return -1;
   }

   if ( (want_len < 1) || (want_len <= (p_darr->size)) ) {
      /* nothing to do */
      return 0;
   }

   new_size = 2 * (p_darr->size);
   while ( want_len > new_size ) { new_size *= 2; }

   p_new = realloc ( (p_darr->arr), new_size * (sizeof *(p_darr->arr)) );
   if ( p_new != NULL ) {
      p_darr->arr  = p_new;
      p_darr->size = new_size;

      return 0;
   }

   dynarray_free ( p_darr );
   return -1;
}


int dynarray_grow ( struct dynarray* const p_darr ) {
   return dynarray_resize ( p_darr, (p_darr->len)+1 );
}


int dynarray_init (
   struct dynarray* const p_darr, const size_t initial_size
) {
   size_t real_init_size;

   real_init_size = (
      initial_size > 0 ? initial_size : DYNARRAY_DEFAULT_INITIAL_CAPACITY
   );

   p_darr->arr = malloc ( real_init_size * (sizeof *(p_darr->arr)) );
   if ( p_darr->arr == NULL ) { return -10; }

   p_darr->size            = real_init_size;
   p_darr->len             = 0;
   p_darr->item_destructor = NULL;

   return 0;
}


struct dynarray* new_dynarray ( const size_t initial_size ) {
   struct dynarray* p_darr;

   p_darr = malloc ( sizeof *p_darr );
   if ( p_darr == NULL ) { return NULL; }

   if ( dynarray_init ( p_darr, initial_size ) != 0 ) {
      x_free ( p_darr );
      return NULL;
   }

   return p_darr;
}


void dynarray_free_const_item_ptr (void** item_ptr) {
   *item_ptr = NULL;
}


void dynarray_set_item_destructor (
   struct dynarray* const p_darr,
   dynarray_free_item_ptr_func item_destructor
) {
   if ( p_darr == NULL ) { return; }
   p_darr->item_destructor = item_destructor;
}


void dynarray_set_data_readonly ( struct dynarray* const p_darr ) {
   dynarray_set_item_destructor (
      p_darr,
      dynarray_free_const_item_ptr
   );
}


int dynarray_release ( struct dynarray* const p_darr ) {
   size_t k;

   if ( p_darr == NULL ) { return 0; }

   if ( p_darr->arr != NULL ) {
      if ( p_darr->item_destructor == NULL ) {
         for ( k = 0; k < p_darr->len; k++ ) {
            x_free ( dynarray_get(p_darr, k) );
         }

      } else {
         for ( k = 0; k < p_darr->len; k++ ) {
            _dynarray_free_item_ptr_use_destructor (
               p_darr, &(dynarray_get(p_darr, k))
            );
         }
      }
   }

   p_darr->len = 0;
   return 0;
}


int dynarray_free ( struct dynarray* const p_darr ) {
   if ( p_darr == NULL ) { return 0; }

   if ( dynarray_release ( p_darr ) != 0 ) { return -1; }

   x_free ( p_darr->arr );
   p_darr->size = 0;
   return 0;
}


void dynarray_free_ptr ( struct dynarray** const p_darr ) {
   if ( p_darr == NULL ) { return; }
   dynarray_free ( *p_darr );
   x_free ( *p_darr );
}


int dynarray_append ( struct dynarray* const p_darr, void* const data ) {
   if ( dynarray_grow ( p_darr ) != 0 ) { return -1; }
   if ( p_darr->size < 1 ) { return -50; }

   (p_darr->arr) [(p_darr->len)++] = data;

   return 0;
}


int dynarray_pop ( struct dynarray* const p_darr, void** const data_out ) {
   void* pdata;

   if (
      (p_darr == NULL) || (p_darr->arr == NULL) ||
      (p_darr->size == 0) || (p_darr->len == 0)
   ) {
      return -5;
   }

   pdata = p_darr->arr [(p_darr->len)-1];
   p_darr->arr [--(p_darr->len)] = NULL;

   if ( data_out != NULL ) {
      *data_out = pdata;
   } else {
      _dynarray_free_item_ptr ( p_darr, &pdata );
   }

   return 0;
}


