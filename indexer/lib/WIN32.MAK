# Generated automatically from Makefile.in by configure.
###########################################################################
#
# Makefile.in -- Makefile for the general purpose library
# Copyright (C) 1994  Neil Sharman; (C) 1995 Tim Shimmin
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#       @(#)Makefile.in 1.3 16 Mar 1994
#
###########################################################################


PACKAGE = mg
VERSION = 1.2
 
AR = lib
CC = cl
#CFLAGS = -g -O 
CPPFLAGS = 
DEFS = -DHAVE_CONFIG_H -D__WIN32__ -D_LITTLE_ENDIAN
 
COMPILE = $(CC) -c $(DEFS) -I. -I..
 
.SUFFIXES:
.SUFFIXES: .c .obj
.c.obj:
	$(COMPILE) $<

ANSI2KNR = 
o = .obj
 
 
INCLUDES = -I.. 
 
HEADERS = \
    bitio_m_random.h   filestats.h        local_strings.h    stem.h \
    bitio_m_stdio.h    getopt.h           memlib.h \
    bitio_gen.h        bitio_mem.h        heap.h             messages.h \
    bitio_m.h          bitio_mems.h       huffman.h          timing.h \
    bitio_m_mem.h      bitio_random.h     huffman_mem.h      perf_hash.h \
    bitio_m_mems.h     bitio_stdio.h      huffman_stdio.h    sptree.h \
    regex.h            rx.h               pathmax.h          getpagesize.h \
    random.h

SOURCES = \
    bitio_random.c    heap.c            memlib.c \
    bitio_stdio.c     huffman.c         messages.c \
    bitio_gen.c       filestats.c       huffman_mem.c     perf_hash.c \
    bitio_mem.c       getopt.c          huffman_stdio.c   sptree.c \
    bitio_mems.c      getopt1.c         local_strings.c   stem.c \
    timing.c          regex.c           rx.c              \
    alloca.c          error.c           xmalloc.c         strstr.c \
    ftruncate.c       strcasecmp.c      random.c          

OBJECTS =   rx.obj \
    bitio_random$o    heap$o            memlib$o \
    bitio_stdio$o     huffman$o         messages$o \
    bitio_gen$o       filestats$o       huffman_mem$o     perf_hash$o \
    bitio_mem$o       getopt$o          huffman_stdio$o   sptree$o \
    bitio_mems$o      getopt1$o         local_strings$o   stem$o \
    timing$o          regex$o \
    error$o           xmalloc$o \
    random$o
 
DISTFILES = Makefile.in $(HEADERS) $(SOURCES)
 
all : libmg.lib

libmg.lib : $(OBJECTS)
	del libmg.lib
	$(AR) /out:libmg.lib $(OBJECTS)

$(OBJECTS): ..\win32cfg.h

install: 
installdirs:
uninstall:
execinstall:
execuninstall:
maninstall:
manuninstall:
 
#dist: $(DISTFILES)
#        for file in $(DISTFILES); do \
#          ln $$file $(distdir) 2> /dev/null \
#         || cp -p $$file $(distdir); \
#       done

mostlyclean:
	rm -f *.o *_.o
 
clean: mostlyclean
	rm -f libmg.lib
 
distclean: clean
	rm -f Makefile
 
maintainer-clean: distclean
	@echo "This command is intended only for maintainers to use;"
	@echo "rebuilding the deleted files may require special tools."

#Makefile: Makefile.in ../config.status
#        cd .. && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= ./config.status
 
# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
#.NOEXPORT:
