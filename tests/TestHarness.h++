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

#ifndef __ccxx_TestHarness_hxx
#define __ccxx_TestHarness_hxx

#include <commonc++/Application.h++>
#include <commonc++/String.h++>

#include <cppunit/TestResultCollector.h>
#include <cppunit/TestResult.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/ui/text/TestRunner.h>

namespace ccxx {

class TestHarness : public Application
{
 public:

  TestHarness(int argc, char** argv, const String& version = "0.0",
              const String& compileDate = "",
              const String& compileTime = "");
  ~TestHarness();

 protected:

  bool processOption(char opt, const String& longOpt, const String& arg);

  void shutdown();

  void printUsage();

 private:

  class CustomProgressListener : public CppUnit::TestListener
  {
   public:

    void startTestRun(CppUnit::Test* test, CppUnit::TestResult* eventManager);
    void startTest(CppUnit::Test* test);
    void startSuite(CppUnit::Test* suite);

    inline void setVerbose(bool verbose)
    { _verbose = verbose; }

   private:

    bool _verbose;
  };

  class CustomTestResultCollector : public CppUnit::TestResultCollector
  {
   public:

    CustomTestResultCollector();

    void startSuite(CppUnit::Test* suite);
    void endSuite(CppUnit::Test* suite);
    void startTest(CppUnit::Test* test);
    void endTest(CppUnit::Test* test);
    void addFailure(const CppUnit::TestFailure& failure);

    void outputSummary();

   private:

    bool _failed;
    uint_t _passedCount;
    uint_t _failedCount;
    StringVec _failedTests;
    String _currentSuite;
  };

  void _listTests();
  void _runTests(const StringVec& tests);

  enum OutputFormat { TextFormat, XMLFormat, CompilerFormat };

  OutputFormat _format;
  bool _listOnly;
  bool _verbose;

  CppUnit::TestResult _controller;
  CustomTestResultCollector _result;
  CustomProgressListener _progress;
  CppUnit::TextUi::TestRunner _runner;
  CppUnit::TestSuite* _suite;
};

} // namespace ccxx

#endif // __ccxx_TestHarness_hxx
