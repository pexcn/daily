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

static char *version="1.5.3";
static char *author="Daniele Depetrini (depetrini@libero.it)";

void print_address(STREAM file,unsigned int net, unsigned char pref);

/* static array to speedup prefix print */
static char *dotted[]={
								"0","1","2","3","4","5","6","7","8","9",
							   "10","11","12","13","14","15","16","17","18","19",
								"20","21","22","23","24","25","26","27","28","29",
								"30","31","32","33","34","35","36","37","38","39",
								"40","41","42","43","44","45","46","47","48","49",
								"50","51","52","53","54","55","56","57","58","59",
								"60","61","62","63","64","65","66","67","68","69",
								"70","71","72","73","74","75","76","77","78","79",
								"80","81","82","83","84","85","86","87","88","89",
								"90","91","92","93","94","95","96","97","98","99",
								"100","101","102","103","104","105","106","107","108","109",
							   "110","111","112","113","114","115","116","117","118","119",
								"120","121","122","123","124","125","126","127","128","129",
								"130","131","132","133","134","135","136","137","138","139",
								"140","141","142","143","144","145","146","147","148","149",
								"150","151","152","153","154","155","156","157","158","159",
								"160","161","162","163","164","165","166","167","168","169",
								"170","171","172","173","174","175","176","177","178","179",
								"180","181","182","183","184","185","186","187","188","189",
								"190","191","192","193","194","195","196","197","198","199",
								"200","201","202","203","204","205","206","207","208","209",
							   "210","211","212","213","214","215","216","217","218","219",
								"220","221","222","223","224","225","226","227","228","229",
								"230","231","232","233","234","235","236","237","238","239",
								"240","241","242","243","244","245","246","247","248","249",
								"250","251","252","253","254","255"
								};

#ifdef INLINE
__inline
#endif
static unsigned char collect_octet(char *line,int current_position,int *end_position)
{
		unsigned int number=0;

		*end_position=current_position;
		while (line[current_position] >='0' && line[current_position] <='9')
		{
			number*=10;
			number+=line[current_position]-48;
			current_position++;
		}

		/*update return pointer only if it's a valid octect*/
		if (number<256)
		{
			*end_position=current_position;
		}

		return number;
}

/*
	Parse a text string in CIDR format
*/
#ifdef INLINE
__inline
#endif
static int parse_line(char* line,struct entry *res)
{
	int i,n;
	int end=-1,stop=0;

	i=0;
	n=24;
	res->network=0;
	res->prefix=INVALID_PREFIX;

	while ((n>=0) && !stop)
	{
		#ifdef INPUT_DEBUG
		printf("Before collect i:%d end:%d line[i]:%c\n",i,end,line[i]);
		#endif

		res->network|=collect_octet(line,i,&end)<<n;

		#ifdef INPUT_DEBUG
		printf("After collect i:%d end:%d line[end]:%c\n",i,end,line[end]);
		#endif

		if ((end==i) && (end>0))
		{
			if (line[end-1]=='.')
			{
				n+=8;
			}
			else
			{
				break;
			}
		}
		switch (line[end])
		{
			case '.':
				if (n>0)
				{
					i=end+1;
				}
				else
				{
					stop=1;
				}
				break;
			case '/':
				if (n==0)
				{
					i=end+1;
					#ifdef INPUT_DEBUG
					printf("Before collect prefix i:%d end:%d line[i]:%c\n",i,end,line[i]);
					#endif
					res->prefix=collect_octet(line,i,&end);
					if ((line[end] != '\0') && (line[end] != ' '))
					{
						res->prefix=INVALID_PREFIX;
					}
					#ifdef INPUT_DEBUG
					printf("After collect prefix i:%d end:%d prefix:%d\n",i,end,res->prefix);
					#endif
				}

				stop=1;
				break;
			case ' ':
				/*no break here*/
				fprintf(stderr,"WARNING: not considering characters after space in line %s",line);
			case '*':
				if ((line[end]=='*') && (line[end+1] != '\0'))
				{
					res->prefix=INVALID_PREFIX;
					n=32;
				}
			case '\0':
/*				line[end]='\n';
				line[end+1]='\0';
			case '\n':
*/				if (n<24)
				{
					res->prefix=32-n;
				}
				stop=1;
				break;
			default:
				stop=1;
				break;
		}
		n-=8;
	}
	#ifdef INPUT_DEBUG
	printf("Final prefix: %d\n",res->prefix);
	#endif
	if (res->prefix>32)
	{
		return 0;
	}

	/* final sanity check, very important: library functions will not work if this costraint is not enforced */
	if ((res->network&TONETMASK(res->prefix))!=res->network)
	{
		return 0;
	}

	return 1;
}

