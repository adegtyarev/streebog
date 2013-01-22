# $Id$

NAME=GOST34.11-2012
VERSION=`cat VERSION`

DISTNAME=$(NAME)-$(VERSION)

HEADERS=gost3411-2012-core.h gost3411-2012-intrin.h gost3411-2012-const.h
SOURCES=gost3411-2012.c gost3411-2012-core.c

CC?=cc
WARN?=-pedantic -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion \
	-Wno-long-long -Wextra -Wpointer-arith -Wcast-qual -Winline
OPTIMIZE?=-O2 -msse2 -mmmx #-msse -msse3 -msse4 -msse4.1 -msse4.2 
DEBUG_FLAGS?=-g
CFLAGS+=${DEBUG_FLAGS} $(OPTIMIZE) $(WARN)
DEFAULT_INCLUDES=-I.
COMPILE=$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CFLAGS) 

all: gost3411

gost3411: $(SOURCES) $(HEADERS)
	$(COMPILE) -o gost3411 $(SOURCES)

remake: clean all

clean:
	-rm gost3411 *.core core 2>/dev/null

dist: clean
	mkdir -p $(DISTNAME)
	cp $(SOURCES) $(HEADERS) Makefile VERSION Changelog $(DISTNAME)
	-rm $(DISTNAME).tar.gz 2>/dev/null
	tar czf $(DISTNAME).tar.gz $(DISTNAME)
	rm -r $(DISTNAME)

distclean: 
	-rm $(DISTNAME).tar.gz 2>/dev/null

bench: 
	make remake CC=clang && ./gost3411 -b
	make remake CC=gcc46 && ./gost3411 -b
	make remake CC=gcc47 && ./gost3411 -b
	make remake CC=gcc && ./gost3411 -b
	which icc && make remake CC=icc && ./gost3411 -b || true
