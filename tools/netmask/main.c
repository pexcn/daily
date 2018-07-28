/* main.c - a netmask generator
 *
 * Copyright (c) 2013  Robert Stone <talby@trap.mtview.ca.us>,
 *                     Tom Lear <tom@trap.mtview.ca.us>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <math.h>

#include "netmask.h"
#include "errors.h"

struct addrmask {
  u_int32_t neta;
  u_int32_t mask;
  struct addrmask *next;
  struct addrmask *prev;
};

struct option longopts[] = {
  { "version",	0, 0, 'v' },
  { "help",	0, 0, 'h' },
  { "debug",	0, 0, 'd' },
  { "standard",	0, 0, 's' },
  { "cidr",	0, 0, 'c' },
  { "cisco",	0, 0, 'i' },
  { "range",	0, 0, 'r' },
  { "hex",	0, 0, 'x' },
  { "octal",	0, 0, 'o' },
  { "binary",	0, 0, 'b' },
  { "nodns",	0, 0, 'n' },
  { "files",	0, 0, 'f' },
//  { "max",	1, 0, 'M' },
//  { "min",	1, 0, 'm' },
  { NULL,	0, 0, 0   }
};

typedef enum {
  OUT_STD, OUT_CIDR, OUT_CISCO, OUT_RANGE, OUT_HEX, OUT_OCTAL, OUT_BINARY
} output_t;

char version[] = "netmask, version "VERSION;
char vversion[] = __DATE__" "__TIME__;
char usage[] = "Try `%s --help' for more information.\n";
char *progname = NULL;

void disp_std(int domain, nm_addr *n, nm_addr *m) {
  char nb[INET6_ADDRSTRLEN + 1],
       mb[INET6_ADDRSTRLEN + 1];

  inet_ntop(domain, n, nb, INET6_ADDRSTRLEN);
  inet_ntop(domain, m, mb, INET6_ADDRSTRLEN);
  printf("%15s/%-15s\n", nb, mb);
}
static void disp_cidr(int domain, nm_addr *n, nm_addr *m) {
  char nb[INET6_ADDRSTRLEN + 1];
  int cidr = 0;

  inet_ntop(domain, n, nb, INET6_ADDRSTRLEN);
  if(domain == AF_INET) {
    uint32_t mask;
    for(mask = ntohl(m->s.s_addr); mask; mask <<= 1)
      cidr++;
  } else {
      uint8_t i, c;
      for(i = 0; i < 16; i++)
          for(c = m->s6.s6_addr[i]; c; c <<= 1)
              cidr++;
  }
  printf("%15s/%d\n", nb, cidr);
}

static void disp_cisco(int domain, nm_addr *n, nm_addr *m) {
  char nb[INET6_ADDRSTRLEN + 1],
       mb[INET6_ADDRSTRLEN + 1];
  int i;

  if(domain == AF_INET6)
    for(i = 0; i < 16; i++)
      m->s6.s6_addr[i] = ~m->s6.s6_addr[i];
  else
      m->s.s_addr = ~m->s.s_addr;

  inet_ntop(domain, n, nb, INET6_ADDRSTRLEN);
  inet_ntop(domain, m, mb, INET6_ADDRSTRLEN);
  printf("%15s %-15s\n", nb, mb);
}

static void range_num(char *dst, uint8_t *src) {
    /* roughly we must convert a 17 digit base 256 number
     * to a 39 digit base 10 number. */
    char digits[41] = { 0 }; /* ceil(17 * log(256) / log(10)) == 41 */
    int i, j, z, overflow;

    for(i = 0; i < 17; i++) {
        overflow = 0;
        for(j = sizeof(digits) - 1; j >= 0; j--) {
            int tmp = digits[j] * 256 + overflow;
            digits[j] = tmp % 10;
            overflow = tmp / 10;
        }

        overflow = src[i];
        for(j = sizeof(digits) - 1; j >= 0; j--) {
            if(!overflow)
                break;
            int sum = digits[j] + overflow;
            digits[j] = sum % 10;
            overflow = sum / 10;
        }
    }
    /* convert to string */
    z = 1;
    for(i = 0; i < sizeof(digits); i++) {
        if(z && digits[i] == 0)
            continue;
        z = 0;
        *dst++ = '0' + digits[i];
    }
    /* special case for zero */
    if(z)
        *dst++ = '0';
    *dst++ = '\0';
}

static void disp_range(int domain, nm_addr *n, nm_addr *m) {
  char nb[INET6_ADDRSTRLEN + 1],
       mb[INET6_ADDRSTRLEN + 1],
       ns[42];
  uint64_t over = 1;
  uint8_t ra[17] = { 0 };
  int i;

  if(domain == AF_INET6) {
    for(i = 15; i >= 0; i--) {
      m->s6.s6_addr[i] = ~m->s6.s6_addr[i];
      over += m->s6.s6_addr[i];
      m->s6.s6_addr[i] |= n->s6.s6_addr[i];
      ra[i + 1] = over & 0xff;
      over >>= 8;
    }
    ra[0] = over;
  } else {
    over += htonl(~m->s.s_addr);
    for(i = 16; i > 11; i--) {
      ra[i] = over & 0xff;
      over >>= 8;
    }
    m->s.s_addr = n->s.s_addr | ~m->s.s_addr;
  }
  range_num(ns, ra);
  inet_ntop(domain, n, nb, INET6_ADDRSTRLEN);
  inet_ntop(domain, m, mb, INET6_ADDRSTRLEN);
  printf("%15s-%-15s (%s)\n", nb, mb, ns);
}

