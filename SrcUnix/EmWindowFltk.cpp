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
#include "EmWindowFltk.h"

#include "EmApplication.h"		// gApplication
#include "EmCommands.h"			// EmCommandID
#include "EmDocument.h"			// EmDocument
#include "EmMenusFltk.h"		// HostCreatePopupMenu
#include "EmPixMapUnix.h"		// ConvertPixMapToHost
#include "EmScreen.h"			// EmScreenUpdateInfo
#include "EmSession.h"			// EmKeyEvent, EmButtonEvent
#include "EmWindow.h"			// EmWindow
#include "Platform.h"			// Platform::AllocateMemory

#include <FL/Fl.H>				// Fl::event_x, event_y
#include <FL/Fl_Box.H>			// Fl_Box
#include <FL/Fl_Image.H>		// Fl_Image::draw
#include <FL/Fl_Menu_Button.H>	// popup
#include <FL/fl_draw.H>			// fl_color

#include <ctype.h>				// isprint, isxdigit

#include "DefaultSmall.xpm"
#include "DefaultLarge.xpm"

const int kDefaultWidth = 220;
const int kDefaultHeight = 330;

#if FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION == 0
// FLTK 1.0.x had no separate Fl_RGB_Image subclass of Fl_Image.
#define Fl_RGB_Image Fl_Image
#endif

EmWindowFltk* gHostWindow;

// ---------------------------------------------------------------------------
//		¥ EmWindow::NewWindow
// ---------------------------------------------------------------------------

EmWindow* EmWindow::NewWindow (void)
{
	// This is the type of window we should create.  However, on Unix, we
	// create one and only one window when we create the application.  This
	// method -- called by the document to create its window -- therefore
	// doesn't need to actually create a window.

//	return new EmWindowFltk;

	EmAssert (gHostWindow != NULL);
	return NULL;
}


#pragma mark -

// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::EmWindowFltk
// ---------------------------------------------------------------------------

