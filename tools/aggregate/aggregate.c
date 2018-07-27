/* $Id: aggregate.c,v 1.12 2002/03/06 16:55:38 jabley Exp $
 * 
 * Copyright (c) 2000-2001 by Metromedia Fiber Network Services, Inc.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND METROMEDIA FIBER NETWORK SERVICES,
 * INC. ("MFN") DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL MFN BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 *   Metromedia Fiber Network
 *   360 Hamilton Avenue
 *   White Plains, NY 10601
 * 
 *   http://www.mmfn.com/
 *
 * aggregate:
 *
 *   Find the most general set of route prefixes which exactly
 *   include a supplied set. Intended to help launder peer-
 *   supplied route filters into a sensibly-sized list.
 *
 * jabley@mfnx.net
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/param.h>


#define MAX_buf 63

#define LESSTHAN(p,q) \
  ( (ntohl(p->prefix.s_addr) < ntohl(q->prefix.s_addr)) || \
    (ntohl(p->prefix.s_addr) == ntohl(q->prefix.s_addr) && \
     p->masklen < q->masklen) )


struct filter_list
{
  struct in_addr prefix;
  int masklen;
  enum { origin_SUPPLIED, origin_REMOVED, origin_ADDED } origin;
  int line;
  struct filter_list *next;
};


int moanf(int fatal, char *fmt, ...)
{
  va_list ap;
  int r;

  va_start(ap, fmt);
  r = fprintf(stderr, "aggregate: ");
  r += vfprintf(stderr, fmt, ap);
  r += fprintf(stderr, "\n");
  va_end(ap);
  if (fatal) exit(1);
  return(r);
}


/* we are rolling our own inet_aton() variant here, since the
 * common libc practice of interpreting leading zeros on octets
 * as octal numbers is non-intuitive, and is not compatible
 * with zero-padded (decimal) data from whois.radb.net. Thanks
 * to Arnold Nipper for noticing. This function was mainly
 * ripped from OpenBSD source, since that's what happened to be
 * handy.
 */

int my_inet_aton(const char *cp, struct in_addr *addr)
{
  struct in_addr val;
  int n;
  char c;
  u_int parts[4];
  u_int *pp = parts;

  c = *cp;
  for (;;) {
    /*
     * collect number up to ``.'' Values are not specified as for C;
     * preceding 0s still cause the digits to be interpreted as
     * decimal, and "x" characters are illegal. This is different
     * to the behaviour of the libc routine inet_aton().
     */
    if (!isdigit(c))
      return (0);

    val.s_addr = 0;

    for (;;) {
      if (isdigit(c)) {
        val.s_addr = (val.s_addr * 10) + (c - '0');
        c = *++cp;
      } else
        break;
    }

    if (c == '.') {
      /*
       * Internet format:
       *      a.b.c.d
       *      a.b.c   (with c treated as 16 bits)
       *      a.b     (with b treated as 24 bits)
       */
      if (pp >= parts + 3)
        return (0);
      *pp++ = val.s_addr;
      c = *++cp;
    } else
      break;
  }

  /* Check for trailing characters. */
  if (c != '\0' && (!isascii(c) || !isspace(c)))
    return (0);

  /* Concoct the address according to the number of parts specified. */
  n = pp - parts + 1;
  switch (n) {
    case 0:       /* initial nondigit */
      return (0);

    case 1:       /* a -- 32 bits */
      break;

    case 2:       /* a.b -- 8.24 bits */
      if ((val.s_addr > 0xffffff) || (parts[0] > 0xff))
        return (0);
      val.s_addr |= parts[0] << 24;
      break;

    case 3:       /* a.b.c -- 8.8.16 bits */
      if ((val.s_addr > 0xffff) || (parts[0] > 0xff) || (parts[1] > 0xff))
        return (0);
      val.s_addr |= (parts[0] << 24) | (parts[1] << 16);
      break;

    case 4:       /* a.b.c.d -- 8.8.8.8 bits */
      if ((val.s_addr > 0xff) || (parts[0] > 0xff) || (parts[1] > 0xff) || \
          (parts[2] > 0xff))
        return (0);
      val.s_addr |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
      break;
  }

  if (addr)
    addr->s_addr = htonl(val.s_addr);
  return (1);
}


