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

#include "commonc++/JavaException.h++"

namespace ccxx {

/*
 */

JavaException::JavaException(String message /* = String::empty */)
  : Exception(message)
{
}

/*
 */

JavaSignatureException::JavaSignatureException(String message
                                               /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaClassNotFoundException::JavaClassNotFoundException(String message
                                                       /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaMethodNotFoundException::JavaMethodNotFoundException(String message
                                                         /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaFieldNotFoundException::JavaFieldNotFoundException(String message
                                                       /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaInstantiationException::JavaInstantiationException(String message
                                                       /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaInvocationException::JavaInvocationException(String message
                                                 /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaFieldAccessException::JavaFieldAccessException(String message
                                                   /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaArrayAccessException::JavaArrayAccessException(String message
                                                   /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaOutOfMemoryException::JavaOutOfMemoryException(String message
                                                   /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaMonitorException::JavaMonitorException(String message
                                           /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaThreadException::JavaThreadException(String message /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaNotAttachedException::JavaNotAttachedException(String message
                                                   /* = String::empty */)
  : JavaException(message)
{
}

/*
 */

JavaClassLoaderException::JavaClassLoaderException(String message
                                                   /* = String::empty */)
  : JavaException(message)
{
}

} // namespace ccxx
