//
// "$Id: HelpApp2.cxx,v 1.5 2000/03/19 19:10:20 mike Exp $"
//
//   Help Application extra routines.
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
//   HelpApp::add_bookmark()   - Add a bookmark.
//   HelpApp::link()           - Handle web accesses...
//   HelpApp::load_bookmarks() - Load bookmarks from disk.
//   HelpApp::save_bookmarks() - Save bookmarks to disk.
//   HelpApp::set_status()     - Set the current status...
//   HelpApp::back()           - Show the previous document in the history.
//   HelpApp::forward()        - Show the next document in the history.
//

//
// Include necessary header files...
//

#include "HelpApp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#ifdef HAVE_LIBCUPS
#  include <cups/http.h>
#endif // HAVE_LIBCUPS */


//
// Globals for all windows...
//

Fl_Menu_Item		*HelpApp::bookmenu_ = (Fl_Menu_Item *)0;
HelpApp			*HelpApp::first_ = (HelpApp *)0;
int			HelpApp::nbookmarks_ = 0;
HelpApp::bookmark	*HelpApp::bookmarks_;
int			HelpApp::prop_bookmark_ = 0;
char			HelpApp::proxy_[1024] = "";
char			HelpApp::homepage_[1024] = "http://www.fltk.org";


//
// 'HelpApp::add_bookmark()' - Add a bookmark.
//

void
HelpApp::add_bookmark(const char *title,	// I - Title
                      const char *url)		// I - URL
{
  char		buffer[1024],			// Title name buffer
		*bufptr;			// Pointer into buffer
  const char	*titleptr;			// Pointer into title
  bookmark	*temp;				// New bookmark array
  Fl_Menu_Item	*menu;				// New menu array
  HelpApp	*w;				// Help windows


  if (nbookmarks_ == 0)
    temp = (bookmark *)malloc(sizeof(struct bookmark));
  else
    temp = (bookmark *)realloc(bookmarks_, (nbookmarks_ + 1) * sizeof(struct bookmark));

  if (!temp)
    return;

  bookmarks_ = temp;
  temp += nbookmarks_;
  nbookmarks_ ++;

  strcpy(temp->title, title);
  strcpy(temp->url, url);

  if (!bookmenu_)
  {
    menu = (Fl_Menu_Item *)malloc(sizeof(Fl_Menu_Item) * 4);
    if (menu)
      memcpy(menu, first_->bookmark_->menu(), sizeof(Fl_Menu_Item) * 2);
  }
  else
    menu = (Fl_Menu_Item *)realloc(bookmenu_, sizeof(Fl_Menu_Item) * (3 + nbookmarks_));

  if (!menu)
    return;

  bookmenu_ = menu;
  menu += 1 + nbookmarks_;

  for (bufptr = buffer, titleptr = title; *titleptr;)
  {
    if (*titleptr == '/')
      *bufptr++ = '\\';
    *bufptr++ = *titleptr++;
  }

  *bufptr++ = '\0';

  memset(menu, 0, 2 * sizeof(Fl_Menu_Item));
  menu->text = strdup(buffer);

  for (w = first_; w; w = w->next_)
    w->bookmark_->menu(bookmenu_);
}


//
// 'HelpApp::link()' - Handle web accesses...
//

