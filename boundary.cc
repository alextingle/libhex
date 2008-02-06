/*                            Package   : libhex
 * boundary.cc                Created   : 2007/10/11
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


int
Boundary::length(void) const ///< in units of K
{
  return int( _edges.size() );
}


bool
Boundary::is_closed(void) const
{
  return( _edges.size()>2 && _edges.front()->is_next( *_edges.back() ) );
}


bool
Boundary::is_container(void) const
{
  if(is_closed())
  {
    try {
      Path p1 =complement().to_path();
      Path p0 =to_path();
      return( p0.length() < p1.length() );
    }
    catch(hex::out_of_range) {
      // If is_closed AND there is no complement, then the boundary must
      // be at the very edge of the grid... it MUST be a container.
      return true;
    }
  }
  return false;
}


Boundary
Boundary::complement(void) const throw(hex::out_of_range)
{
  std::list<Edge*> result;
  for(std::list<Edge*>::const_iterator e=_edges.begin(); e!=_edges.end(); ++e)
  {
    Edge* c =(**e).complement();
    if(c)
        result.push_back( c );
    else
        throw hex::out_of_range("Boundary complement out of range.");
  }
  return result;
}


Path
Boundary::to_path(void) const
{
  std::list<Hex*> result;
  for(std::list<Edge*>::const_iterator e=_edges.begin(); e!=_edges.end(); ++e)
    if( result.empty() || (**e).hex() != result.back() )
        result.push_back( (**e).hex() );
  return result;
}


bool
Boundary::clockwise(void) const
{
  // Boundaries usually go round in a positive (anti-clockwise) direction.
  if(_edges.size() > 1)
  {
    std::list<Edge*>::const_iterator e =_edges.begin();
    Edge* e0 = *e;
    Edge* e1 = *(++e);
    return( e0==e1->next_in() || e0==e1->next_out() );
  }
  return false;
}


std::string
Boundary::str(void) const
{
  assert(!_edges.empty());
  std::string result =_edges.front()->hex()->str() + (clockwise()?"-":"+");
  for(std::list<Edge*>::const_iterator e=_edges.begin(); e!=_edges.end(); ++e)
      result += to_char( (**e).direction() );
  return result;
}


std::list<Point>
Boundary::stroke(float bias) const
{
  std::list<Point> result;
  bool cw =this->clockwise();
  if(bias==0.0)
  {
    if(!_edges.empty())
    {
      std::list<Edge*>::const_iterator e =_edges.begin();
      result.push_back( (**e).start_point(0.0,cw) );
      while(e!=_edges.end())
      {
        result.push_back( (**e).end_point(0.0,cw) );
        ++e;
      }
    }
  }
  else
  {
    Edge* last =NULL;
    for(std::list<Edge*>::const_iterator e=_edges.begin(); e!=_edges.end(); ++e)
    {
      if(last)
          result.push_back( last->join_point(*e,bias) );
      last = *e;
    }
    if(is_closed())
    {
      Point p =last->join_point(_edges.front(),bias);
      result.push_front( p );
      result.push_back( p );
    }
    else
    {
      result.push_front( _edges.front()->start_point(bias,cw) );
      result.push_back( last->end_point(bias,cw) );
    }
  }
  return result;
}


Area
Boundary::area(void) const
{
  assert(is_closed());
  std::set<Hex*> beyond;
  std::set<Hex*> queue;
  for(std::list<Edge*>::const_iterator e=_edges.begin(); e!=_edges.end(); ++e)
  {
    queue.insert(  (**e).hex() );
    beyond.insert( (**e).hex()->go( (**e).direction() ) ); // NULL is OK.
  }
  return fill(beyond,queue);
}


} // end namespace hex
