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

#ifndef __ccxx_Version_hxx
#define __ccxx_Version_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A class representing an interface version number. The version number
 * consists of three components:
 * <ul>
 * <li><i>current:</i> The current interface version.
 * <li><i>revision:</i> The implementation number of current interface.
 * <li><i>age:</i> The number of interface versions, beginning with the
 * current interface and working backwards, that the current version is
 * API-compatible with.
 * </ul>
 * These three components are <i>not</i> to be interpreted as major version,
 * minor version, and build number.
 * <p>
 * A version begins at 0:0:0. The version number changes according to the
 * following rules:
 * <ul>
 * <li>If the implementation changes in any way, <i>revision</i> is
 * incremented.
 * <li>If any interfaces are added, removed, or changed, <i>current</i> is
 * incremented and <i>revision</i> is set to 0.
 * <li>If any new interfaces are added, <i>age</i> is incremented.
 * <li>If any existing interfaces are removed, <i>age</i> is set to 0.
 * </ul>
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Version
{
 public:

  /**
   * Construct a new Version from the specified string representation.
   *
   * @param version The string representation of a version, in the form
   * <i>C:R:A</i> or <i>C-A.A.R</i>. For example, <i>9:0:6</i> and
   * <i>3.6.0</i> are equivalent.
   */
  Version(const String& version);

  /**
   * Construct a new Version for the given components.
   *
   * @param current The <i>current</i> component.
   * @param revision The <i>revision</i> component.
   * @param age The <i>age</i> component, which must always be less than
   * or equal to <i>current</i>; if an invalid age is specified, the value
   * of <i>current</i> is used for <i>age</i>.
   */
  Version(uint_t current = 0, uint_t revision = 0, uint_t age = 0);

  /** Copy constructor. */
  Version(const Version& other);

  /** Destructor. */
  virtual ~Version();

  /**
   * Determine if an interface with this version satisfies a request for
   * an interface with a specific requested version (i.e., if it is
   * API-compatible with the requested interface).
   *
   * @param requested The requested version.
   * @return <b>true</b> if an interface with this version can be used by
   * a caller requesting the <i>requested</i> version, <b>false</b>
   * otherwise.
   */
  bool satisfies(const Version& requested) const;

  /**
   * Determine if a requested interface with this version is satisfied by
   * a specific available version (i.e., if the available interface is
   * API-compatible with the requested version of the interface).
   *
   * @param available The version that is available.
   * @return <b>true</b> if an interface with the available version can be
   * used by a caller requesting this version, <b>false</b> otherwise.
   */
  bool satisfiedBy(const Version& available) const;

  /** Get the <i>current</i> component of the version. */
  inline uint_t getCurrent() const
  { return(_current); }

  /** Get the <i>revision</i> component of the version. */
  inline uint_t getRevision() const
  { return(_revision); }

  /** Get the <i>age</i> component of the version. */
  inline uint_t getAge() const
  { return(_age); }

  /** Adjust the version to reflect the addition of new interfaces. */
  void interfacesAdded();

  /** Adjust the version to reflect the removal of existing interfaces. */
  void interfacesRemoved();

  /**
   * Adjust the version to reflect non-backward-compatible changes to
   * the interfaces.
   */
  void interfacesChanged();

  /** Adjust the version to reflect internal implementation changes. */
  void implementationChanged();

  /** Get a String representation of the version, in <i>C-A.A.R</i> form. */
  String toString() const;

  /** Get a String representation of the version, in <i>C:R:A</i> form. */
  String toCRAString() const;

  /** Parse the verison from a String. */
  void fromString(const String& version);

  /** Equality operator. */
  bool operator==(const Version& other) const;

  /** Inequality operator. */
  inline bool operator!=(const Version& other) const
  { return(! operator==(other)); }

  /** Determine if this Version is older than another version. */
  bool operator<(const Version& other) const;

  /** Determine if this Version is older than or equal to another version. */
  inline bool operator<=(const Version& other) const
  { return(operator<(other) || operator==(other)); }

  /** Determine if this Version is newer than another version. */
  bool operator>(const Version& other) const;

  /** Determine if this Version is newer than or equal to another version. */
  inline bool operator>=(const Version& other) const
  { return(operator>(other) || operator==(other)); }

  /** Assignment operator. */
  Version& operator=(const Version& other);

 private:

  uint_t _current;
  uint_t _revision;
  uint_t _age;
  mutable String _str;
  mutable String _vstr;
};

inline std::ostream& operator<<(std::ostream& stream, const Version& v)
{
  CString cstr_v = v.toString().toUTF8();
  return(stream << cstr_v.data());
}

} // namespace ccxx

#endif // __ccxx_Version_hxx
