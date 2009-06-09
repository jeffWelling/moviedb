# /**********************************************************************\
#  *                                                                    *
#  * Makefile for man-pages-section of The Internet Movie Database v3.2 *
#  *                                                                    *
#  **********************************************************************
#  *       (C) Lars J Aas <larsa@colargol.stud.idb.hist.no> 1995        *
# \**********************************************************************/

.SUFFIXES: .man .1 .ps .txt .cat .html

# If you have "install" you can use it instead of cp
# INSTALL = install -c -m 644
INSTALL = cp

all:
	@echo "I don't think you've read the install doc..."

##########################################################################

FILES    = alist.1 lguide.1 list.1 lseen.1 mkballot.1 \
           lformat.1 lindex.1 listall.1 ltrace.1 title.1

PSFILES  = alist.ps lguide.ps list.ps lseen.ps mkballot.ps \
           lformat.ps lindex.ps listall.ps ltrace.ps title.ps

TXTFILES = alist.txt lguide.txt list.txt lseen.txt mkballot.txt \
           lformat.txt lindex.txt listall.txt ltrace.txt title.txt

CATFILES = alist.cat lguide.cat list.cat lseen.cat mkballot.cat \
           lformat.cat lindex.cat listall.cat ltrace.cat title.cat

HTMLFILES= alist.html lguide.html list.html lseen.html mkballot.html \
           lformat.html lindex.html listall.html ltrace.html title.html

##########################################################################

.man.1:
	./makeman $< "$(DBDIR)" "$(RELEASEDATE)" "$(VERSIONNUMBER)" "$(RAWDIR)"

.1.ps:
	./makeps $<

.cat.txt:
	./maketxt $<

.1.cat:
	./makecatman $<

.cat.html:
	./makehtml $< > `basename $< .cat`.html

manpages: $(FILES)

manps: $(PSFILES)

mantxt: $(TXTFILES)

mancatman: $(CATFILES)

manhtml: $(HTMLFILES)

installman: manpages
	$(INSTALL) $(FILES) $(MANDIR)man1/;

clean cleanman:
	rm -f $(FILES) $(PSFILES) $(TXTFILES) $(CATFILES) $(HTMLFILES)

	
##########################################################################
# DEPENDENCIES

alist.1:    alist.man    bottom.man $(MAINDIR)Makefile makeman
lguide.1:   lguide.man   bottom.man $(MAINDIR)Makefile makeman
list.1:     list.man     bottom.man $(MAINDIR)Makefile makeman
lseen.1:    lseen.man    bottom.man $(MAINDIR)Makefile makeman
mkballot.1: mkballot.man bottom.man $(MAINDIR)Makefile makeman
lformat.1:  lformat.man  bottom.man $(MAINDIR)Makefile makeman
lindex.1:   lindex.man   bottom.man $(MAINDIR)Makefile makeman
listall.1:  listall.man  bottom.man $(MAINDIR)Makefile makeman
ltrace.1:   ltrace.man   bottom.man $(MAINDIR)Makefile makeman
title.1:    title.man    bottom.man $(MAINDIR)Makefile makeman

##########################################################################
