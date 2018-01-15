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

#ifndef __ccxx_RefSetImpl_hxx
#define __ccxx_RefSetImpl_hxx

#ifndef __ccxx_RefSet_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template<typename T> RefSet<T>::RefSet()
{
}

/*
 */

template<typename T> RefSet<T>::~RefSet()
{
}

/*
 */

template<typename T> bool RefSet<T>::add(T value)
{
  bool added = false;
  typename std::map<T, int>::iterator iter = this->find(value);

  if(iter != std::map<T, int>::end())
    ((*iter).second)++;
  else
  {
    this->insert(std::map<T, int>::begin(), std::make_pair(value, 1));
    added = true;
  }

  return(added);
}

/*
 */

template<typename T> bool RefSet<T>::remove(T value)
{
  bool removed = false;

  typename std::map<T, int>::iterator iter = this->find(value);

  if(iter != std::map<T, int>::end())
  {
    if(--((*iter).second) == 0)
    {
      this->erase(iter);
      removed = true;
    }
  }

  return(removed);
}

/*
 */

template<typename T> bool RefSet<T>::contains(T value) const
{
  typename std::map<T, int>::const_iterator iter = this->find(value);

  return(iter != std::map<T, int>::end());
}

/*
 */

template<typename T> int RefSet<T>::refCount(T value) const
{
  typename std::map<T, int>::const_iterator iter = this->find(value);

  if(iter == std::map<T, int>::end())
    return(0);
  else
    return(iter->second);
}

#endif // __ccxx_RefSetImpl_hxx
