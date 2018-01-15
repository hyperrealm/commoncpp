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

/* Adapted from md5crypt.c (from the OpenSSH package). Original copyright
 * notice:
 * ---------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@login.dknet.dk> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.
 * Poul-Henning Kamp
 * ---------------------------------------------------------------------------
 */

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/MD5Password.h++"
#include "commonc++/MD5Digest.h++"
#include "commonc++/Random.h++"

#include <cstring>

namespace {

static const char* I_TO_A64 = /* 0 ... 63 => ascii - 64 */
  "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static const char* MAGIC = "$ccxx$";
static const int SALT_LENGTH = 8;

static void to64(char* s, unsigned long v, int n)
{
  while(--n >= 0)
  {
    *s++ = I_TO_A64[v & 0x3F];
    v >>= 6;
  }
}

} // namespace

namespace ccxx {

/*
 */

String MD5Password::encrypt(const String& password)
{
  char buf[SALT_LENGTH + 1], *p;
  Random rand;
  int i;

  for(i = 0, p = buf; i < SALT_LENGTH; ++i, ++p)
    *p = I_TO_A64[2 + rand.nextInt(62)];

  *p = NUL;

  String salt = buf;
  return(encrypt(password, salt));
}

/*
 */

String MD5Password::encrypt(const String& password, const String& salt)
{
  char passwd[128], *p;
  CString cstr_password = password.toUTF8();
  CString cstr_salt = salt.toUTF8();
  int i, j;
  unsigned long l;
  MD5Digest digest1, digest2;
  byte_t hash1[16], hash2[16];

  // The password first, since that is what is most unknown.
  digest1.update(cstr_password.bytes(), cstr_password.length());

  // Then our magic string.
  digest1.update(reinterpret_cast<const byte_t*>(MAGIC), std::strlen(MAGIC));

  // Then the raw salt.
  digest1.update(cstr_salt.bytes(), cstr_salt.length());

  // Then just as many characters of the MD5(pw,salt,pw).
  digest2.update(cstr_password.bytes(), cstr_password.length());
  digest2.update(cstr_salt.bytes(), cstr_salt.length());
  digest2.update(cstr_password.bytes(), cstr_password.length());
  digest2.finish(hash2);

  for(int pl = cstr_password.length(); pl > 0; pl -= 16)
    digest1.update(hash2, pl > 16 ? 16 : pl);

  // Then something really weird...
  for(j = 0, i = cstr_password.length(); i; i >>= 1)
  {
    if(i & 1)
      digest1.update(hash2 + j, 1);
    else
    {
      char c = cstr_password[j];
      digest1.update(reinterpret_cast<const byte_t*>(&c), 1);
    }
  }

  digest1.finish(hash1);

  // And now, just to make sure things don't run too fast On a 60 Mhz
  // Pentium this takes 34 msec, so you would need 30 seconds to build
  // a 1000 entry dictionary...
  for(i = 0; i < 1000; ++i)
  {
    MD5Digest digest3;

    if(i & 1)
      digest3.update(cstr_password.bytes(), cstr_password.length());
    else
      digest3.update(hash1, 16);

    if(i % 3)
      digest3.update(cstr_salt.bytes(), cstr_salt.length());
    if(i % 7)
      digest3.update(cstr_password.bytes(), cstr_password.length());
    if(i & 1)
      digest3.update(hash1, 16);
    else
      digest3.update(cstr_password.bytes(), cstr_password.length());

    digest3.finish(hash1); // overwrite hash1
  }

  // Now make the output string.
  std::strcpy(passwd, MAGIC);
  std::strcat(passwd, cstr_salt.data());
  std::strcat(passwd, "$");

  p = passwd + std::strlen(passwd);
  l = (hash1[0] << 16) | (hash1[ 6] << 8) | hash1[12]; to64(p, l, 4); p += 4;
  l = (hash1[1] << 16) | (hash1[ 7] << 8) | hash1[13]; to64(p, l, 4); p += 4;
  l = (hash1[2] << 16) | (hash1[ 8] << 8) | hash1[14]; to64(p, l, 4); p += 4;
  l = (hash1[3] << 16) | (hash1[ 9] << 8) | hash1[15]; to64(p, l, 4); p += 4;
  l = (hash1[4] << 16) | (hash1[10] << 8) | hash1[ 5]; to64(p, l, 4); p += 4;
  l =                     hash1[11]                  ; to64(p, l, 2); p += 2;
  *p = NUL;

  // Don't leave anything around in vm they could use.
  std::memset(hash1, 0, sizeof hash1);
  std::memset(hash2, 0, sizeof(hash2));

  String hash = passwd;

  return(hash);
}

/*
 */

bool MD5Password::validate(const String& password, const String& ciphertext)
{
  if(!ciphertext.startsWith(MAGIC))
    return(false);

  int pos = std::strlen(MAGIC);
  String salt = ciphertext.substring(pos, pos + SALT_LENGTH);
  String hash = encrypt(password, salt);

  return(hash == ciphertext);
}


} // namespace ccxx
