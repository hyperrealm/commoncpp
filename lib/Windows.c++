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

#include "commonc++/Windows.h++"
#include "commonc++/AllocationMap.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_WINDOWS

#include <Tchar.h>
#include <Lmcons.h>

#include <iostream>

#ifndef SECURITY_MAX_SID_SIZE
#define SECURITY_MAX_SID_SIZE 68
#endif

#define CCXX_TLS_MAX 256

/*
 */

namespace ccxx {

/*
 */

const size_t Windows::_tlsMax = CCXX_TLS_MAX;
AllocationMap *Windows::_tlsMap = NULL;
DWORD Windows::_tlsKey;
DWORD Windows::_threadKey;
CRITICAL_SECTION Windows::_tlsLock;
PSID Windows::_worldSID = NULL;

/*
 */

void Windows::initTLS()
{
  ::InitializeCriticalSection(&_tlsLock);

  _tlsMap = new AllocationMap(_tlsMax);

  _tlsKey = ::TlsAlloc();
  if(_tlsKey == TLS_OUT_OF_INDEXES)
  {
    std::cerr << "Unable to allocate TLS!" << std::endl;
    abort();
    return;
  }

  _threadKey = ::TlsAlloc();
  if(_threadKey == TLS_OUT_OF_INDEXES)
  {
    std::cerr << "Unable to allocate TLS!" << std::endl;
    abort();
    return;
  }
}

/*
 */

void Windows::shutdownTLS()
{
  if(_tlsKey != TLS_OUT_OF_INDEXES)
    ::TlsFree(_tlsKey);

  delete _tlsMap;

  if(_threadKey != TLS_OUT_OF_INDEXES)
    ::TlsFree(_threadKey);

  ::DeleteCriticalSection(&_tlsLock);
}

/*
 */

int Windows::allocTLS()
{
  int index = -1;

  ::EnterCriticalSection(&_tlsLock);
  index = _tlsMap->reserve();
  ::LeaveCriticalSection(&_tlsLock);

  return(index);
}

/*
 */

void Windows::freeTLS(uint_t index)
{
  ::EnterCriticalSection(&_tlsLock);

  if(_tlsMap->release(index))
  {
    TLSSlot *slots = (TLSSlot *)::TlsGetValue(_tlsKey);

    if(slots)
    {
      TLSSlot &slot = slots[index];
      slot.reset();
    }
  }

  ::LeaveCriticalSection(&_tlsLock);
}

/*
 */

void *Windows::getTLS(uint_t index)
{
  TLSSlot *slots = (TLSSlot *)::TlsGetValue(_tlsKey);

  if((index < _tlsMax) && slots)
    return(slots[index].value);
  else
    return(NULL);
}

/*
 */

void Windows::setTLS(uint_t index, void *value, void (*destructor)(void *))
{
  if(index < _tlsMax)
  {
    TLSSlot *slots = (TLSSlot *)::TlsGetValue(_tlsKey);

    if(! slots)
    {
      slots = new TLSSlot[_tlsMax];
      ::TlsSetValue(_tlsKey, slots);
    }

    slots[index].reset(value, destructor);
  }
}

/*
 */

void Windows::cleanupTLS()
{
  TLSSlot *slots = (TLSSlot *)::TlsGetValue(_tlsKey);

  if(slots)
  {
    TLSSlot *slot = slots;
    for(size_t index = 0; index < _tlsMax; ++index, ++slot)
      slot->reset();

    delete[] slots;
    ::TlsSetValue(_tlsKey, NULL);
  }
}

/*
 */

void Windows::setCurrentThread(void *thread)
{
  ::TlsSetValue(_threadKey, thread);
}

/*
 */

void *Windows::getCurrentThread()
{
  return(::TlsGetValue(_threadKey));
}

/*
 */

uint_t Windows::getTLSMax()
{
  return(_tlsMax);
}

/*
 */

void Windows::initPerms()
{
  SID_IDENTIFIER_AUTHORITY sidAuth = SECURITY_WORLD_SID_AUTHORITY;
  ::AllocateAndInitializeSid(&sidAuth, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0,
                             0, 0, &_worldSID);
}

/*
 */

void Windows::cleanupPerms()
{
  if(_worldSID != NULL)
  {
    ::FreeSid(_worldSID);
    _worldSID = NULL;
  }
}

/*
 */

bool Windows::decodePermissions(WinPerms &wperm, Permissions &perm)
{
  TRUSTEE ident = { NULL, NO_MULTIPLE_TRUSTEE, TRUSTEE_IS_SID };
  ACCESS_MASK mask;

  // user

  ident.TrusteeType = TRUSTEE_IS_USER;
  ident.ptstrName = (LPTSTR)wperm.user;

  if(::GetEffectiveRightsFromAcl(wperm.dacl, &ident, &mask) != ERROR_SUCCESS)
    return(false);

  (mask & FILE_READ_DATA) ? perm.setUserRead() : perm.clearUserRead();
  (mask & FILE_WRITE_DATA) ? perm.setUserWrite() : perm.clearUserWrite();
  (mask & FILE_EXECUTE) ? perm.setUserExecute() : perm.clearUserExecute();

  // group

  ident.TrusteeType = TRUSTEE_IS_GROUP;
  ident.ptstrName = (LPTSTR)wperm.group;

  if(::GetEffectiveRightsFromAcl(wperm.dacl, &ident, &mask) != ERROR_SUCCESS)
    return(false);

  (mask & FILE_READ_DATA) ? perm.setGroupRead() : perm.clearGroupRead();
  (mask & FILE_WRITE_DATA) ? perm.setGroupWrite() : perm.clearGroupWrite();
  (mask & FILE_EXECUTE) ? perm.setGroupExecute() : perm.clearGroupExecute();

  // world

  ident.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
  ident.ptstrName = (LPTSTR)_worldSID;

  if(::GetEffectiveRightsFromAcl(wperm.dacl, &ident, &mask) != ERROR_SUCCESS)
    return(false);

  (mask & FILE_READ_DATA) ? perm.setOthersRead() : perm.clearOthersRead();
  (mask & FILE_WRITE_DATA) ? perm.setOthersWrite() : perm.clearOthersWrite();
  (mask & FILE_EXECUTE) ? perm.setOthersExecute() : perm.clearOthersExecute();

  return(true);
}

/*
 */

bool Windows::encodePermissions(const Permissions& perm, WinPerms &wperm)
{
  if(wperm.pdesc)
    ::LocalFree(wperm.pdesc);

  wperm.pdesc = NULL;
  return(true);  // disabled for now

  static uint_t masks[9] = { Permissions::USER_READ,
                             Permissions::USER_WRITE,
                             Permissions::USER_EXECUTE,
                             Permissions::GROUP_READ,
                             Permissions::GROUP_WRITE,
                             Permissions::GROUP_EXECUTE,
                             Permissions::OTHERS_READ,
                             Permissions::OTHERS_WRITE,
                             Permissions::OTHERS_EXECUTE };

  static int types[3] = { (FILE_READ_DATA | READ_CONTROL
                           | FILE_READ_ATTRIBUTES | FILE_READ_EA),

                          (FILE_WRITE_DATA | FILE_APPEND_DATA
                           | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA
                           | DELETE | FILE_DELETE_CHILD
                           | WRITE_DAC | WRITE_OWNER),

                          FILE_EXECUTE };

  bool status = false;
  int i;

  if(wperm.pdesc)
    ::LocalFree(wperm.pdesc);

  wperm.pdesc = (PSECURITY_DESCRIPTOR)::LocalAlloc(
    0, sizeof(SECURITY_DESCRIPTOR));

  if(! ::InitializeSecurityDescriptor(wperm.pdesc,
                                      SECURITY_DESCRIPTOR_REVISION))
    return(false);

  PACL pAcl = NULL;
  LPTSTR GrpNms[3] = { _T(""), _T(""), _T("Everyone") };
  PSID sids[3] = { NULL, NULL, NULL };
  SID_NAME_USE nameuse [] = { SidTypeUser, SidTypeGroup,
                              SidTypeWellKnownGroup };
  TCHAR RefDomain[3][DOM_SIZE];
  TCHAR user[UNLEN + 1];
  DWORD RefDomSize[3] = { DOM_SIZE, DOM_SIZE, DOM_SIZE };
  DWORD sidSize[3] = { SECURITY_MAX_SID_SIZE, SECURITY_MAX_SID_SIZE,
                        SECURITY_MAX_SID_SIZE };
  DWORD sz = UNLEN + 1;
  bool ok = false;

  if(! ::GetUserName(user, &sz))
    goto END;

  GrpNms[0] = user;

  // Get the user's primary group. 

  GrpNms[1] = (LPTSTR)::HeapAlloc(wperm.heap, 0, UNLEN + 1);
  if(GrpNms[1] == NULL)
    goto END;

  findPrimaryGroup(GrpNms[1]);

  for(i = 0; i < 3; i++)
  {
    sids[i] = ::HeapAlloc(wperm.heap, 0, SECURITY_MAX_SID_SIZE);
    if(sids[i] == NULL)
      goto END;

    if(! ::LookupAccountName(NULL, GrpNms[i], sids[i], &sidSize[i],
                             RefDomain[i], &RefDomSize[i], &nameuse[i]))
      goto END;
  }

  pAcl = (PACL)::HeapAlloc(wperm.heap, 0, ACL_SIZE);
  if(pAcl == NULL)
    goto END;

  if(! ::InitializeAcl(pAcl, ACL_SIZE, ACL_REVISION))
    goto END;

  // Add all the ACEs. Scan the permission bits, adding an allowed ACE when
  // the bit is set and a denied ACE when the bit is reset.
  for(i = 0; i < 9; i++)
  {
    bool f = (perm & masks[i]);

    if(f)
      ok = ::AddAccessAllowedAce(pAcl, ACL_REVISION, types[i % 3],
                                 sids[i / 3]);
    else
      ok = ::AddAccessDeniedAce(pAcl, ACL_REVISION, types[i % 3],
                                sids[i / 3]);

    if(! ok)
      goto END;
  }

  // Add a final deny all to everyone ACE

  if(! ::AddAccessDeniedAce(pAcl, ACL_REVISION,
                            STANDARD_RIGHTS_ALL | SPECIFIC_RIGHTS_ALL,
                            sids[2]))
    goto END;

  // The ACL is now complete. Associate it with the security descriptor.

  if(! ::SetSecurityDescriptorDacl(wperm.pdesc, TRUE, pAcl, FALSE))
    goto END;

  if(! ::IsValidSecurityDescriptor(wperm.pdesc))
    goto END;

  status = true;

END:

  return(status);
}

/*
 */

time_ms_t Windows::decodeFileTime(const FILETIME ft)
{
  LARGE_INTEGER li;
  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;

  time_ms_t val = static_cast<time_ms_t>(li.QuadPart);

  // subtract 100ns between Jan 1, 1601 and Jan 1, 1970
  val -= 0x19DB1DED53E8000LL;
  // convert from 100ns to ms
  val /= 10000LL;

  return(val);
}

/*
 */

uint64_t Windows::decodeFileTimeAsDelta(const FILETIME ft)
{
  LARGE_INTEGER li;
  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;

  uint64_t val = static_cast<uint64_t>(li.QuadPart);
  // convert from 100ns to ms
  val /= 10000LL;

  return(val);
}

/*
 */

bool Windows::findPrimaryGroup(LPTSTR name)
{
  /* Experimentation shows that the groups entered are as follows:
     0        -      None
     1        -      Everyone
     2        -      The first non-trivial group
     3...     -  Keep looking up to the count, which is part
     of the structure - see the documentation! */

  TCHAR RefDomain[DOM_SIZE];
  DWORD RefDomCnt = DOM_SIZE;
  SID_NAME_USE nameuse  = SidTypeGroup;
  HANDLE tHandle;
  TOKEN_GROUPS TokenG[20]; /* You need some space for this. */
  DWORD TISize, AcctSize = UNLEN + 1;

  if(! ::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS,
                          &tHandle))
    return(false);

  if(! ::GetTokenInformation(tHandle, TokenGroups, &TokenG, sizeof(TokenG),
                             &TISize))
    return(false);

  if(! ::LookupAccountSid(NULL, TokenG[0].Groups[2].Sid, name,
                          &AcctSize, RefDomain, &RefDomCnt, &nameuse))
    return(false);

  return(true);
}

/*
 */

String Windows::decodePathName(const String& path)
{
  if(path.indexOf('~') < 0)
    return(path);

  DWORD len = ::GetLongPathName(path.c_str(), NULL, 0);
  if(len == 0)
    return(String::null); // error

  char *buf = new char[len];
  len = ::GetLongPathName((LPCTSTR)path.c_str(), buf, len);
  if(len == 0)
  {
    delete [] buf;
    return(String::null);
  }

  String fullpath(buf, 0, 0, false);
  return(fullpath);
}

#endif // CCXX_OS_WINDOWS


} // namespace ccxx
