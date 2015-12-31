# Makefile for "regionset" utility
# -- preliminary makefile -- 
# there will be a config/automake solution.
# this one works for now, more or less
#
# cleanups by Mirko Dölle <cooper@linvdr.org>
#

CC = gcc -Wall
MAKE = make

all: regionset
.PHONY : all

regionset: regionset.c dvd_udf.h dvd_udf.c
	$(CC) -o regionset regionset.c dvd_udf.c

.PHONY : clean
clean:
	rm -f regionset
