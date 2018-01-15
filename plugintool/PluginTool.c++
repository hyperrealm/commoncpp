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

#include "PluginTool.h++"

#include <iostream>

#include <commonc++/DynamicArray.h++>
#include <commonc++/File.h++>
#include <commonc++/Hex.h++>
#include <commonc++/IOException.h++>
#include <commonc++/MD5Digest.h++>
#include <commonc++/Plugin.h++>
#include <commonc++/PluginLoader.h++>
#include <commonc++/SHA1Digest.h++>

namespace ccxx {
namespace tools {

/*
 */

PluginTool::PluginTool(int argc, char **argv,
                       const String &version  /* = 0.0 */,
                       const String &compileDate /* = "" */,
                       const String &compileTime /* = "" */)
  : Application(argc, argv, version, compileDate, compileTime),
    _showName(false),
    _showVersion(false),
    _showAuthor(false),
    _showDate(false),
    _showClassName(false),
    _tags(true),
    _genMD5(false),
    _genSHA1(false)
{
  registerOption(NUL, "md5", String::null,
                 "Generate MD5 checksum for plugin.");
  registerOption(NUL, "sha1", String::null,
                 "Generate SHA1 checksum for plugin.");
  registerOption(NUL, "pname", String::null,
                 "Display plugin name.");
  registerOption(NUL, "pversion", String::null,
                 "Display plugin version.");
  registerOption(NUL, "pauthor", String::null,
                 "Display plugin author.");
  registerOption(NUL, "pdate", String::null,
                 "Display plugin build date/time.");
  registerOption(NUL, "pclass", String::null,
                 "Display plugin class name.");
  registerOption(NUL, "info", String::null,
                 "Display all plugin information (default).");

  int index;

  if(! parseOptions(argc, argv, index) || (index == argc))
  {
    printUsage();
    exit(EXIT_FAILURE);
  }

  if(!(_showName || _showVersion || _showAuthor || _showClassName || _showDate
       || _genMD5 || _genSHA1))
  {
    _showName = _showVersion = _showAuthor = _showClassName = _showDate = true;
  }

  int count = 0;
  if(_showName) ++count;
  if(_showVersion) ++count;
  if(_showAuthor) ++count;
  if(_showClassName) ++count;
  if(_showDate) ++count;
  _tags = (count > 1);

  bool single = (index == (argc - 1));
  for(int i = index; i < argc; ++i)
  {
    String path = argv[i];

    if(i > index)
      std::cout << std::endl;

    if(! single)
      std::cout << "File:       " << path << std::endl;

    if(_genMD5)
    {
      MD5Digest digest;
      outputDigest(path, digest);
    }
    else if(_genSHA1)
    {
      SHA1Digest digest;
      outputDigest(path, digest);
    }
    else
      outputPluginInfo(path);
  }
}

/*
 */

PluginTool::~PluginTool()
{
}

/*
 */

bool PluginTool::processOption(char opt, const String &longOpt,
                               const String& arg)
{
  if(longOpt == "pname")
    _showName = true;
  else if(longOpt == "pversion")
    _showVersion = true;
  else if(longOpt == "pauthor")
    _showAuthor = true;
  else if(longOpt == "pdate")
    _showDate = true;
  else if(longOpt == "pclass")
    _showClassName = true;
  else if(longOpt == "md5")
    _genMD5 = true;
  else if(longOpt == "sha1")
    _genSHA1 = true;
  else if(longOpt == "info")
    _showName = _showVersion = _showAuthor = _showDate = _showClassName = true;
  else
    return(Application::processOption(opt, longOpt, arg));

  return(true);
}

/*
 */

void PluginTool::printUsage()
{
  Application::printUsage();
}

/*
 */

void PluginTool::outputPluginInfo(const String& file)
{
  PluginLoader loader(file);

  try
  {
    loader.load();

    if(_showName)
      std::cout << (_tags ? "Name:       " : "") << loader.getName()
                << std::endl;

    if(_showVersion)
      std::cout << (_tags ? "Version:    " : "") << loader.getVersion()
                << std::endl;

    if(_showAuthor)
      std::cout << (_tags ? "Author:     " : "") << loader.getAuthor()
                << std::endl;

    if(_showDate)
      std::cout << (_tags ? "Build Date: " : "") << loader.getBuildDate()
                << std::endl;

    if(_showClassName)
      std::cout << (_tags ? "Class Name: " : "") << loader.getClassName()
                << std::endl;
  }
  catch(const IOException &ioex)
  {
    CString cstr_file = file.toUTF8();
    CString cstr_msg = ioex.getMessage().toUTF8();
    printError("Error reading plugin: %s\n", cstr_file.data());
    printError("%s\n", cstr_msg.data());
  }
}

/*
 */

void PluginTool::outputDigest(const String& path, Digest& digest)
{
  try
  {
    File file(path);
    file.open(ccxx::IORead, ccxx::FileOpen);

    size_t r;
    byte_t buf[16384];

    while((r = file.read(buf, sizeof(buf))) > 0)
      digest.update(buf, r);

    file.close();
  }
  catch(const EOFException &eofex)
  {
    const size_t sumLen = digest.size();
    DynamicArray<byte_t> sum(sumLen);
    digest.finish(sum.data());

    const size_t sumTextLen = sumLen * 2;
    DynamicArray<char> sumText(sumTextLen + 1);
    Hex::encode(sum.data(), sumLen, sumText, sumTextLen + 1);
    sumText[sumTextLen] = NUL;

    std::cout << sumText << std::endl;
  }
  catch(const IOException &ioex)
  {
    printError("Error reading plugin: %s\n", ioex.toString().toUTF8().data());
  }
}


} // namespace tools
} // namespace ccxx
