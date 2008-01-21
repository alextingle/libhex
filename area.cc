/*                            Package   : libhex
 * area.cc                    Created   : 2007/10/11
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


Boundary
Area::boundary(void) const
{
  // Start with a random hex.
  Hex* h =*_hexes.begin();
  const Grid& grid =h->grid();
  const int i0 =h->i;
  const int j0 =h->j;
  // Find an edge.
  int i=0;
  while(i<=i0 && !contains(grid.hex(i,j0)))
      ++i;
  std::list<Edge*> result;
  result.push_back( grid.hex(i,j0)->edge(D) );
  // Follow the edge round.
  while(true)
  {
    Edge* e =result.back()->next_out();
    if( !e || !contains(e->hex()) )
        e = result.back()->next_in();
    if(e == result.front())
        break;
    result.push_back(e);
  }
  return result;
}


std::list<Area>
Area::enclosed_areas(void) const
{
  Area a =boundary().area();
  return areas( set_difference( a._hexes, _hexes ) );
}


std::list<Boundary>
Area::skeleton(bool include_boundary) const
{
  std::list<Boundary> result;
  for(std::set<Hex*>::const_iterator h=_hexes.begin(); h!=_hexes.end(); ++h)
  {
    std::list<Edge*> edges;
    edges.push_back( (**h).edge(A) );
    edges.push_back( (**h).edge(B) );
    edges.push_back( (**h).edge(C) );
    result.push_back(edges);
  }
  if(include_boundary)
      result.push_back(boundary());
  return result;
}


std::list<hex::Path>
Area::fillpaths(Hex* origin) const
{
  // Try to calculate a path that fills area.
  std::set<Hex*> queue =_hexes;
  std::set<Hex*> seen;
  std::list<Path> result;
  std::list<Hex*> path;
  Hex*      hex = origin? origin: *queue.begin();
  Direction dir =F;
  while(!queue.empty())
  {
    path.push_back( hex );
    queue.erase( hex );
    seen.insert( hex );
    Direction d=dir+1;
    while(true)
    {
      if(d==dir)
      {
        result.push_back( path );
        path.clear();
        hex = *queue.begin();
        break;
      }
      Hex* hd =hex->go(d);
      if(queue.count(hd) && !seen.count(hd))
      {
        hex = hd;
        dir = d+3;
        break;
      }
      ++d;
    }
  }
  return result;
}


std::string
Area::str(Hex* origin) const
{
  if(!origin)
      origin=*_hexes.begin();
  std::ostringstream result;
  std::list<Path> paths  =this->fillpaths(origin);
  result << origin->str();
  for(std::list<Path>::const_iterator p =paths.begin(); p!=paths.end(); ++p)
  {
    if(p->hexes().front()!=origin)
        result << ">" << hex::steps(origin,p->hexes().front());
    result << ":" << p->steps();
  }
  return result.str();
}


Area
Area::go(const Direction& d, int distance) const throw(hex::out_of_range)
{
  Area result;
  for(std::set<Hex*>::const_iterator h=_hexes.begin(); h!=_hexes.end(); ++h)
  {
    Hex* hex =(**h).go(d,distance);
    if(!hex)
        throw hex::out_of_range("Area::go");
    result._hexes.insert(hex);
  }
  return result;
}


Area
Area::go(const std::string& steps) const throw(hex::out_of_range)
{
  Area result;
  for(std::set<Hex*>::const_iterator h=_hexes.begin(); h!=_hexes.end(); ++h)
  {
    Hex* hex =(**h).go(steps);
    if(!hex)
        throw hex::out_of_range("Area::go");
    result._hexes.insert(hex);
  }
  return result;
}


Area::Area(const std::set<Hex*>& hexes): _hexes(hexes)
{
#ifdef HEX_PARANOID_CHECKS
  assert(is_connected(hexes));
#endif
}


Area::Area(Hex* h, int distance): _hexes()
{
  for(int i=0; i<=distance; ++i)
  {
    std::set<Hex*> s =range(h,i);
    std::copy(s.begin(),s.end(),inserter(_hexes,_hexes.end()));
  }
}


} // end namespace hex
