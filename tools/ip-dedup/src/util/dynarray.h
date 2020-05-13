/*
 * Copyright (c) 2019 André Erdmann <dywi@mailerd.de>
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _DATA_DYNARRAY_H_
#define _DATA_DYNARRAY_H_

#include <stdlib.h>
#include <sys/types.h>


#ifndef DYNARRAY_DEFAULT_INITIAL_CAPACITY
enum { DYNARRAY_DEFAULT_INITIAL_CAPACITY = 1 };
#endif

typedef void(*dynarray_free_item_ptr_func)(void**);

/**
 * dynamically allocated array - data type
 */
struct dynarray {
   void** arr;    /** the actual array */
   size_t len;    /** length of the array (used size) */
   size_t size;   /** capacity of the array (allocated size) */

   dynarray_free_item_ptr_func item_destructor;
};


/**
 * Initializes a dynamically allocated array.
 *
 * @param p_darr          pointer to the dynarray (must not be NULL)
 * @param initial_size    initial capacity (0 for default)
 *
 * @return 0 on success, else non-zero
 */
__attribute__((warn_unused_result))
int dynarray_init ( struct dynarray* const p_darr, const size_t initial_size );

/**
 * Creates a new dynarray of the requested size.
 *
 * @param initial_size
 *
 * @return pointer to dynarray or NULL
 */
__attribute__((warn_unused_result))
struct dynarray* new_dynarray ( const size_t initial_size );

/**
 * "Frees" a const dynarray item,
 * which simply sets the item pointer to NULL.
 *
 * @param item_ptr    pointer to dynarray item
 */
void dynarray_free_const_item_ptr (void** item_ptr);

/**
 * Sets the dynarray's item destructor function.
 *
 * @param p_darr              pointer to the dynarray (should not be NULL)
 * @param item_destructor     impure function :: void** -> void
 */
void dynarray_set_item_destructor (
   struct dynarray* const p_darr,
   dynarray_free_item_ptr_func item_destructor
);

/**
 * Sets or unsets a dynarray's "readonly data" flag.
 *
 * @param p_darr  pointer to the dynarray (must not be NULL)
 */
void dynarray_set_data_readonly ( struct dynarray* const p_darr );

/**
 * Resizes a dynarray so that it has enough space for at least want_len items.
 * No-op if the array's capacity is already sufficient.
 *
 * @param p_darr    pointer to the dynarray (must not be NULL)
 * @param want_len
 *
 * @return 0 on success, else non-zero
 */
__attribute__((warn_unused_result))
int dynarray_resize ( struct dynarray* const p_darr, const size_t want_len );

/**
 * Resizes a dynarray so that it has enough space for at least one more item.
 * No-op if the array is already big enough.
 *
 * @param p_darr  pointer to the dynarray (must not be NULL)
 *
 * @return 0 on success, else non-zero
 */
__attribute__((warn_unused_result))
int dynarray_grow ( struct dynarray* const p_darr );

/**
 * Empties a dynarray.
 *
 * Releases the items of a dynarray by setting them to NULL and resets the
 * length (but not the capacity) to 0.
 * The items get freed if the "readonly data" flag is not set.
 *
 * @param p_darr  pointer to the dynarray (may be NULL)
 *
 * @return 0
 */
int dynarray_release ( struct dynarray* const p_darr );

/**
 * Destroys a dynarray.
 *
 * Empties the dynarray and frees the actual array.
 *
 * @param p_darr  pointer to the dynarray (may be NULL)
 *
 * @return 0
 */
int dynarray_free ( struct dynarray* const p_darr );

void dynarray_free_ptr ( struct dynarray** const p_darr );

/**
 * Appends arbitrary data to a dynarray.
 *
 * Reallocates the array if necessary.
 *
 * @param p_darr  pointer to the dynarray (must not be NULL)
 * @param data    data to append
 *
 * @return 0 on success, else non-zero
 */
int dynarray_append ( struct dynarray* const p_darr, void* const data );

/**
 * Removes the last entry of a dynarry and stores it in data_out (if not NULL).
 *
 * Frees the removed item if data_out is NULL and the "readonly data" flag
 * is not set. *
 *
 * @param p_darr    pointer to the dynarray (should not be NULL)
 * @param data_out  NULL or a pointer "container" (<type>**)
 *
 * @return 0 on success, else non-zero
 */
int dynarray_pop ( struct dynarray* const p_darr, void** const data_out );


#define dynarray_set(d,k,v)               ((d)->arr)[k] = v;
#define dynarray_get(d,k)                 (((d)->arr)[k])
#define dynarray_get_as(d,k,t)            (t)(dynarray_get(d,k))
#define dynarray_get_str(d,k)             dynarray_get_as(d,k,char*)
#define dynarray_get_const_str(d,k)       dynarray_get_as(d,k,const char*)

#define dynarray_argc(d)                  ((d)->len)
#define dynarray_argv(d)                  (char**)((d)->arr)
#define dynarray_const_argv(d)            (const char**)((d)->arr)

#define _dynarray_shift(d,o)              (((d)->arr)+o)

#define dynarray_argc_shift(d,o)          (((d)->len)-o)
#define dynarray_argv_shift(d,o)          (char**)(_dynarray_shift(d,o))
#define dynarray_const_argv_shift(d,o)    (const char**)(_dynarray_shift(d,o))

#define dynarray_foreach(d, idx_var)  \
   for ( idx_var = 0; idx_var < dynarray_argc(d); idx_var++ )

#define dynarray_foreach_reversed(d, idx_var)  \
   for ( idx_var = ( dynarray_argc(d) - 1 ); idx_var >= 0; idx_var-- )

#endif
