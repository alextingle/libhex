/*                            Package   : libhex
 * grid.cc                    Created   : 2007/10/11
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
#include <cmath>
#include <cerrno>
#include <cassert>

namespace hex {


//
// Point


Point::Point(const std::string s) throw (out_of_range,invalid_argument)
{
  // Parse strings like: / *[-+]?\d+_ *[-+]?\d+([^\d].*)?/
  // E.g.  '1_2' ' +2_ 4' '2_4 # comment'
  const char* buf =s.c_str();
  char* endptr =NULL;
  errno=0;
  this->x = ::strtod(buf,&endptr);
  if(endptr==buf || *endptr!=',')
      throw hex::invalid_argument(s);
  buf =endptr+1;
  this->y = ::strtod(buf,&endptr);
  if(endptr==buf)
      throw hex::invalid_argument(s);
  if(ERANGE==errno)
      throw hex::out_of_range(s);
}


std::string
Point::str(void) const
{
  std::ostringstream ss;
  ss<< this->x << "," << this->y;
  return ss.str();
}


//
// Grid


Hex*
Grid::hex(int i, int j) const throw(hex::out_of_range)
{
  int key =Hex::_key(i,j);
  std::map<int,Hex*>::const_iterator pos =_hexes.find(key);
  if(pos==_hexes.end())
  {
    if(0>i || i>=_cols)
        throw hex::out_of_range("i");
    if(0>j || j>=_rows)
        throw hex::out_of_range("j");
    Hex* newhex = new Hex(*this,i,j);
    _hexes.insert( std::make_pair(key,newhex) );
    return newhex;
  }
  return pos->second;
}


Hex*
Grid::hex(Distance x, Distance y) const throw(hex::out_of_range)
{
  // (Note I==1.0, so the factor of I has been omitted.)
  const static Distance K_2 =K/2.0;
  // BI is unit vector in direction B
  const static Distance BIx = 0.5;
  const static Distance BIy = 1.5 * K;
  // CI is unit vector in direction C
  const static Distance CIx = -BIx;
  const static Distance CIy =  BIy;

  // Calculate the 'simple' solution.
  y -= K;
  int j = lround( y/J );
  if(j % 2)
      x -= 1.0; // odd rows
  else
      x -= 0.5; // even rows
  int i = lround( x );                                              //   x / I
  // Now calculate the x,y offsets (in units of (I,J) )
  Distance dx = x - Distance(i);                                    //   i * I
  Distance dy = y - Distance(j) * J;
  // Only need more work if |dy| > K/2
  if( dy < -K_2 || K_2 < dy )
  {
    Distance BId = (BIx * dx) + (BIy * dy);
    Distance CId = (CIx * dx) + (CIy * dy);

    if(      BId >  0.5 )
                          go( i,j, B );
    else if( BId < -0.5 )
                          go( i,j, E );
    else if( CId >  0.5 )
                          go( i,j, C );
    else if( CId < -0.5 )
                          go( i,j, F );
  }
  return hex(i,j);
}


Hex*
Grid::hex(const Point& p) const throw(hex::out_of_range)
{
  return hex(p.x,p.y);
}


Area
Grid::to_area(void) const
{
  using namespace std;
  set<Hex*> hexes;
  for(int i=0; i<_cols; ++i)
      for(int j=0; j<_rows; ++j)
          hexes.insert( hex(i,j) );
  return Area(hexes);  
}


Hex*
Grid::hex(const std::string& s) const throw(out_of_range,invalid_argument)
{
  // Parse strings like: / *[-+]?\d+_ *[-+]?\d+([^\d].*)?/
  // E.g.  '1_2' ' +2_ 4' '2_4 # comment'
  const char* buf =s.c_str();
  char* endptr =NULL;
  errno=0;
  int i =::strtol(buf,&endptr,10);
  if(endptr==buf || *endptr!='_')
      throw hex::invalid_argument(s);
  buf =endptr+1;
  int j =::strtol(buf,&endptr,10);
  if(endptr==buf)
      throw hex::invalid_argument(s);
  if(ERANGE==errno)
      throw hex::out_of_range(s);
  return this->hex(i,j);
}


std::set<Hex*>
Grid::hexes(const std::string& s) const throw(out_of_range,invalid_argument)
{
  // Parse string of space-separated hex coordinates.
  // E.g. 1_2 2_3 3_4
  std::istringstream is(s);
  std::set<Hex*> result;
  while(is.good())
  {
    std::string tok;
    is>>tok;
    if(!tok.empty())
        result.insert( this->hex(tok) );
  }
  return result;
}


Area
Grid::area(const std::string& s) const throw(out_of_range,invalid_argument)
{
  // Parse string of area fillpaths
  // E.g. 1,2>CDE:ABC
  std::set<Hex*> result;
  std::string::size_type pos =s.find_first_of(":>");
  if(pos==std::string::npos)
      throw hex::invalid_argument(s);
  Hex* origin =this->hex( s.substr(0,pos) );
  Hex* start  =origin;
  while(pos!=std::string::npos)
  {
    std::string::size_type next =s.find_first_of(":>",pos+1);
    std::string steps =s.substr( pos+1, (next==s.npos)?(next):(next-pos-1) );
    if(s[pos]=='>')
    {
      start=origin->go(steps);
    }
    else // ':'
    {
      std::list<Hex*> hexes =Path(start,steps).hexes();
      std::copy(hexes.begin(), hexes.end(), inserter(result,result.end()));
      start=origin;
    }
    pos=next;
  }
  return result;
}


Path
Grid::path(const std::string& s) const throw(out_of_range,invalid_argument)
{
  std::string::size_type colon =s.find(':');
  if(colon==std::string::npos || (colon+1)>=s.size())
      throw hex::invalid_argument(s);
  Hex* origin =this->hex( s.substr(0,colon) );
  Path result(origin,s.substr(colon+1));
  return result;
}


Boundary
Grid::boundary(const std::string& s) const throw(out_of_range,invalid_argument)
{
  std::list<Edge*> result;
  std::string::size_type plus_minus =s.find_first_of("+-");
  if(plus_minus==std::string::npos || (plus_minus+1)>=s.size())
      throw hex::invalid_argument(s);
  bool clockwise =( '-' == s[plus_minus] );
  for(std::string::size_type pos =plus_minus+1; pos<s.size(); ++pos)
  {
    Direction d =to_direction(s[pos]);
    Edge* next;
    if(result.empty())
    {
      next = this->hex(s.substr(0,plus_minus))->edge(d);
    }
    else
    {
      next = result.back()->next_in(clockwise);
      if(next->direction() != d)
      {
        next = result.back()->next_out(clockwise);
        if(next->direction() != d)
            throw hex::invalid_argument(s);
      }
    }
    result.push_back(next);
  }
  return result;
}


Grid::Grid(int cols, int rows) throw(hex::out_of_range)
  : _hexes(), _cols(cols), _rows(rows)
{
  // Perhaps it's perverse to use ints, and then restrict them to be only
  // short. I think it just makes the interface less quirky, the code a
  // bit more efficient, and using shorts wouldn't really save much memory.
  if(0>cols || cols>=0x4000)
      throw hex::out_of_range("cols");
  if(0>rows || rows>=0x4000)
      throw hex::out_of_range("rows");
}


Grid::Grid(const Grid& v): _hexes(), _cols(v._cols), _rows(v._rows)
{
  using namespace std;
  for(map<int,Hex*>::const_iterator h =v._hexes.begin(); h!=v._hexes.end(); ++h)
      _hexes.insert( make_pair(h->first,new Hex(*this,*h->second)) );
}


Grid::~Grid()
{
  using namespace std;
  for(map<int,Hex*>::const_iterator h =_hexes.begin(); h!=_hexes.end(); ++h)
      delete h->second;
}


} // end namespace hex
