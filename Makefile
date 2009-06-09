#
# Makefile for the movie database programs
#
# Version 3.2 29-Nov-94
#  by Philippe Queinnec <queinnec@dgac.fr> or <queinnec@enseeiht.fr>
#   3.1/3.2 modifications by Colin Needham
#
# Make without arguments defaults to "compile" + "installbin" + "cleanobj" +
#  "installman"
#
# "make compile" generates all the executables.
#
# "make installbin" installs the executables.
#
# "make update" updates the list files from the ftp server (using lfetch)
#   and then does a "make update-local".
#   NOTE: see LFETCHOPT before using this.
#
# "make update-local" updates the databases and remove the old lists
#   according to KEEPLIST.
#
# "make install" installs whole system (all, update)
#
# "make install-local" installs whole system (all, update) from local files
#
# "make reset-ftp" removes the file that logs file sizes on the FTP site
#   so that the next time the site is checked, all the lists will be fetched
#   NOTE: only to be used when absolutely necessary
#
# "make databases" generates the databases.
#
# "make compress" compresses all the data and key files, but leaves the 
#   index files uncompressed since they don't compress too well and performance
#   is better with them uncompressed
#
# "make maxcompress" compresses all the data, key and index files for the
#   maximum possible compression
#
# "make touch-dbs" creates empty files for any missing databases, use if you
#  don't have room for everything and you've deleted some of the files
#
# "make cleanlists" removes all the lists without any check.
#
# "make cleanobj" removes the intermediary object files. 
#
# "make clean" removes the executables and the intermediary object files.
#
# "make cleandbs" removes the databases which can be regenerated from
#   the lists (ie, it removes actors.data, ...). Use it at your own risks: 
#   if you have lost the original lists, you won't be able to regenerate the 
#   databases, except by doing a "make update".
#
# "make veryclean" removes all the databases, all the lists, all the
#   executables and all the intermediary object files. You should get
#   almost the initial distribution.
#
# "make installman" installs the manual-pages in MANDIR.
#
# "make allman" makes postscript, catman and text man pages

#-------------------------------------------------------------------------
#
# Configuration Options
#
#-------------------------------------------------------------------------

# Options for lfetch
# YOU MUST SET THE -user option to your e-mail address 
# for example: LFETCHOPT = -auto -user userid@machine.domain
# Note: -auto is a special option for lfetch, do not delete it!
LFETCHOPT = -auto

# Your C Compiler
# NOTE: must be ANSI compatible, use gcc if your standard compiler isn't!
#CC = gcc
CC = cc

# C flags
CFLAGS = -O -DCOMPRESS

# Linker
LD = $(CC)

# ld flags
#LDFLAGS = $(CFLAGS) -s
LDFLAGS = $(CFLAGS)

# ZLISTCAT is a command to cat a compressed list file. It must be capable
# of cat'ing files generated with 'gzip'. For security you are advised to
# use an absolute path
#
# ZLISTCATOPTS specifies any options required for ZLISTCAT
#
# ZLISTEXT is the extension used to identify a compressed list file. 
#
# The variables ZDBSCAT, ZDBSCATOPTS, ZDBSEXT and ZDBSCOMPRESS tell the system 
# how to compress the database files if you don't have enough disk-space to 
# hold them in their uncompressed form. If you have it, 'gzip' is preferable 
# since it is faster and gives a better compression than `compress'
#
# ZDBSCOMPRESS is the command to compress the databases. It is only used
# in the Makefile so you can depend on your path.
# 
# ZDBSEXT is the extension used to identify the compressed database file. For
# 'gzip' it is '.gz' or for 'compress' it is '.Z' 
#
# ZDBSCAT is a command to cat a compressed database file. If using 'gzip' 
# for ZDBSCOMPRESS use 'gzcat' (if you have it) or 'gzip' (in which case 
# set ZDBSCATOPTS to -cd). For security you are advised to use an absolute path
#
# ZDBSCATOPTS specifies any options required for ZDBSCAT (e.g. -cd for gzip)
#
# If you don't want any support for compressed databases, remove the
# definition COMPRESS from the CFLAGS variable

