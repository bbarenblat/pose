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

#define PILOT_PRECOMPILED_HEADERS_OFF

#include <PalmOS.h>
#include <SerialMgrOld.h>	// SerSettingsType

#include "SerialPortTools.h"

// for Handspring API access
#define	sysTrapHsSelector				sysTrapOEMDispatch
#define hsSelExtKeyboardEnable			5
#define SYS_SEL_TRAP(trapNum, selector) \
	_SYSTEM_API(_CALL_WITH_16BIT_SELECTOR)(_SYSTEM_TABLE, trapNum, selector)

Err		HsExtKeyboardEnable (Boolean enable)
				SYS_SEL_TRAP (sysTrapHsSelector, hsSelExtKeyboardEnable);

#ifndef serMgrVersion

	// Bits of the Palm OS 4.0 Serial Manager we need until
	// the SDK is available

	#define sysFtrNewSerialVersion		2

	#define sysSerialOpenV4				24
	#define sysSerialOpenBkgndV4		25
	#define sysSerialCustomControl		26

	//
	// Open Configuration Structure
	//
	typedef struct SrmOpenConfigType {
		UInt32 baud;			// Baud rate that the connection is to be opened at.
								// Applications that use drivers that do not require
								// baud rates can set this to zero or any other value.
								// Drivers that do not require a baud rate should 
								// ignore this field
		UInt32 function;		//	Designates the function of the connection. A value
								// of zero indictates default behavior for the protocol.
								// Drivers that do not support multiple functions should
								// ignore this field.	
		MemPtr drvrDataP;		// Pointer to driver specific data.
		UInt16 drvrDataSize;	// Size of the driver specific data block.	
		UInt32 sysReserved1;    // System Reserved
		UInt32 sysReserved2;    // System Reserved 
	} SrmOpenConfigType;

	typedef SrmOpenConfigType* SrmOpenConfigPtr;

	Err SrmExtOpen(UInt32 port, SrmOpenConfigType* configP, UInt16 configSize, UInt16 *newPortIdP)
		SERIAL_TRAP(sysSerialOpenV4);

#endif


// Globals.

UInt16			gPortID;
int				gSerialManagerVersion;


// ---------------------------------------------------------------------------
//		¥ Comm_Initialize
// ---------------------------------------------------------------------------
// Opens the communications port.

Int16 Comm_Initialize (UInt32 iBaud)
{
	Err					err = errNone;
	SrmOpenConfigType	config;
	SerSettingsType		serSettings;
	UInt32				srmSettings;
	UInt16				srmSettingsSize;
	UInt32				systemVersion;
	UInt32				ftrValue;

	// Exit if communications have already been established.

	if (gPortID)
		return (serErrAlreadyOpen);

	// Clear the globals.

	gPortID					= 0;
	gSerialManagerVersion	= 0;

	// Determine what version Serial Manager we have:
	//
	//	0 = undetermined
	//	1 = Original driver in Pilot 1000/5000
	//	2 = Updated driver in PalmPilot (new Send/Receive calls)
	//	3 = New Serial Manager (Srm calls)
	//	4 = Updated Serial Manager (includes SrmExtOpen call)

	err = FtrGet (sysFileCSystem, sysFtrNumROMVersion, &systemVersion);

	ErrFatalDisplayIf (err, "Unable to determine System version");

	if (sysGetROMVerMajor(systemVersion) < 2)
	{
		gSerialManagerVersion = 1;
	}
	else
	{
		err = FtrGet (sysFileCSerialMgr, sysFtrNewSerialPresent, &ftrValue);

		if (err || ftrValue == 0)
		{
			gSerialManagerVersion = 2;
		}
		else
		{
			err = FtrGet (sysFileCSerialMgr, sysFtrNewSerialVersion, &ftrValue);

			if (err)
			{
				gSerialManagerVersion = 3;
			}
			else if (ftrValue <= 2)
			{
				gSerialManagerVersion = 3;

				// Palm OS 3.5.2 for Handspring implements
				// sysFtrNewSerialVersion and returns a feature value of 2.
				// Palm OS 4.0 also implements sysFtrNewSerialVersion and
				// returns a value of 2.  However, the two serial managers
				// are different: the latter implements SrmExtOpen.  In
				// order to differentiate between the two, we have to check
				// the OS version.

				if (ftrValue == 2 && sysGetROMVerMajor (systemVersion) >= 4)
				{
					gSerialManagerVersion = 4;
				}
			}
			else
			{
				// ftrValue should be at least 3 for versions of the
				// Serial Manager that implement SrmExtOpen.

				gSerialManagerVersion = ftrValue + 1;
			}
		}
	}

	// <chg 02-01-2000 BP>  If on Handspring device, disable the keyboard 
	// thread before opening the serial library.  

	if (FtrGet ('hsEx', 0, &ftrValue) == errNone)
	{
		HsExtKeyboardEnable (false);
	}

	// Open the serial port in a manner appropriate for this platform.

	if (gSerialManagerVersion <= 2)
	{
		// Find the serial library.

		err = SysLibFind ("Serial Library", &gPortID);
		if (err != errNone)
			return (err);

		// Open the serial port.

		err = SerOpen (gPortID, kPort_Pilot, iBaud);
		if (err != errNone)
			goto Exit;

		// Set the serial attributes.

		serSettings.baudRate	= iBaud;
		serSettings.flags		= serSettingsFlagBitsPerChar8 | serSettingsFlagStopBits1;		
		serSettings.ctsTimeout	= serDefaultCTSTimeout;

		err = SerSetSettings (gPortID, &serSettings);
		if (err != errNone)
			goto Exit;
	}
	else	// if (gSerialManagerVersion >= 3)
	{
		// Open the serial port.

		if (gSerialManagerVersion == 3)
		{
			err = SrmOpen (serPortLocalHotSync, iBaud, &gPortID);
		}
		else
		{
			MemSet (&config, sizeof (config), 0);

			config.baud		= iBaud;
			config.function	= 'ROMX';

			err = SrmExtOpen (	serPortLocalHotSync,
								&config, sizeof (config),
								&gPortID);
		}

		if (err != errNone)
			goto Exit;

		// Set the serial attributes.

		srmSettings		= srmSettingsFlagBitsPerChar8 | srmSettingsFlagStopBits1;
		srmSettingsSize	= sizeof (srmSettings);
		err = SrmControl (gPortID, srmCtlSetFlags,
							&srmSettings, &srmSettingsSize);
		if (err != errNone)
			goto Exit;
	}

Exit:
	if (err != errNone)
	{
		if (gPortID)
		{
			SrmClose (gPortID);
			gPortID = 0;
		}
	}

	return err;
}


