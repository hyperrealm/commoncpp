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

#ifndef __ccxx_URL_hxx
#define __ccxx_URL_hxx

#include <memory>

#include <commonc++/Common.h++>
#include <commonc++/ParseException.h++>
#include <commonc++/RegExp.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A class representing a URL (Uniform Resource Locator). This class will
 * parse simple, common URLs, but is not a fully-conformant URL parser.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API URL
{
 public:

  /** Construct a new, empty URL. */
  URL();

  /** Construct a new URL for the given %URL string. */
  URL(const String& url);

  /** Copy constructor. */
  URL(const URL& other);

  /** Destructor. */
  ~URL();

  /** Set the scheme component of the URL. */
  inline void setScheme(const String& scheme)
  { _scheme = scheme; _url = String::null; }

  /** Get the scheme component of the URL. */
  inline String getScheme() const
  { return(_scheme); }

  /** Set the hostname component of the URL. */
  inline void setHost(const String& host)
  { _host = host; _url = String::null; }

  /** Get the hostname component of the URL. */
  inline String getHost() const
  { return(_host); }

  /**
   * Set the port number for the URL. A value of 0 indicates no port number.
   */
  inline void setPort(uint16_t port)
  { _port = port; _url = String::null; }

  /**
   * Get the port number for the URL. Returns 0 if no port number was
   * specified.
   */
  inline uint16_t getPort() const
  { return(_port); }

  /** Set the path component of the URL. */
  void setPath(const String& path);

  /** Get the path component of the URL. */
  inline String getPath() const
  { return(_path); }

  /** Get the path component of the URL, in canonicalized form. */
  inline String getCanonicalPath() const
  { return(_canonicalPath); }

  /**
   * Set the query component of the URL.
   *
   * @param query The query, in URL-encoded form.
   */
  inline void setQuery(const String& query)
  { _query = query; _url = String::null; }

  /**
   * Get the query component of the URL.
   *
   * @return The query, in its original, URL-encoded form.
   */
  inline String getQuery() const
  { return(_query); }

  /** Test if the URL is valid. */
  bool isValid() const;

  /** Get a String representation of the URL. */
  String toString() const;

  /** Equality operator. */
  bool operator==(const URL& other) const;

  /** Inequality operator. */
  inline bool operator!=(const URL& other) const
  { return(! operator==(other)); }

  /** Assignment operator. */
  URL& operator=(const String& other);

  /** Assignment operator. */
  URL& operator=(const URL& other);

  /**
   * URL-encode a String.
   *
   * @param str The string to encode.
   * @return The encoded form.
   */
  static String encode(const String& str);

  /**
   * Decode a URL-encoded String.
   *
   * @param str The string to decode.
   * @return THe decoded form.
   */
  static String decode(const String& str);

 private:

  void _init();
  void _parseURL(const String& url);
  String _canonicalizePath(const String& path);

  String _scheme;
  String _host;
  uint16_t _port;
  String _path;
  String _canonicalPath;
  String _query;
  mutable String _url;
  RegExp _re;
};

} // namespace ccxx

#endif // __ccxx_URL_hxx