const char *			// O - New filename or NULL
HelpApp::link(const char *url)	// I - URL for access...
{
#ifdef HAVE_LIBCUPS
  char		method[HTTP_MAX_URI],	// URL method
		username[HTTP_MAX_URI],	// Username from URL
		hostname[HTTP_MAX_URI],	// Hostname from URL
		host[HTTP_MAX_URI],	// Host: field
		resource[HTTP_MAX_URI];	// Resource from URL
  int		port;			// Port number for method
  http_t	*http;			// HTTP connection
  http_status_t	status;			// HTTP status
  FILE		*fp;			// Temporary file
  char		buffer[8192];		// Copy buffer
  int		bytes;			// Number of bytes read
  int		tbytes;			// Total number of bytes read
  int		length;			// Content length
  static char	tempfile[1024];		// Temporary filename
  static char	auth[255] = "";		// Authentication string


  if (strncmp(url, "file:", 5) == 0 || strchr(url, ':') == NULL)
    return (url);

  httpSeparate(url, method, username, hostname, &port, resource);
  strcpy(host, hostname);

  if (proxy_[0])
  {
    httpSeparate(proxy_, method, username, hostname, &port, resource);
    strcpy(resource, url);
  }

  if (strcmp(method, "http") != 0 && strcmp(method, "ipp") != 0)
    return (NULL);

  set_status("Contacting host %s...", hostname);

  if ((http = httpConnect(hostname, port)) == NULL)
  {
    set_status(NULL);
    fl_alert("Unable to connect to %s:\n\n%s", hostname, strerror(errno));
    return (NULL);
  }

  if (username[0])
    httpEncode64(auth, username);

  set_status("Host contacted, sending request...");

  // Do HTTP GET requests until we succeed or error out...
  do
  {
    httpClearFields(http);
    httpSetField(http, HTTP_FIELD_HOST, host);

    if (auth[0])
    {
      sprintf(buffer, "Basic %s", auth);
      httpSetField(http, HTTP_FIELD_AUTHORIZATION, buffer);
    }

    if (httpGet(http, resource))
    {
      status = HTTP_UNAUTHORIZED;
      continue;
    }

    while ((status = httpUpdate(http)) == HTTP_CONTINUE);

    if (status == HTTP_UNAUTHORIZED)
    {
      const char *userpass;


      sprintf(buffer, "Authentication required.\nEnter username:password for %s:",
              host);
      if ((userpass = fl_input(buffer, username)) == NULL)
	break;

      httpEncode64(auth, userpass);
      httpFlush(http);
    }
  }
  while (status == HTTP_UNAUTHORIZED);

  // Copy the output from the server to a single temp file (no caching for now)
  sprintf(tempfile, "%s/%d.html",
          getenv("TMPDIR") == NULL ? "/var/tmp" : getenv("TMPDIR"), getuid());
  if ((fp = fopen(tempfile, "wb")) == NULL)
  {
    set_status(NULL);
    httpClose(http);
    return (NULL);
  }

  length = atoi(httpGetField(http, HTTP_FIELD_CONTENT_LENGTH));
  tbytes = 0;
  while ((bytes = httpRead(http, buffer, sizeof(buffer))) > 0)
  {
    tbytes += bytes;

    if (length == 0)
      set_status("Received %d bytes...", tbytes);
    else
      set_status("Received %d of %d bytes (%d%%)...", tbytes, length,
        	 100 * tbytes / length);

    fwrite(buffer, 1, bytes, fp);
  }

  set_status(NULL);
  fclose(fp);
  httpClose(http);

  return (tempfile);
#else
  return (url);
#endif // HAVE_LIBCUPS
}


//
// 'HelpApp::load_bookmarks()' - Load bookmarks from disk.
//

void
HelpApp::load_bookmarks()
{
  char	flsurfrc[1024];	// Filename
  FILE	*fp;		// .flsurfrc file
  char	title[1024],	// Title from file
	url[1024];	// URL from file


  if (getenv("HOME") == NULL)
    return;

  sprintf(flsurfrc, "%s/.flsurfrc", getenv("HOME"));

  if ((fp = fopen(flsurfrc, "r")) == NULL)
    return;

  // Read home page URL and strip LF...
  fgets(homepage_, sizeof(homepage_), fp);
  homepage_[strlen(homepage_) - 1] = '\0';

  // Read proxy URL and strip LF...
  fgets(proxy_, sizeof(proxy_), fp);
  proxy_[strlen(proxy_) - 1] = '\0';

  // Read pairs of lines with the bookmark title and URL...
  while (fgets(title, sizeof(title), fp) != NULL)
  {
    // Strip title LF
    title[strlen(title) - 1] = '\0';

    // Read URL and strip LF
    fgets(url, sizeof(url), fp);
    url[strlen(url) - 1] = '\0';

    // Add the bookmark...
    add_bookmark(title, url);
  }

  fclose(fp);
}


//
// 'HelpApp::save_bookmarks()' - Save bookmarks to disk.
//

void
HelpApp::save_bookmarks()
{
  int	i;		// Looping var
  char	flsurfrc[1024];	// Filename
  FILE	*fp;		// .flsurfrc file


  if (getenv("HOME") == NULL)
    return;

  sprintf(flsurfrc, "%s/.flsurfrc", getenv("HOME"));

  if ((fp = fopen(flsurfrc, "w")) == NULL)
    return;

  // Write home page URL and strip LF...
  fprintf(fp, "%s\n", homepage_);

  // Write proxy URL and strip LF...
  fprintf(fp, "%s\n", proxy_);

  // Write pairs of lines with the bookmark title and URL...
  for (i = 0; i < nbookmarks_; i ++)
  {
    fprintf(fp, "%s\n", bookmarks_[i].title);
    fprintf(fp, "%s\n", bookmarks_[i].url);
  }

  fclose(fp);
}


//
// 'HelpApp::set_status()' - Set the current status...
//

