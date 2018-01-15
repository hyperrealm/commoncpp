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

#ifndef __ccxx_EnumTraits_hxx
#define __ccxx_EnumTraits_hxx

#include <commonc++/Common.h++>

#include <cstring>
#include <utility>
#include <vector>

namespace ccxx {

/**
 * A class of functions that describe traits of an enumeration. Used
 * in conjuction with the enum_cast() template functions. Template
 * specializations should be provided for specific enumeration types.
 *
 * @author Mark Lindner
 */
template<typename E> class EnumTraits
{
 public:

  /** Get the minimum enumeration value. */
  static E min();

  /** Get the maximum enumeration value. */
  static E max();

  /** Get the default enumeration value. */
  static E defval();
};

/**
 * Safe enumeration cast. Casts a value of type T to an enumeration of type
 * E, ensuring that the value returned is a valid one for E. It is assumed
 * that the values in the enumeration E are sequential integers, that is,
 * that there are no "gaps" in the range.
 *
 * @param val The value to cast.
 * @return The value cast to the enumeration type E. If the value is
 * outside the allowed range for E, the default value for E is
 * returned, as specified by the specialization of EnumTraits::defVal().
 */
template<typename T, typename E> E enum_cast(T val)
{
  if((val >= static_cast<T>(EnumTraits<E>::min()))
     && (val <= static_cast<T>(EnumTraits<E>::max())))
    return static_cast<E>(val);
  else
    return EnumTraits<E>::defval();
}

/**
 * Safe enumeration cast. Casts a value of type T to an enumeration of type
 * E, ensuring that the value returned is a valid one for E. It is assumed
 * that the values in the enumeration E are sequential integers, that is,
 * that there are no "gaps" in the range.
 *
 * @param val The value to cast.
 * @param defval The default enumeration value to return, if the passed-in
 * value is not a valid one for the enumeration.
 * @return The value cast to the enumeration type E. If the value is
 * outside the allowed range for E, defVal is returned.
 */
template<typename T, typename E> E enum_cast(T val, E defval)
{
  if((val >= static_cast<T>(EnumTraits<E>::min()))
     && (val <= static_cast<T>(EnumTraits<E>::max())))
    return static_cast<E>(val);
  else
    return defval;
}

/**
 * A bidirectional mapping between enum values and static string constants.
 * An EnumMap can be constructed as follows:
 *
 * <pre>
 * enum FileType { TypeFile, TypeDirectory, TypeSymLink };
 *
 * EnumMap<FileType> fileTypeMap;
 * fileTypeMap.add(TypeFile,      "file")
 *            .add(TypeDirectory, "directory")
 *            .add(TypeSymLink,   "symlink");
 * </pre>
 *
 * @author Mark Lindner
 */
template<typename E> class EnumMap
  : private std::vector<std::pair<E, const char *> >
{
 public:

  /** Constructor. */
  EnumMap() { }

  /** Destructor. */
  ~EnumMap() { }

  /** Add a mapping. */
  EnumMap& add(E value, const char* str)
  {
    this->push_back(make_pair(value, str));
    return(*this);
  }

  /**
   * Map a string to its enum value.
   *
   * @param str The string to map.
   * @return The corresponding enumeration value, or EnumTraits<E>::defval()
   * if there is no mapping for the given string.
   */
  E valueForString(const char* str) const
  {
    for(typename Entries::const_iterator iter = this->begin();
        iter != this->end();
        ++iter)
    {
      if(std::strcmp(str, this->second))
        return(this->first);
    }

    return(EnumTraits<E>::defval());
  }

  /**
   * Map an enum value to its string.
   *
   * @param value The enum value to map.
   * @return The corresponding string, nor NULL if there is no mapping for
   * the given enum value.
   */
  const char* stringForValue(E value) const
  {
    for(typename Entries::const_iterator iter = this->begin();
        iter != this->end();
        ++iter)
    {
      if(value == iter->first)
        return(this->second);
    }

    return(NULL);
  }

 private:

  typedef std::vector<std::pair<E, const char*> > Entries;
};

} // namespace ccxx

#endif // __ccxx_EnumTraits_hxx
