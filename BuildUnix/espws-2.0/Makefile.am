## -*- mode: Makefile; tab-width: 4; -*-
## ======================================================================================
##	Copyright (c) 2000-2001 Palm, Inc. or its subsidiaries.
##	All rights reserved.
## ======================================================================================

##
## makefile.am for poser-specific espws code
##
SUFFIXES		=	.cxx

VPATH			=	$(srcdir)/../../SrcUnix/espws-2.0:

CFLAGS			=   $(POSER_CFLAGS) $(LOCAL_CFLAGS) $(X_CFLAGS)
CXXFLAGS		= 	$(POSER_CXXFLAGS) $(LOCAL_CFLAGS) $(X_CFLAGS)

INCLUDES		=	-I$(srddir)/../../SrcUnix/espws-2.0

noinst_LIBRARIES	=	libposerespws.a

SRC_ESPWS		=	CheckButton.cxx		\
					FileBrowser.cxx		\
					FileChooser.cxx		\
					FileChooser2.cxx	\
					FileIcon.cxx		\
					FileInput.cxx		\
					Fl_Wizard.cxx		\
					HelpApp.cxx			\
					HelpApp2.cxx		\
					HelpDialog.cxx		\
					HelpView.cxx

libposerespws_a_SOURCES = $(SRC_ESPWS)
