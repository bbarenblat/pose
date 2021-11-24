/* -*- mode: C++; tab-width: 4 -*- */
/* ===================================================================== *\
	Copyright (c) 2001 Palm, Inc. or its subsidiaries.
	All rights reserved.

	This file is part of the Palm OS Emulator.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
\* ===================================================================== */

#ifndef EmMenusFLTK_h
#define EmMenusFLTK_h

#include "EmMenus.h"			// EmCommandID

#include <FL/Fl_Menu_Item.h>

typedef vector<Fl_Menu_Item>	Fl_Menu_Item_List;

void HostCreatePopupMenu (const EmMenuItemList&, Fl_Menu_Item_List&);

#endif	// EmMenusFLTK_h
