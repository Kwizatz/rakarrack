/*
  rakarrack - a guitar effects software

 varios.C  -  some extra functions
  Copyright (C) 2008 Daniel Vidal & Josep Andreu
  Author: Daniel Vidal & Josep Andreu

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Group.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Widget.H>
#include "icono_rakarrack_32x32.xpm"
#include "global.h"


int
RKR::Message (const char *labelwin, const char *message_text)
{


  Fl_Widget *w = fl_message_icon ();

  Fl_Image *a = new Fl_Pixmap (icono_rakarrack_32x32_xpm);
  w->color (FL_WHITE);
  w->label ("");
  w->image (a);
  w->align (FL_ALIGN_TOP | FL_ALIGN_INSIDE);
  w->parent ()->copy_label (labelwin);
  fl_message (message_text);
  return (0);

};



char *
RKR::PrefNom (const char *dato)
{

  bzero (tmpprefname, sizeof (tmpprefname));
  sprintf (tmpprefname, "%s %s", jackcliname, dato);
  return (tmpprefname);

}

