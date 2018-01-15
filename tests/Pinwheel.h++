/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2012  Mark A Lindner

   This file is part of commonc++.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   ---------------------------------------------------------------------------
 */

#ifndef __ccxx_Pinwheel_hxx
#define __ccxx_Pinwheel_hxx

#include <iostream>

#include <commonc++/System.h++>

class Pinwheel
{
 public:

  Pinwheel()
    : _pos(0), _last(0)
  { }

  ~Pinwheel()
  {
    std::cerr << "\b \b" << std::flush;
  }

  inline void spin()
  {
    static const char *glyphs = "-\\|/";

    ccxx::time_ms_t now = ccxx::System::currentTimeMillis();
    if((now - _last) > 70)
    {
      _last = now;

      std::cerr << glyphs[_pos] << '\b' << std::flush;

      if(++_pos == 4)
      _pos = 0;
    }
  }

 private:

  int _pos;
  ccxx::time_ms_t _last;
};

#endif // __ccxx_Pinwheel_hxx
