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
  return( _edges.front()->is_next( *_edges.back() ) );
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
    catch(hex::range_error) {
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


std::list<Point>
Boundary::stroke(float bias) const
{
  std::list<Point> result;
  if(bias==0.0)
  {
    if(!_edges.empty())
    {
      std::list<Edge*>::const_iterator e =_edges.begin();
      result.push_back( (**e).start_point() );
      while(e!=_edges.end())
      {
        result.push_back( (**e).end_point() );
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
      result.push_front( _edges.front()->start_point(bias) );
      result.push_back( last->end_point(bias) );
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
