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

#ifndef __ccxx_PluginTool_hxx
#define __ccxx_PluginTool_hxx

#include <commonc++/Application.h++>
#include <commonc++/Digest.h++>
#include <commonc++/String.h++>

using ccxx::Application;
using ccxx::Digest;
using ccxx::String;

namespace ccxx {
namespace tools {

class PluginTool : public Application
{
  public:

  PluginTool(int argc, char** argv, const String& version = "1.1",
             const String& compileDate = __DATE__,
             const String& compileTime = __TIME__);
  ~PluginTool();

  protected:

  bool processOption(char opt, const String& longOpt, const String& arg);

  void printUsage();

  void outputPluginInfo(const String& file);
  void outputDigest(const String& file, Digest& digest);

  private:

  bool _showName;
  bool _showVersion;
  bool _showAuthor;
  bool _showDate;
  bool _showClassName;
  bool _tags;
  bool _genMD5;
  bool _genSHA1;
};

} // namespace tools
} // namespace ccxx

#endif // __ccxx_PluginTool_hxx
