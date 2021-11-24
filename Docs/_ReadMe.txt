========================================================================
Read Me for the Palm OS Emulator
Copyright (c) 1999-2001 Palm, Inc. or its subsidiaries.
All rights reserved.

Please send bug reports, comments, suggestions, etc. to
<mailto:bug.reports@corp.palm.com>
========================================================================

This document is the Read Me for the Palm OS Emulator (formerly known as
Copilot).

Palm OS Emulator is an application that emulates the hardware for most
Palm Computing Platform Hardware devices (e.g., Pilot, PalmPilot, Palm
III, Palm V, Palm VII, etc.).  The Emulator runs on most standard
desktop computers, includes those running Windows 95, Windows 98,
Windows NT 4.0, Windows 2000, Mac OS 8.6, Mac OS 9.x, Mac OS X, and
several flavors of Unix.  On the Mac, you need CarbonLib 1.2.5 or later.
You can find CarbonLib at:

	<http://developer.apple.com/carbon/>
or:
	<http://developer.apple.com/sdk/>

With the Palm OS Emulator, you can emulate the functions of a Palm
hardware device, including running the built-in application, as well as
installing and running 3rd party applications.

Palm OS Emulator is an Open Source project.  That means that the source
code is available for you to peruse and modify.  If you make any changes
that you think might benefit others, we urge you to consider sending
them back to Palm, Inc., for inclusion in the main source code base.

You can find the Palm OS Emulator at:

	<http://www.palmos.com/dev/tech/tools/emulator/>

You will also find this URL in the About Box for the Emulator.  The URL
is an active hyperlink; you can usually just click on it to launch your
Web browser and have it automatically access the Emulator home page.


Guide to the documentation files:
---------------------------------
UserGuide.pdf
	The official Poser documentation.  The current version documents
	features up to Palm OS Emulator 3.0a8.  See the _News.txt or
	_OldNews.txt files for changes since that version.

Bugs.txt
    A small set of bugs that we think you need to know about.  Not
    all known bugs are listed here -- just fairly relevent ones.

Building.txt
    Instructions on how to build (compile) the Palm OS Emulator on
    the various supported platforms.

Contributing.txt
    Guidelines to follow (or at least consider) if you are thinking
    of contributing to this Open Source project.

Credits.txt
    List of people who have contributed to the Emulator project.

GPL.txt
    Palm OS Emulator is an Open Source project distributed under and
    protected by the GNU GENERAL PUBLIC LICENSE.  The text of the
    license is included in this file.

News.txt
    Blow-by-blow descriptions of changes made to the emulator as it
    evolves over time.

OldNews.txt
    News.txt was getting too big, so older entries have been archived
    to this file.

ReadMe.txt
    The document you're currently reading.

ToDo.txt
    A short list of To Do items, for people wondering what features
    are planned or for people wondering how they can contribute to
    the project.

12rollin.pdf
	Handheld Systems Journal article written by the current Palm OS
	Emulator engineer.  Covers the differences between the Palm OS
	Emulator and Copilot, the program from which it was derived.

13hewgil.pdf
	Handheld Systems Journal article written by the original author
	of Copilot.


Notes on the Profile version:
-----------------------------
Some Palm OS Emulator releases include a profiling version (indicated by
its having "Profile" appended to its name).  This version of the
emulator allows users to turn profiling on and off, and to save the
results to disk in a format compatible with the CodeWarrior Profile
application, or in a tab-delimited text file suitable for spreadsheets.

Because the extra code involved with profiling an application slows down
all program execution (even when profiling isn't currently being used),
profiling support is provided only in this special version.  If you
don't intend to profile your application, you should use the normal
version of the emulator.
