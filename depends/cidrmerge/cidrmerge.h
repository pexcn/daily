/*
    Copyright 2004-2008 Daniele Depetrini
    Author: 	Daniele Depetrini (depetrini@libero.it)
    Version: 1.5.3
    Date:	07/07/08

    This file is part of cidrmerge.

    CIDRMerge is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    CIDRMerge is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cidrmerge; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef CIDRMERGE
#define CIDRMERGE

#include <stdio.h>

#ifdef UNIX_IO
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>

	#define STREAM int
	#define STDIN 0
	#define STDOUT 1
	#define STDERR 2

	#define READ(f,b,n) read(f,b,n)
	#define WRITE(f,b,n) write(f,b,n)
	#define OPEN(name) open(name,O_RDONLY)
	#define CLOSE(f) close(f)
	#define N_EOF(f,ret) (ret==0)

#else

	#define STREAM FILE *
	#define STDIN (stdin)
	#define STDOUT (stdout)
	#define STDERR (stderr)

	#define READ(f,b,n) fread(b,1,n,f)
	#define WRITE(f,b,n) fwrite(b,1,n,f)
	#define OPEN(name) fopen(name,"r")
	#define CLOSE(f) fclose(f)
	#define N_EOF(f,ret) feof(f)

#endif

#ifdef TIMESTAMP
#include <sys/time.h>
#include <time.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAXLINE 1023
#define BUFFER 8192*8

/* Allocate BUCKET array entries per time */
#define BUCKET 100000
#define MAXCIDR 23

/*Special prefix values.
Important: INVALID_PREFIX must be < EXPANDED_PREFIX.
EXPANDED_PREFIX + 32 must be < 255
*/
#define INVALID_PREFIX 200
#define EXPANDED_PREFIX 220

#define MASK1 0xff000000
#define MASK2 0xff0000
#define MASK3 0xff00
#define MASK4 0xff

#ifndef MAXINT
#define MAXINT 0xffffffff
#endif

/* timestamping precision definitions */
#define PRECISION (1000)
#define PRECISION_INV (1000000/PRECISION)
#define PRECISION_STRING "milliseconds"

struct entry
{
	uint32_t network;
	#ifdef CHAR_PREFIX
	unsigned char prefix;
	#else
	uint32_t prefix;
	#endif
};

static uint32_t prefix_table[]= 	{
												0x0,0x80000000,0xc0000000,0xe0000000,0xf0000000,0xf8000000,0xfc000000,0xfe000000,0xff000000,0xff800000,
												0xffc00000,0xffe00000,0xfff00000,0xfff80000,0xfffc0000,0xfffe0000,0xffff0000,0xffff8000,0xffffc000,
												0xffffe000,0xfffff000,0xfffff800,0xfffffc00,0xfffffe00,0xffffff00,0xffffff80,0xffffffc0,0xffffffe0,
												0xfffffff0,0xfffffff8,0xfffffffc,0xfffffffe,0xffffffff
											};

/*#define TONETMASK(PREFIX) (((PREFIX)==0)?(0):(MAXINT<<(32-(PREFIX))))*/
#define TONETMASK(PREFIX) prefix_table[PREFIX]

/*
	Function execute the reduction of entry array addr.It returns the number of entries in the final array.
	PARAM: addr array: contains entries to be optimized
	PARAM: len: number of entries into addr array.
	PARAM: do_sort: if 0, sort will NOT be perfomed. Be carefull: optimizing a non-sorted input will produce unpredictable results
	NOTE3: resulting array may contains entries with prefix bigger than EXPANDED_PREFIX. Those are logical pointers: expanded_list[entry.network] (array returned by apply_whitelist function) is the starting position and the leght is entry.prefix- EXPANDED_PREFIX
*/
unsigned int optimize(struct entry *addr,unsigned int len,int do_sort);

/*
	Remove from array *entry_list occurence of array white.
	Returns number of element into output array expanded_list (that can be reassigned to allow array extension).
	Function allocate more memory if needed. It does NOT deallocate any if the result is smaller.
	PARAM: *entry_list: pointer to input/output param entry list.
	PARAM: **expanded_list: pointer to output param entry list, used to store expanded networks.
	PARAM: *white: white list entry array
	PARAM: len1: number of entry into entry_list in input
	PARAM: len2: number of entry into white_list in input
 	PARAM: size_expanded: return paramether that contains memory size of the output expanded_list array
	PARAM: do_sort: if 0, sort will NOT be perfomed on both entry_list and white. Be carefull: optimizing a non-sorted input will produce unpredictable results
	NOTE1: both arrays entry_list and white have to be sorted with sort_entries() function BEFORE calling this.function OR paramether do_sort needs to be set to non zero. Unpredictable results if not
	NOTE2: resulting array may contains entries with INVALID_PREFIX value prefix. This means entry is invalid and it doesn't have to be taken into account.
	NOTE3: resulting array may contains entries with prefix bigger than EXPANDED_PREFIX. Those are logical pointers: expanded_list[entry.network] is the starting position and the leght is entry.prefix- EXPANDED_PREFIX
	NOTE4: resulning array is ordered apart from invalid entries (the ones with INVALID_PREFIX value in prefix)
*/
unsigned int apply_whitelist(struct entry *addr,struct entry **expanded_list,struct entry *white,unsigned int len1,unsigned int len2,unsigned int *size_expanded,int do_sort);

/*
	Execute entries sorting using quicksort.
	PARAM: addr array: contains entries to be sorted
	PARAM: len: number of entries into addr array.
*/
void sort_entries(struct entry *addr,unsigned int len);

#endif
