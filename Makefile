# $Id$

NAME=GOST34.11-2012
VERSION=`cat VERSION`

DISTNAME=$(NAME)-$(VERSION)

SOURCES= gost3411-2012.c gost3411-2012-core.h gost3411-2012-core.c

CC?=gcc
CFLAGS+=-g -O2 -Wall
DEFAULT_INCLUDES=-I.
COMPILE=$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CFLAGS) 

all: gost3411

gost3411: $(SOURCES)
	$(COMPILE) -o gost3411 gost3411-2012-core.c gost3411-2012.c

clean:
	-rm gost3411 *.core 2>/dev/null

dist: clean
	mkdir -p $(DISTNAME)
	cp $(SOURCES) Makefile VERSION $(DISTNAME)
	tar czf $(DISTNAME).tar.gz $(DISTNAME)
	rm -r $(DISTNAME)

distclean: 
	-rm $(DISTNAME).tar.gz 2>/dev/null
