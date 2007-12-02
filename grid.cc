#include "hex.h"

#include <sstream>
#include <cmath>
#include <cerrno>
#include <cassert>

namespace hex {


//
// Point


Point::Point(const std::string s) throw (out_of_range,invalid_argument)
{
  // Parse strings like: / *[-+]?\d+_ *[-+]?\d+([^\d].*)?/
  // E.g.  '1_2' ' +2_ 4' '2_4 # comment'
  const char* buf =s.c_str();
  char* endptr =NULL;
  errno=0;
  this->x = ::strtod(buf,&endptr);
  if(endptr==buf || *endptr!=',')
      throw hex::invalid_argument(s);
  buf =endptr+1;
  this->y = ::strtod(buf,&endptr);
  if(endptr==buf)
      throw hex::invalid_argument(s);
  if(ERANGE==errno)
      throw hex::out_of_range(s);
}


std::string
Point::str(void) const
{
  std::ostringstream ss;
  ss<< this->x << "," << this->y;
  return ss.str();
}


//
// Grid


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
  // (Note I==1.0, so the factor of I has been omitted.)
  const static Distance K_2 =K/2.0;
  // BI is unit vector in direction B
  const static Distance BIx = 0.5;
  const static Distance BIy = 1.5 * K;
  // CI is unit vector in direction C
  const static Distance CIx = -BIx;
  const static Distance CIy =  BIy;

  // Calculate the 'simple' solution.
  y -= K;
  int j = lround( y/J );
  if(j % 2)
      x -= 1.0; // odd rows
  else
      x -= 0.5; // even rows
  int i = lround( x );                                              //   x / I
  // Now calculate the x,y offsets (in units of (I,J) )
  Distance dx = x - Distance(i);                                    //   i * I
  Distance dy = y - Distance(j) * J;
  // Only need more work if |dy| > K/2
  if( dy < -K_2 || K_2 < dy )
  {
    Distance BId = (BIx * dx) + (BIy * dy);
    Distance CId = (CIx * dx) + (CIy * dy);

    if(      BId >  0.5 )
                          go( i,j, B );
    else if( BId < -0.5 )
                          go( i,j, E );
    else if( CId >  0.5 )
                          go( i,j, C );
    else if( CId < -0.5 )
                          go( i,j, F );
  }
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
  // Parse strings like: / *[-+]?\d+_ *[-+]?\d+([^\d].*)?/
  // E.g.  '1_2' ' +2_ 4' '2_4 # comment'
  const char* buf =s.c_str();
  char* endptr =NULL;
  errno=0;
  int i =::strtol(buf,&endptr,10);
  if(endptr==buf || *endptr!='_')
      throw hex::invalid_argument(s);
  buf =endptr+1;
  int j =::strtol(buf,&endptr,10);
  if(endptr==buf)
      throw hex::invalid_argument(s);
  if(ERANGE==errno)
      throw hex::out_of_range(s);
  return this->hex(i,j);
}


std::set<Hex*>
Grid::hexes(const std::string& s) const throw(out_of_range,invalid_argument)
{
  // Parse string of space-separated hex coordinates.
  // E.g. 1_2 2_3 3_4
  std::istringstream is(s);
  std::set<Hex*> result;
  while(is.good())
  {
    std::string tok;
    is>>tok;
    if(!tok.empty())
        result.insert( this->hex(tok) );
  }
  return result;
}


Area
Grid::area(const std::string& s) const throw(out_of_range,invalid_argument)
{
  // Parse string of area fillpaths
  // E.g. 1,2>CDE:ABC
  std::set<Hex*> result;
  std::string::size_type pos =s.find_first_of(":>");
  if(pos==std::string::npos)
      throw hex::invalid_argument(s);
  Hex* origin =this->hex( s.substr(0,pos) );
  Hex* start  =origin;
  while(pos!=std::string::npos)
  {
    std::string::size_type next =s.find_first_of(":>",pos+1);
    std::string steps =s.substr( pos+1, (next==s.npos)?(next):(next-pos-1) );
    if(s[pos]=='>')
    {
      start=origin->go(steps);
    }
    else // ':'
    {
      std::list<Hex*> hexes =Path(start,steps).hexes();
      std::copy(hexes.begin(), hexes.end(), inserter(result,result.end()));
      start=origin;
    }
    pos=next;
  }
  return result;
}


Path
Grid::path(const std::string& s) const throw(out_of_range,invalid_argument)
{
  std::string::size_type colon =s.find(':');
  if(colon==std::string::npos || (colon+1)>=s.size())
      throw hex::invalid_argument(s);
  Hex* origin =this->hex( s.substr(0,colon) );
  Path result(origin,s.substr(colon+1));
  return result;
}


Boundary
Grid::boundary(const std::string& s) const throw(out_of_range,invalid_argument)
{
  std::list<Edge*> result;
  std::string::size_type plus_minus =s.find_first_of("+-");
  if(plus_minus==std::string::npos || (plus_minus+1)>=s.size())
      throw hex::invalid_argument(s);
  bool clockwise =( '-' == s[plus_minus] );
  for(std::string::size_type pos =plus_minus+1; pos<s.size(); ++pos)
  {
    Direction d =to_direction(s[pos]);
    Edge* next;
    if(result.empty())
    {
      next = this->hex(s.substr(0,plus_minus))->edge(d);
    }
    else
    {
      next = result.back()->next_in(clockwise);
      if(next->direction() != d)
      {
        next = result.back()->next_out(clockwise);
        if(next->direction() != d)
            throw hex::invalid_argument(s);
      }
    }
    result.push_back(next);
  }
  return result;
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
