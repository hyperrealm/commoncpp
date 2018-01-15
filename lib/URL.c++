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

#include "commonc++/URL.h++"
#include "commonc++/RegExp.h++"

#include <cstring>

namespace ccxx {

static const char *__unreserved = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz" "0123456789" "-+_.~/";

#define TO_HEX(C) ((C) + ((C) < 10 ? '0' : '7'))

/*
 */

URL::URL()
  : _scheme(String::null),
    _host(String::null),
    _port(0),
    _path(String::null),
    _query(String::null),
    _url(String::null)
{
  _init();
}

/*
 */

URL::URL(const URL& other)
  : _scheme(other._scheme),
    _host(other._host),
    _port(other._port),
    _path(other._path),
    _query(String::null),
    _url(String::null)
{
  _init();

  _canonicalPath = (other._canonicalPath ? other._canonicalPath
                    : _canonicalizePath(_path));
}

/*
 */

URL::URL(const String& url)
  : _url(String::null)
{
  _init();
  _parseURL(url);
}

/*
 */

URL::~URL()
{
}

/*
 */

void URL::_init()
{
  try
  {
    _re.setPattern("([-+\\.a-z0-9]*)://"                   // scheme
                   "(([-a-zA-Z0-9\\.]+)"                   // hostname
                   "(:([0-9]+))?)?"                        // port
                   "(/[-A-za-z0-9_\\.\\~\\%/\\+=]*)"       // path
                   "(\\?([-A-Za-z0-9_\\.\\~\\%/=&\\+]+))?" // query
      );
  }
  catch(ParseException& )
  {
    // shouldn't happen
  }
}

/*
 */

void URL::setPath(const String& path)
{
  _path = path;
  _canonicalPath = _canonicalizePath(_path);
  _url = String::null;
}

/*
 */

bool URL::isValid() const
{
  return(! (_scheme.isNull() || _path.isNull()));
}

/*
 */

String URL::toString() const
{
  if(isValid())
  {
    if(_url.isNull())
    {
      _url << _scheme << "://";

      if(! _host.isNull())
      {
        _url << _host;

        if(_port != 0)
          _url << ':' << _port;
      }

      _url << encode(_canonicalPath);

      if(! _query.isNull())
        _url << '?' << encode(_query);
    }
  }

  return(_url);
}

/*
 */

void URL::_parseURL(const String& url)
{
  RegExpMatch subs[9];

  if(_re.match(url, subs, CCXX_LENGTHOF(subs)))
  {
    _scheme = url(subs[1].getStartIndex(), subs[1].getEndIndex())
      .toLowerCase();

    _host = url(subs[3].getStartIndex(), subs[3].getEndIndex());
    if(_host == "")
      _host = String::null;

    String p = url(subs[5].getStartIndex(), subs[5].getEndIndex());
    int v = p.toInt();

    if((v < 0) || (v > 65535))
       v = 0;

    _port = v;

    _path = decode(url(subs[6].getStartIndex(), subs[6].getEndIndex()));
    _canonicalPath = _canonicalizePath(_path);

    // avoid URL-decoding the query, since it may need further processing
    _query = url(subs[8].getStartIndex(), subs[8].getEndIndex());
    if(_query == "")
      _query = String::null;
  }
}

/*
 */

URL& URL::operator=(const URL& other)
{
  _scheme = other._scheme;
  _host = other._host;
  _port = other._port;
  _path = other._path;

  _canonicalPath = (other._canonicalPath ? other._canonicalPath
                    : _canonicalizePath(_path));
  _query = other._query;
  _url = String::null;

  return(*this);
}

/*
 */

URL& URL::operator=(const String& other)
{
  _parseURL(other);

  return(*this);
}

/*
 */

bool URL::operator==(const URL& other) const
{
  return((_scheme == other._scheme)
         && (_host == other._host)
         && (_port == other._port)
         && (_path == other._path)
         && (_query == other._query)
         && (_url == other._url));
}

/*
 */

String URL::encode(const String& str)
{
  String r = "";
  CString cstr_str = str.toUTF8();
  const char *s = cstr_str.data(), *p;

  for(p = s; *p; ++p)
  {
    if(std::strchr(__unreserved, *p))
      r << *p;
    else if(*p == ' ')
      r << '+';
    else
    {
      r << '%' << static_cast<char>(TO_HEX((*p >> 4) & 0x0F))
        << static_cast<char>(TO_HEX(*p & 0x0F));
    }
  }

  return(r);
}

/*
 */

String URL::decode(const String& str)
{
  String r = "";
  CString cstr_str = str.toUTF8();
  const char *s = cstr_str.data(), *p;
  bool escaped = false;
  int ec = 0;
  char cur = 0;

  for(p = s; *p; ++p)
  {
    if(*p == '+')
      r << ' ';
    else if(*p == '%')
    {
      escaped = true;
      ec = 0;
      cur = 0;
    }
    else if(escaped)
    {
      int c = 0;

      if((*p >= '0') && (*p <= '9'))
        c = *p - '0';
      else if((*p >= 'A') && (*p <= 'F'))
        c = *p - 'A' + 10;
      else if((*p >= 'a') && (*p <= 'f'))
        c = *p - 'a' + 10;
      else
        c = 0;

      if(ec++ == 0)
        cur = c;

      else
      {
        cur <<= 4;
        cur += c;
        r << (cur >= ' ' ? cur : '?');
        escaped = false;
        ec = 0;
        cur = 0;
      }
    }
    else
      r << *p;
  }

  return(r);
}

/*
 */

String URL::_canonicalizePath(const String& path)
{
  String cpath = "/";

  uint_t index = 0;

  for(;;)
  {
    String tok = path.nextToken(index, "/");

    if(! tok)
      break;
    else if(tok == ".")
      continue;
    else if(tok == "..")
    {
      int i = cpath.lastIndexOf('/');
      if(i >= 0)
        cpath.remove(i == 0 ? 1 : i);
    }
    else
    {
      if(! cpath.endsWith('/'))
        cpath += '/';

      cpath += tok;
    }
  }

  if(path.endsWith('/') && ! cpath.endsWith('/'))
    cpath += '/';

  return(cpath);
}


} // namespace ccxx
