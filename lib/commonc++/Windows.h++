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

#ifndef __ccxx_Windows_hxx
#define __ccxx_Windows_hxx

#include <commonc++/Common.h++>
#include <commonc++/AllocationMap.h++>
#include <commonc++/Permissions.h++>

#include <aclapi.h>

#define ACL_SIZE 2048
#define SID_SIZE 1024
#define DOM_SIZE 1024

namespace ccxx {

class COMMONCPP_API WinPerms
{
 public:

  WinPerms()
    : heap(NULL),
      pdesc(NULL),
      user(NULL),
      group(NULL),
      dacl(NULL)
  {
    heap = ::HeapCreate(0, 0, 0);
  }

  ~WinPerms()
  {
    if(pdesc != NULL)
      ::LocalFree(pdesc);

    if(dacl != NULL)
      ::LocalFree(dacl);

    if(heap != NULL)
      ::HeapDestroy(heap);
  }

  HANDLE heap;
  PSECURITY_DESCRIPTOR pdesc;
  PSID user;
  PSID group;
  PACL dacl;
};

class COMMONCPP_API Windows
{
 public:

  static bool decodePermissions(WinPerms& wperm, Permissions& perm);
  static bool encodePermissions(const Permissions& perm, WinPerms& wperm);

  static String decodePathName(const String& path);

  static time_ms_t decodeFileTime(const FILETIME ft);
  static uint64_t decodeFileTimeAsDelta(const FILETIME ft);

  static void initPerms();
  static void cleanupPerms();

  static void initTLS();
  static void shutdownTLS();
  static int allocTLS();
  static void freeTLS(uint_t index);
  static void setTLS(uint_t index, void* value, void (*destructor)(void*));
  static void* getTLS(uint_t index);
  static void cleanupTLS();

  static void setCurrentThread(void* thread);
  static void* getCurrentThread();

  static uint_t getTLSMax();

 private:

  struct TLSSlot
  {
    TLSSlot()
      : value(NULL), destructor(NULL)
    { }

    void reset(void* new_value = NULL, void (*new_destructor)(void*) = NULL)
    {
      if(value && destructor)
        destructor(value);

      value = new_value;
      destructor = new_destructor;
    }

    void *value;
    void (*destructor)(void*);
  };

  static bool findPrimaryGroup(LPTSTR name);

  static const size_t _tlsMax;
  static AllocationMap* _tlsMap;
  static DWORD _tlsKey;
  static DWORD _threadKey;
  static CRITICAL_SECTION _tlsLock;
  static PSID _worldSID;
};

} // namespace ccxx

#endif // __ccxx_Windows_hxx
