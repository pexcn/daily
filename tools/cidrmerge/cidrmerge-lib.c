/*
    Copyright 2004-2008 Daniele Depetrini
    Author: 	Daniele Depetrini (depetrini@libero.it)
    Version: 1.5.3
    Date:  06/07/08

    This file is part of CIDRMerge.

    CIDRMerge is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    CIDRMerge is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CIDRMerge; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include "cidrmerge.h"

#ifdef LIBRARY_DEBUG
#include <stdarg.h>

/* Using those functions from cidrmerge.c for debugging purposes only */
void print_address(STREAM file,uint32_t net, unsigned char pref);
void print_addresses(STREAM f,struct entry *addr,int size,struct entry *expanded,int level);

void print_debug(STREAM f,char *buf,char *fmt,...)
{
	va_list ap;
	va_start(ap, fmt);
	sprintf(buf,fmt,ap);
	WRITE(f,buf,strlen(buf));
	va_end(ap);
}

#endif
/*
	Exectute one entry comparation. a1 is bigger than a2 if:
	- a1.network is bigger than a2.network
	- a1.network is equal to a2.network but a1.prefix is bigger than a2.prefix
	PARAM: a1 first entry
	PARAM: a2 second entry
	RETURN VALUE: it returns -1 if a1<a2, 0 if a1=a2 and 1 if a1>a2
*/
#ifndef OPTIMIZED_SORT
typedef int(*cmp_type)(const void *,const void*);

static int cmp_entry(struct entry *a1, struct entry *a2)
{
	if ((*a1).network<(*a2).network)
	{
		return -1;
	}
	if ((*a1).network>(*a2).network)
	{
		return 1;
	}
/* Network are ==, so we compare netmasks*/
	if ((*a1).prefix<(*a2).prefix)
	{
		return -1;
	}
	if ((*a1).prefix>(*a2).prefix)
	{
		return 1;
	}

	return 0;
}
#endif

/*
	Please see cidrmerge.h
*/
void sort_entries(struct entry *addr,unsigned int len)
{
	#ifndef OPTIMIZED_SORT
	/*standard C quicksort*/
	qsort(addr,len,sizeof(struct entry),(cmp_type)cmp_entry);
	#else
	/*Optimized quicksort. Thanks to Michael Tokarev*/
	#include "optimized-sort.h"
	#define cmp_entry(a1,a2) (((a1)->network<(a2)->network) || (((a1)->network==(a2)->network) && ((a1)->prefix<(a2)->prefix)))

	if (len>1)
	{
		/*sprintf(debug_buf,"SORT: addr[0] %d len %d\n",addr[0].network,len);*/
		QSORT(struct entry,addr,len,cmp_entry);
	}
	#endif
}

#ifdef INLINE
__inline
#endif
static void expand(struct entry **expanded_list,struct entry to_expand,struct entry *white,unsigned int len_white,unsigned int n,unsigned int *current_position,unsigned int *size_expanded)
{
	int first=*current_position,last=*current_position+n-1;
	unsigned int first_conflict,last_conflict,i;
	struct entry tmp_small,tmp_big;
	struct entry *result=*expanded_list;
	#ifdef LIBRARY_DEBUG
	char debug_buf[MAXLINE];
	#endif

	*current_position+=n;

	if (*current_position>*size_expanded)
	{
		/* we need to increase expanded_list array size */
		#ifdef LIBRARY_DEBUG
		print_debug(STDOUT,debug_buf,"BEFORE realloc: size_expanded: %u, MEM: %u\n", *size_expanded,(*size_expanded)*sizeof(struct entry));
		#endif

		*size_expanded+=BUCKET;
		#ifdef LIBRARY_DEBUG
		print_debug(STDOUT,debug_buf,"AFTER realloc size_expanded: %u,MEM: %u\n", *size_expanded,(*size_expanded)*sizeof(struct entry));
		#endif

		result=*expanded_list=realloc(*expanded_list,(*size_expanded)*sizeof(struct entry));
		if (result==NULL)
		{
			fprintf(stderr,"Error reallocating %lu bytes\n",(unsigned long)(*size_expanded)*sizeof(struct entry));
			exit(1);
		}
	}

	#ifdef LIBRARY_DEBUG
	print_debug(STDOUT,debug_buf,"\nTO EXPAND n %u:\n",n);
	print_address(STDOUT,to_expand.network,to_expand.prefix);

	print_debug(STDOUT,debug_buf,"START WHITE len_white %u:\n",len_white);
	print_addresses(STDOUT,white,len_white,NULL,0);
	print_debug(STDOUT,debug_buf,"END   WHITE\n");
	#endif

	while (first<=last)
	{
		tmp_small.prefix=tmp_big.prefix=to_expand.prefix+1;
		tmp_small.network=to_expand.network; 										/*last network bit set to 0 */
		tmp_big.network=to_expand.network+(0x1<<(32-tmp_small.prefix)); 	/*last network bit set to 1 */

		if ( (white[0].network & TONETMASK(tmp_small.prefix) ) == tmp_small.network )
		{
			/* conflicting whith tmp_small. */
			i=1;
			/* search for first conflicting position with tmp_big (it cant be the first) */
			while((i<len_white) && !( (white[i].network & TONETMASK(tmp_big.prefix) ) == tmp_big.network ))
			{
				i++;
			}
			first_conflict=i;
			/* search for last conflicting position with tmp_big*/
			while((i<len_white) && ( (white[i].network & TONETMASK(tmp_big.prefix) ) == tmp_big.network ))
			{
				i++;
			}
			last_conflict=i;
			/* check if there is at least one conflicting with tmp_big network into whitelist */
			if (first_conflict!=len_white)
			{
				if (white[first_conflict].prefix!=tmp_big.prefix)
				{

					#ifdef LIBRARY_DEBUG
					print_debug(STDOUT,debug_buf,"RE-CONFLICT BIG first %u last %u\n",first_conflict,last_conflict);
					print_address(STDOUT,tmp_big.network,tmp_big.prefix);
					#endif

					result[last].network=*current_position;
					result[last].prefix=EXPANDED_PREFIX+white[first_conflict].prefix-tmp_big.prefix;

					expand(expanded_list,tmp_big,&white[first_conflict],last_conflict-first_conflict,white[first_conflict].prefix-tmp_big.prefix,current_position,size_expanded);
				}
				else
				{
					#ifdef LIBRARY_DEBUG
					print_debug(STDOUT,debug_buf,"INVALIDATING BIG POSITION %u\n",last);
					#endif
					result[last].prefix=INVALID_PREFIX;
				}
			}
			else
			{
				#ifdef LIBRARY_DEBUG
				print_debug(STDOUT,debug_buf,"VALID BIG position %u:\n",last);
				print_address(STDOUT,tmp_big.network,tmp_big.prefix);
				#endif
				result[last].network=tmp_big.network;
				result[last].prefix=tmp_big.prefix;
			}
			to_expand.network=tmp_small.network;
			to_expand.prefix=tmp_small.prefix;
			last--;
		}
		else
		{
			/* conflicting with tmp_big */
			i=1;
			/* search for first conflicting position with tmp_small (it cant be the first)*/
			while((i<len_white) && !( (white[i].network & TONETMASK(tmp_small.prefix) ) == tmp_small.network ))
			{
				i++;
			}
			first_conflict=i;
			/* search for last conflicting position with tmp_small*/
			while((i<len_white) && ( (white[i].network & TONETMASK(tmp_small.prefix) ) == tmp_small.network ))
			{
				i++;
			}
			last_conflict=i;

			/* check if there is at least one conflicting with tmp_small network into whitelist */
			if (first_conflict!=len_white)
			{
				if (white[first_conflict].prefix!=tmp_small.prefix)
				{
					#ifdef LIBRARY_DEBUG
					print_debug(STDOUT,debug_buf,"RE-CONFLICT LOW first %u last %u\n",first_conflict,last_conflict);
					print_address(STDOUT,tmp_small.network,tmp_small.prefix);
					#endif

					result[first].network=*current_position;
					result[first].prefix=EXPANDED_PREFIX+white[first_conflict].prefix-tmp_small.prefix;

					expand(expanded_list,tmp_small,&white[first_conflict],last_conflict-first_conflict,white[first_conflict].prefix-tmp_small.prefix,current_position,size_expanded);
				}
				else
				{
					#ifdef LIBRARY_DEBUG
					print_debug(STDOUT,debug_buf,"INVALIDATING LOW POSITION %u\n",first);
					#endif
					result[first].prefix=INVALID_PREFIX;
				}
			}
			else
			{
				#ifdef LIBRARY_DEBUG
				print_debug(STDOUT,debug_buf,"VALID LOW position %u:\n",first);
				print_address(STDOUT,tmp_small.network,tmp_small.prefix);
				#endif

				result[first].network=tmp_small.network;
				result[first].prefix=tmp_small.prefix;
			}
			to_expand.network=tmp_big.network;
			to_expand.prefix=tmp_big.prefix;
			first++;
		}

		#ifdef LIBRARY_DEBUG
		print_debug(STDOUT,debug_buf,"first %d last %d\n",first,last);
		#endif
	}

	#ifdef LIBRARY_DEBUG
	print_debug(STDOUT,debug_buf,"EXPAND END. Local position %d, current position %u:\n",n,*current_position);
	print_debug(STDOUT,debug_buf,"\n");
	#endif
}

