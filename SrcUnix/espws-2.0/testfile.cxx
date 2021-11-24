//
// "$Id: testfile.cxx,v 1.6 2000/01/04 13:45:55 mike Exp $"
//
//   File chooser test program.
//
//   Copyright 1997-2000 by Easy Software Products.
//
//   These coded instructions, statements, and computer programs are the
//   property of Easy Software Products and are protected by Federal
//   copyright law.  Distribution and use rights are outlined in the file
//   "COPYING" which should have been included with this file.  If this
//   file is missing or damaged please contact Easy Software Products
//   at:
//
//       Attn: ESP Licensing Information
//       Easy Software Products
//       44141 Airport View Drive, Suite 204
//       Hollywood, Maryland 20636-3111 USA
//
//       Voice: (301) 373-9600
//       EMail: info@easysw.com
//         WWW: http://www.easysw.com
//
// Contents:
//
//   main()           - Create a file chooser and wait for a selection to be
//                      made.
//   close_callback() - Close the main window...
//   show_callback()  - Show the file chooser...
//

//
// Include necessary headers...
//

#include <stdio.h>
#include "FileChooser.h"


//
// Globals...
//

Fl_Input	*filter;
FileChooser	*fc;


//
// Functions...
//

void	close_callback(void);
void	show_callback(void);


//
// 'main()' - Create a file chooser and wait for a selection to be made.
//

int			// O - Exit status
main(int  argc,		// I - Number of command-line arguments
     char *argv[])	// I - Command-line arguments
{
  Fl_Window	*window;// Main window
  Fl_Button	*button;// Buttons
  FileIcon	*icon;	// New file icon


  // Make the file chooser...
  FileIcon::load_system_icons();

  fc = new FileChooser(".", "*", FileChooser::MULTI, "FileChooser Test");
  fc->color((Fl_Color)196);

  // Make the main window...
  window = new Fl_Window(400, 80, "File Chooser Test");

  filter = new Fl_Input(50, 10, 315, 25, "Filter:");
  if (argc > 1)
    filter->value(argv[1]);

  button = new Fl_Button(365, 10, 25, 25);
  button->labelcolor(FL_YELLOW);
  button->callback((Fl_Callback *)show_callback);

  icon   = FileIcon::find(".", FileIcon::DIRECTORY);
  icon->label(button);

  button = new Fl_Button(340, 45, 50, 25, "Close");
  button->callback((Fl_Callback *)close_callback);

  window->end();
  window->show();

  Fl::run();

  return (0);
}


//
// 'close_callback()' - Close the main window...
//

void
close_callback(void)
{
  exit(0);
}


//
// 'show_callback()' - Show the file chooser...
//

void
show_callback(void)
{
  int		i;	// Looping var
  int		count;	// Number of files selected


  fc->show();
  if (filter->value()[0])
    fc->filter(filter->value());

  fc->show();

  while (fc->visible())
    Fl::wait();

  count = fc->count();
  printf("count = %d\n", count);
  for (i = 1; i <= count; i ++)
    printf("file %d = \"%s\"\n", i, fc->value(i));
}


//
// End of "$Id: testfile.cxx,v 1.6 2000/01/04 13:45:55 mike Exp $".
//
