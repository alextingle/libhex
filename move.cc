/*                            Package   : libhex
 * move.cc                    Created   : 2008/02/19
 *                            Author    : Alex Tingle
 *
 *    Copyright (C) 2008, Alex Tingle.
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

#include "hexmove.h"

#include <cmath>


namespace hex {
namespace move {


Topography::Topography()
  : _has_default(false), _default(0.0), _hexes(), _edges()
  {}


Topography::Topography(Cost default_hex_cost)
  : _has_default(true), _default(default_hex_cost), _hexes(), _edges()
  {}


std::set<hex::Hex*>
Topography::accessible(void) const
{
  using namespace std;
  set<hex::Hex*> result;
  for(map<hex::Hex*,Cost>::const_iterator i=_hexes.begin(); i!=_hexes.end();++i)
    result.insert(i->first);
  for(map<hex::Edge*,Cost>::const_iterator i=_edges.begin();i!=_edges.end();++i)
    result.insert(i->first->hex());
  return result;
}


void
Topography::increase_hex_cost(hex::Hex* h, Cost c)
{
  {
    std::map<hex::Hex*,Cost>::iterator pos = _hexes.find(h);
    if(pos==_hexes.end())
        _hexes[h] = _default + c;
    else
        pos->second += c;
  }
  for(int d=0; d<DIRECTIONS; ++d)
  {
    hex::Edge* e =h->edge(hex::A+d);
    std::map<hex::Edge*,Cost>::iterator pos = _edges.find(e);
    if(pos!=_edges.end())
        pos->second += c;
  }
}


void
Topography::override_hex_cost(hex::Hex* h, Cost c)
{
  _hexes[h] = c;
  for(int d=0; d<DIRECTIONS; ++d)
  {
    hex::Edge* e =h->edge(hex::A+d);
    _edges.erase(e);
  }
}
      
void Topography::increase_edge_cost(hex::Edge* e, Cost c)
{
  if(!e)
    return;
  std::map<hex::Edge*,Cost>::iterator epos = _edges.find(e);
  if(epos==_edges.end())
  {
    std::map<hex::Hex*,Cost>::iterator hpos = _hexes.find( e->hex() );
    if(hpos!=_hexes.end())
      _edges[e] = hpos->second + c;
    else if(_has_default)
      _edges[e] = _default + c;
  }
  else
  {
    epos->second += c;
  }
}


void
Topography::override_edge_cost(hex::Edge* e, Cost c)
{
  if(e)
    _edges[e] = c;
}


void
Topography::increase_cost(const hex::Area& a, Cost c)
{
  const std::set<hex::Hex*>& hexes = a.hexes();
  for(std::set<hex::Hex*>::const_iterator i=hexes.begin(); i!=hexes.end(); ++i)
    increase_hex_cost(*i,c);
}


void
Topography::override_cost(const hex::Area& a, Cost c)
{
  const std::set<hex::Hex*>& hexes = a.hexes();
  for(std::set<hex::Hex*>::const_iterator i=hexes.begin(); i!=hexes.end(); ++i)
    override_hex_cost(*i,c);
}


void
Topography::increase_cost(const hex::Boundary& b, Cost c)
{
  const std::list<hex::Edge*>& edges = b.edges();
  for(std::list<hex::Edge*>::const_iterator i=edges.begin(); i!=edges.end(); ++i)
    increase_edge_cost(*i,c);
}


void
Topography::override_cost(const hex::Boundary& b, Cost c)
{
  const std::list<hex::Edge*>& edges = b.edges();
  for(std::list<hex::Edge*>::const_iterator i=edges.begin(); i!=edges.end(); ++i)
    override_edge_cost(*i,c);
}


hex::Path
Topography::best_path(hex::Hex* start, hex::Hex* goal) const throw(no_solution)
{
  std::set<hex::Hex*> visited;
  std::multiset<_Route> queue;
  queue.insert( _Route::factory(start,goal) );
  while(!queue.empty())
  {
    _Route curr_route = *queue.begin();
    queue.erase( queue.begin() );
    hex::Hex* curr_hex = curr_route.path.back();
    if(visited.count(curr_hex))
        continue;
    if(curr_hex==goal)
        return hex::Path(curr_route.path);
    visited.insert(curr_hex);
    for(int dir=0; dir<DIRECTIONS; ++dir)
    {
      Topography::Step s = step(curr_hex,hex::A+dir);
      if(s.to_hex && 0==visited.count(s.to_hex))
          queue.insert( curr_route.step(s.to_hex,s.cost,goal) );
    }
  }
  throw no_solution("best_path");
}


hex::Area
Topography::horizon(hex::Hex* start, Cost budget) const throw(no_solution)
{
  std::set<hex::Hex*> visited;
  std::multiset<_Route> queue;
  queue.insert( _Route::factory(start) );
  while(!queue.empty())
  {
    _Route curr_route = *queue.begin();
    queue.erase( queue.begin() );
    hex::Hex* curr_hex = curr_route.path.back();
    if(visited.count(curr_hex))
        continue;
    if(curr_route.cost > budget)
        continue;
    visited.insert(curr_hex);
    for(int dir=0; dir<DIRECTIONS; ++dir)
    {
      Topography::Step s = step(curr_hex,hex::A+dir);
      if(s.to_hex && 0==visited.count(s.to_hex))
          queue.insert( curr_route.step(s.to_hex,s.cost) );
    }
  }
  return visited;
}


Topography::Step
Topography::step(hex::Hex* from_hex, Direction direction) const
{
  Step result = {NULL,_default};
  // Find destination
  result.to_hex = from_hex->go(direction);
  if(!result.to_hex)
    return result;
  // If there's an edge cost, then use that.
  hex::Edge* to_edge = result.to_hex->edge(direction+3);
  std::map<hex::Edge*,Cost>::const_iterator epos = _edges.find(to_edge);
  if(epos!=_edges.end())
  {
    result.cost = epos->second;
    return result;
  }
  // Otherwise use cost of to_hex
  std::map<hex::Hex*,Cost>::const_iterator hpos = _hexes.find( result.to_hex );
  if(hpos!=_hexes.end())
      result.cost = hpos->second;
  else if(!_has_default)
      result.to_hex = NULL;
  return result;
}


//
// ROUTE

_Route
_Route::factory(hex::Hex* start, hex::Hex* goal)
{
  _Route result;
  result.path.push_back(start);
  result.cost  = 0; // g()
  result._value = result.distance(goal); // f()
  return result;
}


_Route
_Route::step(hex::Hex* next, Cost cost, hex::Hex* goal) const
{
  _Route result;
  result.path  = path;
  result.path.push_back(next);
  result.cost  = this->cost + cost; // g()
  result._value = result.cost + result.distance(goal); // f()
  return result;
}

   
hex::Distance
_Route::distance(hex::Hex* goal) const
{
  if(goal)
  {
    hex::Point vector = goal->centre() - path.back()->centre();
    return sqrt( vector.x * vector.x  +  vector.y * vector.y );
  }
  return 0;
}


} // end namespace svg
} // end namespace hex
