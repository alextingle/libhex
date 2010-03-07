/*                            Package   : libhex
 * hex.h                      Created   : 2010/03/07
 *                            Author    : Alex Tingle
 *
 *    Copyright (C) 2010, Alex Tingle.
 *
 *    This file is part of the libhex application.
 *
 *    libhex is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    libhex is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef FIRETREE__HEX__INTERNAL_H
#define FIRETREE__HEX__INTERNAL_H 1


#include <list>

namespace hex {


/** Length comparison for lists. L.size()>N is inefficient, because
 *  std::list<>::size() counts through every item in the list. This function
 *  counts through no more than N+1 items. */
template<typename T>
inline bool size_greater(const std::list<T> L, size_t N)
{
  for(typename std::list<T>::const_iterator i=L.begin(); i!=L.end(); ++i)
  {
    if(N==0)
        return true;
    --N;
  }
  return false;
}


} // end namespace hex


#endif // FIRETREE__HEX__INTERNAL_H


#ifdef FIRETREE__HEX__INTERNAL__TEST

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
  // Syntax: L'LENGTH N
  size_t length = ::atol(argv[1]);
  size_t N      = ::atol(argv[2]);
  std::list<int> L;
  for(int i=0; i<length; ++i)
      L.push_back(i);
  if( hex::size_greater(L,N) )
  {
    std::cout<<"YES"<<std::endl;
    return 0;
  }
  else
  {
    std::cout<<"NO"<<std::endl;
    return 1;
  }
}

#endif // FIRETREE__HEX__INTERNAL__TEST
