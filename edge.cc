#include "hex.h"

#include <cassert>

namespace hex {


Edge*
Edge::complement(void) const
{
  Hex* adjacent_hex =_hex->go(_direction);
  if(adjacent_hex)
      return adjacent_hex->edge(_direction+3);
  else
      return NULL;
}


bool
Edge::is_next(const Edge& v) const
{
  // TRUE iff: &v == next_in(T) || next_in(F) || next_out(T) || next_out(F)
  if( _hex == v._hex )
      return( _direction+1 == v._direction || _direction-1 == v._direction );
  else
      return( &v == next_out(true) || &v == next_out(false) );
}


Edge*
Edge::next_in(bool clockwise) const
{
  int one =(clockwise? -1: 1);
  return _hex->edge( _direction + one );
}


Edge*
Edge::next_out(bool clockwise) const
{
  int one =(clockwise? -1: 1);
  Edge* c =_hex->edge( _direction + one )->complement();
  if(c)
      return c->_hex->edge( _direction - one );
  else
      return NULL;
}


/** Helper function. Offsets p towards the start_point of edge d. */
Point corner_offset(Point p, Direction d, float bias)
{
  Distance dx =0.0;
  Distance dy =0.0;
  switch(d)
  {
    case A: dx =  I/2.0; dy = -K/2.0; break;
    case B: dx =  I/2.0; dy =  K/2.0; break;
    case C: dx =    0.0; dy =  K    ; break;
    case D: dx = -I/2.0; dy =  K/2.0; break;
    case E: dx = -I/2.0; dy = -K/2.0; break;
    case F: dx =    0.0; dy = -K    ; break;
  }
  if(bias==0.0)
      return p.offset(dx,dy);
  else
      return p.offset(dx*(1.0+bias),dy*(1.0+bias));
}


Point
Edge::start_point(float bias, bool clockwise) const
{
  if(clockwise)
      return corner_offset( _hex->centre(), _direction + 1, bias );
  else
      return corner_offset( _hex->centre(), _direction    , bias );
}


Point
Edge::end_point(float bias, bool clockwise) const
{
  return start_point(bias,!clockwise);
}


Point
Edge::join_point(const Edge* next, float bias) const
{
  if(bias==0.0 || this->_hex==next->_hex)
  {
    if(this->_direction+1 == next->_direction)
        return end_point(bias);
    else
        return start_point(bias); // clockwise
  }
  else if(next == next_out())
  {
    Point p =corner_offset( _hex->centre(), _direction + 2, 0.0 );
    return corner_offset( p, _direction, bias );
  }
  else // clockwise
  {
#ifdef HEX_PARANOID_CHECKS
    assert(next == next_out(true));
#endif
    Point p =corner_offset( _hex->centre(), _direction - 1, 0.0 );
    return corner_offset( p, _direction + 1, bias );
  }
}


} // end namespace hex