ZLISTEXT = .gz
ZLISTCAT = /usr/bin/gzip
ZLISTCATOPTS = -cd

ZDBSCOMPRESS = gzip
ZDBSEXT = .gz
ZDBSCAT = /usr/bin/gzip
ZDBSCATOPTS = -cd


# Set KEEPLIST to @true if you want to keep the lists.
# Set it to the empty string if you want to remove the lists as soon as
# possible, ie as soon as it has been used to generate the corresponding
# database.
#KEEPLIST =
KEEPLIST = @true


# Set AUTOUNCOMPRESS to nothing if you want to keep the source lists as
# compressed files (that is, as they come from the server). Setting it
# to nothing will reduce the extra space required during the install.
#AUTOUNCOMPRESS =
AUTOUNCOMPRESS = autouncompress

# Options for mkdb (for instance -m, -debug or -nochar)
# set to -nochar to ignore character names when processing the cast lists
#MKDBOPT = -nochar
MKDBOPT = 


# Where are the source lists?
#RAWDIR = ../lists/
RAWDIR = `pwd`/lists/

# Where will the databases be?
#DBDIR = ../dbs/
DBDIR = `pwd`/dbs/

# Secondary files
#ETCDIR = ../etc/
ETCDIR = `pwd`/etc/

# Source files
#SRCDIR = ../src/
SRCDIR = `pwd`/src/

# Executable files
#BINDIR = ../bin/
BINDIR = `pwd`/bin/

# Top-level manpages directory (man1 should be a subdirectory of this)
#MANDIR = /usr/man/
MANDIR = `pwd`/man/

# Main directory
#MAINDIR = ./
MAINDIR = `pwd`/

# Documentation directory
#DOCDIR = ../docs/
DOCDIR = `pwd`/docs/

# --------------------------------------------------------------------------
#
# The following variables control the use of log files for tracking usage
# of the movie database programs. Probably only useful on a multi-user system.

LOGFILENAME = /tmp/.imdb_log
LOGFILE = 0 # 1 for logging

# --------------------------------------------------------------------------
#
# The following variables are used only if you choose to compile the
# completion programs from imoviedb.tar.Z which is distributed as a
# separate package. In the top-level directory, untar imoviedb.tar.Z 
# and *read imoviedb/README*.

# Choose your pager program ("more" or "less")
MORE = less

# If you have read imoviedb/README, you know that you need the readline
# library.
# Tell me where it is installed.
READLINEINCDIR = /usr/local/include
READLINELIBDIR = /usr/local/lib

# If you have the vfork() system call, you should probably use it instead
# of fork(). 'man vfork' will probably tell you if it exists.
#VFORK = 1
VFORK = 0


# libreadline.a may need additional libraries (for instance -lbsd on Linux).
# See imoviedb/MACHINES for more information
#READLINEAUXLIBS = -lbsd
READLINEAUXLIBS = 

# --------------------------------------------------------------------------

#
# SYSTEM SPECIFICS
# You probably have nothing to change here.

# If you get shell syntax error when doing `make', it's probably
# because your make is so stupid that it tries to use your current
# interactive shell, which is really completely dumb, as no two
# people use the same interactive shell. Complain to your vendor,
# and in the mean time, set SHELL to a correct value.
# (this is the default configuration, as it generally doesn't harm to set it).
SHELL = /bin/sh

# If your make automatically sets the $(MAKE) variable, comment out
# the following line. Leaving it does no harm.
MAKE = make

# If the sanity check insists on telling you that ZLISTCAT, ZDBSCAT
# and so on are invalid, and you are definitively sure they are ok,
# set ZSANITYCHECK to the empty string.
ZSANITYCHECK = sanity-check-z

