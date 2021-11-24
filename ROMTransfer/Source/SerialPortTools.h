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

#ifndef SERIAL_PORT_TOOLS_H
#define SERIAL_PORT_TOOLS_H

// Globals.

#define kPort_Pilot				0

#define kBaud_1200				1200
#define kBaud_2400				2400
#define kBaud_4800				4800
#define kBaud_9600				9600
#define kBaud_14400				14400
#define kBaud_19200				19200
#define kBaud_28800				28800
#define kBaud_38400				38400
#define kBaud_57600				57600
#define kBaud_57600				57600
#define kBaud_115200			115200


// Prototypes.

#ifdef __cplusplus
extern "C" {
#endif


Int16	Comm_Initialize	(UInt32 iBaud);
Err		Comm_Receive	(void* bufP, UInt32 count, UInt32 timeout);
Err		Comm_Send		(void* bufP, UInt32 count);
void	Comm_Dispose	(void);
void	Comm_ClearError	(void);


#ifdef __cplusplus
}
#endif

#endif
