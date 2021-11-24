/* -*- mode: C++; tab-width: 4 -*- */
/* ===================================================================== *\
	Copyright (c) 1998-2001 Palm, Inc. or its subsidiaries.
	All rights reserved.

	This file is part of the Palm OS Emulator.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
\* ===================================================================== */

#ifndef ROM_TRANSFER_H
#define ROM_TRANSFER_H


#include "SerialPortTools.h"


// Constants.

#define kItem_1200		9
#define kItem_2400		8
#define kItem_4800		7
#define kItem_9600		6
#define kItem_14400		5
#define kItem_19200		4
#define kItem_28800		3
#define kItem_38400		2
#define kItem_57600		1
#define kItem_115200	0

// Error macros.

#define Throw_IfNil(iHandle,iError)		{ if ((iHandle) == NULL) ErrThrow (iError); }
#define Throw_IfError(iCode,iError)		{ if ((iCode) != 0) ErrThrow (iError); }

// Function prototypes.

UInt32		PilotMain				(UInt16 iCommand,
									 void* iCommandParams,
									 UInt16 iLaunchFlags);

void		InitializeApplication	(void);

void		DisposeApplication		(void);

void		ExecuteApplication		(void);

Boolean		ProcessEvent			(EventPtr iEvent);

void		TransferROM				(void);

#endif
