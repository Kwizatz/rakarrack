/*
  rakarrack - a guitar effects software

  XIcon.hpp - X11 window icon globals (GUI-only)

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.
*/

#pragma once

#ifndef _WIN32
#include <X11/xpm.h>
extern Pixmap p;
extern Pixmap mask;
extern XWMHints *hints;
#endif
