## ======================================================================================
##	Copyright (c) 1999-2001 Palm, Inc. or its subsidiaries.
##	All rights reserved.
## ======================================================================================

##
## makefile.am for poser-specific gzip code
##
SUFFIXES =	.cpp

VPATH =		$(srcdir)/../../SrcShared/Gzip:

## no subdir specific flags needed -- just go with globals
LOCAL_CFLAGS =

CFLAGS =   	$(POSER_CFLAGS) $(LOCAL_CFLAGS)
CXXFLAGS = 	$(POSER_CXXFLAGS) $(LOCAL_CFLAGS)

INCLUDES =	-I$(srcdir)/../../SrcShared/Gzip

noinst_LIBRARIES = libposergzip.a

SRC_GZIP =	bits.c							\
		crypt.h							\
		deflate.c						\
		gzip.h							\
		inflate.c						\
		lzw.h							\
		revision.h						\
		tailor.h						\
		trees.c							\
		util.c

libposergzip_a_SOURCES = $(SRC_GZIP)