/*
	Please see cidrmerge.h
*/
unsigned int apply_whitelist(struct entry *addr,struct entry **expanded_list,struct entry *white,unsigned int len1,unsigned int len2,unsigned int *size_expanded,int do_sort)
{
	unsigned int i1,i2,invalid,tmp,expanded_index=0;
	uint32_t supermask;
	struct entry tmp_entry;
	#ifdef LIBRARY_DEBUG
	char debug_buf[MAXLINE];
	unsigned int step=0;
	print_debug(STDOUT,debug_buf,"START apply_whitelist\n");
	#endif


	if (do_sort)
	{
		#ifdef LIBRARY_DEBUG
		print_debug(STDOUT,debug_buf,"Sorting entries\n");
		#endif
		sort_entries(addr,len1);
		sort_entries(white,len2);
	}

	i1=i2=0;
	while ((i1<len1)&&(i2<len2))
	{
		#ifdef LIBRARY_DEBUG
		print_debug(STDOUT,debug_buf,"STEP=%u I1=%d I2=%d\n",step++,i1,i2);
		#endif
		#ifdef LIBRARY_DEBUG_FULL
		print_addresses(STDOUT,addr,len1,NULL,0);
		#endif

		supermask=TONETMASK(addr[i1].prefix)&TONETMASK(white[i2].prefix);

		if ((addr[i1].prefix<=32)&&(addr[i1].network&supermask)==(white[i2].network&supermask))
		{
			#ifdef LIBRARY_DEBUG
			print_debug(STDOUT,debug_buf,"CONFLICT\n");
			print_address(STDOUT,addr[i1].network,addr[i1].prefix);
			print_address(STDOUT,white[i2].network,white[i2].prefix);
			#endif
			if (addr[i1].prefix<white[i2].prefix)
			{
				/*we have to expand the network*/

				/*invalidate all addr[i1] subnetworks */
				invalid=i1+1;
				while ((invalid<len1)&&(addr[invalid].network&supermask)==(white[i2].network&supermask))
				{
					/*address is already present in the expanded network, just drop it*/
					#ifdef LIBRARY_DEBUG
					print_debug(STDOUT,debug_buf,"INVALIDATING ");
					print_address(STDOUT,addr[invalid].network,addr[invalid].prefix);
					#endif
					addr[invalid].prefix=INVALID_PREFIX;

					invalid++;
				}

				invalid-=i1+1; /*invalid represents the number of invalidated positions*/

				#ifdef LIBRARY_DEBUG
				print_debug(STDOUT,debug_buf,"INVALID %u LEN1 %u\n",invalid,len1);
				#endif

				tmp=i2;
				i2+=1;
				while ( (i2<len2) && ((addr[i1].network&supermask)==(white[i2].network&supermask)) )
				{
					i2++;
				}

				#ifdef LIBRARY_DEBUG
				print_debug(STDOUT,debug_buf,"EXPAND expanded_index %u whitelist elements: %u num expand %d\n",expanded_index,i2-tmp,white[tmp].prefix-addr[i1].prefix);
				#endif

				tmp_entry.network=addr[i1].network;
				tmp_entry.prefix=addr[i1].prefix;

				addr[i1].prefix=EXPANDED_PREFIX+white[tmp].prefix-addr[i1].prefix;
				addr[i1].network=expanded_index;
				/* expanded positions are clean and optimized */
				expand(expanded_list,tmp_entry,&(white[tmp]),i2-tmp,white[tmp].prefix-tmp_entry.prefix,&expanded_index,size_expanded);

				i1+=invalid+1;

				#ifdef LIBRARY_DEBUG
				print_debug(STDOUT,debug_buf,"END MAIN EXPAND expanded_index %u i1: %u i2: %u\n",expanded_index,i1,i2);
				#endif

			}
			else
			{
				/*just invalidating entry i1*/
				#ifdef LIBRARY_DEBUG
				printf ("INVALIDATING ");
				print_address(STDOUT,addr[i1].network,addr[i1].prefix);
				#endif
				addr[i1].prefix=INVALID_PREFIX;
				i1++;
			}

		}
		else if ((addr[i1].prefix==INVALID_PREFIX)||(addr[i1].network&supermask)<=(white[i2].network&supermask))
		{
			i1++;
		}
		else
		{
			i2++;
		}
	}
	#ifdef LIBRARY_DEBUG
	print_debug(STDOUT,debug_buf,"END apply_whitelist\n");
	#endif
	return expanded_index;
}

