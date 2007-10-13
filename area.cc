#include "hex.h"

#include <cassert>

namespace hex {


Boundary
Area::boundary(void) const
{
  // Start with a random hex.
  Hex* h =*_hexes.begin();
  const Grid& grid =h->grid();
  int i0 =h->i();
  int j0 =h->j();
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
