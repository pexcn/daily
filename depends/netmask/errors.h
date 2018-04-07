/* errors.h -- error message handlers.
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

#ifndef _HAVE_ERRORS_H
#define _HAVE_ERRORS_H

/* call initerrors before using these other functions
 *
 * these functions seem pretty straightforward to me, the messaging
 * functions take sprintf formatted strings and have a limit of
 * 1024 byte long error messages.
 *	progname should be set to argv[0]
 *	if progname is NULL, it is unchanged
 *	type == 0 for stderr
 *	type == 1 for syslog
 *	otherwise type is unchanged
 *	stat == 0 to skip status reporting
 *	stat == 1 to print status messages
 *	otherwise stat is unchanged
 *	defaults: progname = NULL, type = 0, stat = 0 */
int initerrors(char *progname, int type, int stat);

int status(const char *fmt, ...);
	/* print a status message */

int warn(const char *fmt, ...);
	/* print a warning message */

int panic(const char *fmt, ...);
	/* print an error and exit */
#endif
