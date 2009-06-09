
EXEBIN = list title ltrace lguide listall mkballot lindex lformat lseen \
         alist template templist

EXEETC = mkdb

EXE = $(EXEBIN) $(EXEETC)

SRC = list.c mkdb.c dbutils.c filmography.c years.c biographies.c \
      aka.c ratings.c title.c titlesearch.c trivia.c plot.c lcs.c ltrace.c \
      lguide.c listall.c mkballot.c lindex.c lformat.c lseen.c alist.c \
      titleinfo.c template.c templist.c literature.c castcomp.c movielinks.c \
      log.c display.c business.c laserdisc.c

OBJ = $(SRC:.c=.o)

all :
	@echo "I don't think you've read the install doc..."

#
# ----------------------------------------------------------------
#

CAUXFLAGS = -DZLISTCAT=\"$(ZLISTCAT)\" -DZLISTEXT=\"$(ZLISTEXT)\" \
            -DZDBSCAT=\"$(ZDBSCAT)\" -DZDBSEXT=\"$(ZDBSEXT)\" \
	    -DZDBSCATOPTS=\"$(ZDBSCATOPTS)\" \
	    -DZLISTCATOPTS=\"$(ZLISTCATOPTS)\" \
	    -DRAWDIR="\"$(RAWDIR)\"" -DDBDIR="\"$(DBDIR)\"" \
	    -DETCDIR="\"$(ETCDIR)\"" -DLOGFILENAME="\"$(LOGFILENAME)\""

#
# ----------------------------------------------------------------
#
# Compilation rules.
#

compile : $(EXE)
	@echo 'All is compiled.'

mkdb : mkdb.o dbutils.o
	$(LD) $(LDFLAGS) -o mkdb mkdb.o dbutils.o

list : list.o dbutils.o filmography.o years.o biographies.o aka.o \
        ratings.o lcs.o titleinfo.o log.o display.o
	$(LD) $(LDFLAGS) -o list list.o dbutils.o filmography.o years.o\
           biographies.o aka.o ratings.o lcs.o titleinfo.o log.o display.o

title : title.o dbutils.o titlesearch.o years.o biographies.o \
          aka.o ratings.o trivia.o plot.o titleinfo.o literature.o castcomp.o \
          movielinks.o business.o laserdisc.o log.o display.o
	$(LD) $(LDFLAGS) -o title title.o dbutils.o titlesearch.o years.o \
          biographies.o aka.o ratings.o trivia.o plot.o titleinfo.o \
          literature.o castcomp.o movielinks.o business.o laserdisc.o \
          log.o display.o

ltrace : ltrace.o dbutils.o titlesearch.o years.o biographies.o \
          aka.o ratings.o trivia.o plot.o filmography.o lcs.o \
          titleinfo.o literature.o castcomp.o movielinks.o log.o \
          business.o laserdisc.o display.o
	$(LD) $(LDFLAGS) -o ltrace ltrace.o dbutils.o titlesearch.o years.o \
          biographies.o aka.o ratings.o trivia.o plot.o \
          filmography.o lcs.o titleinfo.o literature.o castcomp.o \
          movielinks.o business.o laserdisc.o log.o display.o

lguide : lguide.o dbutils.o titlesearch.o years.o biographies.o \
          aka.o ratings.o trivia.o plot.o titleinfo.o literature.o castcomp.o \
          movielinks.o business.o laserdisc.o log.o display.o
	$(LD) $(LDFLAGS) -o lguide lguide.o dbutils.o titlesearch.o years.o \
          biographies.o aka.o ratings.o trivia.o plot.o titleinfo.o \
          literature.o castcomp.o movielinks.o business.o laserdisc.o \
          log.o display.o

listall : listall.o dbutils.o titlesearch.o years.o biographies.o \
          aka.o ratings.o trivia.o plot.o filmography.o lcs.o \
          titleinfo.o literature.o castcomp.o movielinks.o log.o display.o \
          business.o laserdisc.o
	$(LD) $(LDFLAGS) -o listall listall.o dbutils.o titlesearch.o years.o \
          biographies.o aka.o ratings.o trivia.o plot.o \
          filmography.o lcs.o titleinfo.o literature.o castcomp.o \
          movielinks.o business.o laserdisc.o log.o display.o

mkballot : mkballot.o dbutils.o filmography.o years.o \
            biographies.o aka.o ratings.o lcs.o titleinfo.o log.o display.o
	$(LD) $(LDFLAGS) -o mkballot mkballot.o dbutils.o filmography.o years.o\
           biographies.o aka.o ratings.o lcs.o titleinfo.o log.o display.o

