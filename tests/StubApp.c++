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

#include "StubApp.h++"

#include <commonc++/Process.h++>
#include <commonc++/String.h++>

#include <iostream>
#include <vector>

using namespace ccxx;

StubApp::StubApp(int argc, char **argv)
  : Application(argc, argv, "1.0", __DATE__, __TIME__)
{
int index;

  registerOption('Q', "quit", "", "Exit the program.");
  registerOption('V', "verylongoptionnameindeed", "bb", "This is an option with a very long name. And it doesn't even do anything.");
  registerOption('L', "logfile", "file", "Specify log file as @@. The standard output and standard error streams of the process will be redirected to @@ at application launch time.");

  if(! parseOptions(argc, argv, index))
  {
    std::cout << "option parsing failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Press Return to exit. ";
  char c;
  std::cin >> c;

  exit(ExitStatus::SUCCESS);
}

/*
 */

bool StubApp::processOption(char opt, const String longopt, const char *arg)
{
  std::cout << "Option: ";

  if(opt != NUL)
    std::cout << opt << " ";

  if(longopt)
    std::cout << longopt << " ";

  std::cout << "= " << (arg ? arg : "(null)") << std::endl;

  return(true);
}

/*
 */

void StubApp::shutdown()
{
  std::cout << std::endl << "Program terminated." << std::endl;
  ::exit(5);
}

/*
 */

int main(int argc, char **argv)
{
  StubApp app(argc, argv);
}
