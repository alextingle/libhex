/*                            Package   : libhex
 * algorithm.cc               Created   : 2007/10/11
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
#include <sstream>

namespace hex {


void
go(int& i, int& j, Direction d, int distance)
{
  for(; distance>0; --distance)
      if(j%2)
          switch(d) // odd
          {
            case A: ++i;      break;
            case B: ++i; ++j; break;
            case C:      ++j; break;
            case D: --i;      break;
            case E:      --j; break;
            case F: ++i; --j; break;
          }
      else
          switch(d) // even
          {
            case A: ++i;      break;
            case B:      ++j; break;
            case C: --i; ++j; break;
            case D: --i;      break;
            case E: --i; --j; break;
            case F:      --j; break;
          }
}


void
go(int& i, int& j, const std::string& steps)
{
  for(std::string::const_iterator s=steps.begin(); s!=steps.end(); ++s)
    if('A'<=*s && *s<='F')
        go(i,j,to_direction(*s));
    else
        break; // Just bail out if we encounter a non-Direction character.
}


std::string
steps(const Hex* from, const Hex* to)
{
  std::string result ="";
  int i =from->i;
  int j =from->j;
  const int to_i =to->i;
  const int to_j =to->j;
  Direction direction;
  while(true)
  {
    if( j < to_j )                                   // go up
    {
        if(      i < to_i )          direction = B;
        else if( i > to_i || j%2 )   direction = C;
        else                         direction = B;
    }
    else if( j > to_j )                              // go down
    {
        if(      i < to_i )          direction = F;
        else if( i > to_i || j%2 )   direction = E;
        else                         direction = F;
    }
    else // j == to_j                                // go across
    {
        if(      i < to_i )          direction = A;
        else if( i > to_i )          direction = D;
        else                         break;          //  Done!
    }
    result += to_char(direction);
    go(i,j,direction);
  }
  return result;
}


int
distance(const Hex* from, const Hex* to)
{
  return steps(from,to).size();
}


std::set<Hex*>
range(Hex* h, int distance)
{
  std::set<Hex*> result;
  if(distance<1)
  {
    result.insert(h);
  }
  else
  {
    const Grid& grid =h->grid();
    int i =h->i;
    int j =h->j;
  
    try {
      go(i,j,hex::A,distance); // in/out: i,j
      result.insert( grid.hex(i,j) );
    } catch(hex::out_of_range&) {}
  
    for(int d=0; d<DIRECTIONS; ++d)
    {
      Direction direction =C+d;
      for(int count=0; count<distance; ++count)
          try {
            go(i,j,direction); // in/out: i,j
            result.insert( grid.hex(i,j) );
          } catch(hex::out_of_range&) {}
    }
  }
  return result;
}


std::set<Hex*>
set_difference(const std::set<Hex*>& a, const std::set<Hex*>& b)
{
  std::set<Hex*> result;
  std::set_difference(
    a.begin(),a.end(),
    b.begin(),b.end(),
    std::inserter(result,result.end())
  );
  return result;
}


std::set<Hex*>
set_intersection(const std::set<Hex*>& a, const std::set<Hex*>& b)
{
  std::set<Hex*> result;
  std::set_intersection(
    a.begin(),a.end(),
    b.begin(),b.end(),
    std::inserter(result,result.end())
  );
  return result;
}


std::set<Hex*>
set_union(const std::set<Hex*>& a, const std::set<Hex*>& b)
{
  std::set<Hex*> result;
  std::set_union(
    a.begin(),a.end(),
    b.begin(),b.end(),
    std::inserter(result,result.end())
  );
  return result;
}


std::string
set_str(const std::set<Hex*>& a)
{
  std::ostringstream ss;
  for(std::set<Hex*>::const_iterator h=a.begin(); h!=a.end(); ++h)
      ss<<(**h).str()<<" ";
  return ss.str();
}


bool
bounding_box(const std::set<Hex*>& a, int& i0, int& j0, int& i1, int& j1)
{
  for(std::set<Hex*>::const_iterator h=a.begin(); h!=a.end(); ++h)
  {
    if(h==a.begin())
    {
      i0 = i1 = (**h).i;
      j0 = j1 = (**h).j;
    }
    else
    {
      i0 = std::min(i0,(**h).i);
      j0 = std::min(j0,(**h).j);
      i1 = std::max(i1,(**h).i);
      j1 = std::max(j1,(**h).j);
    }
  }
  return !a.empty();
}


Area
fill(const std::set<Hex*>& beyond, Hex* h)
{
  return fill( beyond, Area(h) );
}


Area
fill(const std::set<Hex*>& beyond, const Area& a)
{
  std::set<Hex*> queue   =a.hexes();
  std::set<Hex*> result  =a.hexes();
  while(!queue.empty())
  {
    Hex* h = *queue.begin();
    queue.erase( queue.begin() );
    for(int d=0; d<DIRECTIONS; ++d)
    {
      Hex* hd =h->go(A+d);
      if(!beyond.count(hd) && !result.count(hd))
      {
        queue.insert(hd);
        result.insert(hd);
      }
    }
  }
  return result;
}


/** Helper: Find the connected set (Area) that contains the first hex in s. */
inline std::set<Hex*>
extract_connected_set(std::set<Hex*>& s)
{
  std::set<Hex*> result;
  std::set<Hex*> queue; // Hexes in result that have not yet been checked.
  Hex* h =*s.begin(); // Just pick a random hex in s.
  while(true)
  {
    result.insert(h);
    s.erase(h);
    if(s.empty())
        break; // optimisation - not strictly necessary.
    std::set<Hex*> range1 =set_intersection( s, range(h,1) );
    queue.insert( range1.begin(), range1.end() );
    if(queue.empty())
        break;
    h = *queue.begin();
    queue.erase( queue.begin() );
  }
  return result;
}


std::list<Area>
areas(const std::set<Hex*>& s)
{
  std::set<Hex*> unallocated = s;
  std::list<Area> result;
  while(!unallocated.empty())
  {
    result.push_back( extract_connected_set(unallocated) );
  }
  return result;
}


bool
is_connected(const std::set<Hex*>& s)
{
  std::set<Hex*> unallocated = s;
  extract_connected_set( unallocated );
  return unallocated.empty();
}


std::list<Boundary>
skeleton(const std::list<Area>& a, bool include_boundary)
{
  std::list<Boundary> result;
  for(std::list<Area>::const_iterator iter=a.begin(); iter!=a.end(); ++iter)
  {
    std::list<Boundary> b =iter->skeleton(include_boundary);
    std::copy(b.begin(),b.end(),inserter(result,result.end()));
  }
  return result;
}


} // end namespace hex
