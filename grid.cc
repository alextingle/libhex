#include "hex.h"

#include <sstream>
#include <cmath>
#include <cerrno>

namespace hex {


Hex*
Grid::hex(int i, int j) const throw(hex::out_of_range)
{
  try
  {
    return _rows.at(j)->at(i);
  }
  catch(std::out_of_range& e)
  {
    throw hex::out_of_range(e.what());
  }
}


Hex*
Grid::hex(Distance x, Distance y) const throw(hex::out_of_range)
{
  int j = lround( (y-K)/J );
  int i;
  if(j % 2)
      i = lround( x/I ) - 1; // odd rows
  else
      i = lround( (x - I/2.0)/I ); // even rows
  return hex(i,j);
}


Hex*
Grid::hex(const Point& p) const throw(hex::out_of_range)
{
  return hex(p.x,p.y);
}


Area
Grid::to_area(void) const
{
  using namespace std;
  set<Hex*> hexes;
  for(vector<Row*>::const_iterator j =_rows.begin(); j!=_rows.end(); ++j)
      for(Row::const_iterator i =(**j).begin(); i!=(**j).end(); ++i)
          hexes.insert( *i );
  return Area(hexes);  
}


Hex*
Grid::hex(const std::string& s) const throw(out_of_range,invalid_argument)
{
  // Parse strings like: / *[-+]?\d+, *[-+]?\d+([^\d].*)?/
  // E.g.  '1,2' ' +2, 4' '2,4 # comment'
  const char* buf =s.c_str();
  char* endptr =NULL;
  errno=0;
  int i =::strtol(buf,&endptr,10);
  if(endptr==buf || *endptr!=',')
      throw hex::invalid_argument(s);
  buf =endptr+1;
  int j =::strtol(buf,&endptr,10);
  if(endptr==buf)
      throw hex::invalid_argument(s);
  if(ERANGE==errno)
      throw hex::out_of_range(s);
  return this->hex(i,j);
}


Grid::Grid(int cols, int rows): _rows()
{
  for(int j=0; j<rows; ++j)
  {
    _rows.push_back( new Row );
    for(int i=0; i<cols; ++i)
        _rows.back()->push_back( new Hex(*this,i,j) );
  }
}


Grid::Grid(const Grid& v): _rows()
{
  using namespace std;
  for(vector<Row*>::const_iterator j =v._rows.begin(); j!=v._rows.end(); ++j)
  {
    _rows.push_back( new Row );
    for(Row::const_iterator i =(**j).begin(); i!=(**j).end(); ++i)
        _rows.back()->push_back( new Hex(*this,**i) );
  }
}


Grid::~Grid()
{
  using namespace std;
  for(vector<Row*>::const_iterator j =_rows.begin(); j!=_rows.end(); ++j)
  {
    for(Row::const_iterator i =(**j).begin(); i!=(**j).end(); ++i)
        delete (*i);
    delete (*j);
  }
}


} // end namespace hex
