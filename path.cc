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


Path::Path(Hex* start, const std::string& steps) throw(std::out_of_range)
  : _hexes()
{
  _hexes.push_back(start);
  for(std::string::size_type i=0; i<steps.size(); ++i)
  {
    Hex* next =_hexes.back()->go( to_direction(steps[i]) );
    if(next)
        _hexes.push_back( next );
    else
        throw std::out_of_range("Path");
  }
}


} // end namespace hex