#ifdef INLINE
__inline
#endif
static int STRCPY(char *dst,char *src)
{
	int i=0;

	for (;src[i]!='\0';i++)
		dst[i]=src[i];

	return i;
}

#ifdef OUTPUT_VERBOSITY
static unsigned int counter=0;
#endif
void print_address(STREAM file,unsigned int net, unsigned char pref)
{
	char buf[MAXCIDR+2];
	int pos=0,len;
#ifdef OUTPUT_VERBOSITY
	fprintf(stdout,"N: %u\tVNet: %u\tMask: %d.%d.%d.%d\t",counter++,net,TONETMASK(pref)>>24,(TONETMASK(pref)&MASK2)>>16,(TONETMASK(pref)&MASK3)>>8,TONETMASK(pref)&MASK4);

/*	fprintf(file,"N: %u\tMask: %d.%d.%d.%d\t ",counter++,TONETMASK(pref)>>24,(TONETMASK(pref)&MASK2)>>16,(TONETMASK(pref)&MASK3)>>8,TONETMASK(pref)&MASK4);
*/
#else
	if (pref!=INVALID_PREFIX)
	{
#endif
		pos+=STRCPY(buf+pos,dotted[net>>24]);
		buf[pos++]='.';
		pos+=STRCPY(buf+pos,dotted[(net&MASK2)>>16]);
		buf[pos++]='.';
		pos+=STRCPY(buf+pos,dotted[(net&MASK3)>>8]);
		buf[pos++]='.';
		pos+=STRCPY(buf+pos,dotted[net&MASK4]);
		buf[pos++]='/';
		pos+=STRCPY(buf+pos,dotted[pref]);
		buf[pos++]='\n';

		len=WRITE(file,buf,pos);
		if (len<pos)
		{
			fprintf(stderr,"Written only %d bytes instead of %d, aborting\n",len,pos);
			exit(1);
		}


#ifndef OUTPUT_VERBOSITY
	}
#endif
}

#ifdef INLINE
__inline
#endif
unsigned int print_address2(unsigned int net, unsigned char pref,char *buf,unsigned int pos)
{
	if (pref!=INVALID_PREFIX)
	{
		pos+=STRCPY(buf+pos,dotted[net>>24]);
		buf[pos++]='.';
		pos+=STRCPY(buf+pos,dotted[(net&MASK2)>>16]);
		buf[pos++]='.';
		pos+=STRCPY(buf+pos,dotted[(net&MASK3)>>8]);
		buf[pos++]='.';
		pos+=STRCPY(buf+pos,dotted[net&MASK4]);
		buf[pos++]='/';
		pos+=STRCPY(buf+pos,dotted[pref]);
		buf[pos++]='\n';
	}
	return pos;
}

