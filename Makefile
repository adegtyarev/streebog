# $Id$

all: gost3411

gost3411: gost3411-2012.c gost3411-2012.h
	gcc -O2 -Wall -I. -o gost3411 gost3411-2012.c

clean:
	-rm gost3411 *.core
