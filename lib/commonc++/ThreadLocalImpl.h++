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

#ifndef __ccxx_ThreadLocalImpl_hxx
#define __ccxx_ThreadLocalImpl_hxx

#ifndef __ccxx_ThreadLocal_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template<typename T> void ThreadLocal<T>::keyDestructor(void* arg)
{
  T* value = reinterpret_cast<T*>(arg);
  delete value;
}

/*
 */

template<typename T> ThreadLocal<T>::ThreadLocal()
{
#ifdef CCXX_OS_WINDOWS

  _key = Windows::allocTLS();
  if(_key < 0)
    throw SystemException("No free TLS slots available");

#else

  if(::pthread_key_create(&_key, &ThreadLocal<T>::keyDestructor) != 0)
    throw SystemException(System::getErrorString());

#endif
}

/*
 */

template<typename T> ThreadLocal<T>::~ThreadLocal()
{
#ifdef CCXX_OS_WINDOWS

  Windows::freeTLS(_key);

#else

  ::pthread_key_delete(_key);

#endif
}

/*
 */

template<typename T> T& ThreadLocal<T>::operator*()
{
  T* val = getValue();

  if(val == NULL)
    throw NullPointerException();

  return(*val);
}

/*
 */

template<typename T> T* ThreadLocal<T>::operator->()
{
  T* val = getValue();

  if(val == NULL)
    throw NullPointerException();

  return(val);
}

/*
 */

template<typename T> T* ThreadLocal<T>::initialValue()
{
  return(NULL);
}

/*
 */

template<typename T> T* ThreadLocal<T>::getValue()
{
#ifdef CCXX_OS_WINDOWS

  T* value = reinterpret_cast<T*>(Windows::getTLS(_key));

#else

  T* value = reinterpret_cast<T*>(::pthread_getspecific(_key));

#endif

  if(value == NULL)
  {
    // didn't exist, so create it

    try
    {
      value = initialValue();
    }
    catch(...)
    {
    }

    setValue(value);
  }

  return(value);
}

/*
 */

template<typename T> void ThreadLocal<T>::setValue(T* value)
{
#ifdef CCXX_OS_WINDOWS

  Windows::setTLS(_key, reinterpret_cast<void*>(value),
                  &ThreadLocal<T>::keyDestructor);

#else

  ::pthread_setspecific(_key, reinterpret_cast<void*>(value));

#endif
}

/*
 */

template<typename T> int ThreadLocal<T>::getSlotCount()
{
#ifdef CCXX_OS_WINDOWS

  return(Windows::getTLSMax());

#else

  return(-1); // unlimited for pthreads

#endif
}

#endif // __ccxx_ThreadLocalImpl_hxx
