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

#ifndef __ccxx_Permissions_hxx
#define __ccxx_Permissions_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * File permissions. Permissions consist of <i>read</i>, <i>write</i>,
 * and <i>execute</i> access for each of <i>user</i> (owner), <i>group</i>,
 * and <i>others</i> (everyone), and are represented efficiently as a mask.
 *
 * @author Mark Lindner.
 */
class COMMONCPP_API Permissions
{
 public:

  /**
   * Construct a new Permissions object with the given mask.
   *
   * @param mask The permissions mask.
   */
  Permissions(uint_t mask = 0);

  /** Copy constructor. */
  Permissions(const Permissions& other);

  /** Destructor. */
  ~Permissions();

  /** User read mask. */
  static const int USER_READ;

  /** User write mask. */
  static const int USER_WRITE;

  /** User read and write mask. */
  static const int USER_READ_WRITE;

  /** User execute mask. */
  static const int USER_EXECUTE;

  /** User (all permissions) mask. */
  static const int USER_ALL;

  /** Group read mask. */
  static const int GROUP_READ;

  /** Group write mask. */
  static const int GROUP_WRITE;

  /** Group read and write mask. */
  static const int GROUP_READ_WRITE;

  /** Group execute mask. */
  static const int GROUP_EXECUTE;

  /** Group (all permissions) mask. */
  static const int GROUP_ALL;

  /** Others read mask. */
  static const int OTHERS_READ;

  /** Others write mask. */
  static const int OTHERS_WRITE;

  /** Others read and write mask. */
  static const int OTHERS_READ_WRITE;

  /** Others execute mask. */
  static const int OTHERS_EXECUTE;

  /** Others (all permissions) mask. */
  static const int OTHERS_ALL;

  /** All read and write mask. */
  static const int ALL_READ_WRITE;

  /** Complete permissions mask. */
  static const int ALLBITS;

  // user

  /** Test for <i>user read</i> access. */
  inline bool canUserRead() const
  { return((_mask & USER_READ) != 0); }

  /** Enable <i>user read</i> access. */
  inline void setUserRead()
  { _mask |= USER_READ; }

  /** Disable <i>user read</i> access. */
  inline void clearUserRead()
  { _mask &= ~USER_READ; }

  /** Test for <i>user write</i> access. */
  inline bool canUserWrite() const
  { return((_mask & USER_WRITE) != 0); }

  /** Enable <i>user write</i> access. */
  inline void setUserWrite()
  { _mask |= USER_WRITE; }

  /** Disable <i>user write</i> access. */
  inline void clearUserWrite()
  { _mask &= ~USER_WRITE; }

  /** Test for <i>user execute</i> access. */
  inline bool canUserExecute() const
  { return((_mask & USER_EXECUTE) != 0); }

  /** Enable <i>user execute</i> access. */
  inline void setUserExecute()
  { _mask |= USER_EXECUTE; }

  /** Disable <i>user execute</i> access. */
  inline void clearUserExecute()
  { _mask &= ~USER_EXECUTE; }

  // group

  /** Test for <i>group read</i> access. */
  inline bool canGroupRead() const
  { return((_mask & GROUP_READ) != 0); }

  /** Enable <i>group read</i> access. */
  inline void setGroupRead()
  { _mask |= GROUP_READ; }

  /** Disable <i>group read</i> access. */
  inline void clearGroupRead()
  { _mask &= ~GROUP_READ; }

  /** Test for <i>group write</i> access. */
  inline bool canGroupWrite() const
  { return((_mask & GROUP_WRITE) != 0); }

  /** Enable <i>group write</i> access. */
  inline void setGroupWrite()
  { _mask |= GROUP_WRITE; }

  /** Disable <i>group write</i> access. */
  inline void clearGroupWrite()
  { _mask &= ~GROUP_WRITE; }

  /** Test for <i>group execute</i> access. */
  inline bool canGroupExecute() const
  { return((_mask & GROUP_EXECUTE) != 0); }

  /** Enable <i>group execute</i> access. */
  inline void setGroupExecute()
  { _mask |= GROUP_EXECUTE; }

  /** Disable <i>group execute</i> access. */
  inline void clearGroupExecute()
  { _mask &= ~GROUP_EXECUTE; }

  // others

  /** Test for <i>others read</i> access. */
  inline bool canOthersRead() const
  { return((_mask & OTHERS_READ) != 0); }

  /** Enable <i>others read</i> access. */
  inline void setOthersRead()
  { _mask |= OTHERS_READ; }

  /** Disable <i>others read</i> access. */
  inline void clearOthersRead()
  { _mask &= ~OTHERS_READ; }

  /** Test for <i>others write</i> access. */
  inline bool canOthersWrite() const
  { return((_mask & OTHERS_WRITE) != 0); }

  /** Enable <i>others write</i> access. */
  inline void setOthersWrite()
  { _mask |= OTHERS_WRITE; }

  /** Disable <i>others write</i> access. */
  inline void clearOthersWrite()
  { _mask &= ~OTHERS_WRITE; }

  /** Test for <i>others execute</i> access. */
  inline bool canOthersExecute() const
  { return((_mask & OTHERS_EXECUTE) != 0); }

  /** Enable <i>others execute</i> access. */
  inline void setOthersExecute()
  { _mask |= OTHERS_EXECUTE; }

  /** Disable <i>others execute</i> access. */
  inline void clearOthersExecute()
  { _mask &= ~OTHERS_EXECUTE; }

  /** Disable all access. */
  inline void clearAll()
  { _mask = 0; }

  // operators

  /** Assignment operator. */
  Permissions& operator=(const Permissions& other);

  /** Get the permissions mask. */
  inline uint_t getMask() const
  { return(_mask); }

  /** Cast operator. Returns the permissions mask. */
  inline operator uint_t()
  { return(_mask); }

  /** Get a String representation of the permissions mask. */
  String toString() const;

  inline uint_t operator=(uint_t rhs)
  { return((_mask = rhs)); }

  inline uint_t operator|=(uint_t rhs)
  { return(_mask |= (rhs & ALLBITS)); }

  inline uint_t operator&=(uint_t rhs)
  { return(_mask &= (rhs & ALLBITS)); }

  /** Default permissions for files: read & write for user, read for group. */
  static const Permissions defaultFilePerms;

  /**
   * Default permissions for directories: read, write & execute for user,
   * read & execute for group.
   */
  static const Permissions defaultDirPerms;

 private:

  uint_t _mask;
};

inline uint_t operator|(const Permissions& perm, const uint_t rhs)
{ return(perm.getMask() | (rhs & Permissions::ALLBITS)); }

inline uint_t operator&(const Permissions& perm, const uint_t rhs)
{ return(perm.getMask() & (rhs & Permissions::ALLBITS)); }

} // namespace ccxx

#endif // __ccxx_Permissions_hxx