# If you are using compressed database files, you have the tempnam() call
# and you want control over the directory where the files are uncompressed,
# set TEMPNAM to 1. See 'man tempnam' for details on how to control the
# directory used.
#TEMPNAM = 1
TEMPNAM = 0

# --------------------------------------------------------------------------
#
# YOU SHOULD HAVE NOTHING TO CHANGE AFTER HERE.
#

# Which version is this?
VERSIONNUMBER  = 3.2d

# When was this package officially released?
RELEASEDATE  = 10th August 1995

#
# --------------------------------
#

all : compile installbin cleanobj installman

compile :
	$(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)" LD="$(LD)" LDFLAGS="$(LDFLAGS)" \
	RAWDIR="$(RAWDIR)" DBDIR="$(DBDIR)" ETCDIR="$(ETCDIR)" do-compile

do-compile : $(ZSANITYCHECK)
	cd $(SRCDIR); $(MAKE) CC="$(CC)" \
		CFLAGS="$(CFLAGS) -DHAVE_TEMPNAM=$(TEMPNAM) \
                -DLOGFILE=$(LOGFILE)" LOGFILENAME="$(LOGFILENAME)" \
                LD="$(LD)" LDFLAGS="$(LDFLAGS)" \
                ZLISTCAT="$(ZLISTCAT)" ZDBSCAT="$(ZDBSCAT)" \
                ZLISTCATOPTS="$(ZLISTCATOPTS)" ZDBSCATOPTS="$(ZDBSCATOPTS)" \
                ZLISTEXT="$(ZLISTEXT)" ZDBSEXT="$(ZDBSEXT)" \
		RAWDIR="$(RAWDIR)" DBDIR="$(DBDIR)" ETCDIR="$(ETCDIR)" compile

installbin : 
	$(MAKE) BINDIR="$(BINDIR)" ETCDIR="$(ETCDIR)" do-installbin

do-installbin :
	cd $(SRCDIR); $(MAKE) BINDIR="$(BINDIR)" ETCDIR="$(ETCDIR)" installbin

installman:
	$(MAKE) MANDIR="$(MANDIR)" DBDIR="$(DBDIR)" MAINDIR="$(MAINDIR)" \
                do-installman

do-installman:
	cd $(MANDIR); $(MAKE) MANDIR="$(MANDIR)" DBDIR="$(DBDIR)" \
		MAINDIR="$(MAINDIR)" \
		RELEASEDATE="$(RELEASEDATE)" VERSIONNUMBER="$(VERSIONNUMBER)"\
                installman

update : fetch $(AUTOUNCOMPRESS) update-local

update-local : $(ZSANITYCHECK) databases

fetch :
	$(MAKE) ETCDIR="$(ETCDIR)" do-lfetch

do-lfetch :
	cd $(RAWDIR); $(MAKE) SHELL="$(SHELL)" LFETCHOPT="$(LFETCHOPT)" \
                  ZLISTEXT="$(ZLISTEXT)" ETCDIR="$(ETCDIR)" do-lfetch

install : all update

install-local : all update-local

reset-ftp :
	cd $(RAWDIR); $(MAKE) reset-ftp

databases : 
	$(MAKE) KEEPLIST="$(KEEPLIST)" \
        ZLISTEXT="$(ZLISTEXT)" ZDBSEXT="$(ZDBSEXT)" \
        ZDBSCOMPRESS="$(ZDBSCOMPRESS)" ETCDIR="$(ETCDIR)" \
	RAWDIR="$(RAWDIR)" MKDBOPT="$(MKDBOPT)" do-databases

do-databases : $(ZSANITYCHECK)
	cd $(DBDIR); $(MAKE) KEEPLIST="$(KEEPLIST)" \
        ZLISTEXT="$(ZLISTEXT)" ZDBSEXT="$(ZDBSEXT)" \
        ZDBSCOMPRESS="$(ZDBSCOMPRESS)" ETCDIR="$(ETCDIR)" \
	RAWDIR="$(RAWDIR)" MKDBOPT="$(MKDBOPT)" databases