static void disp_hex(int domain, nm_addr *n, nm_addr *m) {
  if(domain == AF_INET) {
    printf("0x%08x/0x%08x\n", htonl(n->s.s_addr), htonl(m->s.s_addr));
  } else {
    printf("0x%02x%02x%02x%02x%02x%02x%02x%02x"
             "%02x%02x%02x%02x%02x%02x%02x%02x/"
           "0x%02x%02x%02x%02x%02x%02x%02x%02x"
             "%02x%02x%02x%02x%02x%02x%02x%02x\n",
      n->s6.s6_addr[0],  n->s6.s6_addr[1],  n->s6.s6_addr[2],  n->s6.s6_addr[3],
      n->s6.s6_addr[4],  n->s6.s6_addr[5],  n->s6.s6_addr[6],  n->s6.s6_addr[7],
      n->s6.s6_addr[8],  n->s6.s6_addr[9],  n->s6.s6_addr[10], n->s6.s6_addr[11],
      n->s6.s6_addr[12], n->s6.s6_addr[13], n->s6.s6_addr[14], n->s6.s6_addr[15],

      m->s6.s6_addr[0],  m->s6.s6_addr[1],  m->s6.s6_addr[2],  m->s6.s6_addr[3],
      m->s6.s6_addr[4],  m->s6.s6_addr[5],  m->s6.s6_addr[6],  m->s6.s6_addr[7],
      m->s6.s6_addr[8],  m->s6.s6_addr[9],  m->s6.s6_addr[10], m->s6.s6_addr[11],
      m->s6.s6_addr[12], m->s6.s6_addr[13], m->s6.s6_addr[14], m->s6.s6_addr[15]);
  }
}

static void disp_octal(int domain, nm_addr *n, nm_addr *m) {
  if(domain == AF_INET) {
    printf("0%10o/0%10o\n", htonl(n->s.s_addr), htonl(m->s.s_addr));
  } else {
    printf("0%03x%03x%03x%03x%03x%03x%03x%03x"
            "%03x%03x%03x%03x%03x%03x%03x%03x/"
           "0%03x%03x%03x%03x%03x%03x%03x%03x"
            "%03x%03x%03x%03x%03x%03x%03x%03x\n",
      n->s6.s6_addr[0],  n->s6.s6_addr[1],  n->s6.s6_addr[2],  n->s6.s6_addr[3],
      n->s6.s6_addr[4],  n->s6.s6_addr[5],  n->s6.s6_addr[6],  n->s6.s6_addr[7],
      n->s6.s6_addr[8],  n->s6.s6_addr[9],  n->s6.s6_addr[10], n->s6.s6_addr[11],
      n->s6.s6_addr[12], n->s6.s6_addr[13], n->s6.s6_addr[14], n->s6.s6_addr[15],

      m->s6.s6_addr[0],  m->s6.s6_addr[1],  m->s6.s6_addr[2],  m->s6.s6_addr[3],
      m->s6.s6_addr[4],  m->s6.s6_addr[5],  m->s6.s6_addr[6],  m->s6.s6_addr[7],
      m->s6.s6_addr[8],  m->s6.s6_addr[9],  m->s6.s6_addr[10], m->s6.s6_addr[11],
      m->s6.s6_addr[12], m->s6.s6_addr[13], m->s6.s6_addr[14], m->s6.s6_addr[15]);
  }
}

static void bin_str(char *dst, uint8_t *src, int len) {
  int i;
  int j;
  for(i = 0; i < len; i++) {
    for(j = 7; j >= 0; j--) {
      *dst++ = src[i] & (1 << j) ? '1' : '0';
    }
    *dst++ = ' ';
  }
  /* replace the last space with a string terminator */
  dst[-1] = '\0';
}

static void disp_binary(int domain, nm_addr *n, nm_addr *m) {
  char ns[144],
       ms[144];
  uint8_t bits[16];

  if(domain == AF_INET) {
      unsigned long l;
      l = htonl(n->s.s_addr);
      bits[0] = 0xff & (l >> 24);
      bits[1] = 0xff & (l >> 16);
      bits[2] = 0xff & (l >>  8);
      bits[3] = 0xff & (l >>  0);
      bin_str(ns, bits, 4);
      l = htonl(m->s.s_addr);
      bits[0] = 0xff & (l >> 24);
      bits[1] = 0xff & (l >> 16);
      bits[2] = 0xff & (l >>  8);
      bits[3] = 0xff & (l >>  0);
      bin_str(ms, bits, 4);
  } else {
      bin_str(ns, n->s6.s6_addr, 16);
      bin_str(ms, m->s6.s6_addr, 16);
  }
  printf("%s / %s\n", ns, ms);
}

void display(NM nm, output_t style) {
  void (*disp)(int, nm_addr *, nm_addr *) = NULL;

  switch(style) {
    case OUT_STD:    disp = &disp_std;    break;
    case OUT_CIDR:   disp = &disp_cidr;   break;
    case OUT_CISCO:  disp = &disp_cisco;  break;
    case OUT_RANGE:  disp = &disp_range;  break;
    case OUT_HEX:    disp = &disp_hex;    break;
    case OUT_OCTAL:  disp = &disp_octal;  break;
    case OUT_BINARY: disp = &disp_binary; break;
    default: return;
  }
  nm_walk(nm, disp);
}

static inline void add_entry(NM *nm, const char *str, int dns) {
  NM new = nm_new_str(str, dns);
  if(new)
    *nm = nm_merge(*nm, new);
  else
    warn("parse error \"%s\"", str);
}

int main(int argc, char *argv[]) {
  int optc, h = 0, v = 0, f = 0, dns = NM_USE_DNS, lose = 0;
//  u_int32_t min = ~0, max = 0;
  output_t output = OUT_CIDR;

  progname = argv[0];
  initerrors(progname, 0, 0); /* stderr, nostatus */
  while((optc = getopt_long(argc, argv, "shoxdrvbincM:m:f", longopts,
    (int *) NULL)) != EOF) switch(optc) {
   case 'h': h = 1;   break;
   case 'v': v++;     break;
   case 'n': dns = 0; break;
   case 'f': f = 1;   break;
//   case 'M': max = mspectou32(optarg); break;
//   case 'm': min = mspectou32(optarg); break;
   case 'd':
    initerrors(NULL, -1, 1); /* showstatus */
    break;
   case 's': output = OUT_STD;    break;
   case 'c': output = OUT_CIDR;   break;
   case 'i': output = OUT_CISCO;  break;
   case 'r': output = OUT_RANGE;  break;
   case 'x': output = OUT_HEX;    break;
   case 'o': output = OUT_OCTAL;  break;
   case 'b': output = OUT_BINARY; break;
   default: lose = 1; break;
  }
  if(v) {
    if(v == 1) fprintf(stderr, "%s\n", version);
    else fprintf(stderr, "%s, %s\n", version, vversion);
    if(!h) exit(0);
  }
  if(h) {
    fprintf(stderr,
      "This is netmask, an address netmask generation utility\n"
      "Usage: %s spec [spec ...]\n"
      "  -h, --help\t\t\tPrint a summary of the options\n"
      "  -v, --version\t\t\tPrint the version number\n"
      "  -d, --debug\t\t\tPrint status/progress information\n"
      "  -s, --standard\t\tOutput address/netmask pairs\n"
      "  -c, --cidr\t\t\tOutput CIDR format address lists\n"
      "  -i, --cisco\t\t\tOutput Cisco style address lists\n"
      "  -r, --range\t\t\tOutput ip address ranges\n"
      "  -x, --hex\t\t\tOutput address/netmask pairs in hex\n"
      "  -o, --octal\t\t\tOutput address/netmask pairs in octal\n"
      "  -b, --binary\t\t\tOutput address/netmask pairs in binary\n"
      "  -n, --nodns\t\t\tDisable DNS lookups for addresses\n"
      "  -f, --files\t\t\tTreat arguments as input files\n"
//      "  -M, --max mask\t\tLimit maximum mask size\n"
//      "  -m, --min mask\t\tLimit minimum mask size (drop small ranges)\n"
      "Definitions:\n"
      "  a spec can be any of:\n"
      "    address\n"
      "    address:address\n"
      "    address:+address\n"
      "    address/mask\n"
      "  an address can be any of:\n"
      "    N\t\tdecimal number\n"
      "    0N\t\toctal number\n"
      "    0xN\t\thex number\n"
      "    N.N.N.N\tdotted quad\n"
      "    hostname\tdns domain name\n"
      "  a mask is the number of bits set to one from the left\n", progname);
    exit(0);
  }
  if(lose || optind == argc) {
    fprintf(stderr, usage, progname);
    exit(1);
  }
  NM nm = NULL;
  for(;optind < argc; optind++) {
    if(f) {
      char buf[1024];
      FILE *fp = strncmp(argv[optind], "-", 2) ?
        fopen(argv[optind], "r") : stdin;
      if(!fp) {
        fprintf(stderr, "fopen: %s: %s\n",
          argv[optind], strerror(errno));
        continue;
      }
      while(fscanf(fp, "%1023s", buf) != EOF)
        add_entry(&nm, buf, dns);
    } else
      add_entry(&nm, argv[optind], dns);
  }
  display(nm, output);
  return(0);
}

