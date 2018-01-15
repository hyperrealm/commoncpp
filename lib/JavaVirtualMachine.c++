/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2014  Mark A Lindner

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

#include "commonc++/JavaVirtualMachine.h++"
#include "commonc++/Log.h++"
#include "commonc++/String.h++"
#include "commonc++/UnsupportedOperationException.h++"

#include <algorithm>

namespace ccxx {

/** The latest JNI version, as of this writing. */
#define JVM_VERSION JNI_VERSION_1_4

/*
 */

JavaVirtualMachine::JavaVirtualMachine(
  const SearchPath& classPath /* = SearchPath::empty */,
  const SearchPath& linkerPath /* = SearchPath::empty */,
  uint_t initialHeapSize /* = 0 */,
  uint_t maxHeapSize /* = 0 */,
  bool server /* = false */,
  bool verbose /* = false */,
  bool nojit /* = false */)
  : _verbose(verbose)
{
#ifdef CCXX_OS_ANDROID

  throw UnsupportedOperationException();

#else // CCXX_OS_ANDROID

  jint res;
  JNIEnv *env;
  int i;
  std::string arg1, arg2, arg3, arg4;
  StringVec options;

  if(! classPath.isEmpty())
  {
    String s = "-Djava.class.path=";
    s += classPath.getPath();

    options << s;
  }

  if(! linkerPath.isEmpty())
  {
    String s = "-Djava.library.path=";
    s += linkerPath.getPath();

    options << s;
  }

  if(initialHeapSize > 0)
  {
    String s = "-Xms";
    s += static_cast<int>(initialHeapSize);
    s += "m";
    options << s;
  }

  if(maxHeapSize > 0)
  {
    String s = "-Xmx";
    s += static_cast<int>(maxHeapSize);
    s += "m";
    options << s;
  }

  if(server)
  {
    String s = "-server";
    options << s;

    s = "-Xrs";
    options << s; // disable signal handling
  }

  if(nojit)
  {
    String s = "-Djava.compiler=NONE";
    options << s;
  }

  if(_verbose)
  {
    String s = "-verbose:gc";
    options << s;

    s = "-verbose:class";
    options << s;

    s = "-verbose:jni";
    options << s;
  }

  JavaVMInitArgs vm_args;
  JavaVMOption *vm_opts = new JavaVMOption[options.size()];

  i = 0;
  for(StringVec::const_iterator iter = options.begin();
      iter != options.end();
      iter++)
  {
    CString cstr_option = (*iter).toUTF8();
    vm_opts[i++].optionString = ::strdup(cstr_option.data());
  }

  vm_args.version = JVM_VERSION;
  vm_args.options = vm_opts;
  vm_args.nOptions = options.size();
  vm_args.ignoreUnrecognized = JNI_TRUE;

  res = JNI_CreateJavaVM(&_jvm, (void **)(void *)&env, &vm_args);

  int len = i;
  for(i = 0; i < len; ++i)
    ::free(vm_opts[i].optionString);
  delete[] vm_opts;

  if(res < 0)
    Log_error("Java VM creation failed.");
  else
    Log_debug("Java VM created.");

  // associate the env pointer with this thread

  attachThread();

  Log_debug("Java VM initialization complete.");

#endif // CCXX_OS_ANDROID
}

/*
 */

JavaVirtualMachine::JavaVirtualMachine(JavaVM* jvm)
  : _jvm(jvm)
{
}

/*
 */

JavaVirtualMachine::~JavaVirtualMachine()
{
#ifndef CCXX_OS_ANDROID

  try
  {
    _jvm->DestroyJavaVM();
  }
  catch(...)
  {
    Log_warning("Exception raised while destroying JVM");
  }

#endif // CCXX_OS_ANDROID
}

/*
 */

JavaContext JavaVirtualMachine::attachThread()
{
  Log_debug("JavaVirtualMachine::getContext()");

  JavaContext& ctx = *_context;
  if(! ctx.isAttached())
  {
    // Not yet attached
    JNIEnv* env;
    if(_jvm->AttachCurrentThread((void **)(void *)&env, NULL) != 0)
    {
      Log_error("AttachCurrentThread() failed");
      throw JavaThreadException();
    }
    ctx.attach(this, env, _verbose);
  }

  return(ctx);
}

/*
 */

void JavaVirtualMachine::detachThread()
{
  Log_debug("JavaVirtualMachine::detachThread()");
  JavaContext& ctx = *_context;

  if(! ctx.isAttached())
  {
    Log_warning("Detaching a thread that is not attached!");
    return;
  }

  if(_jvm->DetachCurrentThread() != 0)
  {
    Log_error("DetachCurrentThread() failed.");
    throw JavaThreadException();
  }
  ctx.detach();
}


} // namespace ccxx
