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

#include "EmCommon.h"
#include "EmMenusFltk.h"

#include <ctype.h>				// isalpha


static int PrvMakeShortcut (int ch)
{
	if (ch == 0)
		return 0;

	if (isalpha (ch))
		ch = tolower (ch);

	return FL_ALT + ch;
}


void HostCreatePopupMenu (const EmMenuItemList& menu, Fl_Menu_Item_List& menuList)
{
	EmMenuItemList::const_iterator iter = menu.begin ();
	while (iter != menu.end ())
	{
		if (iter->GetIsDivider ())
		{
			// Do nothing here.  Dividers are not their own menu item
			// in FLTK.  Rather, they are an attribute of the previous
			// menu item.  The setting of this attribute is taken care
			// of below when the menu item is created.
		}
		else
		{
			const EmMenuItemList&	children = iter->GetChildren ();
			Fl_Menu_Item			fltkItem;

			fltkItem.text			= iter->GetTitle ().c_str ();
			fltkItem.shortcut_		= ::PrvMakeShortcut (iter->GetShortcut ());
			fltkItem.callback_		= NULL;
			fltkItem.user_data_		= (void*) iter->GetCommand ();
			fltkItem.flags			= 0;
			fltkItem.labeltype_		= 0;
			fltkItem.labelfont_		= 0;
			fltkItem.labelsize_		= FL_NORMAL_SIZE;
			fltkItem.labelcolor_	= 0;

			if (!iter->GetIsActive ())
			{
				fltkItem.flags		|= FL_MENU_INACTIVE;
			}

			if (iter->GetIsChecked ())
			{
				fltkItem.flags		|= FL_MENU_TOGGLE;
			}

			if (children.size () > 0)
			{
				fltkItem.flags		|= FL_SUBMENU;
			}

			if (((iter + 1) < menu.end ()) && (iter + 1)->GetIsDivider ())
			{
				fltkItem.flags		|= FL_MENU_DIVIDER;
			}

			menuList.push_back (fltkItem);

			if (children.size () > 0)
			{
				::HostCreatePopupMenu (children, menuList);
			}
		}

		++iter;
	}

	// Add a terminating item.

	Fl_Menu_Item	fltkItem = {0};
	menuList.push_back (fltkItem);
}