int get_entries(STREAM f,struct entry ** addr,unsigned int *size)
{
	unsigned int i;
	int start=0,end=0,n,stop,quit;
	char buf[BUFFER];
	char line[MAXLINE+1];
	char error[MAXLINE+50];

	i=*size=0;

	quit=0;
	while (!quit)
	{
		/*buffered read */
		#ifdef INPUT_DEBUG
		printf("READ START, start %d, end %d\n",start,end);
		#endif
		stop=0;
		n=0;
		/*Read one line*/
		while (!stop)
		{
			while((end>start) && (buf[start]!= '\n') && (buf[start]!= '\r'))
			{
				#ifdef INPUT_DEBUG
				printf("READ COPY, start %d, end %d n %d\n",start,end,n);
				#endif
				line[n]=buf[start];
				n++;
				if (n!=MAXLINE)
				{
					start++;
				}
				else
				{
					fprintf(stderr,"Error, line too long, taking first %d chars\n",MAXLINE);
					buf[start]='\n';
				}
			}
			if (end>start)
			{
				line[n]='\0';
				start++;
				/* DOS line separator */
				if ((end>start) && (buf[start]== '\n')) start++;
				stop=1;
			}
			else
			{
				#ifdef INPUT_DEBUG
				printf("READ DO READ\n");
				#endif
				start=0;
				end=READ(f,buf,BUFFER);
				if (end<=0)
				{
					if (!N_EOF(f,end))
					{
						fprintf(stderr,"Error reading file, aborting\n");
						exit (1);
					}
					quit=1;
					line[n]='\0';
					stop=1;
				}
			}
		}
		#ifdef INPUT_DEBUG
		printf("READ END: read %d, start %d, end %d n %d\n",n+1,start,end,n);
		#endif

		/* end buffered read */

		if (*size<=i)
		{
			*size+=BUCKET;
			*addr=(struct entry *)realloc(*addr,sizeof(struct entry)*(*size));
			if (addr==NULL)
			{
				fprintf(stderr,"Error allocating %lu bytes\n",(unsigned long)sizeof(struct entry)*(*size));
				exit(1);
			}

		}

		if (!parse_line(line,&((*addr)[i])))
		{
			int line_len;

			n=STRCPY(error,"Invalid line ");
			line_len=STRCPY(error+n,line);
			n+=line_len;
			n+=STRCPY(error+n,"\n");
			if (line_len>0)
			{
				WRITE(STDERR,error,n);
			}
		}
		else
		{
			i++;
		}
	}

	return i;
}

void print_addresses(STREAM f,struct entry *addr,int size,struct entry *expanded,int level)
{
	int i=0,pos=0,len;
	char buf[BUFFER];

	if (level>256)
	{
		/*Sanity ckeck */
		fprintf(stderr,"Array too nested: internal error, aborting. Please report this issue together with input files to depetrini@libero.it\n");
		exit(1);
	}

	#ifdef OUTPUT_VERBOSITY
	counter=0;
	printf ("++TOPRINT: %u LEVEL %d++\n",size,level);
	#endif

	while (i<size)
	{
		if (addr[i].prefix<EXPANDED_PREFIX)
		{
			#ifdef OUTPUT_VERBOSITY
			print_address(f,addr[i].network,addr[i].prefix);
			#else
			pos=print_address2(addr[i].network,addr[i].prefix,buf,pos);
			if (pos>BUFFER-MAXCIDR-2)
			{
				len=WRITE(f,buf,pos);
				if (len<pos)
				{
					fprintf(stderr,"Written only %d bytes instead of %d, aborting\n",len,pos);
					exit(1);
				}
				pos=0;
			}
			#endif
		}
		else
		{
			if (expanded==NULL)
			{
				/*Sanity ckeck */
				fprintf(stderr,"Expanded is NULL. Please report this issue together with input files and paramethers to depetrini@libero.it\n");
				exit(1);
			}

			#ifdef OUTPUT_VERBOSITY
			printf("EXPANDED position %d,num expanded %d, logical position %d\n",i,addr[i].prefix-EXPANDED_PREFIX,addr[i].network);
			#endif
			len=WRITE(f,buf,pos);
			if (len<pos)
			{
				fprintf(stderr,"Written only %d bytes instead of %d, aborting\n",len,pos);
				exit(1);
			}

			pos=0;
			print_addresses(f,&(expanded[addr[i].network]),addr[i].prefix-EXPANDED_PREFIX,expanded,level+1);
		}

		i++;
	}

	/* flush the buffer */
	if (pos>0)
	{
		len=WRITE(f,buf,pos);
		if (len<pos)
		{
			fprintf(stderr,"Written only %d bytes instead of %d, aborting\n",len,pos);
			exit(1);
		}
	}

	#ifdef OUTPUT_VERBOSITY
	printf ("--END PRINT--\n");
	#endif
}

