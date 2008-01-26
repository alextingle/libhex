/*                            Package   : libhex
 * path.cc                    Created   : 2007/10/11
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

#include <cassert>

namespace hex {


Area
Path::to_area(void) const
{
  std::set<Hex*> result;
  std::copy(_hexes.begin(), _hexes.end(), std::inserter(result,result.end()));
  return result;
}


int
Path::length(void) const
{
  return int( _hexes.size() );
}


std::string
Path::steps(void) const
{
  std::string result ="";
  const Hex* curr =NULL;
  for(std::list<Hex*>::const_iterator h=_hexes.begin(); h!=_hexes.end(); ++h)
  {
    if(curr)
        result += hex::steps(curr,*h);
    curr = *h;
  }
  return result;
}


std::string
Path::str(void) const
{
  assert(!this->_hexes.empty());
  std::string result =this->_hexes.front()->str();
  result+=":"+this->steps();
  return result;
}


/** Helper: populate _hexes from steps. */
inline std::list<Hex*>
path(Hex* start, const std::string& steps)
  throw(hex::out_of_range,hex::invalid_argument)
{
  // See also: go(int&,int&,const std::string&)
  std::list<Hex*> hexes;
  hexes.push_back(start);
  std::string::size_type cur =0;
  while(cur<steps.size() && steps[cur]!='?')
  {
    // Find direction
    Direction dir =to_direction( steps[cur] );
    ++cur;
    bool repeat =(cur<steps.size() && steps[cur]=='*');
    do{
        Hex* next =hexes.back()->go( dir );
        if(next)
            hexes.push_back( next );
        else if(*steps.rbegin()=='?' || repeat)
            return hexes; // bail out instead of throwing
        else
            throw hex::out_of_range(start->str()+":"+steps);
    } while(repeat);
  }
  return hexes;
}


Path::Path(Hex* start, const std::string& steps)
  throw(hex::out_of_range,hex::invalid_argument)
  : _hexes( path(start,steps) )
{}


Path::Path(Hex* from, const Hex* to) throw()
  : _hexes( path(from,hex::steps(from,to)) )
{}


} // end namespace hex