EmWindowFltk::EmWindowFltk (void) :
	Fl_Window (kDefaultWidth, kDefaultHeight, "pose"),
	EmWindow (),
	fMessage (NULL),
	fCachedSkin (NULL)
{
	EmAssert (gHostWindow == NULL);
	gHostWindow = this;

	this->box (FL_FLAT_BOX);
	this->color (fl_gray_ramp (FL_NUM_GRAY - 1));

	// Install a function to get called when the window is closed
	// via a WM_DELETE_WINDOW message.

	this->callback (&EmWindowFltk::CloseCallback, NULL);

	// Ensure that the user can't resize this window.

	this->resizable (NULL);

	// Create the message to display when there's no session running.

	fMessage = new Fl_Box (0, 0, 200, 40,
						   "Right click on this window to show a menu of commands.");
	fMessage->box (FL_NO_BOX);
	fMessage->align (FL_ALIGN_CENTER | FL_ALIGN_WRAP | FL_ALIGN_INSIDE);

	this->end ();

	this->redraw (); // Redraw help message

	// Set the X-Windows window class.  Normally, this is done when
	// Fl_Window::show (argc, argv) is called (in main()).  However, the
	// EmWindow class gets in the way and automatically shows the host
	// window by calling Fl_Window::show().  The latter does not set the
	// window class.  And even when the other "show" method is called later,
	// the damage is done:  the X Windows window is already created with a
	// NULL window class.
	//
	// Setting the window class to the title of the window is a workaround
	// at best.  In previous versions of Poser, the window class would be
	// set to the name of the executable.  These should both be "pose",
	// but it's possible for them to be different.

	this->xclass (this->label ());
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::~EmWindowFltk
// ---------------------------------------------------------------------------

EmWindowFltk::~EmWindowFltk (void)
{
	this->PreDestroy ();

	// Get rid of the cached skin.

	this->CacheFlush ();

	EmAssert (gHostWindow == this);
	gHostWindow = NULL;
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::CloseCallback
// ---------------------------------------------------------------------------
// When receiving the WM_CLOSE message on Windows or the WM_DELETE_WINDOW
// message on X, FLTK calls Fl::handle with the FL_CLOSE message.  Fl::handle
// handles this message not by passing it to Fl_Window::handle, but by calling
// the window's installed callback function.  By default, the callback
// function hides the window (presumably so that a main event loop would know
// it was time to quit by checking the window's visiblity).   We override that
// behavior in order to treat the FL_CLOSE message in the same way as handling
// a Quit menu selection.

void EmWindowFltk::CloseCallback (Fl_Widget*, void*)
{
	gApplication->HandleCommand (kCommandQuit);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::draw
// ---------------------------------------------------------------------------

void EmWindowFltk::draw (void)
{
	if (gDocument)
	{
		this->HandleUpdate ();
	}
	else
	{
		fl_color (255, 255, 255);
		fl_rect (0, 0, this->w (), this->h ());

		fMessage->position (
			(this->w () - fMessage->w ()) / 2,
			(this->h () - fMessage->h ()) / 3);
		this->draw_child (*fMessage);
	}
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::handle
// ---------------------------------------------------------------------------

int EmWindowFltk::handle (int event)
{
	EmPoint	where (Fl::event_x(), Fl::event_y());

	switch (event)
	{
		case FL_PUSH:
			if (Fl::event_button () == 3)
			{
				this->PopupMenu ();
				return 1;
			}
			// Fall through...

		case FL_DRAG:
			this->HandlePenEvent (where, true);
			return 1;

		case FL_RELEASE:
			this->HandlePenEvent (where, false);
			return 1;

/*
	These don't seem to get called when we want them to...

		case FL_ACTIVATE:
			this->HandleActivate (true);
			return 1;

		case FL_DEACTIVATE:
			this->HandleActivate (false);
			return 1;
*/

		case FL_FOCUS:
			this->HandleActivate (true);
			return 1;

		case FL_UNFOCUS:
			this->HandleActivate (false);
			return 1;

		case FL_SHORTCUT:
		{
			if (Fl::test_shortcut (FL_F + 10 | FL_SHIFT))
			{
				this->PopupMenu ();
				return 1;
			}

			Fl_Menu_Item_List hostMenu;
			this->GetHostMenu (hostMenu);

			const Fl_Menu_Item* selected = hostMenu[0].test_shortcut ();
			if (selected)
			{
				this->DoMenuCommand (*selected);
				return 1;
			}

			return 0;
		}

		case FL_KEYBOARD:
			if (Fl::event_state (FL_ALT | FL_META))
			{
				// Reserved for shortcuts
				return 0;
			}

			if (gDocument != NULL)
			{
				// Handle printable characters.

				if (strlen (Fl::event_text ()) > 0)
				{
					int key = (unsigned char) Fl::event_text()[0];
					EmKeyEvent event (key);
					// !!! Need to get modifiers
					gDocument->HandleKey (event);
					return 1;
				}

				// Handle all other characters.

				int c = Fl::event_key ();

				struct KeyConvert
				{
					int fEventKey;
					SkinElementType fButton;
					int fKey;
				};

				KeyConvert kConvert[] =
				{
					{ FL_Enter,		kElement_None, chrLineFeed },
					{ FL_KP_Enter,	kElement_None, chrLineFeed },
					{ FL_Left,		kElement_None, leftArrowChr },
					{ FL_Right,		kElement_None, rightArrowChr },
					{ FL_Up,		kElement_None, upArrowChr },
					{ FL_Down,		kElement_None, downArrowChr },
					{ FL_F + 1,		kElement_App1Button },
					{ FL_F + 2,		kElement_App2Button },
					{ FL_F + 3,		kElement_App3Button },
					{ FL_F + 4,		kElement_App4Button },
					{ FL_F + 9,		kElement_PowerButton },
					{ FL_Page_Up,	kElement_UpButton },
					{ FL_Page_Down,	kElement_DownButton }
				};

				for (size_t ii = 0; ii < countof (kConvert); ++ii)
				{
					if (c == kConvert[ii].fEventKey)
					{
						if (kConvert[ii].fButton != kElement_None)
						{
							gDocument->HandleButton (kConvert[ii].fButton, true);
							gDocument->HandleButton (kConvert[ii].fButton, false);
							return 1;
						}

						if (kConvert[ii].fKey)
						{
							EmKeyEvent event (kConvert[ii].fKey);
							// !!! Need to get modifiers
							gDocument->HandleKey (event);
							return 1;
						}
					}
				}

				if (c == FL_F + 10)
				{
					this->PopupMenu ();
					return 1;
				}

				if (c < 0x100)
				{
					EmKeyEvent event (c);
					// !!! Need to get modifiers
					gDocument->HandleKey (event);
					return 1;
				}
			}

			return 0;
	}

	return Fl_Window::handle (event);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::PopupMenu
// ---------------------------------------------------------------------------

void EmWindowFltk::PopupMenu (void)
{
	// Get the menu.

	Fl_Menu_Item_List hostMenu;
	this->GetHostMenu (hostMenu);

	const Fl_Menu_Item* item = hostMenu[0].popup (Fl::event_x (), Fl::event_y ());
	if (item)
	{
		this->DoMenuCommand (*item);
	}
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::GetHostMenu
// ---------------------------------------------------------------------------

void EmWindowFltk::GetHostMenu (Fl_Menu_Item_List& hostMenu)
{
	EmMenu*	menu = ::MenuFindMenu (kMenuPopupMenuPreferred);
	EmAssert (menu);

	::MenuUpdateMruMenus (*menu);
	::MenuUpdateMenuItemStatus (*menu);
	::HostCreatePopupMenu (*menu, hostMenu);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::DoMenuCommand
// ---------------------------------------------------------------------------

void EmWindowFltk::DoMenuCommand (const Fl_Menu_Item& item)
{
	EmCommandID id = (EmCommandID) item.argument ();

	if (gDocument)
		if (gDocument->HandleCommand (id))
			return;

	if (gApplication)
		if (gApplication->HandleCommand (id))
			return;

	EmAssert (false);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::CacheFlush
// ---------------------------------------------------------------------------

void EmWindowFltk::CacheFlush (void)
{
	delete fCachedSkin;
	fCachedSkin = NULL;
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::CacheFlush
// ---------------------------------------------------------------------------

Fl_Image* EmWindowFltk::GetSkin (void)
{
	if (!fCachedSkin)
	{
		const EmPixMap& p = this->GetCurrentSkin ();
		EmPoint size = p.GetSize ();

		fCachedSkin = new Fl_RGB_Image ((uchar*) p.GetBits (), size.fX, size.fY,
							   3, p.GetRowBytes ());
	}

	EmAssert (fCachedSkin);

	return fCachedSkin;
}

#pragma mark -

// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostWindowReset
// ---------------------------------------------------------------------------
// Update the window's appearance due to a skin change.

void EmWindowFltk::HostWindowReset (void)
{
	// Delete te old image.

	this->CacheFlush ();

	// Change the window to accomodate the settings and bitmap.

	// Get the desired client size.

	EmRect	newBounds = this->GetCurrentSkinRegion ().Bounds ();
	EmCoord	w = newBounds.Width ();
	EmCoord	h = newBounds.Height ();

	// Protect against this function being called when their's
	// no established skin.

	if (w == 0)
		w = kDefaultWidth;

	if (h == 0)
		h = kDefaultHeight;

	// Resize the window.

	this->size (w, h);
	this->size_range (w, h, w, h);

	// Invalidate the window contents now (necessary?).

	this->redraw ();
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostMouseCapture
// ---------------------------------------------------------------------------
// Capture the mouse so that all mouse events get sent to this window.

void EmWindowFltk::HostMouseCapture (void)
{
	Fl::grab (this);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostMouseRelease
// ---------------------------------------------------------------------------
// Release the mouse so that mouse events get sent to the window the
// cursor is over.

void EmWindowFltk::HostMouseRelease (void)
{
	Fl::grab (NULL);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostDrawingBegin
// ---------------------------------------------------------------------------
// Prepare the host window object for drawing outside of an update event.

void EmWindowFltk::HostDrawingBegin (void)
{
	this->make_current ();
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostWindowMoveBy
// ---------------------------------------------------------------------------
// Move the host window object by the given offset.

void EmWindowFltk::HostWindowMoveBy (const EmPoint& offset)
{
	this->HostWindowMoveTo (this->HostWindowBoundsGet ().TopLeft () + offset);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostWindowMoveTo
// ---------------------------------------------------------------------------
// Move the host window object to the given location.

void EmWindowFltk::HostWindowMoveTo (const EmPoint& loc)
{
	this->position (loc.fX, loc.fY);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostWindowBoundsGet
// ---------------------------------------------------------------------------
// Get the global bounds of the host window object.

EmRect EmWindowFltk::HostWindowBoundsGet (void)
{
	return EmRect (
		this->x (),
		this->y (),
		this->x () + this->w (),
		this->y () + this->h ());
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostWindowCenter
// ---------------------------------------------------------------------------
// Center the window to the main display.

void EmWindowFltk::HostWindowCenter (void)
{
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostWindowShow
// ---------------------------------------------------------------------------
// Make the host window object visible.

void EmWindowFltk::HostWindowShow (void)
{

	this->show ();
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostRectFrame
// ---------------------------------------------------------------------------
// Draw a rectangle frame with the given width in the given color.

void EmWindowFltk::HostRectFrame (const EmRect& r, const EmPoint& pen, const RGBType& color)
{
	EmRect r2 (r);
	fl_color (color.fRed, color.fGreen, color.fBlue);

	// !!! This could be changed to not assume a square pen, but since
	// we're kind of tied to that on Windows right now, that's the
	// assumption we'll make.

	for (EmCoord size = 0; size < pen.fX; ++size)
	{
		fl_rect (r2.fLeft, r2.fTop, r2.Width (), r2.Height ());
		r2.Inset (1, 1);
	}
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostOvalPaint
// ---------------------------------------------------------------------------
// Fill an oval with the given color.

void EmWindowFltk::HostOvalPaint (const EmRect& r, const RGBType& color)
{
	fl_color (color.fRed, color.fGreen, color.fBlue);
	fl_pie (r.fLeft, r.fTop, r.Width (), r.Height (), 0, 360);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostPaintCase
// ---------------------------------------------------------------------------
// Draw the skin.

void EmWindowFltk::HostPaintCase (const EmScreenUpdateInfo&)
{
	Fl_Image* skin = this->GetSkin ();
	skin->draw (0, 0);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostPaintLCD
// ---------------------------------------------------------------------------
// Draw the LCD area.  info contains the raw LCD data, including a partically
// updated fImage, and fFirstList and fLastLine which indicate the valid
// range of the image.  srcRect and destRect also indicate the range that
// needs to be updated, and have also been scaled appropriately.  scaled is
// true if we need to scale info.fImage during the process of converting it
// to a host pixmap.

void EmWindowFltk::HostPaintLCD (const EmScreenUpdateInfo& info, const EmRect& srcRect,
						  const EmRect& destRect, Bool scaled)
{
	// Determine the buffer size and allocate it.
	// We assume that ConvertPixMapToHost is converting to 24-bit RGB.

	int		rowBytes	= srcRect.fRight * 3;
	int		bufferSize	= srcRect.fBottom * rowBytes;
	uchar*	buffer		= (uchar*) Platform::AllocateMemory (bufferSize);

	// Convert the image, scaling along the way.

	::ConvertPixMapToHost (info.fImage, buffer,
						   info.fFirstLine, info.fLastLine, scaled);

	// Draw the converted image.

	fl_draw_image (buffer + srcRect.fTop * rowBytes,
				   destRect.fLeft, destRect.fTop,
				   destRect.Width (), destRect.Height ());

	// Clean up.

	Platform::DisposeMemory (buffer);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostGetDefaultSkin
// ---------------------------------------------------------------------------
// Get the default (built-in) skin image.

void EmWindowFltk::HostGetDefaultSkin (EmPixMap& pixMap, int scale)
{
	char** xpm = (scale == 2) ? DefaultLarge : DefaultSmall;

	/*
		An XPM file is an array of strings composed of four sections:

				<Values>
				<Colors>
				<Pixels>
				<Extensions>

		Each string is composed of words separated by spaces.
	*/

	/*
		<Values> is a string containing four or six integers in base
		10 that correspond to width, height, number of colors, number
		of characters per pixel, and (optionally) hotspot location.
	*/

	int w = 0;
	int h = 0;
	int num_colors = 0;
	int cpp = 0;
	int hot_x = 0;
	int hot_y = 0;
	int i = sscanf (xpm [0], "%d %d %d %d %d %d", &w, &h, &num_colors,
					&cpp, &hot_x, &hot_y);

	EmAssert (i == 4);
	EmAssert (w > 0);
	EmAssert (h > 0);
	EmAssert (num_colors > 0);
	EmAssert (cpp == 1);
	EmAssert (hot_x == 0);
	EmAssert (hot_y == 0);

	/*
		<Colors> contains as many lines as there are colors.  Each string
		contains the following words:

			<color_code> {<key> <color>}+
	*/

	RGBType colorMap[0x80];

	for (int color_num = 0; color_num < num_colors; ++color_num)
	{
		const char*	this_line	= xpm [1 + color_num];
		int			color_code	= this_line[0];
		char		key[3]		= {0};
		char		color[8]	= {0};

		i = sscanf (this_line + 1 + 1, "%s %s", key, color);

		EmAssert (i == 2);
		EmAssert (strlen (key) == 1);
		EmAssert (strlen (color) == 7);
		EmAssert (key[0] == 'c');
		EmAssert (color[0] == '#');
		EmAssert (isxdigit (color[1]));
		EmAssert (isxdigit (color[2]));
		EmAssert (isxdigit (color[3]));
		EmAssert (isxdigit (color[4]));
		EmAssert (isxdigit (color[5]));
		EmAssert (isxdigit (color[6]));

		int r, g, b;
		i = sscanf (color, "#%2x%2x%2x", &r, &g, &b);

		EmAssert (i == 3);
		EmAssert (isprint (color_code));

		colorMap [color_code] = RGBType (r, g, b);
	}

	/*
		<Pixels> contains "h" lines, each containing cpp * "w"
		characters in them.  Each set of cpp characters maps to
		one of the colors in the <Colors> array.
	*/

	uint8* buffer = (uint8*) Platform::AllocateMemory (w * h * 3);
	uint8* dest = buffer;

	for (int yy = 0; yy < h; ++yy)
	{
		char* src = xpm [1 + num_colors + yy];

		for (int xx = 0; xx < w; ++xx)
		{
			int color_code = *src++;
			EmAssert (isprint (color_code));

			const RGBType& rgb = colorMap [color_code];

			*dest++ = rgb.fRed;
			*dest++ = rgb.fGreen;
			*dest++ = rgb.fBlue;
		}
	}

	EmAssert ((dest - buffer) <= (w * h * 3));

	// We now have the data in RGB format.  Wrap it up in a temporary
	// EmPixMap so that we can copy it into the result EmPixMap.

	EmPixMap	wrapper;

	wrapper.SetSize (EmPoint (w, h));
	wrapper.SetFormat (kPixMapFormat24RGB);
	wrapper.SetRowBytes (w * 3);
	wrapper.SetBits (buffer);

	// Copy the data to the destination.

	pixMap = wrapper;

	// Clean up.

	Platform::DisposeMemory (buffer);
}


// ---------------------------------------------------------------------------
//		¥ EmWindowFltk::HostGetCurrentMouse
// ---------------------------------------------------------------------------
// Get the current mouse location.

EmPoint EmWindowFltk::HostGetCurrentMouse (void)
{
	return EmPoint (Fl::event_x (), Fl::event_y ());
}
