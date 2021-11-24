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

#include "ROM_Transfer.h"
#include "ROM_Transfer.rsrc.h"

UInt32			HwrMemReadable(void *addr)
						HAL_CALL(sysTrapHwrMemReadable);

static void		SendXModem		(FormType* form, UInt8* romStart, Int32 romSize);
static void		SendRaw			(FormType* form, UInt8* romStart, Int32 romSize);
static void		UpdatePercentComplete (FormType* form, Int32 offset, Int32 romSize);
static void		GetROMStartSize	(UInt8** romStart, Int32* romSize);
static void		PrvUpdateStatus	(FormType* form, const char*);
static void		PrvStrPrintF	(char* dest, const char* fmt, Int32 num);
static void		PrvStrIToX		(char* dest, UInt32 val);

static UInt32	gROMVersion;

const char* kSerialErrors[] =
{
	"Bad Param",
	"Bad Port",
	"No Memory",
	"Bad ConnID",
	"Timeout",
	"Line Error",
	"Already Open",
	"Still Open",
	"Not Open",
	"Not Supported",
	"No Devices"
};


// ---------------------------------------------------------------------------
//		¥ PilotMain
// ---------------------------------------------------------------------------
// The main entry point for ROM Transfer.

UInt32 PilotMain (	UInt16		iCommand,
					void*		iCommandParams,
					UInt16		iLaunchFlags)
{
	if (iCommand != sysAppLaunchCmdNormalLaunch)
		return 0;

	ErrTry
	{
		InitializeApplication ();
		ExecuteApplication ();
	}

	// Catch any unhandled exceptions.

	ErrCatch (iError)
	{
	}
	ErrEndCatch

	// Clean up.

	DisposeApplication ();

	return 0;
}


#pragma mark -


// ---------------------------------------------------------------------------
//		¥ InitializeApplication
// ---------------------------------------------------------------------------
// Initializes the application globals and prepares to execute the event loop.

void InitializeApplication (void)
{
	Char*			aLabel;
	ListPtr			aList;
	UInt16			aListItem;
	ControlPtr		aControl;
	FormPtr			aMainForm;

	FtrGet (sysFtrCreator, sysFtrNumROMVersion, &gROMVersion);

	// Initialize and draw the main form.

	aMainForm = FrmInitForm (MainForm);
	Throw_IfNil (aMainForm, 0);

	FrmSetActiveForm (aMainForm);
	FrmDrawForm	(aMainForm);

	// Set up the speed popup.

	aListItem	= kItem_115200;
	aList		= FrmGetObjectPtr (aMainForm, FrmGetObjectIndex (aMainForm, MainSpeedList));
	aLabel		= LstGetSelectionText (aList, aListItem);
	aControl	= FrmGetObjectPtr (aMainForm, FrmGetObjectIndex (aMainForm, MainSpeedPopTrigger));
	CtlSetLabel (aControl, aLabel);
	LstSetSelection (aList, aListItem);

#if 0
{
	#define sysFtrNewSerialVersion		2
	int	fred;

#define gSerialManagerVersion fred

	Err		err;
	char	buffer[20];
	UInt32	systemVersion;
	UInt32	ftrValue;

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

				if (ftrValue == 2 &&
					sysGetROMVerMajor (systemVersion) == 4 &&
					sysGetROMVerMinor (systemVersion) == 0)
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

	StrPrintF (buffer, "%d  %d", (int) err, (int) gSerialManagerVersion);

	PrvUpdateStatus (aMainForm, buffer);
}
#endif

#if 0
{
	char	buffer1[20];
	char	buffer2[20];
	char	buffer3[20];
	UInt8*	romStart;
	Int32	romSize;

	GetROMStartSize (&romStart, &romSize);

	PrvStrIToX (buffer1, (UInt32) romStart);
	PrvStrIToX (buffer2, (UInt32) romSize);
	
	StrCopy (buffer3, buffer1);
	StrCat (buffer3, "   ");
	StrCat (buffer3, buffer2);

	PrvUpdateStatus (aMainForm, buffer3);
}
#endif
}


// ---------------------------------------------------------------------------
//		¥ DisposeApplication
// ---------------------------------------------------------------------------
// Disposes the application globals and prepares to exit.

void DisposeApplication (void)
{
	FrmCloseAllForms ();
}


// ---------------------------------------------------------------------------
//		¥ ExecuteApplication
// ---------------------------------------------------------------------------
// Executes an event loop until the application quits.

