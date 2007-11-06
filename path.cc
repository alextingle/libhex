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


std::string
Path::steps(void) const
{
  std::string result ="";
  const Hex* curr =NULL;
  for(std::list<Hex*>::const_iterator h=_hexes.begin(); h!=_hexes.end(); ++h)
  {
    if(curr)
        result += hex::steps(curr,*h);
    curr = *h;
  }
  return result;
}


std::string
Path::str(void) const
{
  assert(!this->_hexes.empty());
  std::string result =this->_hexes.front()->str();
  result+=":"+this->steps();
  return result;
}


/** Helper: populate _hexes from steps. */
inline std::list<Hex*>
path(Hex* start, const std::string& steps)
  throw(hex::out_of_range,hex::invalid_argument)
{
  // See also: go(int&,int&,const std::string&)
  std::list<Hex*> hexes;
  hexes.push_back(start);
  std::string::size_type cur =0;
  while(cur<steps.size() && steps[cur]!='?')
  {
    // Find direction
    Direction dir =to_direction( steps[cur] );
    ++cur;
    bool repeat =(cur<steps.size() && steps[cur]=='*');
    do{
        Hex* next =hexes.back()->go( dir );
        if(next)
            hexes.push_back( next );
        else if(*steps.rbegin()=='?' || repeat)
            return hexes; // bail out instead of throwing
        else
            throw hex::out_of_range(std::string("path:")+steps);
    } while(repeat);
  }
  return hexes;
}


Path::Path(Hex* start, const std::string& steps)
  throw(hex::out_of_range,hex::invalid_argument)
  : _hexes( path(start,steps) )
{}


Path::Path(Hex* from, Hex* to) throw()
  : _hexes( path(from,hex::steps(from,to)) )
{}


} // end namespace hex
