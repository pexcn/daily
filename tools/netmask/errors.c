/* errors.c -- error message handlers.
   Copyright (C) 1998 Robert Stone <talby@trap.mtview.ca.us>

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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_SYSLOG_H
#  include <syslog.h>
#endif /* HAVE_SYSLOG_H */
#include <errno.h>
#include "errors.h"


/* compatability section */
#ifndef HAVE_SYSLOG_H
#warning no syslog facility?  Errors will go to stderr.
#  define syslog(x,y,z)
#  define LOG_DEBUG	7
#  define LOG_WARNING	4
#  define LOG_ERR	3
#endif

#ifndef HAVE_VPRINTF
#error no vprintf? not ANSI C3.159-1989 (``ANSI C'') compliant?
#endif

#ifndef HAVE_STRERROR
#define strerror(x) "system error"
#endif
/* end compatability section */

static char *progname = NULL;

static int show_status = 0;
static int use_syslog = 0;

static int message(int, const char *);

int initerrors(char *pn, int type, int stat) {
#ifdef HAVE_SYSLOG_H
    if(type == 0 || type == 1) use_syslog = type;
#endif /* HAVE_SYSLOG_H */
    if(pn != NULL) progname = pn;
    if(stat == 0 || stat == 1) show_status = stat;
    return(0);
}

int status(const char *fmt, ...) {
    static char buf[1024];
    va_list args;

    if(!show_status) return(0);
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    return(message(LOG_DEBUG, buf));
}

int warn(const char *fmt, ...) {
    static char buf[1024];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    return(message(LOG_WARNING, buf));
}

int panic(const char *fmt, ...) {
    static char buf[1024];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    message(LOG_ERR, buf);
    exit(1);
}

int message(int priority, const char *msg) {
    char buf[1024];

    /* only handle errno if this is not an informational message */
    if(errno && priority < 5) {
	sprintf(buf, "%s: %s", msg, strerror(errno));
	errno = 0;
    } else strcpy(buf, msg);
    if(use_syslog) syslog(priority, "%s", buf);
    else           fprintf(stderr, "%s: %s\n", progname, buf);
    return(0);
}
