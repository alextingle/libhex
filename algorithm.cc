#include "hex.h"

#include <cassert>

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


Path
path(Hex* from, Hex* to)
{
  std::list<Hex*> result;
  while( from->j() < to->j() )
  {
    result.push_back(from);
    if( from->i() < to->i() )
        from=from->go(B);
    else
        from=from->go(C);
  }
  while( from->j() > to->j() )
  {
    result.push_back(from);
    if( from->i() < to->i() )
        from=from->go(F);
    else
        from=from->go(E);
  }
  Direction di = (from->i()<to->i()? A: D);
  while( from->i() != to->i() )
  {
    result.push_back(from);
    from=from->go(di);
  }
  assert(from==to);
  result.push_back(to);
  return result;
}


int
distance(Hex* h0, Hex* h1)
{
  Path p =path(h0,h1);
  return p.length();
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
    int i =h->i();
    int j =h->j();
  
    try {
      go(i,j,hex::A,distance); // in/out: i,j
      result.insert( grid.hex(i,j) );
    } catch(std::out_of_range&) {}
  
    for(int d=0; d<DIRECTIONS; ++d)
    {
      Direction direction =C+d;
      for(int count=0; count<distance; ++count)
          try {
            go(i,j,direction); // in/out: i,j
            result.insert( grid.hex(i,j) );
          } catch(std::out_of_range&) {}
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
