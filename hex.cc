#include "hex.h"

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
  int i =_i;
  int j =_j;
  hex::go(i,j,d,distance); // in/out: i,j 
  try
  {
    return _grid.hex(i,j);
  }
  catch(std::out_of_range)
  {
    return NULL;
  }
}


Point
Hex::centre(void) const
{
  Point result;
  if(_j % 2)
      result.x = I * (1 + _i); // odd rows
  else 
      result.x = I/2.0 + I * _i; // even rows
  result.y = K + J * _j;
  return result;
}


Hex::Hex(const Grid& grid, int i, int j):
  _edgeA(this,A), _edgeB(this,B), _edgeC(this,C),
  _edgeD(this,D), _edgeE(this,E), _edgeF(this,F),
  _grid(grid), _i(i), _j(j)
{}


Hex::Hex(const Grid& grid, const Hex& h):
  _edgeA(this,A), _edgeB(this,B), _edgeC(this,C),
  _edgeD(this,D), _edgeE(this,E), _edgeF(this,F),
 _grid(grid), _i(h._i), _j(h._j)
{}


} // end namespace hex
