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

#include "commonc++/Variant.h++"
#include "commonc++/Hex.h++"

#include <cstdio>
#include <vector>
#include <map>

namespace ccxx {

/*
 */

class Variant::VariantList : public std::vector<Variant>
{
  public:

  bool operator==(const VariantList &other) const
  {
    if(size() != other.size())
      return(false);

    const_iterator iter = begin();
    const_iterator oiter = other.begin();

    for(; iter != end(); ++iter, ++oiter)
    {
      const Variant &val = *iter;
      const Variant &oval = *oiter;

      if(val != oval)
        return(false);
    }

    return(true);
  }

  VariantList& operator=(const VariantList &other)
  {
    if(this != &other)
    {
      clear();

      for(const_iterator iter = other.begin();
          iter != other.end();
          ++iter)
      {
        const Variant &val = *iter;
        push_back(Variant(val));
      }
    }

    return(*this);
  }
};

/*
 */

class Variant::VariantMap : public std::map<String, Variant>
{
  public:

  bool operator==(const VariantMap &other) const
  {
    if(size() != other.size())
      return(false);

    const_iterator iter = begin();
    const_iterator oiter = other.begin();

    for(; iter != end(); ++iter, ++oiter)
    {
      const String &name = iter->first;
      const String &oname = oiter->first;

      if(name != oname)
        return(false);

      const Variant &val = iter->second;
      const Variant &oval = oiter->second;

      if(val != oval)
        return(false);
    }

    return(true);
  }

