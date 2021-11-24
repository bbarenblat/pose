## -*- mode: Makefile; tab-width: 4; -*-
## ======================================================================================
##	Copyright (c) 1999-2001 Palm, Inc. or its subsidiaries.
##	All rights reserved.
## ======================================================================================

##
## makefile.am for poser-specific jpeg code
##
SUFFIXES		=	.cpp

VPATH			=	$(srcdir)/../../SrcShared/jpeg:

## no subdir specific flags needed -- just go with globals
LOCAL_CFLAGS	=

CFLAGS			=   $(POSER_CFLAGS) $(LOCAL_CFLAGS)
CXXFLAGS		= 	$(POSER_CXXFLAGS) $(LOCAL_CFLAGS)

INCLUDES		=	-I$(srcdir)/../../SrcShared/jpeg	\
					-I$(srcdir)/../../SrcUnix

noinst_LIBRARIES	=	libposerjpeg.a

SRC_JPEG		=	cderror.h			\
					jcapimin.c			\
					jcapistd.c			\
					jccoefct.c			\
					jccolor.c			\
					jcdctmgr.c			\
					jchuff.c			\
					jchuff.h			\
					jcinit.c			\
					jcmainct.c			\
					jcmarker.c			\
					jcmaster.c			\
					jcomapi.c			\
					jconfig.h			\
					jcparam.c			\
					jcphuff.c			\
					jcprepct.c			\
					jcsample.c			\
					jctrans.c			\
					jdapimin.c			\
					jdapistd.c			\
					jdatadst.c			\
					jdatasrc.c			\
					jdcoefct.c			\
					jdcolor.c			\
					jdct.h				\
					jddctmgr.c			\
					jdhuff.c			\
					jdhuff.h			\
					jdinput.c			\
					jdmainct.c			\
					jdmarker.c			\
					jdmaster.c			\
					jdmerge.c			\
					jdphuff.c			\
					jdpostct.c			\
					jdsample.c			\
					jdtrans.c			\
					jerror.c			\
					jerror.h			\
					jfdctflt.c			\
					jfdctfst.c			\
					jfdctint.c			\
					jidctflt.c			\
					jidctfst.c			\
					jidctint.c			\
					jidctred.c			\
					jinclude.h			\
					jmemmgr.c			\
					jmemnobs.c			\
					jmemsys.h			\
					jmorecfg.h			\
					jpegint.h			\
					jpeglib.h			\
					jquant1.c			\
					jquant2.c			\
					jutils.c			\
					jversion.h

libposerjpeg_a_SOURCES = $(SRC_JPEG)