void
HelpApp::set_status(const char *format,	// I - printf-style string
                    ...)		// I - Additional args as needed
{
  va_list	ap;			// Pointer to additional args
  Fl_Cursor	cursor;			// Cursor to use...
  HelpApp	*w;			// Help windows
  static char	status[1024];		// Status string...


  if (format == NULL)
  {
    cursor = FL_CURSOR_DEFAULT;
    strcpy(status, "Ready.");
  }
  else
  {
    cursor = FL_CURSOR_WAIT;
    va_start(ap, format);
    vsprintf(status, format, ap);
    va_end(ap);
  }

  for (w = first_; w != NULL; w = w->next_)
  {
    w->window_->cursor(cursor);
    w->status_->label(status);
    w->status_->redraw();

    if (format)
      w->stop_->activate();
    else
      w->stop_->deactivate();
  }

  Fl::check();
}


//
// 'HelpApp::back()' - Show the previous document in the history.
//

void
HelpApp::back()
{
  if (index_ > 0)
    index_ --;

  if (index_ == 0)
    back_->deactivate();

  forward_->activate();

  if (strcmp(view_->filename(), file_[index_]) != 0)
    view_->load(file_[index_]);

  view_->topline(line_[index_]);
  location_->value(view_->filename());
  window_->label(view_->title());
}


//
// 'HelpApp::forward()' - Show the next document in the history.
//

void
HelpApp::forward()
{
  if (index_ < max_)
    index_ ++;

  if (index_ >= max_)
    forward_->deactivate();

  back_->activate();

  if (strcmp(view_->filename(), file_[index_]) != 0)
    view_->load(file_[index_]);

  view_->topline(line_[index_]);
  location_->value(view_->filename());
  window_->label(view_->title());
}


//
// 'HelpApp::show_bookmark()' - Show a bookmark...
//

void
HelpApp::show_bookmark()
{
  if (bookmark_->value() < 2)
    return;

  load(bookmarks_[bookmark_->value() - 2].url);
}


//
// 'HelpApp::edit_bookmarks()' - Show the bookmark editing window.
//

void
HelpApp::edit_bookmarks()
{
  HelpApp	*h;		// Current help windows...
  int		i;		// Looping var
  bookmark	*b;		// Current bookmark


  // See if the bookmarks window is already open from another window...
  for (h = first_; h != (HelpApp *)0; h = h->next_)
    if (h->bmWindow_->shown())
      break;

  if (h)
    h->bmWindow_->show();	// Raise the current window
  else
  {
    // Fill the list with the current bookmarks...
    bmList_->clear();
    for (i = nbookmarks_, b = bookmarks_; i > 0; i --, b ++)
      bmList_->add(b->title);

    // Show the window...
    bmWindow_->show();
  }
}


//
// 'HelpApp::list_cb()' - Handle clicks in the bookmark list.
//

void
HelpApp::list_cb(int clicks)
{
  int		i;	// Current item
  bookmark	*b;	// Current bookmark...


  i = bmList_->value() - 1;

  if (clicks && i >= 0)
  {
    // Double-click in list; show properties of current item...
    b              = bookmarks_ + i;
    prop_bookmark_ = i;

    propTitle_->value(b->title);
    propURL_->value(b->url);
    propWindow_->show();
  }
  else if (i)
  {
    bmDelete_->deactivate();
    bmMoveUp_->deactivate();
    bmMoveDown_->deactivate();
    bmProperties_->deactivate();
  }
  else
  {
    bmDelete_->activate();
    bmMoveUp_->activate();
    bmMoveDown_->activate();
    bmProperties_->activate();
  }
}


//
// 'HelpApp::prop_cb()' - Apply the changes to the current bookmark...
//

void
HelpApp::prop_cb()
{
  HelpApp	*h;		// Current help windows...
  bookmark	*b;	// Bookmark to change


  // See which properties window is open...
  for (h = first_; h != (HelpApp *)0; h = h->next_)
    if (h->propWindow_->shown())
      break;

  if (h)
  {
    // Found it; copy the new values over...
    b = bookmarks_ + prop_bookmark_;
    strncpy(b->title, h->propTitle_->value(), sizeof(b->title) - 1);
    strncpy(b->url, h->propURL_->value(), sizeof(b->url) - 1);

    h->bmList_->remove(prop_bookmark_ + 1);
    h->bmList_->insert(prop_bookmark_ + 1, b->title);

    save_bookmarks();
  }
}


//
// End of "$Id: HelpApp2.cxx,v 1.5 2000/03/19 19:10:20 mike Exp $".
//