votes : 
	$(MAKE) ZLISTEXT="$(ZLISTEXT)" ZDBSEXT="$(ZDBSEXT)" \
	ETCDIR="$(ETCDIR)" ZDBSCOMPRESS="$(ZDBSCOMPRESS)" \
	RAWDIR="$(RAWDIR)" MKDBOPT="$(MKDBOPT)" do-votes

do-votes : 
	cd $(DBDIR); $(MAKE) \
        ZLISTEXT="$(ZLISTEXT)" ZDBSEXT="$(ZDBSEXT)" ETCDIR="$(ETCDIR)" \
        ZDBSCOMPRESS="$(ZDBSCOMPRESS)" \
	RAWDIR="$(RAWDIR)" MKDBOPT="$(MKDBOPT)" do-votes

touch-dbs : 
	$(MAKE) MKDBOPT="$(MKDBOPT)" ETCDIR="$(ETCDIR)" do-touch-dbs

do-touch-dbs : 
	cd $(DBDIR); $(MAKE) MKDBOPT="$(MKDBOPT)" ETCDIR="$(ETCDIR)" touch-dbs

#
# --------------------------------
#

compress : $(ZSANITYCHECK)
	cd $(DBDIR); $(MAKE) ZDBSCOMPRESS="$(ZDBSCOMPRESS)" compress

maxcompress : $(ZSANITYCHECK)
	cd $(DBDIR); $(MAKE) ZDBSCOMPRESS="$(ZDBSCOMPRESS)" maxcompress

autouncompress : $(ZSANITYCHECK)
	cd $(RAWDIR); $(MAKE) ZLISTEXT="$(ZLISTEXT)" ZLISTCAT="$(ZLISTCAT)" \
        ZLISTCATOPTS="$(ZLISTCATOPTS)" \
        autouncompress

#
# --------------------------------
#

cleanlists :
	cd $(RAWDIR); $(MAKE) ZLISTEXT="$(ZLISTEXT)" cleanlists

cleanobj :
	cd $(SRCDIR); $(MAKE) cleanobj

cleanbin :
	$(MAKE) BINDIR="$(BINDIR)" ETCDIR="$(ETCDIR)" do-cleanbin

do-cleanbin :
	cd $(SRCDIR); $(MAKE) BINDIR="$(BINDIR)" ETCDIR="$(ETCDIR)" cleanbin
	-[ -d imoviedb ] && (cd imoviedb; $(MAKE) cleanbin)

cleanman:
	$(MAKE) MANDIR="$(MANDIR)" DBDIR="$(DBDIR)" do-cleanman

do-cleanman:
	cd $(MANDIR); $(MAKE) MANDIR="$(MANDIR)" clean

clean : cleanobj cleanbin cleanman

cleandbs :
	cd $(DBDIR); $(MAKE) ZDBSEXT="$(ZDBSEXT)" cleandbs

veryclean : clean cleanlists cleandbs

#
# --------------------------------
#

sanity-check-z :
	@$(ETCDIR)/check-for-program ZLISTCAT "$(ZLISTCAT)"
	@$(ETCDIR)/check-for-program ZDBSCAT "$(ZDBSCAT)"
	@$(ETCDIR)/check-for-program ZDBSCOMPRESS "$(ZDBSCOMPRESS)"

#
# --------------------------------
#