void ExecuteApplication (void)
{
	EventType aEvent;

	aEvent.eType = nilEvent;

	// Repeat until the application quits.

	while (aEvent.eType != appStopEvent)
	{
		// Get the next available event.

		EvtGetEvent (&aEvent, evtWaitForever);

		// Give the system a chance to handle the event.

		if (!SysHandleEvent (&aEvent))

			// Try to handle the event ourselves.

			if (!ProcessEvent (&aEvent))

				// Let the form provide default handling of the event.

				FrmHandleEvent (FrmGetActiveForm (), &aEvent);
	}
}


// ---------------------------------------------------------------------------
//		¥ ProcessEvent
// ---------------------------------------------------------------------------
// Attempts to process the specified event.

Boolean ProcessEvent (EventPtr iEvent)
{
	Boolean		aWasHandled = false;

	if (iEvent->eType == ctlSelectEvent)
	{
		switch (iEvent->data.ctlEnter.controlID)
		{
			case MainBeginTransferButton:
			{
				TransferROM ();

		   		aWasHandled = true;
		   		break;
		   	}
		}
	}

	return aWasHandled;
}


// ---------------------------------------------------------------------------
//		¥ TransferROM
// ---------------------------------------------------------------------------
// Downloads the ROM via the serial connection.

static UInt32 MySysTicksPerSecond ()
{
	if (sysGetROMVerMajor(gROMVersion) < 2)
		return sysTicksPerSecond;

	return SysTicksPerSecond ();
}

void TransferROM (void)
{
	Int16	err;
	UInt8*	romStart;
	Int32	romSize;

	Int16	speedChoice;
	UInt32	speed;
	FormPtr form;

	form = FrmGetActiveForm ();

	ErrTry
	{
		ErrFatalDisplayIf (!form, "Unable to get form");

		speedChoice = LstGetSelection (FrmGetObjectPtr (form, FrmGetObjectIndex (form, MainSpeedList)));
		switch (speedChoice)
		{
			case kItem_1200:	speed = kBaud_1200;		break;
			case kItem_2400:	speed = kBaud_2400;		break;
			case kItem_4800:	speed = kBaud_4800;		break;
			case kItem_9600:	speed = kBaud_9600;		break;
			case kItem_14400:	speed = kBaud_14400;	break;
			case kItem_19200:	speed = kBaud_19200;	break;
			case kItem_28800:	speed = kBaud_28800;	break;
			case kItem_38400:	speed = kBaud_38400;	break;
			case kItem_57600:	speed = kBaud_57600;	break;
			case kItem_115200:	speed = kBaud_115200;	break;
			default:			speed = kBaud_14400;	break;
		}

		err = Comm_Initialize (speed);
		Throw_IfError (err, err);

		// Get the size and location of the ROM.

		GetROMStartSize (&romStart, &romSize);

		// Send the data using a protocol appropriate for the connection.

#if 0
		if (using_USB)
		{
			SendRaw (form, romStart, romSize);
		}
		else
#endif
		{
			SendXModem (form, romStart, romSize);
		}

		// Change the status text.

		PrvUpdateStatus (form, "Finished");
	}

	// Catch any exceptions.

	ErrCatch (iError)
	{
		// Change the status text.

		if (form != NULL)
		{
			char	buffer[20];

			if (iError >= serErrBadParam &&
				iError <= serErrNoDevicesAvail)
			{
				StrCopy (buffer, kSerialErrors[iError - serErrBadParam]);
			}
			else
			{
				PrvStrPrintF (buffer, "Error #%d", iError);
			}

			PrvUpdateStatus (form, buffer);
		}
	}
	ErrEndCatch

	Comm_Dispose ();
}


// ---------------------------------------------------------------------------
//		¥ PrvSendChar
// ---------------------------------------------------------------------------

static void PrvSendChar (UInt8 ch)
{
	Err	err = Comm_Send (&ch, 1);
	Throw_IfError (err, err);

	EvtResetAutoOffTimer ();
}


// ---------------------------------------------------------------------------
//		¥ PrvReceiveChar
// ---------------------------------------------------------------------------

static void PrvReceiveChar (UInt8* ch)
{
	Err err = Comm_Receive (ch, 1, 30 * MySysTicksPerSecond ());
	Throw_IfError (err, err);

	EvtResetAutoOffTimer ();
}


// ---------------------------------------------------------------------------
//		¥ SendXModem
// ---------------------------------------------------------------------------

