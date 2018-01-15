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

#include "TestHarness.h++"

#include <cstdlib>
#include <stdexcept>

#include <cppunit/Exception.h>
#include <cppunit/SourceLine.h>
#include <cppunit/TestFailure.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <commonc++/TerminalAttr.h++>

using namespace ccxx;

/*
 */

int main(int argc, char **argv)
{
  TestHarness app(argc, argv);

  exit(EXIT_SUCCESS);
}

/*
 */

TestHarness::TestHarness(int argc, char** argv,
                         const String& version  /* = 0.0 */,
                         const String& compileDate /* = "" */,
                         const String& compileTime /* = "" */)
  : Application(argc, argv, version, compileDate, compileTime),
    _format(TextFormat),
    _listOnly(false),
    _verbose(false)
{
  registerOption('l', "list", String::null,
                 "List available tests.");
  registerOption('t', "text", String::null,
                 "Output results in text format.");
  registerOption('x', "xml", String::null,
                 "Output results in XML format.");
  registerOption('c', "compiler", String::null,
                 "Output results in compiler format.");
  registerOption('v', "verbose", String::null,
                 "Display verbose progress output.");

  int index;

  if(! parseOptions(argc, argv, index))
  {
    printUsage();
    exit(EXIT_FAILURE);
  }

  _controller.addListener(&_result);
  _controller.addListener(&_progress);

  CppUnit::TestFactoryRegistry &registry
    = CppUnit::TestFactoryRegistry::getRegistry();

  _suite = dynamic_cast<CppUnit::TestSuite *>(registry.makeTest());

  if(_listOnly)
    _listTests();
  else
  {
    _runner.addTest(_suite);

    StringVec names;
    for(int i = index; i < argc; ++i)
      names << argv[i];

    _runTests(names);
  }

  _result.outputSummary();

  exit(EXIT_SUCCESS);
}

/*
 */

TestHarness::~TestHarness()
{
}

/*
 */

void TestHarness::_listTests()
{
  int n = _suite->getChildTestCount();
  for(int i = 0; i < n; i++)
  {
    CppUnit::Test *ctest = _suite->doGetChildTestAt(i);
    std::cout << ctest->getName() << '\n';
  }
}

/*
 */

void TestHarness::_runTests(const StringVec& tests)
{
  _progress.setVerbose(_verbose);

  size_t n = tests.size();

  if(n > 0)
  {
    for(size_t i = 0; i < n; ++i)
    {
      CString cstr = tests[i].toUTF8();
      std::string testPath = cstr.data();

      std::cout << "testPath: " << testPath << std::endl;

      try
      {
        _runner.run(_controller, testPath);
      }
      catch(std::invalid_argument& ex)
      {
        std::cerr << "Warning: " << ex.what() << std::endl;
      }
    }
  }
  else
  {
    // run all the tests
    _runner.run(_controller);
  }

  switch(_format)
  {
    case TextFormat:
    {
//      CppUnit::TextOutputter output(&_result, std::cerr);
//      output.write();
      break;
    }

    case XMLFormat:
    {
      CppUnit::XmlOutputter output(&_result, std::cerr);
      output.write();
      break;
    }

    case CompilerFormat:
    {
      CppUnit::CompilerOutputter output(&_result, std::cerr);
      output.write();
      break;
    }
  }
}

/*
 */

bool TestHarness::processOption(char opt, const String& longOpt,
                                const String& arg)
{
  switch(opt)
  {
    case 't':
      _format = TextFormat;
      break;

    case 'x':
      _format = XMLFormat;
      break;

    case 'c':
      _format = CompilerFormat;
      break;

    case 'l':
      _listOnly = true;
      break;

    case 'v':
      _verbose = true;
      break;

    default:
      return(Application::processOption(opt, longOpt, arg));
  }

  return(true);
}

/*
 */

void TestHarness::shutdown()
{
  std::cout << "Keyboard interrupt." << std::endl;
  Application::shutdown();
}

/*
 */

void TestHarness::printUsage()
{
  Application::printUsage();
  std::cout
    << "And arguments may be zero or more test names; if no test names\n"
    "are specified, all tests in the suite are run.\n\n";
}

/*
 */

void TestHarness::CustomProgressListener::startTestRun(
  CppUnit::Test *test, CppUnit::TestResult *eventManager)
{
}

/*
 */

void TestHarness::CustomProgressListener::startTest(
  CppUnit::Test *test)
{
  if(_verbose)
    std::cerr << "  [" << test->getName() << "]" << std::endl;
}

/*
 */

void TestHarness::CustomProgressListener::startSuite(
  CppUnit::Test *test)
{
  if(_verbose)
    std::cerr << "[" << test->getName() << "]" << std::endl;
}

/*
 */

TestHarness::CustomTestResultCollector::CustomTestResultCollector()
  : _failed(false),
    _passedCount(0),
    _failedCount(0)
{
}

/*
 */

void TestHarness::CustomTestResultCollector::startSuite(CppUnit::Test* suite)
{
  std::cerr << "[SUITE] " << suite->getName() << std::endl;
  _currentSuite = suite->getName().c_str();
}

/*
 */

void TestHarness::CustomTestResultCollector::endSuite(CppUnit::Test* suite)
{
}

/*
 */

void TestHarness::CustomTestResultCollector::startTest(CppUnit::Test* test)
{
  std::cerr << "  [TEST] " << test->getName() << '\r';
  _failed = false;
}

/*
 */

void TestHarness::CustomTestResultCollector::endTest(CppUnit::Test* test)
{
  std::cerr << "  " << (_failed
                        ? TERMATTR_FG_RED "[FAIL]"
                        : TERMATTR_FG_GREEN "[PASS]")
            << TERMATTR_FG_NORMAL << ' '
            << test->getName() << std::endl;
  if(_failed)
    ++_failedCount;
  else
    ++_passedCount;

  _failed = false;
}

/*
 */

void TestHarness::CustomTestResultCollector::addFailure(
  const CppUnit::TestFailure& failure)
{
  _failed = true;
  const CppUnit::SourceLine location = failure.sourceLine();
  const CppUnit::Exception *exception = failure.thrownException();
  std::cerr << TERMATTR_BOLD
            << _currentSuite << '.' << failure.failedTestName()
            << " (" << location.fileName() << ':' << location.lineNumber()
            << "):" << '\n';

  const CppUnit::Message &message = exception->message();
  const int count = message.detailCount();
  for(int i = 0; i < count; ++i)
  {
    std::cerr << '\t' << message.detailAt(i) << '\n';
  }
  std::cerr << TERMATTR_NORMAL << std::endl;

  String name;
  name << _currentSuite << '.' << failure.failedTestName().c_str();
  _failedTests << name;
}

/*
 */

void TestHarness::CustomTestResultCollector::outputSummary()
{
  std::cerr << "\n[SUMMARY] "
            << _passedCount << " passed, "
            << _failedCount << " failed, "
            << (_passedCount + _failedCount) << " total."
            << '\n' << std::endl;

  if(_failedTests.size() > 0)
  {
    std::cerr << "Failed tests:\n";
    for(StringVec::const_iterator iter = _failedTests.begin();
        iter != _failedTests.end();
        ++iter)
    {
      std::cerr << '\t' << *iter << '\n';
    }
    std::cerr << std::endl;
  }
}