relocate:
	@[ -d $(ETCDIR) ] || mkdir $(ETCDIR)
	cp etc/* $(ETCDIR)
	@[ -d $(MANDIR) ] || mkdir $(MANDIR)
	cp man/*.man man/make* man/Makefile $(MANDIR)
	@[ -d $(DBDIR) ] || mkdir $(DBDIR)
	cp dbs/Makefile $(DBDIR)
	@[ -d $(RAWDIR) ] || mkdir $(RAWDIR)
	cp lists/Makefile $(RAWDIR)
	@[ -d $(DOCDIR) ] || mkdir $(DOCDIR)
	cp docs/* $(DOCDIR)

#
# --------------------------------
#

mancatman:
	$(MAKE) MANDIR="$(MANDIR)" DBDIR="$(DBDIR)" MAINDIR="$(MAINDIR)" \
		RAWDIR="$(RAWDIR)" do-mancatman

do-mancatman:
	cd $(MANDIR); $(MAKE) DBDIR="$(DBDIR)" RELEASEDATE="$(RELEASEDATE)" \
	        MAINDIR="$(MAINDIR)" VERSIONNUMBER="$(VERSIONNUMBER)" \
		RAWDIR="$(RAWDIR)" mancatman

mantxt:
	$(MAKE) MANDIR="$(MANDIR)" DBDIR="$(DBDIR)" MAINDIR="$(MAINDIR)" \
		 RAWDIR="$(RAWDIR)" do-mantxt

do-mantxt:
	cd $(MANDIR); $(MAKE) DBDIR="$(DBDIR)" RELEASEDATE="$(RELEASEDATE)" \
		MAINDIR="$(MAINDIR)" RAWDIR="$(RAWDIR)" \
		VERSIONNUMBER="$(VERSIONNUMBER)" mantxt

manps:
	$(MAKE) MANDIR="$(MANDIR)" DBDIR="$(DBDIR)" MAINDIR="$(MAINDIR)" \
		RAWDIR="$(RAWDIR)" do-manps

do-manps:
	cd $(MANDIR); $(MAKE) DBDIR="$(DBDIR)" RELEASEDATE="$(RELEASEDATE)" \
		MAINDIR="$(MAINDIR)" RAWDIR="$(RAWDIR)" \
		VERSIONNUMBER="$(VERSIONNUMBER)" manps

allman: mancatman mantxt manps

#
# --------------------------------
#

imoviedb : nothing
	$(MAKE) RAWDIR="$(RAWDIR)" DBDIR="$(DBDIR)" ETCDIR="$(ETCDIR)" needed-src
	$(MAKE) DBDIR="$(DBDIR)" imoviedb-compile

needed-src :
	cd $(SRCDIR); $(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)" LD="$(LD)" \
		LDFLAGS="$(LDFLAGS)" \
		ZLISTCAT="$(ZLISTCAT)" ZDBSCAT="$(ZDBSCAT)" \
		ZLISTCATOPTS="$(ZLISTCATOPTS)" ZDBSCATOPTS="$(ZDBSCATOPTS)" \
		ZLISTEXT="$(ZLISTEXT)" ZDBSEXT="$(ZDBSEXT)" \
		RAWDIR="$(RAWDIR)" DBDIR="$(DBDIR)" ETCDIR="$(ETCDIR)" \
		dbutils.o

imoviedb-compile :
	cd imoviedb; $(MAKE) CC="$(CC)" \
		CFLAGS="$(CFLAGS) -DHAVE_VFORK=$(VFORK)" LD="$(LD)" \
		LDFLAGS="$(LDFLAGS)" \
		DBDIR="$(DBDIR)" MORE="$(MORE)" \
		READLINEINCDIR="$(READLINEINCDIR)" \
		READLINELIBDIR="$(READLINELIBDIR)" \
		READLINEAUXLIBS="$(READLINEAUXLIBS)" \
		compile
	cd imoviedb; $(MAKE) installbin
	cd imoviedb; $(MAKE) cleanobj
	cd $(SRCDIR); $(MAKE) cleanobj

#
# --------------------------------
#

depend :
	cd $(SRCDIR); $(MAKE) CFLAGS="$(CFLAGS)" depend

# Just in case someone sets AUTOUNCOMPRESS to nothing...
nothing :

# Local variables:
# indent-tabs-mode: t
# fill-column: 500
# End:
