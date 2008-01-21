/*                            Package   : libhex
 * hex.cc                     Created   : 2007/10/11
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

#include <sstream>

namespace hex {


Edge*
Hex::edge(const Direction& d)
{
  switch(d)
  {
    case A:  return &_edgeA;
    case B:  return &_edgeB;
    case C:  return &_edgeC;
    case D:  return &_edgeD;
    case E:  return &_edgeE;
    default: return &_edgeF;
  }
}


const Edge*
Hex::edge(const Direction& d) const
{
  return const_cast<Edge*>( const_cast<Hex*>(this)->edge( d ) );
}


Hex*
Hex::go(const Direction& d, int distance) const
{
  int i_ =this->i;
  int j_ =this->j;
  hex::go(i_,j_,d,distance); // in/out: i_,j_ 
  try
  {
    return _grid.hex(i_,j_);
  }
  catch(hex::out_of_range)
  {
    return NULL;
  }
}


Hex*
Hex::go(const std::string& steps) const
{
  int i_ =this->i;
  int j_ =this->j;
  hex::go(i_,j_,steps); // in/out: i_,j_ 
  try
  {
    return _grid.hex(i_,j_);
  }
  catch(hex::out_of_range)
  {
    return NULL;
  }
}


Point
Hex::centre(void) const
{
  Point result;
  if(this->j % 2)
      result.x = I * (1 + this->i); // odd rows
  else 
      result.x = I/2.0 + I * this->i; // even rows
  result.y = K + J * this->j;
  return result;
}


std::string
Hex::str(void) const
{
  std::ostringstream ss;
  ss<< this->i <<"_"<< this->j;
  return ss.str();
}


Hex::Hex(const Grid& grid, int i_, int j_):
  _edgeA(this,A), _edgeB(this,B), _edgeC(this,C),
  _edgeD(this,D), _edgeE(this,E), _edgeF(this,F),
  _grid(grid), i(i_), j(j_)
{}


Hex::Hex(const Grid& grid, const Hex& h):
  _edgeA(this,A), _edgeB(this,B), _edgeC(this,C),
  _edgeD(this,D), _edgeE(this,E), _edgeF(this,F),
 _grid(grid), i(h.i), j(h.j)
{}


} // end namespace hex