// ---------------------------------------------------------------------------
//		¥ Comm_Receive
// ---------------------------------------------------------------------------

Err Comm_Receive (void* bufP, UInt32 count, UInt32 timeout)
{
	Err	err;

	if (gSerialManagerVersion == 1)
	{
		err = SerReceive10 (gPortID, bufP, count, timeout);
	}
	else if (gSerialManagerVersion == 2)
	{
		(void) SerReceive (gPortID, bufP, count, timeout, &err);
	}
	else	// if (gSerialManagerVersion >= 3)
	{
		(void) SrmReceive (gPortID, bufP, count, timeout, &err);
	}

	return err;
}


// ---------------------------------------------------------------------------
//		¥ Comm_Send
// ---------------------------------------------------------------------------

#define CORRUPT_SENDS 0

#if CORRUPT_SENDS
static UInt32 PrvRange (UInt32 maxValue)
{
	static int initialized;
	if (!initialized)
	{
		initialized = true;
		SysRandom (1);
	}

	return (SysRandom (0) * maxValue) / (1UL + sysRandomMax);
}
#endif


Err Comm_Send (void* bufP, UInt32 count)
{	
	Err	err;

#if CORRUPT_SENDS
	void* buf2 = MemPtrNew (count);

	MemMove (buf2, bufP, count);

	bufP = buf2;

	if (PrvRange (100) <= (count / 100))
	{
		// Corrupt a character

		((char*) bufP)[PrvRange (count)]++;
	}
	else if (PrvRange (100) <= (count / 100))
	{
		// Drop a character

		UInt32	index = PrvRange (count);

		if (count - index - 1 > 0)
		{
			MemMove ((char*) bufP + index,
					(char*) bufP + index + 1,
					count - index - 1);
		}
		count--;
	}
#endif

	if (gSerialManagerVersion == 1)
	{
		err = SerSend10 (gPortID, bufP, count);
	}
	else if (gSerialManagerVersion == 2)
	{
		(void) SerSend (gPortID, bufP, count, &err);
	}
	else	// if (gSerialManagerVersion >= 3)
	{
		(void) SrmSend (gPortID, bufP, count, &err);
	}

#if CORRUPT_SENDS
	MemPtrFree (bufP);
#endif

	return err;
}


// ---------------------------------------------------------------------------
//		¥ Comm_Dispose
// ---------------------------------------------------------------------------
// Closes the communications port.

void Comm_Dispose (void)
{
	// Exit if communications have not been established.

	if (!gPortID)
		return;

	// Close the serial port.

	if (gSerialManagerVersion <= 2)
	{
		SerClose (gPortID);
	}
	else	// if (gSerialManagerVersion >= 3)
	{
		SrmClose (gPortID);
	}

	// Clear the globals.

	gPortID = 0;
}


// ---------------------------------------------------------------------------
//		¥ Comm_ClearError
// ---------------------------------------------------------------------------
// Clears the current serial errors.

void Comm_ClearError (void)
{
	// Exit if communications have not been established.

	if (!gPortID)
		return;

	if (gSerialManagerVersion <= 2)
	{
		SerClearErr (gPortID);
	}
	else	// if (gSerialManagerVersion >= 3)
	{
		SrmClearErr (gPortID);
	}
}