lindex : lindex.o dbutils.o years.o ratings.o titleinfo.o log.o
	$(LD) $(LDFLAGS) -o lindex lindex.o dbutils.o years.o ratings.o \
           titleinfo.o log.o

lformat : lformat.o dbutils.o years.o ratings.o  log.o
	$(LD) $(LDFLAGS) -o lformat lformat.o dbutils.o years.o ratings.o log.o

lseen : lseen.o dbutils.o years.o ratings.o  log.o
	$(LD) $(LDFLAGS) -o lseen lseen.o dbutils.o years.o ratings.o log.o

alist : alist.o dbutils.o years.o  log.o
	$(LD) $(LDFLAGS) -o alist alist.o dbutils.o years.o log.o

template : template.o
	$(LD) $(LDFLAGS) -o template template.o

templist : templist.o
	$(LD) $(LDFLAGS) -o templist templist.o

.c.o:
	$(CC) $(CFLAGS) $(CAUXFLAGS) -c $<

#
# ----------------------------------------------------------------
#

installbin :
	cp $(EXEBIN) $(BINDIR)
	cp $(EXEETC) $(ETCDIR)

cleanobj :
	rm -f $(OBJ) $(EXE)
	rm -f Makefile.bak

cleanbin :
	cd $(BINDIR); rm -f $(EXEBIN)
	cd $(ETCDIR); rm -f $(EXEETC)


#
# ----------------------------------------------------------------
#

# Automatic generation of dependencies. This uses gcc. You can also use
# the makedepend program distributed with X11.
#depend:
#	makedepend -- $(CFLAGS) -- $(SRC)

depend:
	mv -f Makefile Makefile.bak
	sed -n '1,/^# DO NOT DELETE THIS LINE/p' < Makefile.bak >Makefile
	echo '' >>Makefile
	gcc -MM $(CFLAGS) $(SRC) >>Makefile

# Local variables:
# indent-tabs-mode: t
# fill-column: 500
# End:

# DO NOT WRITE ANYTHING AFTER THIS LINE. It will disappear with the next
# "make depend".
#
# DO NOT DELETE THIS LINE -- make depend depends on it.

list.o: list.c log.h moviedb.h dbutils.h filmography.h display.h
mkdb.o: mkdb.c moviedb.h dbutils.h
dbutils.o: dbutils.c moviedb.h dbutils.h
filmography.o: filmography.c moviedb.h dbutils.h filmography.h years.h \
 biographies.h aka.h ratings.h lcs.h titleinfo.h
years.o: years.c moviedb.h dbutils.h
biographies.o: biographies.c moviedb.h dbutils.h
aka.o: aka.c moviedb.h dbutils.h
ratings.o: ratings.c moviedb.h dbutils.h
title.o: title.c log.h moviedb.h dbutils.h titlesearch.h display.h
titlesearch.o: titlesearch.c moviedb.h dbutils.h filmography.h years.h \
 aka.h ratings.h plot.h trivia.h titleinfo.h literature.h business.h \
 laserdisc.h movielinks.h castcomp.h display.h
trivia.o: trivia.c moviedb.h dbutils.h
plot.o: plot.c moviedb.h dbutils.h
lcs.o: lcs.c
ltrace.o: ltrace.c log.h moviedb.h dbutils.h filmography.h \
 titlesearch.h display.h
lguide.o: lguide.c log.h moviedb.h dbutils.h titlesearch.h display.h
listall.o: listall.c log.h moviedb.h dbutils.h titlesearch.h \
 filmography.h display.h
mkballot.o: mkballot.c log.h moviedb.h dbutils.h filmography.h \
 display.h
lindex.o: lindex.c log.h moviedb.h dbutils.h ratings.h titleinfo.h
lformat.o: lformat.c moviedb.h dbutils.h years.h ratings.h
lseen.o: lseen.c moviedb.h dbutils.h years.h ratings.h
alist.o: alist.c log.h moviedb.h dbutils.h years.h
titleinfo.o: titleinfo.c moviedb.h dbutils.h
template.o: template.c
templist.o: templist.c
literature.o: literature.c moviedb.h dbutils.h literature.h
castcomp.o: castcomp.c moviedb.h dbutils.h
movielinks.o: movielinks.c moviedb.h dbutils.h
log.o: log.c
display.o: display.c moviedb.h dbutils.h
business.o: business.c moviedb.h dbutils.h literature.h
laserdisc.o: laserdisc.c moviedb.h dbutils.h