int main(int argc, char **argv)
{
  int verbose = 0, quiet = 0, truncate = 0;
  int default_prefix_length = 0;
  int max_prefix_length = 32;
  int max_prefix_opt = 32;
  int ch, line, i, masklen, found;
  struct filter_list *head, *p, *q, *r, *s;
  char buf[MAX_buf + 1];
  struct in_addr prefix;

  /* initialise */

  head = NULL;

  /* parse command-line switches */

  while ((ch = getopt(argc, argv, "tqvm:o:p:")) != -1)
    switch (ch)
    {
      case 'm':
        max_prefix_length = atoi(optarg);
        if (max_prefix_length < 1 || max_prefix_length > 32)
          moanf(1, "can't set maximum prefix length to %d", max_prefix_length);
        break;

      case 'o':
        max_prefix_opt = atoi(optarg);
        if (max_prefix_opt < 1 || max_prefix_opt > 32)
          moanf(1, "can't set maximum prefix length for optimisation to %d", max_prefix_opt);
        break;

      case 'p':
        default_prefix_length = atoi(optarg);
        if (default_prefix_length < 1 || default_prefix_length > 32)
          moanf(1, "can't set default prefix length to %d", \
            default_prefix_length);
        break;

      case 'q':
        quiet = 1;
        break;

      case 't':
        truncate = 1;
        break;

      case 'v':
        verbose = 1;
        break;

      case '?':
        moanf(1, "Syntax: aggregate [-m max-length] [-o max-opt-length] [-p default-length] [-q] [-t] [-v]");
        break;
    }

  if (default_prefix_length > max_prefix_length && !quiet)
    moanf(0, "default prefix length (%d) is greater than maximum prefix length (%d)", \
      default_prefix_length, max_prefix_length);

  if (!quiet)
    moanf(0, "maximum prefix length permitted will be %d", max_prefix_length);

  if (max_prefix_opt > max_prefix_length && !quiet)
    moanf(0, "no optimisation for prefixes longer than %d bits", max_prefix_length);

  if (default_prefix_length && !quiet)
    moanf(0, "prefix length of %d bits will be used where none specified", \
      default_prefix_length);

  /* read a list of prefixes on stdin */

  line = 0;

  while (fgets(buf, MAX_buf, stdin))
  {
    line++;

    /* parse the line we just read */

    if (default_prefix_length)
    {
      i = 0;
      while (buf[i] == '.' || (buf[i] >= '0' && buf[i] <= '9') && \
        i < MAX_buf) i++;
      if (i == MAX_buf)
      {
        if (!quiet)
          moanf(0, "[line %d] line too long; ignoring line", line);
        continue;
      }
      buf[i] = 0;
      masklen = default_prefix_length;
    } else {
      i = 0;
      while (buf[i] != '/' && i < MAX_buf) i++;
      if (i == MAX_buf)
      {
        if (!quiet)
          moanf(0, "[line %d] line too long; ignoring line", line);
        continue;
      }
      buf[i] = 0;
  
      masklen = atoi(buf + i + 1);
      if (masklen < 1 || masklen > max_prefix_length)
      {
        if (!quiet)
          moanf(0, "[line %d] mask length %d out of range; ignoring line", \
            line, masklen);
        continue;
      }
    }

    if (my_inet_aton(buf, &prefix) == 0)
    {
      if (!quiet)
        moanf(0, "[line %d] can't parse prefix '%s'; ignoring line", line, \
          buf);
      continue;
    }

    if ((ntohl(prefix.s_addr) & (0xffffffff >> masklen)) != 0 && masklen < 32)
    {
      if (truncate) {
        prefix.s_addr = htonl(ntohl(prefix.s_addr) & \
          (0xffffffff << (32-masklen)));
        if (!quiet)
          moanf(0, "[line %d] prefix %s/%d truncated to %s/%d", \
            line, buf, masklen, inet_ntoa(prefix), masklen);
      } else {
        if (!quiet)
          moanf(0, "[line %d] %s/%d inconsistent; ignoring line", \
            line, inet_ntoa(prefix), masklen);
        continue;
      }
    }

    /* create a list entry, and insertion sort it */

    if ((p = malloc(sizeof(struct filter_list))) == NULL)
      moanf(1, "[line %d] malloc() failure", line);

    p->prefix.s_addr = prefix.s_addr;
    p->masklen = masklen;
    p->origin = origin_SUPPLIED;
    p->line = line;

    if (head)
    {
      q = head;
      r = NULL;

      while (LESSTHAN(q, p) && q->next)
      {
        r = q;
        q = q->next;
      }

      if (LESSTHAN(q, p))
      {
        p->next = q->next;
        q->next = p;
      } else {
        if (r)
        {
          p->next = r->next;
          r->next = p;
        } else {
          p->next = q;
          head = p;
        }
      }

    } else {
      head = p;
      head->next = NULL;
    }

  }

  if (!head) {
    if (!quiet)
      moanf(0, "no prefixes supplied");
    exit(0);
  }

  /* list of prefixes is now complete; do our crude optimisations */

  /* first eliminate overlapping prefixes */

  p = head->next;
  while (p)
  {
    q = head;
    while (q && q != p)
    {
      if ((p->masklen <= max_prefix_opt) &&
        (ntohl(p->prefix.s_addr) / (1<<(32 - q->masklen))) == 
        (ntohl(q->prefix.s_addr) / (1<<(32 - q->masklen))))
      {
        p->origin = origin_REMOVED;
        break;
      }
      q = q->next;
    }
    p = p->next;
  }

  /* now hunt for adjacent entries that can be combined */

  do
  {
    found = 0;
    p = head;
    r = NULL;
    while (p)
    {
      while (p->next && p->origin == origin_REMOVED)
        p = p->next;
      if (!(p->next))
        break;

      q = p->next;
      while (q->next && q->origin == origin_REMOVED)
        q = q->next;
      if (q->origin == origin_REMOVED)
        break;

      if ( (p->masklen == q->masklen) && \
           (p->masklen <= max_prefix_opt) && \
           (ntohl(p->prefix.s_addr) % (2 << (32 - p->masklen)) == 0) && \
           (ntohl(q->prefix.s_addr) - ntohl(p->prefix.s_addr) == \
             (1 << (32 - p->masklen))))
      {
        if ((s = malloc(sizeof(struct filter_list))) == NULL)
          moanf(1, "malloc() failure during adjacency collapse");
        s->prefix = p->prefix;
        s->masklen = p->masklen - 1;
        s->origin = origin_ADDED;
        s->next = p;
        s->line = 0;

        p->origin = origin_REMOVED;
        q->origin = origin_REMOVED;

        if (r)
          r->next = s;
        else
          head = s;

        found = 1;
        break;
      }
      r = p;
      p = p->next;
    }
  } while (found);

  /* output the results */

  p = head;
  while (p)
  {
    if (verbose)
    {
      printf("[%5d] ", p->line);
      switch (p->origin)
      {
        case origin_SUPPLIED:
          putchar(' ');
          break;
  
        case origin_REMOVED:
          putchar('-');
          break;
  
        case origin_ADDED:
          putchar('+');
          break;
      }
      putchar(' ');
    }
    if (verbose || p->origin != origin_REMOVED)
      printf("%s/%d\n", inet_ntoa(p->prefix), p->masklen);
    p = p->next;
  }

  return(0);
}

