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

#ifndef __ccxx_Plugin_hxx
#define __ccxx_Plugin_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A class representing a dynamically loaded plugin. The class provides no
 * behavior of its own; it is simply used as a way to tag classes which
 * represent loadable plugins.
 *
 * See the CCXX_EXPORT_PLUGIN() macro.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Plugin
{
 protected:

  /**
   * Constructor. Subclasses <i>must</i> provide a default (no-argument)
   * constructor.
   */
  Plugin();

 public:

  /** Destructor. */
  virtual ~Plugin();

 private:

  CCXX_COPY_DECLS(Plugin);
};

} // namespace ccxx

#ifdef CCXX_OS_WINDOWS
#define COMMONCPP_PLUGIN_API __declspec(dllexport)
#else
#define COMMONCPP_PLUGIN_API
#endif // CCXX_OS_WINDOWS

/**
 * @def CCXX_EXPORT_PLUGIN(CLASS, NAME, VERSION)
 *
 * Exports the plugin whose implementation is in the class named CLASS
 * (which must be a subclass of Plugin) whose name is indicated by the
 * C string NAME, whose version is indicated by the C string VERSION,
 * and whose author/copyright information is indicated by the C string
 * AUTHOR, so that it can be dynamically loaded by a
 * PluginLoader. This macro should be called from within the plugin
 * implementation's source file but <i>not</i> within any namespace
 * scope.
 */
#define CCXX_EXPORT_PLUGIN(CLASS, NAME, VERSION, AUTHOR)                \
  extern "C" {                                                          \
    COMMONCPP_PLUGIN_API ccxx::Plugin *ccxx_plugin_new(void)            \
    { return(new CLASS); }                                              \
    COMMONCPP_PLUGIN_API const char *ccxx_plugin_version(void)          \
    { return(VERSION); }                                                \
    COMMONCPP_PLUGIN_API const char *ccxx_plugin_name(void)             \
    { return(NAME); }                                                   \
    COMMONCPP_PLUGIN_API const char *ccxx_plugin_author(void)           \
    { return(AUTHOR); }                                                 \
    COMMONCPP_PLUGIN_API const char *ccxx_plugin_build_date(void)       \
    { return(__DATE__ " " __TIME__); }                                  \
    COMMONCPP_PLUGIN_API const char *ccxx_plugin_class_name(void)       \
    { return(#CLASS); }                                                 \
  };

#endif // __ccxx_Plugin_hxx