int main (int argc, char *argv[])
{
	struct entry *addr=NULL;
	struct entry *white_list=NULL;
	struct entry *expanded_list=NULL;
	unsigned int len1,len2,len_expanded=0;
	unsigned int size1=0,size2=0,size_expanded=0;
	STREAM WHITE;
#ifdef TIMESTAMP
	struct timeval now,start,prev;
	FILE *PROFILE;
	unsigned int in_input=0;
#endif

	if ((argc >1)&&(strcmp(argv[1],"-v")==0))
	{
		printf("%s by %s\n",version,author);
		exit(0);
	}

	if ((argc >1)&&(strcmp(argv[1],"-h")==0))
	{
		printf("Usage: cidrmerge [whitelist file] [NOOPTIMIZE]\n");
		exit(0);
	}

#ifdef TIMESTAMP
	PROFILE=fopen("timestamp.out","w");
	if (!PROFILE)
	{
		fprintf(stderr,"Error opening profile file timestamp,out, aborting\n");
		exit (1);
	}
	gettimeofday(&now,NULL);
	start.tv_sec=prev.tv_sec=now.tv_sec;
	start.tv_usec=prev.tv_usec=now.tv_usec;
	fprintf(PROFILE,"START %s",ctime(&start.tv_sec));
#endif

	len1=get_entries(STDIN,&addr,&size1);

	len2=0;

	if (argc >1)
	{
		if ((WHITE=OPEN(argv[1])))
		{
			len2=get_entries(WHITE,&white_list,&size2);
			CLOSE(WHITE);
		}
		else
		{
			fprintf(stderr,"Error opening %s\n",argv[1]);
			exit(1);
		}
	}

#ifdef TIMESTAMP
	in_input=len1;
	gettimeofday(&now,NULL);
	fprintf(PROFILE,"%d get_entries\n", (int)((now.tv_sec-prev.tv_sec)*PRECISION+(now.tv_usec-prev.tv_usec)/PRECISION_INV));
#endif

	sort_entries(addr,len1);
	sort_entries(white_list,len2);
#ifdef TIMESTAMP
	prev.tv_sec=now.tv_sec;
	prev.tv_usec=now.tv_usec;
	gettimeofday(&now,NULL);
	fprintf(PROFILE,"%d sort_entries\n", (int)((now.tv_sec-prev.tv_sec)*PRECISION+(now.tv_usec-prev.tv_usec)/PRECISION_INV));
#endif

	if (len2>0)
	{
		len_expanded=apply_whitelist(addr,&expanded_list,white_list,len1,len2,&size_expanded,0);
#ifdef TIMESTAMP
		prev=now;
		gettimeofday(&now,NULL);
		fprintf(PROFILE,"%d apply_whitelist\n", (int)((now.tv_sec-prev.tv_sec)*PRECISION+(now.tv_usec-prev.tv_usec)/PRECISION_INV));
#endif
	}

	if (argc < 3)
	{
		len1=optimize(addr,len1,0);
#ifdef TIMESTAMP
		prev.tv_sec=now.tv_sec;
		prev.tv_usec=now.tv_usec;
		gettimeofday(&now,NULL);
		fprintf(PROFILE,"%d optimize\n", (int)((now.tv_sec-prev.tv_sec)*PRECISION+(now.tv_usec-prev.tv_usec)/PRECISION_INV));
#endif
	}

	#ifdef LIBRARY_DEBUG
	printf ("FINAL RESULT\n");
	#endif

	print_addresses(STDOUT,addr,len1,expanded_list,0);

#ifdef TIMESTAMP
	prev.tv_sec=now.tv_sec;
	prev.tv_usec=now.tv_usec;
	gettimeofday(&now,NULL);
	fprintf(PROFILE,"%d print_addresses\n", (int)((now.tv_sec-prev.tv_sec)*PRECISION+(now.tv_usec-prev.tv_usec)/PRECISION_INV));
#endif

	free(addr);
	if (len2>0)
	{
		free(white_list);
	}
	if (expanded_list)
	{
		free(expanded_list);
	}

#ifdef TIMESTAMP
	gettimeofday(&now,NULL);
	fprintf(PROFILE,"Blacklist: in input %u, expanded %u final optimize %u\n",in_input,len_expanded,len1);
	fprintf(PROFILE,"Whitelist: total %u\n",len2);
	fprintf(PROFILE,"Total execution time %d %s.\n", (int)((now.tv_sec-start.tv_sec)*PRECISION+(now.tv_usec-start.tv_usec)/PRECISION_INV),PRECISION_STRING);
	fprintf(PROFILE,"END %s",ctime(&now.tv_sec));
	fclose(PROFILE);
#endif

	return 0;
}
