#include "hex.h"

#include <cassert>

namespace hex {


Area
Path::to_area(void) const
{
  std::set<Hex*> result;
  std::copy(_hexes.begin(), _hexes.end(), std::inserter(result,result.end()));
  return result;
}


int
Path::length(void) const
{
  return int( _hexes.size() );
}


/** Helper: populate _hexes from steps. */
inline void
steps_to_hexes(
    Hex*               start,
    const std::string& steps,
    std::list<Hex*>&   hexes
  ) throw(std::out_of_range)
{
  hexes.push_back(start);
  for(std::string::size_type i=0; i<steps.size(); ++i)
  {
    Hex* next =hexes.back()->go( to_direction(steps[i]) );
    if(next)
        hexes.push_back( next );
    else
        throw std::out_of_range("Path");
  }
}


Path::Path(Hex* start, const std::string& steps) throw(std::out_of_range)
  : _hexes()
{
  steps_to_hexes(start,steps,_hexes);
}


Path::Path(Hex* from, Hex* to) throw(std::out_of_range)
  : _hexes()
{
  steps_to_hexes(from,steps(from,to),_hexes);
}



} // end namespace hex
