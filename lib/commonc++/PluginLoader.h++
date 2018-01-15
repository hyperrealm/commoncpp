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

#ifndef __ccxx_PluginLoader_hxx
#define __ccxx_PluginLoader_hxx

#include <commonc++/LoadableModule.h++>

namespace ccxx {

class COMMONCPP_API Plugin;

/**
 * A convenience class for loading Plugin objects. The template
 * parameter <i>T</i> should be a suitable subclass of Plugin.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API PluginLoader
{
 public:

  /**
   * Construct a new PluginLoader with the specified path.
   *
   * @param path The path to the loadable object file from which the plugin
   * will be loaded.
   */
  PluginLoader(const String& path = String::null);

  /** Destructor. Unloads the plugin. */
  ~PluginLoader();

  /**
   * Set the plugin file path. If the plugin is loaded, the call has
   * no effect.
   */
  inline void setPath(const String& path)
  { _module.setPath(path); }

  /** Get the plugin file path. */
  inline String getPath() const
  { return(_module.getPath()); }

  /**
   * Load the plugin. If the plugin is already loaded, the method has
   * no effect.
   *
   * @throw IOException If the object could not be loaded, or if no path
   * has been specified.
   */
  void load()
  { _module.open(); }

  /**
   * Unload the plugin. If the plugin has not been loaded, the method has
   * no effect.
   */
  inline void unload()
  { _module.close(); }

  /** Determine if the plugin is loaded. */
  inline bool isLoaded() const
  { return(_module.isOpen()); }

  /**
   * Get the plugin name.
   *
   * @return The name, or <b>String::null</b> if the name could not
   * be determined, or if the plugin has not yet been loaded via
   * load().
   */
  String getName();

  /**
   * Get the plugin version number.
   *
   * @return The version number, or <b>String::null</b> if the version number
   * could not be determined, or if the plugin has not yet been loaded
   * via load().
   */
  String getVersion();

  /**
   * Get the plugin author information.
   *
   * @return The author information, or <b>String::null</b> if the
   * author information could not be determined, or if the plugin
   * has not yet been loaded via load().
   */
  String getAuthor();

  /**
   * Get the plugin class name.
   *
   * @return The plugin class name, or <b>String::null</b> if the
   * class name could not be determined, or if the plugin has not yet
   * been loaded via load().
   */
  String getClassName();

  /**
   * Get the plugin build date & time.
   *
   * @return The build date and time, or <b>String::null</b> if the
   * build date and time could not be determined, or if the plugin has
   * not yet been loaded via load().
   */
  String getBuildDate();

  /**
   * Construct a new instance of a plugin of type T, where T is a
   * subclass of Plugin.
   *
   * @return The new instance, or <b>NULL</b> if the plugin could not
   * be instantiated, if the plugin has not yet been loaded via load(), or
   * if the cast to type T failed.
   */
  template<class T> T* newInstance()
  {
    Plugin *p = NULL;
    try
    {
      p = _newInstance();
    }
    catch(...) { }

    if(! p)
      return(NULL);

    T* plugin = NULL;
    plugin = dynamic_cast<T *>(p);
    if(! plugin)
      delete p;

    return plugin;
  }

 private:

  String _fetchString(const char* symbol);
  Plugin* _newInstance();

  LoadableModule _module;

  CCXX_COPY_DECLS(PluginLoader);
};

} // namespace ccxx

#endif // __ccxx_PluginLoader_hxx