  VariantMap& operator=(const VariantMap &other)
  {
    if(this != &other)
    {
      clear();

      for(const_iterator iter = other.begin();
          iter != other.end();
          ++iter)
      {
        const String &name = iter->first;
        const Variant &val = iter->second;

        operator[](name) = Variant(val);
      }
    }

    return(*this);
  }

};

/*
 */

const Variant Variant::null;

/*
 */

Variant::Variant()
  : _type(TypeNull)
{
  _initValue();
}

/*
 */

Variant::Variant(const Variant &other)
  : _type(TypeNull)
{
  _copy(other);
}

/*
 */

Variant::Variant(bool val)
  : _type(TypeBool)
{
  _value.boolVal = val;
}

/*
 */

Variant::Variant(char val)
  : _type(TypeChar)
{
  _value.charVal = val;
}

/*
 */

Variant::Variant(char16_t val)
  : _type(TypeChar16)
{
  _value.char16Val = val;
}

/*
 */

Variant::Variant(int32_t val)
  : _type(TypeInt)
{
  _value.intVal = val;
}

/*
 */

Variant::Variant(uint32_t val)
  : _type(TypeUInt)
{
  _value.uintVal = val;
}

/*
 */

Variant::Variant(float val)
  : _type(TypeFloat)
{
  _value.floatVal = val;
}

/*
 */

Variant::Variant(const double &val)
  : _type(TypeDouble)
{
  _value.doubleVal = val;
}

/*
 */

Variant::Variant(const int64_t &val)
  : _type(TypeInt64)
{
  _value.int64Val = val;
}

/*
 */

Variant::Variant(const uint64_t &val)
  : _type(TypeUInt64)
{
  _value.uint64Val = val;
}

/*
 */

Variant::Variant(const String& val)
  : _type(TypeString)
{
  _value.stringVal = new String(val);
}

/*
 */

Variant::Variant(const Blob &val)
  : _type(TypeBlob)
{
  _value.blobVal = new Blob(val);
}

/*
 */

Variant::Variant(const char *val)
  : _type(TypeString)
{
  _value.stringVal = new String(val);
}

/*
 */

Variant::Variant(const char16_t *val)
  : _type(TypeString)
{
  _value.stringVal = new String(val);
}

/*
 */

Variant::Variant(void *val)
  : _type(TypeOpaque)
{
  _value.opaqueVal = val;
}

/*
 */

Variant::~Variant()
{
  _deleteValue();
}

/*
 */

void Variant::setType(Variant::Type type)
{
  _deleteValue();
  _type = type;
  _initValue();
}

/*
 */

bool Variant::toBool() const
{
  switch(_type)
  {
    case TypeNull:
      return(false);

    case TypeBool:
      return(_value.boolVal);

    case TypeChar:
      return(_value.charVal != NUL);

    case TypeChar16:
      return(_value.char16Val != 0);

    case TypeInt:
      return(_value.intVal != 0);

    case TypeUInt:
      return(_value.uintVal != 0);

    case TypeFloat:
      return(_value.floatVal != 0.0f);

    case TypeDouble:
      return(_value.doubleVal != 0.0);

    case TypeInt64:
      return(_value.int64Val != INT64_CONST(0));

    case TypeUInt64:
      return(_value.uint64Val != UINT64_CONST(0));

    case TypeString:
      return(_value.stringVal->toBool());

    default:
      return(false);
  }
}

/*
 */

char Variant::toChar() const
{
  return(_type == TypeChar ? _value.charVal : NUL);
}

/*
 */

char16_t Variant::toChar16() const
{
  return(_type == TypeChar16 ? _value.char16Val : 0);
}

/*
 */

int32_t Variant::toInt() const
{
  switch(_type)
  {
    case TypeNull:
      return(0);

    case TypeBool:
      return(_value.boolVal ? 1 : 0);

    case TypeChar:
      return(static_cast<int>(_value.charVal));

    case TypeChar16:
      return(static_cast<int>(_value.char16Val));

    case TypeInt:
      return(_value.intVal);

    case TypeUInt:
      if(_value.uintVal <= INT32_MAX)
        return(static_cast<int32_t>(_value.uintVal));
      else
        return(0);

    case TypeFloat:
      return(static_cast<int32_t>(_value.floatVal));

    case TypeDouble:
      return(static_cast<int32_t>(_value.doubleVal));

    case TypeInt64:
      if((_value.int64Val <= static_cast<int64_t>(INT32_MAX))
         && (_value.int64Val >= static_cast<int64_t>(INT32_MIN)))
        return(static_cast<int32_t>(_value.int64Val));
      else
        return(0);

    case TypeUInt64:
      if(_value.uint64Val <= INT32_MAX)
        return(static_cast<int32_t>(_value.uint64Val));
      else
        return(0);

    case TypeString:
      return(_value.stringVal->toInt());

    default:
      return(0);
  }
}

/*
 */

uint32_t Variant::toUInt() const
{
  switch(_type)
  {
    case TypeNull:
      return(0);

    case TypeBool:
      return(_value.boolVal ? 1 : 0);

    case TypeChar:
      return(static_cast<int>(_value.charVal));

    case TypeChar16:
      return(static_cast<int>(_value.char16Val));

    case TypeInt:
      if(_value.intVal >= 0)
        return(_value.intVal);
      else
        return(0);

    case TypeUInt:
      return(_value.uintVal);

    case TypeFloat:
      return(static_cast<uint32_t>(_value.floatVal));

    case TypeDouble:
      return(static_cast<uint32_t>(_value.doubleVal));

    case TypeInt64:
      if((_value.int64Val <= UINT32_MAX) && (_value.int64Val >= 0))
        return(static_cast<uint32_t>(_value.int64Val));
      else
        return(0);

    case TypeUInt64:
      if((_value.uint64Val <= UINT32_MAX) && (_value.uint64Val >= 0))
        return(static_cast<uint32_t>(_value.uint64Val));
      else
        return(0);

    case TypeString:
      return(_value.stringVal->toUInt());

    default:
      return(0);
  }
}

/*
 */

float Variant::toFloat() const
{
  switch(_type)
  {
    case TypeNull:
      return(0.0f);

    case TypeBool:
      return(_value.boolVal ? 1.0f : 0.0f);

    case TypeChar:
      return(static_cast<float>(_value.charVal));

    case TypeChar16:
      return(static_cast<float>(_value.char16Val));

    case TypeInt:
      return(static_cast<float>(_value.intVal));

    case TypeUInt:
      return(static_cast<float>(_value.uintVal));

    case TypeFloat:
      return(_value.floatVal);

    case TypeDouble:
      return(static_cast<float>(_value.doubleVal));

    case TypeString:
      return(_value.stringVal->toFloat());

    case TypeInt64:
    case TypeUInt64:
      return(0.0f); // disallow due to possible data loss

    default:
      return(0.0f);
  }
}

/*
 */

double Variant::toDouble() const
{
  switch(_type)
  {
    case TypeNull:
      return(0.0);

    case TypeBool:
      return(_value.boolVal ? 1.0 : 0.0);

    case TypeChar:
      return(static_cast<double>(_value.charVal));

    case TypeChar16:
      return(static_cast<double>(_value.char16Val));

    case TypeInt:
      return(static_cast<double>(_value.intVal));

    case TypeUInt:
      return(static_cast<double>(_value.uintVal));

    case TypeFloat:
      return(static_cast<double>(_value.floatVal));

    case TypeDouble:
      return(_value.doubleVal);

    case TypeString:
      return(_value.stringVal->toDouble());

    case TypeInt64:
    case TypeUInt64:
      return(0.0); // disallow due to possible data loss

    default:
      return(0.0);
  }
}

/*
 */

int64_t Variant::toInt64() const
{
  switch(_type)
  {
    case TypeNull:
      return(INT64_CONST(0));

    case TypeBool:
      return(_value.boolVal ? INT64_CONST(1) : INT64_CONST(0));

    case TypeChar:
      return(static_cast<int64_t>(_value.charVal));

    case TypeChar16:
      return(static_cast<int64_t>(_value.char16Val));

    case TypeInt:
      return(static_cast<int64_t>(_value.intVal));

    case TypeUInt:
      if(_value.uintVal >= 0)
        return(static_cast<int64_t>(_value.uintVal));
      else
        return(INT64_CONST(0));

    case TypeFloat:
      return(static_cast<int64_t>(_value.floatVal));

    case TypeDouble:
      return(static_cast<int64_t>(_value.doubleVal));

    case TypeInt64:
      return(_value.int64Val);

    case TypeUInt64:
      if(_value.uint64Val <= static_cast<uint64_t>(INT64_MAX))
        return(static_cast<int64_t>(_value.uint64Val));
      else
        return(INT64_CONST(0));

    case TypeString:
      return(_value.stringVal->toInt64());

    default:
      return(INT64_CONST(0));
  }
}

/*
 */

uint64_t Variant::toUInt64() const
{
  switch(_type)
  {
    case TypeNull:
      return(UINT64_CONST(0));

    case TypeBool:
      return(_value.boolVal ? UINT64_CONST(1) : UINT64_CONST(0));

    case TypeChar:
      return(static_cast<uint64_t>(_value.charVal));

    case TypeChar16:
      return(static_cast<uint64_t>(_value.char16Val));

    case TypeInt:
      if(_value.intVal >= 0)
        return(static_cast<uint64_t>(_value.intVal));
      else
        return(UINT64_CONST(0));

    case TypeUInt:
      return(static_cast<uint64_t>(_value.uintVal));

    case TypeFloat:
      if(_value.floatVal < 0.0f)
        return(UINT64_CONST(0));
      else
        return(static_cast<uint64_t>(_value.floatVal));

    case TypeDouble:
      if(_value.doubleVal < 0.0)
        return(UINT64_CONST(0));
      else
        return(static_cast<uint64_t>(_value.doubleVal));

    case TypeInt64:
      if(_value.int64Val >= 0)
        return(static_cast<uint64_t>(_value.int64Val));
      else
        return(UINT64_CONST(0));

    case TypeUInt64:
      return(_value.uint64Val);

    case TypeString:
      return(_value.stringVal->toUInt64());

    default:
      return(UINT64_CONST(0));
  }
}

/*
 */

Blob Variant::toBlob() const
{
  return(_type == TypeBlob ? Blob(*(_value.blobVal)) : Blob::null);
}

/*
 */

void * Variant::toOpaque() const
{
  return(_type == TypeOpaque ? _value.opaqueVal : NULL);
}

/*
 */

String Variant::toString() const
{
  switch(_type)
  {
    case TypeBool:
    {
      String s;
      s += _value.boolVal;
      return(s);
    }

    case TypeChar:
    {
      String s;
      s += _value.charVal;
      return(s);
    }

    case TypeChar16:
       return(String(&_value.char16Val, 0, 1));

    case TypeInt:
    {
      String s;
      s += _value.intVal;
      return(s);
    }

    case TypeUInt:
    {
      String s;
      s += _value.uintVal;
      return(s);
    }

    case TypeFloat:
    {
      String s;
      s += _value.floatVal;
      return(s);
    }

    case TypeDouble:
    {
      String s;
      s += _value.doubleVal;
      return(s);
    }

    case TypeInt64:
    {
      String s;
      s += _value.int64Val;
      return(s);
    }

    case TypeUInt64:
    {
      String s;
      s += _value.uint64Val;
      return(s);
    }

    case TypeString:
    {
      String s = *(_value.stringVal);
      return(s);
    }

    case TypeBlob:
      return(String::null);

    case TypeNull:
    default:
      return(String::null);
  }
}

/*
 */

uint_t Variant::length() const
{
  switch(_type)
  {
    case TypeList:
      return(_value.listVal->size());

    case TypeMap:
      return(_value.mapVal->size());

    default:
      return(0);
  }
}

/*
 */

void Variant::clear()
{
  switch(_type)
  {
    case TypeBool:
      _value.boolVal = false;
      break;

    case TypeChar:
      _value.charVal = NUL;
      break;

    case TypeChar16:
      _value.char16Val = 0;
      break;

    case TypeInt:
      _value.intVal = 0;
      break;

    case TypeUInt:
      _value.uintVal = 0;
      break;

    case TypeFloat:
      _value.floatVal = 0.0f;
      break;

    case TypeDouble:
      _value.doubleVal = 0.0;
      break;

    case TypeInt64:
      _value.int64Val = INT64_CONST(0);
      break;

    case TypeUInt64:
      _value.uint64Val = UINT64_CONST(0);
      break;

    case TypeString:
      _value.stringVal->clear();
      break;

    case TypeBlob:
      _value.blobVal->clear();
      break;

    case TypeOpaque:
      _value.opaqueVal = NULL;
      break;

    case TypeList:
      _value.listVal->clear();
      break;

    case TypeMap:
      _value.mapVal->clear();
      break;

    default:
      break;
  }
}

/*
 */

void Variant::setNull()
{
  _deleteValue();
  _type = TypeNull;
  _initValue();
}

/*
 */

Variant& Variant::operator=(bool val)
{
  _deleteValue();
  _type = TypeBool;
  _value.boolVal = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(char val)
{
  _deleteValue();
  _type = TypeChar;
  _value.charVal = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(char16_t val)
{
  _deleteValue();
  _type = TypeChar16;
  _value.char16Val = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(int32_t val)
{
  _deleteValue();
  _type = TypeInt;
  _value.intVal = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(uint32_t val)
{
  _deleteValue();
  _type = TypeUInt;
  _value.uintVal = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(float val)
{
  _deleteValue();
  _type = TypeFloat;
  _value.floatVal = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(const double &val)
{
  _deleteValue();
  _type = TypeDouble;
  _value.doubleVal = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(const int64_t &val)
{
  _deleteValue();
  _type = TypeInt64;
  _value.int64Val = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(const uint64_t &val)
{
  _deleteValue();
  _type = TypeUInt64;
  _value.uint64Val = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(const String& val)
{
  _deleteValue();
  _type = TypeString;
  _initValue();

  String& s = *(_value.stringVal);
  s = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(const Blob &val)
{
  _deleteValue();
  _type = TypeBlob;
  _initValue();

  Blob& b = *(_value.blobVal);
  b = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(void *val)
{
  _deleteValue();
  _type = TypeOpaque;
  _value.opaqueVal = val;

  return(*this);
}

/*
 */

Variant& Variant::operator=(const Variant &other)
{
  _copy(other);
  return(*this);
}

/*
 */

void Variant::_copy(const Variant &other)
{
  if(&other != this)
  {
    _deleteValue();
    _type = other._type;

    switch(_type)
    {
      case TypeString:
        _value.stringVal = new String(*(other._value.stringVal));
        break;

      case TypeBlob:
        _value.blobVal = new Blob(*(other._value.blobVal));
        break;

      case TypeList:
        _value.listVal = new VariantList();
        *(_value.listVal) = *(other._value.listVal);
        break;

      case TypeMap:
        _value.mapVal = new VariantMap();
        *(_value.mapVal) = *(other._value.mapVal);
        break;

      default:
        std::memcpy(&_value, &other._value, sizeof(_value));
        break;
    }
  }
}

/*
 */

Variant& Variant::operator[](const String& name)
{
  if(_type != TypeMap)
  {
    _deleteValue();
    _type = TypeMap;
    _initValue();
  }

  VariantMap *map = _value.mapVal;
  VariantMap::iterator iter = map->find(name);

  if(iter == map->end())
    return((*map)[name] = Variant());
  else
    return(iter->second);
}

/*
 */

const Variant &Variant::get(const String& name) const
{
  if(_type != TypeMap)
    return(Variant::null);

  VariantMap *map = _value.mapVal;
  VariantMap::iterator iter = map->find(name);

  if(iter == map->end())
    return(Variant::null);
  else
    return(iter->second);
}

/*
 */

Variant& Variant::operator[](int index)
{
  if(index < 0)
    throw OutOfBoundsException();

  size_t ind = static_cast<size_t>(index);

  if(_type != TypeList)
  {
    _deleteValue();
    _type = TypeList;
    _initValue();
  }

  VariantList *list = _value.listVal;
  size_t sz = list->size();

  if(sz <= ind) {
    for(size_t i = sz; i <= ind; ++i)
      list->push_back(Variant());
  }

  return((*list)[ind]);
}

/*
 */

const Variant &Variant::get(int index) const
{
  if(index < 0)
    throw OutOfBoundsException();

  size_t ind = static_cast<size_t>(index);

  if(_type != TypeList)
    return(Variant::null);

  VariantList *list = _value.listVal;

  if(ind >= list->size())
    return(Variant::null);

  return((*list)[ind]);
}

/*
 */

bool Variant::operator==(const Variant &other) const
{
  if(_type != other._type)
    return(false);

  switch(_type)
  {
    case TypeNull:
      return(true); // null == null

    case TypeBool:
      return(_value.boolVal == other._value.boolVal);

    case TypeChar:
      return(_value.charVal == other._value.charVal);

    case TypeChar16:
      return(_value.char16Val == other._value.char16Val);

    case TypeInt:
      return(_value.intVal == other._value.intVal);

    case TypeUInt:
      return(_value.uintVal == other._value.uintVal);

    case TypeFloat:
      return(_value.floatVal == other._value.floatVal);

    case TypeDouble:
      return(_value.doubleVal == other._value.doubleVal);

    case TypeInt64:
      return(_value.int64Val == other._value.int64Val);

    case TypeUInt64:
      return(_value.uint64Val == other._value.uint64Val);

    case TypeString:
    {
      const String &s1 = *(_value.stringVal);
      const String &s2 = *(other._value.stringVal);
      return(s1 == s2);
    }

    case TypeBlob:
    {
      const Blob &b1 = *(_value.blobVal);
      const Blob &b2 = *(other._value.blobVal);
      return(b1 == b2);
    }

    case TypeOpaque:
      return(_value.opaqueVal == other._value.opaqueVal);

    case TypeList:
    {
      const VariantList &l1 = *(_value.listVal);
      const VariantList &l2 = *(other._value.listVal);
      return(l1 == l2);
    }

    case TypeMap:
    {
      const VariantMap &m1 = *(_value.mapVal);
      const VariantMap &m2  = *(other._value.mapVal);
      return(m1 == m2);
    }

    default:
      return(false);
  }
}

/*
 */

bool Variant::getKeys(StringVec& vec) const
{
  vec.clear();

  if(_type != TypeMap)
    return(false);

  VariantMap *map = _value.mapVal;

  for(VariantMap::const_iterator iter = map->begin();
      iter != map->end();
      ++iter)
  {
    vec << iter->first;
  }

  return(true);
}

/*
 */

bool Variant::hasKey(const String& key) const
{
  if(_type != TypeMap)
    return(false);

  VariantMap *map = _value.mapVal;

  VariantMap::const_iterator iter = map->find(key);
  return(iter != map->end());
}

/*
 */

void Variant::write(std::ostream &stream) const
{
  switch(_type)
  {
    case TypeNull:
      stream << "(null)";
      break;

    case TypeBool:
      stream << (_value.boolVal ? "true" : "false");
      break;

    case TypeChar:
      stream << '\'' << _value.charVal << '\'';
      break;

    case TypeChar16:
    {
      char buf[16];
      snprintf(buf, sizeof(buf), "'\\u%04X'", _value.char16Val);
      stream << buf;
      break;
    }

    case TypeInt:
      stream << _value.intVal;
      break;

    case TypeUInt:
      stream << _value.uintVal;
      break;

    case TypeFloat:
      stream << _value.floatVal;
      break;

    case TypeDouble:
      stream << _value.doubleVal;
      break;

    case TypeInt64:
      stream << _value.int64Val;
      break;

    case TypeUInt64:
      stream << _value.uint64Val;
      break;

    case TypeString:
      stream << '\"' << *(_value.stringVal) << '\"';
      break;

    case TypeBlob:
      stream << Hex::encode(*(_value.blobVal));
      break;

    case TypeOpaque:
      stream << std::hex << _value.opaqueVal << std::dec;
      break;

    case TypeList:
    {
      VariantList *list = _value.listVal;
      stream << "( ";
      bool first = true;

      for(VariantList::iterator iter = list->begin();
          iter != list->end();
          ++iter)
      {
        Variant &val = *iter;

        if(! first)
          stream << ", ";
        else
          first = false;

        stream << val;
      }
      stream << " )";
      break;
    }

    case TypeMap:
    {
      VariantMap *map = _value.mapVal;
      stream << "{ ";
      bool first = true;

      for(VariantMap::iterator iter = map->begin();
          iter != map->end();
          ++iter)
      {
        const String &name = iter->first;
        const Variant &val = iter->second;

        if(! first)
          stream << ", ";
        else
          first = false;

        stream << name << " = " << val;
      }

      stream << " }";
      break;
    }
  }
}

/*
 */

void Variant::_deleteValue()
{
  switch(_type)
  {
    case TypeString:
      delete _value.stringVal;
      break;

    case TypeBlob:
      delete _value.blobVal;
      break;

    case TypeList:
      delete _value.listVal;
      break;

    case TypeMap:
      delete _value.mapVal;
      break;

    default:
      break;
  }

  _type = TypeNull;
}

/*
 */

void Variant::_initValue()
{
  switch(_type)
  {
    case TypeBool:
      _value.boolVal = false;
      break;

    case TypeChar:
      _value.charVal = NUL;
      break;

    case TypeChar16:
      _value.char16Val = 0;
      break;

    case TypeInt:
      _value.intVal = 0;
      break;

    case TypeUInt:
      _value.uintVal = 0;
      break;

    case TypeFloat:
      _value.floatVal = 0.0f;
      break;

    case TypeDouble:
      _value.doubleVal = 0.0;
      break;

    case TypeString:
      _value.stringVal = new String();
      break;

    case TypeBlob:
      _value.blobVal = new Blob();
      break;

    case TypeList:
      _value.listVal = new VariantList();
      break;

    case TypeMap:
      _value.mapVal = new VariantMap();
      break;

    default:
      break;
  }
}


} // namespace ccxx
