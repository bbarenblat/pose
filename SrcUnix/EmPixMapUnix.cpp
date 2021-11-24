/* -*- mode: C++; tab-width: 4 -*- */
/* ===================================================================== *\
	Copyright (c) 2000-2001 Palm, Inc. or its subsidiaries.
	All rights reserved.

	This file is part of the Palm OS Emulator.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
\* ===================================================================== */

#include "EmCommon.h"
#include "EmPixMapUnix.h"



// ---------------------------------------------------------------------------
//		¥ ConvertPixMapToHost
// ---------------------------------------------------------------------------

void ConvertPixMapToHost (	const EmPixMap& src, void* dest,
							int firstLine, int lastLine, Bool scale)
{
	// Determine a lot of the values we'll need.

	int					factor			= scale ? 2 : 1;
	EmPoint				factorPoint		= EmPoint (factor, factor);

	EmPoint				srcSize			= src.GetSize ();
	EmPixMapRowBytes	destRowBytes	= srcSize.fX * 3 * factor;

	// Finally, copy the bits, converting to 24 bit format along the way.

	EmPixMap	wrapper;

	wrapper.SetSize (srcSize * factorPoint);
	wrapper.SetFormat (kPixMapFormat24RGB);
	wrapper.SetRowBytes (destRowBytes);
	wrapper.SetColorTable (src.GetColorTable ());
	wrapper.SetBits (dest);

	EmRect	srcBounds (0, firstLine, srcSize.fX, lastLine);
	EmRect	destBounds (srcBounds * factorPoint);

	EmPixMap::CopyRect (wrapper, src, destBounds, srcBounds);
}
