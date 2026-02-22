/*
  rakarrack - a guitar effects software

  XIcon.cpp - X11 window icon globals (GUI-only)

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.
*/

#include "XIcon.hpp"

#ifndef _WIN32
Pixmap p, mask;
XWMHints *hints;
#endif
