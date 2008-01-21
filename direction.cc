/*                            Package   : libhex
 * direction.cc               Created   : 2007/10/11
 *                            Author    : Alex Tingle
 *
 *    Copyright (C) 2007-2008, Alex Tingle.
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

#include "hex.h"

namespace hex {


Direction to_direction(char c) throw(hex::invalid_argument)
{
  if(c<'A' || c>'F')
      throw hex::invalid_argument(std::string("to_direction:")+c);
  return A + static_cast<int>(c-'A');
}


char to_char(Direction d)
{
  static const char root =static_cast<char>( A );
  return static_cast<char>( 'A' + (static_cast<char>(d) - root) );
}


Direction& operator+=(Direction& d, int i)
{
  int d_ =static_cast<int>( d );
  d_= (d_+i) % DIRECTIONS;
  while(d_<0)
    d_ += DIRECTIONS;
  d=static_cast<Direction>( d_ );
  return d;
}

Direction operator+(const Direction& d, int i)
{
  Direction result =d;
  result+=i;
  return result;
}

/** Prefix increment */
Direction& operator++(Direction& d)
{
  return d+=1;
}

/** Postfix increment */
Direction operator++(const Direction& d,int)
{
  Direction result =d;
  ++result;
  return result;
}

Direction& operator-=(Direction& d, int i)
{
  return d += (-i);
}

Direction operator-(const Direction& d, int i)
{
  return d + (-i);
}

/** Prefix decrement */
Direction& operator--(Direction& d)
{
  return d-=1;
}

/** Postfix decrement */
Direction operator--(const Direction& d,int)
{
  Direction result =d;
  --result;
  return result;
}

std::ostream& operator<<(std::ostream& os, const Direction& d)
{
  os << to_char(d);
  return os;
}


std::string rotate(const std::string& steps, int i)
{
  // Rotate all letters A-F, but leave other characters alone.
  std::string result =steps;
  for(std::string::iterator c=result.begin(); c!=result.end(); ++c)
      if('A'<=*c && *c<='F')
      {
        Direction d =to_direction(*c);
        *c = to_char(d+i);
      }
  return result;
}


} // end namespace hex
