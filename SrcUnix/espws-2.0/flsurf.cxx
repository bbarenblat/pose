//
// "$Id: flsurf.cxx,v 1.2 2000/01/24 01:27:21 mike Exp $"
//
//   flsurf program.
//
//   Copyright 1997-2000 by Easy Software Products.
//
//   These coded instructions, statements, and computer programs are the
//   property of Easy Software Products and are protected by Federal
//   copyright law.  Distribution and use rights are outblockd in the file
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
//   main() - Display the help GUI...
//

//
// Include necessary headers...
//

#include "HelpApp.h"
#include <FL/x.H>


//
// 'main()' - Display the help GUI...
//

int				// O - Exit status
main(int  argc,			// I - Number of command-line arguments
     char *argv[])		// I - Command-line arguments
{
  HelpApp	*app;		// Help application


  fl_open_display();

  app = new HelpApp;

  if (argc >= 2)
    app->load(argv[1]);

  app->show();

  Fl::run();

  delete app;

  return (0);
}


//
// End of "$Id: flsurf.cxx,v 1.2 2000/01/24 01:27:21 mike Exp $".
//