/*
	Please see cidrmerge.h
*/
unsigned int optimize(struct entry *addr,unsigned int len,int do_sort)
{
	unsigned int i,cur;
	unsigned int tmp_net;
	#ifdef LIBRARY_DEBUG
	char debug_buf[MAXLINE];
	unsigned int step=0;

	print_debug(STDOUT,debug_buf,"START optimize\n");
	#endif

	i=0;   /*pointer to last valid position*/
	cur=1; /*pointer to next addr to analize*/

	if (len <= 1)
	{
		/* empty or sigle element array is optimized by definition.*/
		return len;
	}

	if (do_sort)
	{
		sort_entries(addr,len);
	}

	/*Find first valid address and move it to first position*/
	while ((addr[0].prefix==INVALID_PREFIX) && (cur<len))
	{
		#ifdef LIBRARY_DEBUG
		printf ("SEARCH FIRST I: %d CUR: %d\n",i,cur);
		#endif
		if (addr[cur].prefix!=INVALID_PREFIX)
		{
			addr[0].network=addr[cur].network;
			addr[0].prefix=addr[cur].prefix;
			addr[cur].prefix=INVALID_PREFIX;
		}
		cur++;
	}

	while (cur<len)
	{
		#ifdef LIBRARY_DEBUG
		printf ("STEP: %u I: %d CUR: %d\n",step++,i,cur);
		#endif
		#ifdef LIBRARY_DEBUG_FULL
		print_addresses(STDOUT,addr,len,NULL,0);
		#endif

		/*check for expanded networks, they can never conflicts*/
		if (addr[cur].prefix>=EXPANDED_PREFIX)
		{
			#ifdef LIBRARY_DEBUG
			printf ("COPY EXPANDED I: %d CUR: %d\n",i,cur);
			#endif
			i++;
			addr[i].network=addr[cur].network;
			addr[i].prefix=addr[cur].prefix;
			cur++;
			while ((addr[i].prefix>=EXPANDED_PREFIX) && (cur<len))
			{
				#ifdef LIBRARY_DEBUG
				printf ("COPY ADDR[CUR] FOR EXPANDED I: %d CUR: %d\n",i,cur);
				#endif
				if (addr[cur].prefix!=INVALID_PREFIX)
				{
					i++;
					if (cur != i)
					{
						addr[i].network=addr[cur].network;
						addr[i].prefix=addr[cur].prefix;
						addr[cur].prefix=INVALID_PREFIX;
					}
				}
				cur++;
			}
		}
		else
		{
			/*If this test will fail we just skip addr[cur]*/
			if ((addr[cur].prefix<=32)&&((addr[cur].network&TONETMASK(addr[i].prefix))!=addr[i].network))
			{
				tmp_net=TONETMASK(addr[i].prefix-1);

				if ( (addr[i].prefix==addr[cur].prefix) && ( (addr[i].network&tmp_net) == (addr[cur].network&tmp_net) ) )
				{
					#ifdef LIBRARY_DEBUG
					printf ("COLLAPSE I: %d CUR: %d\n",i,cur);
					#endif

					if (i>0)
					{
						addr[cur].prefix=addr[i].prefix-1;
						addr[cur].network&=tmp_net;
						i--;
					}
					else
					{
						addr[i].prefix=addr[i].prefix-1;
						addr[i].network&=tmp_net;
						cur++;
					}
				}
				else
				{
					i++;

					addr[i].network=addr[cur].network;
					addr[i].prefix=addr[cur].prefix;

					cur++;
				}
			}
			else
			{
				#ifdef LIBRARY_DEBUG
				printf ("SKIP CUR: %d\n",cur);
				#endif

				cur++;
			}
		}
	}

	#ifdef LIBRARY_DEBUG
	print_debug(STDOUT,debug_buf,"END optimize\n");
	#endif

	if (addr[i].prefix!=INVALID_PREFIX)
	{
		return i+1;
	}
	else
	{
		return i;
	}
}

