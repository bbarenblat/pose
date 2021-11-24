#!/usr/bin/perl -w
########################################################################
#
#	File:			SkipStartup.pl
#
#	Purpose:		Skip the Palm V startup application.
#
#	Description:	Run this script to skip past the application that
#					automatically starts when cold-booting a Palm V
#					(or Palm IIIx).  It generates the appropriate
#					pen events to "tap past" the initial forms.
#
########################################################################

use EmRPC;			# EmRPC::OpenConnection, CloseConnection
use EmFunctions;
use EmUtils;		# TapPenSync, TapButtonSync


EmRPC::OpenConnection(6415, "localhost");

	TapPenSync (100, 100);			# Tap past first setup screen
	TapPenSync (100, 100);			# Tap past second setup screen

	TapPenSync (10, 10);			# First tap in pen calibration screen
	TapPenSync (160-10, 160-10);	# Second tap in pen calibration screen
	TapPenSync (80, 60);			# Confirmation tap in pen calibration screen

	Wait();
	Resume();
	my ($titleptr, $title) = FrmGetTitle (FrmGetActiveForm ());

	if ($title eq "Select Language")
	{
		# Tap past the extra screens in a 4.0 EFIGS ROM
		TapButtonSync ("OK");
		TapButtonSync ("Yes");
	}

	TapButtonSync ("Next");			# Tap Next button
	TapButtonSync ("Done");			# Tap Done button

EmRPC::CloseConnection();