void SendXModem (FormType* form, UInt8* romStart, Int32 romSize)
{
	const int	kXModemBodySize		= 1024;	// 1k-XModem variant
	const int	kXModemSohOffset	= 0;
	const int	kXModemBlkOffset	= kXModemSohOffset + 1;
	const int	kXModemNBlkOffset	= kXModemBlkOffset + 1;
	const int	kXModemBodyOffset	= kXModemNBlkOffset + 1;
	const int	kXModemChk1Offset	= kXModemBodyOffset + kXModemBodySize;
	const int	kXModemChk2Offset	= kXModemChk1Offset + 1;
	const int	kXModemBlockSize	= kXModemChk2Offset + 1;

	const char	kXModemSoh			= 1;	// start of block header
	const char	kXModemEof			= 4;	// end of file signal
	const char	kXModemAck			= 6;	// acknowledge
	const char	kXModemNak			= 21;	// negative acknowledge (resend packet)
	const char	kXModemCan			= 24;	// cancel
	const char	kXModemNakCrc		= 'C';	// used instead of NAK for initial block

	Err			err;
	UInt8		c;
	UInt8		blockNum;
	UInt32		i;
	Boolean		bXModemCrc;
	char		block[kXModemBlockSize];
	Boolean 	bXModemHeaderBlock;
	Int32		offset;
	UInt16		crc;
	char*		body = &block[kXModemBodyOffset];

	// Change the status text.

	PrvUpdateStatus (form, "Waiting...");

	// First, wait for the receiver to send us a nak. If it's
	// a real nak, then we'll send in checksum mode. If it's
	// a 'C', we'll send in CRC mode.

	Comm_ClearError ();

	while (true)
	{
		PrvReceiveChar (&c);

		if (c == kXModemNakCrc)
		{
			bXModemCrc = true;
			break;
		}
		else if (c == kXModemNak)
		{
			bXModemCrc = false;
			break;
		}
		else if (c == kXModemCan)
		{
			ErrThrow (c);
		}
	}

	// Send the ROM one block at a time until it's done.

	offset		= 0;
	blockNum	= 0;
	bXModemHeaderBlock = true;

	while (offset < romSize)
	{
		// Change the status text.

		UpdatePercentComplete (form, offset, romSize);

		// Build up the block.

		block[kXModemSohOffset]		= kXModemSoh;
		block[kXModemBlkOffset]		= blockNum;
		block[kXModemNBlkOffset]	= ~blockNum;

		if (bXModemHeaderBlock)
		{
			// The very first block sends over a "file name"
			// and "file size".

			MemSet (body, kXModemBodySize, 0);
			StrCopy (body, "PalmOS.ROM"); // What's in a name?
			StrIToA (&body[StrLen (body) + 1], romSize);
		}
		else
		{
			// All subseqent blocks contain 1K of the ROM.

			MemMove (body, romStart + offset, kXModemBodySize);
		}

		// Figure out and add the checksum.

		if (bXModemCrc)
		{
			// Calculate the checksum of the body data, as
			// well as the two empty spots that will get filled
			// in with the checksum bytes in a moment.

			block[kXModemChk1Offset] = 0;
			block[kXModemChk2Offset] = 0;

			crc = Crc16CalcBlock (body, kXModemBodySize + 2, 0);

			block[kXModemChk1Offset] = crc >> 8;
			block[kXModemChk2Offset] = crc;

			err = Comm_Send (block, kXModemBlockSize);
		}
		else
		{
			c = 0;

			for (i = 0; i < kXModemBodySize; i++)
			{
				c += (romStart + offset)[i];
			}

			body[kXModemChk1Offset] = c;

			err = Comm_Send (block, kXModemBlockSize - 1);
		}

		Throw_IfError (err, err);

		EvtResetAutoOffTimer ();

		// Wait for the other end to ack or nak us. Leave
		// plenty of timeout so the receiver has time to
		// decide whether it got a partial block.

		do
		{
			PrvReceiveChar (&c);
		} while (c != kXModemAck && c != kXModemNak && c != kXModemNakCrc);

		if (c == kXModemAck)
		{
			if (!bXModemHeaderBlock)
			{
				// Only do this if we're into sending the file
				// (don't increment on the file header block).

				offset += kXModemBodySize;
			}

			blockNum++;
			bXModemHeaderBlock = false;
		}
	}

	// Send an EOF and wait for an acknowledgement.

	do
	{
		PrvSendChar (kXModemEof);
		PrvReceiveChar (&c);
	} while (c != kXModemAck);
}


// ---------------------------------------------------------------------------
//		¥ SendRaw
// ---------------------------------------------------------------------------

