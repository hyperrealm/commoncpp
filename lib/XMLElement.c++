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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include <iostream>
#include <map>
#include <cctype>
#include <cstdio>
#include <cstring>

#include <expat.h>

#include "commonc++/DeleteFunctor.h++"
#include "commonc++/XMLElement.h++"
#include "commonc++/XMLDocument.h++"

namespace ccxx {

/*
 */

class XMLElement::XMLAttributeMap : public std::map<String, String>
{
};

/*
 */

XMLElement XMLElement::null(String::null, NULL);

/*
 */

XMLElement::XMLElement(const String& name, XMLElement* parent)
  : _name(name),
    _attrs(new XMLAttributeMap()),
    _firstChild(NULL),
    _lastChild(NULL),
    _next(NULL),
    _parent(parent)
{
}

/*
 */

XMLElement::~XMLElement()
{
  removeChildren();
  delete _attrs;
}

/*
 */

XMLElement& XMLElement::getParent()
{
  return(_parent ? *_parent : XMLElement::null);
}

/*
 */

const XMLElement& XMLElement::getParent() const
{
  return(_parent ? *_parent : XMLElement::null);
}

/*
 */

void XMLElement::setAttribute(const String& name, const String& value)
{
  if(isNull())
    return;

  validateName(name);

  (*_attrs)[name] = value;
}

/*
 */

void XMLElement::setAttribute(const String& name, bool value)
{
  if(isNull())
    return;

  validateName(name);

  (*_attrs)[name] = (value ? "true" : "false");
}

/*
 */

void XMLElement::setAttribute(const String& name, int value)
{
  if(isNull())
    return;

  validateName(name);

  String s;
  s << value;

  (*_attrs)[name] = s;
}

/*
 */

void XMLElement::setAttribute(const String& name, const int64_t& value)
{
  if(isNull())
    return;

  validateName(name);

  String s;
  s << value;

  (*_attrs)[name] = s;
}

/*
 */

void XMLElement::setAttribute(const String& name, const double& value)
{
  if(isNull())
    return;

  validateName(name);

  String s;
  s << value;

  (*_attrs)[name] = s;
}

/*
 */

bool XMLElement::getAttribute(const String& name, String& value,
                              const String& defaultValue /*= ""*/) const
{
  validateName(name);

  XMLAttributeMap::const_iterator iter = _attrs->find(name);
  if(iter == _attrs->end())
  {
    value = defaultValue;
    return(false);
  }

  value = iter->second;
  return(true);
}

/*
 */

bool XMLElement::getAttribute(const String& name, int& value,
                              int defaultValue /* = 0 */) const
{
  validateName(name);

  XMLAttributeMap::const_iterator iter = _attrs->find(name);
  if(iter == _attrs->end())
  {
    value = defaultValue;
    return(false);
  }

  value = (iter->second).toInt();
  return(true);
}

/*
 */

bool XMLElement::getAttribute(const String& name, int64_t& value,
                              const int64_t& defaultValue /* = 0 */) const
{
  validateName(name);

  XMLAttributeMap::const_iterator iter = _attrs->find(name);
  if(iter == _attrs->end())
  {
    value = defaultValue;
    return(false);
  }

  value = (iter->second).toInt64();
  return(true);
}

/*
 */

bool XMLElement::getAttribute(const String& name, bool& value,
                              bool defaultValue /* = false */) const
{
  validateName(name);

  XMLAttributeMap::const_iterator iter = _attrs->find(name);
  if(iter == _attrs->end())
  {
    value = defaultValue;
    return(false);
  }

  value = (! iter->second.compareTo("true", false));
  return(true);
}

/*
 */

bool XMLElement::getAttribute(const String& name, double& value,
                              const double& defaultValue /* = 0.0 */) const
{
  validateName(name);

  XMLAttributeMap::const_iterator iter = _attrs->find(name);
  if(iter == _attrs->end())
  {
    value = defaultValue;
    return(false);
  }

  value = iter->second.toDouble();
  return(true);
}

/*
 */

void XMLElement::getAttributeNames(StringVec& names) const
{
  names.clear();

  for(XMLAttributeMap::const_iterator iter = _attrs->begin();
      iter != _attrs->end();
      ++iter)
  {
    names.push_back(iter->first);
  }
}

/*
 */

void XMLElement::removeAttributes()
{
  if(isNull())
    return;

  _attrs->clear();
}

/*
 */

bool XMLElement::hasAttributes() const
{
  return(_attrs->size() > 0);
}

/*
 */

bool XMLElement::hasChildren() const
{
  return(_firstChild != NULL);
}

/*
 */

uint_t XMLElement::getChildCount() const
{
  uint_t count = 0;

  for(XMLElement *e = _firstChild; e; e = e->_next)
    ++count;

  return(count);
}

/*
 */

XMLElement & XMLElement::addChild(const String& name)
{
  if(isNull())
    return(XMLElement::null);

  validateName(name);

  XMLElement *elem = new XMLElement(name, this);

  if(! _lastChild)
    _firstChild = _lastChild = elem;
  else
  {
    _lastChild->_next = elem;
    _lastChild = elem;
  }

  return(*elem);
}

/*
 */

bool XMLElement::hasChild(const String& name) const
{
  validateName(name);

  for(XMLElement *elem = _firstChild; elem; elem = elem->_next)
  {
    if(elem->_name == name)
      return(true);
  }

  return(false);
}

/*
 */

XMLElement& XMLElement::getChild(uint_t index)
{
  const XMLElement &e = _getChild(index);
  return(const_cast<XMLElement &>(e));
}

/*
 */

const XMLElement& XMLElement::getChild(uint_t index) const
{
  const XMLElement &e = _getChild(index);
  return(e);
}

/*
 */

const XMLElement& XMLElement::_getChild(uint_t index) const
{
  uint_t count = 0;
  for(XMLElement *elem = _firstChild; elem; elem = elem->_next)
  {
    if(count == index)
      return(*elem);

    ++count;
  }

  return(XMLElement::null);
}

/*
 */

XMLElement& XMLElement::operator[](int index)
{
  const XMLElement &e = _getChild(index);
  return(const_cast<XMLElement &>(e));
}

/*
 */

const XMLElement& XMLElement::operator[](int index) const
{
  std::cout << "operator[] on index="<< index << std::endl;
  if(index < 0)
    throw OutOfBoundsException();

  return(_getChild(static_cast<uint_t>(index)));
}

/*
 */

XMLElement& XMLElement::operator[](const String& name)
{
  if(isNull())
    return(XMLElement::null);

  validateName(name);

  const XMLElement &elem = _getChild(name);

  if(elem)
    return(const_cast<XMLElement &>(elem));

  XMLElement &nelem = addChild(name);
  return(nelem);
}

/*
 */

XMLElement& XMLElement::operator[](const char* name)
{
  if(isNull())
    return(XMLElement::null);

  validateName(name);

  const XMLElement &elem = _getChild(name);

  if(elem)
    return(const_cast<XMLElement &>(elem));

  XMLElement &nelem = addChild(name);
  return(nelem);
}

/*
 */

void XMLElement::removeChildren()
{
  if(isNull())
    return;

  for(XMLElement *e = _firstChild; e;)
  {
    XMLElement *c = e;
    e = e->_next;
    delete c;
  }

  _firstChild = _lastChild = NULL;
}

/*
 */

void XMLElement::removeChildren(const String& name)
{
  if(isNull())
    return;

  validateName(name);

  for(XMLElement *e = _firstChild; e;)
  {

    if(e->_name == _name)
    {
      XMLElement *c = e->_next;
      delete e;
      e = c;
    }
    else
      e = e->_next;
  }
}

/*
 */

XMLElement& XMLElement::getChild(const String& name /* = String::null */)
{
  const XMLElement &e = _getChild(name);
  return(const_cast<XMLElement &>(e));
}

/*
 */

const XMLElement& XMLElement::getChild(
  const String& name /* = String::null */) const
{
  const XMLElement &e = _getChild(name);
  return(e);
}

/*
 */

const XMLElement& XMLElement::_getChild(
  const String& name /* = String::null */) const
{
  XMLElement *e = _firstChild;

  if(name)
  {
    while(e)
    {
      if(e->_name == name)
        break;

      e = e->_next;
    }
  }

  return(e ? *e : XMLElement::null);
}

/*
 */

XMLElement& XMLElement::operator=(bool val)
{
  if(! isNull())
  {
    String s;
    s << val;
    setContent(s);
  }

  return(*this);
}

/*
 */

XMLElement& XMLElement::operator=(long val)
{
  if(!isNull())
  {
    String s;
    s << static_cast<int32_t>(val);
    setContent(s);
  }

  return(*this);
}

/*
 */

XMLElement& XMLElement::operator=(unsigned long val)
{
  if(! isNull())
  {
    String s;
    s << static_cast<uint32_t>(val);
    setContent(s);
  }

  return(*this);
}

/*
 */

XMLElement& XMLElement::operator=(const int64_t& val)
{
  if(! isNull())
  {
    String s;
    s << val;
    setContent(s);
  }

  return(*this);
}

/*
 */

XMLElement& XMLElement::operator=(const uint64_t& val)
{
  if(! isNull())
  {
    String s;
    s << val;
    setContent(s);
  }

  return(*this);
}

/*
 */

XMLElement& XMLElement::operator=(float val)
{
  if(! isNull())
  {
    String s;
    s << val;
    setContent(s);
  }

  return(*this);
}

/*
 */

XMLElement& XMLElement::operator=(const double& val)
{
  if(! isNull())
  {
    String s;
    s << val;
    setContent(s);
  }

  return(*this);
}

/*
 */

XMLElement& XMLElement::operator=(const String& val)
{
  if(! isNull())
    setContent(val);

  return(*this);
}

/*
 */

bool XMLElement::toBool() const
{
  String s = getContent();
  return(s.toBool());
}

/*
 */

int XMLElement::toInt() const
{
  String s = getContent();
  return(s.toInt());
}

/*
 */

uint_t XMLElement::toUInt() const
{
  String s = getContent();
  return(s.toUInt());
}

/*
 */

int64_t XMLElement::toInt64() const
{
  String s = getContent();
  return(s.toInt64());
}

/*
 */

uint64_t XMLElement::toUInt64() const
{
  String s = getContent();
  return(s.toUInt64());
}

/*
 */

float XMLElement::toFloat() const
{
  String s = getContent();
  return(s.toFloat());
}

/*
 */

double XMLElement::toDouble() const
{
  String s = getContent();
  return(s.toDouble());
}

/*
 */

void XMLElement::write(std::ostream &stream, int depth, uint_t tabWidth) const
{
  bool endTag = false;
  int i;

  if(tabWidth)
    for(i = depth * tabWidth; i--; stream << " ");

  stream << "<" << getName();

  if(hasAttributes())
  {
    // iterate over attributes

    for(XMLAttributeMap::const_iterator iter = _attrs->begin();
        iter != _attrs->end();
        iter++)
    {
      stream << ' ' << iter->first << "=\"";

      const String& name = iter->second;
      if(! name.isNull())
        stream << name;

      stream << '\"';
    }
  }

  // we either have children or content, but not both

  if(hasChildren() || hasContent())
  {
    stream << '>';
    endTag = true;
  }

  // output content

  if(hasContent())
  {
    String content = getContent();
    String escapedContent;

    for(const char16_t* c = content.begin(); c < content.end(); ++c)
    {
      if(*c == '<')
        escapedContent << "&lt;";
      else if(*c == '>')
        escapedContent << "&gt;";
      else if(*c == '&')
        escapedContent << "&amp;";
      else if(Char::isASCII(*c))
      {
        char buf[16];
        snprintf(buf, sizeof(buf), "&#x%04X;", *c);
        escapedContent << buf;
      }
      else
        escapedContent << *c;
    }

    CString cstr_escapedContent = escapedContent.toUTF8();
    stream.write(cstr_escapedContent.data(), cstr_escapedContent.length());
  }

  // recurse on children

  if(hasChildren())
  {
    if(tabWidth)
      stream << '\n';

    for(XMLElement *e = _firstChild; e; e = e->_next)
      e->write(stream, depth + 1, tabWidth);
  }

  if(endTag)
  {
    if(hasChildren() && tabWidth)
      for(i = depth; i--; stream << "  ");

    stream << "</" << getName();
  }
  else
    stream << '/';

  stream << '>';

  if(tabWidth)
    stream << '\n';
}

/*
 */

void XMLElement::validateName(const String& name)
{
  std::cout << "validating:" << name << std::endl;
  if(name.isEmpty())
    throw InvalidArgumentException();

  Char c = name.charAt(0);
  if(! (c.isAlphabetic() || (c == '_') || (c == ':')))
    throw InvalidArgumentException();

  int len = name.length();
  for(int i = 1; i < len; ++i)
  {
    Char c = name.charAt(i);

    if(! (c.isAlphabetic() || c.isDigit() || (c == '-') || (c == '_')
          || (c == ':') || (c == '.')))
      throw InvalidArgumentException();
  }
  std::cout << "OK" << std::endl;
}


} // namespace ccxx
