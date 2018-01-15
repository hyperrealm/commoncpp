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

#include "StubService.h++"

#include <commonc++/Process.h++>
#include <commonc++/System.h++>
#include <commonc++/Thread.h++>

#include <iostream>

using namespace ccxx;

int main(int argc, char **argv)
{
  StubService svc(argc, argv);
}

/*
*/

StubService::StubService(int argc, char **argv)
  : Service(argc, argv, "1.0", __DATE__, __TIME__)
{
  int index;

  setDescription("commonc++ stub service");

  if(! parseOptions(argc, argv, index))
  {
    std::cout << "option parsing failed" << std::endl;
    exit(ExitStatus::FAILURE);
  }

  try
  {
    detach();
  }
  catch(SystemException& ex)
  {
    std::cerr << "Unable to detach: " << ex.getMessage() << std::endl;
    exit(ExitStatus::FAILURE);
  }

  exit(ExitStatus::SUCCESS);
}

/*
*/

void StubService::run()
{
  for(;;)
  {
    testShutdown();
    Thread::sleep(1000);
  }
}

/*
*/

void StubService::shutdown()
{
  std::cerr << "Service shutdown()" << std::endl;
}
