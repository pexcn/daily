DESTDIR     =
PREFIX      = /usr/local
EXEC_PREFIX = $(PREFIX)
BINDIR      = $(EXEC_PREFIX:/=)/bin
#SBINDIR     = $(EXEC_PREFIX:/=)/sbin
#LIBDIR_NAME = lib
#LIBDIR      = $(EXEC_PREFIX:/=)/$(LIBDIR_NAME)
SHAREDIR    = $(PREFIX:/=)/share
MANDIR      = $(PREFIX:/=)/man
BASHCOMPDIR = $(SHAREDIR)/bash-completion/completions

EXEMODE ?= 0755
INSMODE ?= 0644
DIRMODE ?= 0755

LN      ?= ln
INSTALL ?= install
MKDIR   ?= mkdir
MKDIRP   = $(MKDIR) -p

DODIR    = $(INSTALL) -d -m $(DIRMODE)
DOEXE    = $(INSTALL) -D -m $(EXEMODE)
DOINS    = $(INSTALL) -D -m $(INSMODE)
DOSYM    = $(LN) -f -s