void SendRaw (FormType* form, UInt8* romStart, Int32 romSize)
{
#define kRawBlockSize	1024

	Err		err;
	Int32	offset = 0;

	// Start by sending the size.

	err = Comm_Send (&romSize, sizeof (romSize));
	Throw_IfError (err, err);

	// Now send the ROM.

	while (offset < romSize)
	{
		// Change the status text.

		UpdatePercentComplete (form, offset, romSize);

		err = Comm_Send (romStart + offset, kRawBlockSize);
		Throw_IfError (err, err);

		offset += kRawBlockSize;
	}
}


// ---------------------------------------------------------------------------
//		¥ UpdatePercentComplete
// ---------------------------------------------------------------------------

void UpdatePercentComplete (FormType* form, Int32 offset, Int32 romSize)
{
	Int32	percentComplete = (offset * 100) / romSize;
	char	buffer[20];

	PrvStrPrintF (buffer, "Sending...%d%%", percentComplete);

	PrvUpdateStatus (form, buffer);
}


// ---------------------------------------------------------------------------
//		¥ GetROMStartSize
// ---------------------------------------------------------------------------

void *			MemHeapPtr(UInt16 heapID)
							SYS_TRAP(sysTrapMemHeapPtr);

#define memHeapFlagReadOnly	0x0001		// heap is read-only (ROM based)

void GetROMStartSize (UInt8** romStartP, Int32* romSizeP)
{
#if 0
	void*	romStart	= (void*) HwrMemReadable ((void*) 0xFFFFFFFF);
	UInt32	romSize		= HwrMemReadable (romStart);

	*romStartP	= romStart;
	*romSizeP	= romSize;
#else
	UInt16	numHeaps = MemNumHeaps (0);
	UInt16	heapID;
	UInt16	heapFlags;
	UInt8*	heapBegin;
	UInt8*	heapEnd;
	UInt32	heapSize;

	UInt8*	romLow	= (UInt8*) 0xFFFFFFFF;
	UInt8*	romHigh	= (UInt8*) NULL;

	for (heapID = 0; heapID < numHeaps; ++heapID)
	{
		heapFlags = MemHeapFlags (heapID);

		if (heapFlags & memHeapFlagReadOnly)
		{
			heapBegin	= (UInt8*) MemHeapPtr (heapID);
			heapSize	= MemHeapSize (heapID);
			heapEnd		= heapBegin + heapSize;

			if (romLow > heapBegin)
			{
				romLow = heapBegin;
			}

			if (romHigh < heapEnd)
			{
				romHigh = heapEnd;
			}
		}
	}

	// Round up/down to 512K boundaries.  (512K is the size
	// of the Pilot 1000/5000 ROM).

	#define MASK (512L * 1024 - 1)

	romLow		= (UInt8*) ((UInt32) romLow & ~MASK);
	romHigh		= (UInt8*) (((UInt32) romHigh + MASK) & ~MASK);

	*romStartP	= romLow;
	*romSizeP	= romHigh - romLow;
#endif
}


// ---------------------------------------------------------------------------
//		¥ PrvUpdateStatus
// ---------------------------------------------------------------------------

void PrvUpdateStatus (FormType* form, const char* txt)
{
	char	buffer[100];
	StrCopy (buffer, txt);
	StrCat (buffer, "                              ");
	
	FrmCopyLabel (form, MainStatusTextLabel, buffer);
}


// ---------------------------------------------------------------------------
//		¥ PrvStrPrintF
// ---------------------------------------------------------------------------
// Roll our own StrPrintF, since it doesn't exist in 1.0.

void PrvStrPrintF (char* dest, const char* fmt, Int32 num)
{
	char		ch;
	const char*	s = fmt;
	char*		d = dest;
	char		numBuff[12];	// -2 xxx xxx xxx \0 = 12 bytes

	while ((ch = *s++) != 0)
	{
		if (ch != '%')
		{
			*d++ = ch;
		}
		else
		{
			ch = *s++;
			
			if (ch == 'd')
			{
				StrIToA (numBuff, num);
				StrCopy (d, numBuff);
				d += StrLen (numBuff);
			}
			else if (ch == 'c')
			{
				*d++ = (char) num;
			}
			else if (ch == '%')
			{
				*d++ = '%';
			}
		}
	}
	
	*d = 0;
}


// ---------------------------------------------------------------------------
//		¥ PrvStrIToX
// ---------------------------------------------------------------------------

void PrvStrIToX (char* dest, UInt32 val)
{
	const char* kDigits = "0123456789ABCDEF";
	int ii;
	
	for (ii = 0; ii < 8; ++ii)
	{
		unsigned nybble = (val >> (28 - ii * 4)) & 0x0F;
		dest[ii] = kDigits[nybble];
	}
	
	dest[8] = 0;
}
